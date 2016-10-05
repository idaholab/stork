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
#include "SinkMapKernel.h"
#include "SinkMapUserObject.h"

template<>
InputParameters validParams<SinkMapKernel>()
{
  InputParameters params = validParams<Reaction>();

  params.addRequiredParam<UserObjectName>("sink_map_user_object", "The name of the SinkMapUserObject.");
  params.addRequiredParam<MaterialPropertyName>("diffusivity_name", "Name of the material property for diffusion coefficient");

  return params;
}

SinkMapKernel::SinkMapKernel(const InputParameters & parameters) :
    Reaction(parameters),
    _sink_map_uo(getUserObject<SinkMapUserObject>("sink_map_user_object")),
    _diffusivity(getMaterialProperty<Real>("diffusivity_name")),
    _element_sink_map(0)
{
}

void
SinkMapKernel::computeResidual()
{
  // get the current element's sink strength vector
  _element_sink_map = _sink_map_uo.getLocalSinkMap(_current_elem);

  // now call the base class's function
  Reaction::computeResidual();
}

Real
SinkMapKernel::computeQpResidual()
{
  return _diffusivity[_qp] * _element_sink_map[_qp] * Reaction::computeQpResidual();
}

void
SinkMapKernel::computeJacobian()
{
  // get the current element's sink strength vector
  _element_sink_map = _sink_map_uo.getLocalSinkMap(_current_elem);

  // now call the base class's function
  Reaction::computeJacobian();
}

Real
SinkMapKernel::computeQpJacobian()
{
  return _diffusivity[_qp] * _element_sink_map[_qp] * Reaction::computeQpJacobian();
}
