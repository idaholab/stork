/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#include "EventMarker.h"

template<>
InputParameters validParams<EventMarker>()
{
  InputParameters params = validParams<Marker>();

  params.addRequiredParam<UserObjectName>("inserter", "The name of the EventInserter UserObject.");
  params.addRequiredParam<UserObjectName>("gaussian_user_object", "The name of the GaussianUserObject.");
  params.addParam<Real>("marker_radius", 3.0, "How many sigmas to mark away from event center.");
  params.addParam<bool>("verbose", false, "Print out extra information when marker is active.");
  params.addCoupledVar("periodic_variable", "Use perodic boundary conditions of this variable to determine the distance to the function peak location");
  params.addParam<bool>("coarsen_events", false, "Coarsen events at some later time. If true, set 'track_old_events=true' in EventInserter.");

  return params;
}


EventMarker::EventMarker(const InputParameters & parameters) :
    Marker(parameters),
    Coupleable(this, false),
    _inserter(getUserObject<EventInserter>("inserter")),
    _gaussian_uo(getUserObject<GaussianUserObject>("gaussian_user_object")),
    _marker_radius(getParam<Real>("marker_radius")),
    _verbose(getParam<bool>("verbose")),
    _periodic_var(isCoupled("periodic_variable") ? (int) coupled("periodic_variable") : -1),
    _coarsen_events(getParam<bool>("coarsen_events")),
    _refine_distance(_marker_radius * _gaussian_uo.getSigma()),
    _event_incoming(false),
    _event_location(0),
    _input_cycles_per_step(_adaptivity.getCyclesPerStep()),
    _coarsening_needed(false),
    _old_event_list(0)
{
  // Check input logic for coarsening events
  if ((_coarsen_events) && (!_inserter.areOldEventsBeingTracked())) // need to tell EventInserter to track old events
    mooseError("When coarsening old events ('coarsen_events = true'), EventInserter object needs to track old events. Please set 'track_old_events = true' in EventInserter block.");
}

void
EventMarker::initialSetup()
{
  // need to check for initial events
  checkForEvent();
}

void
EventMarker::timestepSetup()
{
  // check for events at the next timestep
  checkForEvent();

  // get old event list if coarsening requested
  _coarsening_needed = false;
  if ((_coarsen_events) && (_inserter.wasOldEventRemoved()))
  {
    _old_event_list = _inserter.getOldEventList();
    _coarsening_needed = true;
    _adaptivity.setCyclesPerStep(_input_cycles_per_step);  // turn adaptivity on
    if (_verbose)
      _console << "EventMarker detected an old event was removed, coarsening mesh (at the end of timestep)..." << std::endl;
  }
}

Marker::MarkerValue
EventMarker::computeElementMarker()
{
  Point centroid = _current_elem->centroid();

  // refine mesh if event is incoming
  if (_event_incoming)
  {

    // distance to center depends on perodicity
    Real r;
    if (_periodic_var < 0)
      r = (_event_location - centroid).norm();
    else
      r = _mesh.minPeriodicDistance(_periodic_var, _event_location, centroid);

    if (r < _refine_distance)
      return REFINE;

    return DO_NOTHING; // default
  }

  if (_coarsening_needed)
  {
    // coarsen everywhere except inside old events
    for (unsigned int i=0; i<_old_event_list.size(); i++)
    {
      _event_location = _old_event_list[i].second;
      // TODO: remove code duplication
      // distance to center depends on perodicity
      Real r;
      if (_periodic_var < 0)
        r = (_event_location - centroid).norm();
      else
        r = _mesh.minPeriodicDistance(_periodic_var, _event_location, centroid);

      if (r < _refine_distance)
        return DO_NOTHING;
    }

    return COARSEN;
  }

  return DO_NOTHING; // satisfy compiler
}

void
EventMarker::checkForEvent()
{
  // turn adaptivity off by default
  _adaptivity.setCyclesPerStep(0);

  // Check if event will occur during the NEXT timestep, so we can refine now and be ready for it
  // Event will be active on the next time step if we are at the next event time NOW
  _event_incoming = false;
  if (_inserter.isEventActive(_fe_problem.time() - _inserter.getTimeTolerance(), _fe_problem.time() + _inserter.getTimeTolerance()))  // fuzzy math comparing floats
  {
    _event_location = _inserter.getActiveEventPoint(_fe_problem.time() - _inserter.getTimeTolerance(), _fe_problem.time() + _inserter.getTimeTolerance());
    if (_verbose)
      _console << "Event incoming! location: " << _event_location << std::endl;
    _adaptivity.setCyclesPerStep(_input_cycles_per_step);  // turn adaptivity on
    _event_incoming = true;
  }
}
