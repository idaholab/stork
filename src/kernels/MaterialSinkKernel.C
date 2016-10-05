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
#include "MaterialSinkKernel.h"

template<>
InputParameters validParams<MaterialSinkKernel>()
{
  InputParameters params = validParams<Reaction>();

  params.addRequiredParam<MaterialPropertyName>("diffusivity_name", "Name of the material property for diffusion coefficient.");
  params.addRequiredParam<MaterialPropertyName>("sink_strength_name", "Name of the material property for sink strength.");

  return params;
}

MaterialSinkKernel::MaterialSinkKernel(const InputParameters & parameters) :
    Reaction(parameters),
    _diffusivity(getMaterialProperty<Real>("diffusivity_name")),
    _sink_strength(getMaterialProperty<Real>("sink_strength_name"))
{
}

Real
MaterialSinkKernel::computeQpResidual()
{
  return _diffusivity[_qp] * _sink_strength[_qp] * Reaction::computeQpResidual();
}

Real
MaterialSinkKernel::computeQpJacobian()
{
  return _diffusivity[_qp] * _sink_strength[_qp] * Reaction::computeQpJacobian();
}
