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
#include "EventInserter.h"
#include "GaussianUserObject.h"

template<>
InputParameters validParams<EventInserterSource>()
{
  InputParameters params = validParams<Kernel>();
  params.addRequiredParam<UserObjectName>("inserter", "The name of the EventInserter UserObject.");
  params.addParam<UserObjectName>("gaussian_user_object", "The name of the GaussianUserObject.");
  params.addParam<bool>("uniform_source", false, "If true, the source term will be uniform over the whole domain with the value set by the 'magnitude' parameter.");
  params.addParam<Real>("magnitude", 0.0, "The total volume-integrated value of the source term. The value will be divided by simulation volume.");
  params.addParam<PostprocessorName>("volume_pps", 0.0, "The name of the Volume Postprocessor.");
  return params;
}

EventInserterSource::EventInserterSource(const InputParameters & parameters) :
    Kernel(parameters),
    _inserter(getUserObject<EventInserter>("inserter")),
    _gaussian_user_object_ptr(parameters.isParamSetByUser("gaussian_user_object") ? &getUserObject<GaussianUserObject>("gaussian_user_object") : NULL),
    _use_uniform_source(getParam<bool>("uniform_source")),
    _magnitude(getParam<Real>("magnitude")),
    _volume(getPostprocessorValue("volume_pps")),
    _is_event_active(false),
    _active_point(0)
{
  // check input logic for uniform source
  if (_use_uniform_source)
  {
    if (parameters.isParamSetByUser("gaussian_user_object"))
      mooseError("In EventInserterSource, both gaussian sources and uniform sources were requested. Only one can be used at a time.");
    if (! parameters.isParamSetByUser("magnitude"))
      mooseError("In EventInserterSource, a 'magnitude' parameter is needed if uniform sources are requested.");
    if (! parameters.isParamSetByUser("volume_pps"))
      mooseError("In EventInserterSource, a Volume Postprocessor is needed when using uniform sources. Set parameter 'volume_pps' to the postprocessor name.");
  }
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
  {
    if (_use_uniform_source)
      return -_test[_i][_qp] * _magnitude/_volume/_dt;

    return -_test[_i][_qp] * _gaussian_user_object_ptr->value(_q_point[_qp], _active_point)/_dt;
  }

  return 0.0;
}

