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

  MooseEnum coarsen_method("time sigma", "time");

  params.addRequiredParam<UserObjectName>("inserter", "The name of the EventInserter UserObject.");
  params.addRequiredParam<UserObjectName>("gaussian_user_object", "The name of the GaussianUserObject.");
  params.addParam<Real>("marker_radius", 3.0, "How many sigmas to mark away from event center.");
  params.addParam<bool>("verbose", false, "Print out extra information when marker is active.");
  params.addCoupledVar("periodic_variable", "Use perodic boundary conditions of this variable to determine the distance to the function peak location");
  params.addParam<bool>("coarsen_events", false, "Coarsen events at some later time. If true, choose method with 'coarsen_method'.");
  params.addParam<MooseEnum>("coarsen_method", coarsen_method, "How to decide when to coarsen old cascade events. Choices are 'time' or 'target_sigma'.");
  params.addParam<Real>("coarsen_time", "Time to wait after an event to coarsen the mesh in the vicinity of that event.");
  params.addParam<Real>("coarsen_sigma", "Target (approx.) sigma value at which to coarsen the mesh in the vicinity of that event.");

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
    _coarsen_method(getParam<MooseEnum>("coarsen_method")),
    _coarsen_time(((_coarsen_events) && (_coarsen_method == "time")) ? getParam<Real>("coarsen_time") : std::numeric_limits<Real>::max()),
    _coarsen_sigma(((_coarsen_events) && (_coarsen_method == "sigma")) ? getParam<Real>("coarsen_sigma") : std::numeric_limits<Real>::max()),
    _refine_distance(_marker_radius * _gaussian_uo.getSigma()),
    _event_incoming(false),
    _event_location(0),
    _input_cycles_per_step(_adaptivity.getCyclesPerStep()),
    _old_event_list(0),
    _coarsening_needed(false)
{
  // Check input logic for coarsening events
  if (_coarsen_events)
  {
    if ((_coarsen_method == "time") && (!parameters.isParamSetByUser("coarsen_time")))
      mooseError("User requested to coarsen events by time but 'coarsen_time' was not set.");
    if ((_coarsen_method == "target_sigma") && (!parameters.isParamSetByUser("sigma")))
      mooseError("User requested to coarsen events by time but 'coarsen_sigma' was not set.");
  }
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

  // check if event needs to be coarsened
  if (_coarsen_events)
  {
    if (_verbose)
      for (unsigned int i=0; i<_old_event_list.size(); i++)
        _console << "old list " << i << ": time: " << _old_event_list[i].first << " location: " << _old_event_list[i].second << std::endl;

    _coarsening_needed = false;
    for (unsigned int i=0; i<_old_event_list.size(); i++)
    {
      if (_coarsen_method == "time")
      {
        if (_old_event_list[i].first < _fe_problem.time() - _coarsen_time) // check if event is old enough
        {
          _coarsening_needed = true;
          _adaptivity.setCyclesPerStep(_input_cycles_per_step);  // turn adaptivity on

          if (_verbose)
            _console << "event needs to be coarsened: (old) event time: " << _old_event_list[i].first << " location: " << _old_event_list[i].second << std::endl;

          // remove old event from list as we will signal the marker to coarsen everywhere where events are active
          _old_event_list[i] = _old_event_list.back();
          _old_event_list.pop_back();
          break;
        }
      }
    }
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

    // save old events if coarsening requested
    if (_coarsen_events)
      _old_event_list.push_back(Event(_fe_problem.time(), _event_location));
  }
}
