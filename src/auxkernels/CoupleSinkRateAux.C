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

#include "CoupledSinkRateAux.h"

template<>
InputParameters validParams<CoupledSinkRateAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredParam<MaterialPropertyName>("diffusivity_name", "The name of the diffusivity material property.");
  params.addRequiredCoupledVar("solution_variable", "The name of the solution variable.");
  params.addRequiredCoupledVar("sink_strength_variable", "The name of the sink strength aux variable.");
  return params;
}

CoupledSinkRateAux::CoupledSinkRateAux(const InputParameters & parameters) :
    AuxKernel(parameters),
    _diffusivity(getMaterialProperty<Real>("diffusivity_name")),
    _v(coupledValue("solution_variable")),
    _sink_strength(coupledValue("sink_strength_variable"))
{
}

Real
CoupledSinkRateAux::computeValue()
{
  return _sink_strength[_qp] * _diffusivity[_qp] * _v[_qp];
}
