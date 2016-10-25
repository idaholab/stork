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
#include "MaterialSinkRateAux.h"

template<>
InputParameters validParams<MaterialSinkRateAux>()
{
  InputParameters params = validParams<AuxKernel>();

  params.addRequiredParam<MaterialPropertyName>("diffusivity_name", "Name of the material property for diffusion coefficient.");
  params.addRequiredParam<MaterialPropertyName>("sink_strength_name", "Name of the material property for sink strength.");
  params.addRequiredCoupledVar("solution_variable", "The name of the solution variable.");

  return params;
}

MaterialSinkRateAux::MaterialSinkRateAux(const InputParameters & parameters) :
    AuxKernel(parameters),
    _diffusivity(getMaterialProperty<Real>("diffusivity_name")),
    _sink_strength(getMaterialProperty<Real>("sink_strength_name")),
    _u(coupledValue("solution_variable"))
{
}

Real
MaterialSinkRateAux::computeValue()
{
  return _diffusivity[_qp] * _sink_strength[_qp] * _u[_qp];
}
