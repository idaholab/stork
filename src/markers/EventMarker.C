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

  return params;
}


EventMarker::EventMarker(const InputParameters & parameters) :
    Marker(parameters),
    _inserter(getUserObject<EventInserter>("inserter")),
    _gaussian_uo(getUserObject<GaussianUserObject>("gaussian_user_object")),
    _marker_radius(getParam<Real>("marker_radius")),
    _refine_distance(_marker_radius * _gaussian_uo.getSigma()),
    _event_incoming(false),
    _event_location(0),
    _input_cycles_per_step(_adaptivity.getCyclesPerStep())
{
}

void
EventMarker::timestepSetup()
{
  //_event_incoming = false;
  _adaptivity.setCyclesPerStep(0);

  // Check if event will occur during the NEXT timestep, so we can refine now and be ready for it
  // Event will be active on the time step if we are at the next event time NOW
  if (_inserter.isEventActive(_fe_problem.time() - _inserter.getTimeTolerance(), _fe_problem.time() + _inserter.getTimeTolerance()))
  {
    //_event_incoming = true;
    _event_location = _inserter.getActiveEventPoint(_fe_problem.time() - _inserter.getTimeTolerance(), _fe_problem.time() + _inserter.getTimeTolerance());
    _console << "Event incoming! location: " << _event_location << std::endl;
    _adaptivity.setCyclesPerStep(_input_cycles_per_step);
  }
}

Marker::MarkerValue
EventMarker::computeElementMarker()
{
  Point centroid = _current_elem->centroid();

  //if ((_event_incoming) && ((_event_location - centroid).size() < _refine_distance))
  if ((_event_location - centroid).size() < _refine_distance)
    return REFINE;

  return DO_NOTHING;
}


