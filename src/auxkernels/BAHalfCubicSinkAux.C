/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#include "BAHalfCubicSinkAux.h"
#include "Function.h"

template<>
InputParameters validParams<BAHalfCubicSinkAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addClassDescription("Yields the values of a BAHalfCubicSink, measured in kg.m^-2.year^-1 (or whatever your base units are).");
  params.addRequiredParam<Real>("max", "Maximum of the flux (measured in kg.m^-2.s^-1).  Denote x = porepressure - centre.  Then Flux out = (max/cutoff^3)*(2x + cutoff)(x - cutoff)^2 for cutoff < x < 0.  Flux out = max for x >= 0.  Flux out = 0 for x <= cutoff.  Note, to make this a source rather than a sink, let max<0");
  params.addRequiredParam<FunctionName>("cutoff", "Cutoff of the cubic (measured in Pa).  This needs to be less than zero.");
  params.addRequiredParam<Real>("centre", "Centre of the cubic (measured in Pa");
  params.addRequiredCoupledVar("pressure_var", "The variable that represents the pressure");
  params.addParam<FunctionName>("multiplying_fcn", 1.0, "The flux will be multiplied by this spatially-and-temporally varying function.");
  return params;
}

BAHalfCubicSinkAux::BAHalfCubicSinkAux(const InputParameters & parameters) :
    AuxKernel(parameters),
    _maximum(getParam<Real>("max")),
    _cutoff(getFunction("cutoff")),
    _centre(getParam<Real>("centre")),
    _pressure_var(coupledValue("pressure_var")),
    _m_func(getFunction("multiplying_fcn"))
{
}

Real
BAHalfCubicSinkAux::computeValue()
{
  Real x = _pressure_var[_qp] - _centre;
  Real full_value = _maximum*_m_func.value(_t, _q_point[_qp]);

  if (x >= 0)
    return full_value;

  Real cutoff = _cutoff.value(_t, _q_point[_qp]);
  if (x <= cutoff)
    return 0;

  Real cutoff3 = cutoff*cutoff*cutoff;
  return full_value*(2*x + cutoff)*(x - cutoff)*(x - cutoff)/cutoff3;
}

