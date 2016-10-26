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

#include "SinkMapRateAux.h"

template<>
InputParameters validParams<SinkMapRateAux>()
{
  InputParameters params = validParams<SinkMapAux>();
  params.addRequiredParam<MaterialPropertyName>("diffusivity_name", "The name of the diffusivity material property.");
  params.addRequiredCoupledVar("solution_variable", "The name of the solution variable.");
  return params;
}

SinkMapRateAux::SinkMapRateAux(const InputParameters & parameters) :
    SinkMapAux(parameters),
    _diffusivity(getMaterialProperty<Real>("diffusivity_name")),
    _v(coupledValue("solution_variable"))
{
  if (isNodal())
    mooseError("Cannot use SinkMapRateAux with Nodal variables");
}

Real
SinkMapRateAux::computeValue()
{
  return _diffusivity[_qp] * _v[_qp] * SinkMapAux::computeValue();
}
