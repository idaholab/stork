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

#include "EventInserterSource.h"

template<>
InputParameters validParams<EventInserterSource>()
{
  InputParameters params = validParams<Kernel>();
  params.addRequiredParam<UserObjectName>("inserter", "The name of the EventInserter UserObject.");
  params.addRequiredParam<UserObjectName>("gaussian_user_object", "The name of the GaussianUserObject.");
  return params;
}

EventInserterSource::EventInserterSource(const InputParameters & parameters) :
    Kernel(parameters),
    _inserter(getUserObject<EventInserter>("inserter")),
    _gaussian_user_object(getUserObject<GaussianUserObject>("gaussian_user_object")),
    _is_event_active(false),
    _active_point(0)
{
}

void
EventInserterSource::timestepSetup()
{
  _is_event_active = false;

  // at time step t_i, integrator is stepping from (t_i - dt) to (t_i)
  if (_inserter.isEventActive(_t - _dt, _t))
  {
    _is_event_active = true;
    _active_point = _inserter.getActiveEventPoint(_t - _dt, _t);
    _console << "*** EventInserterSource::timestepSetup: active cascade detected!" << std::endl;
    _console << "*** EventInserterSource::timestepSetup: Point:" << _active_point << std::endl;
  }
}

Real
EventInserterSource::computeQpResidual()
{
  if (_is_event_active)
    return -_test[_i][_qp] * _gaussian_user_object.value(_q_point[_qp], _active_point)/_dt;

  return 0.0;
}

