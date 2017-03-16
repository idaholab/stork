/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/


//  This post processor returns the mass due to a half-gaussian sink flux from the boundary of a volume.
//
#include "BAHalfCubicSinkFlux.h"

// MOOSE includes
#include "Function.h"
#include "FEProblem.h"

template<>
InputParameters validParams<BAHalfCubicSinkFlux>()
{
  InputParameters params = validParams<SideIntegralVariablePostprocessor>();
  params.addRequiredParam<Real>("max", "Maximum of the flux (measured in kg.m^-2.s^-1).  Denote x = porepressure - centre.  Then Flux out = (max/cutoff^3)*(2x + cutoff)(x - cutoff)^2 for cutoff < x < 0.  Flux out = max for x >= 0.  Flux out = 0 for x <= cutoff.  Note, to make this a source rather than a sink, let max<0");
  params.addRequiredParam<FunctionName>("cutoff", "Cutoff of the cubic (measured in Pa).  This needs to be less than zero.");
  params.addRequiredParam<Real>("centre", "Centre of the cubic (measured in Pa");
  params.addParam<FunctionName>("multiplying_fcn", 1.0, "The flux will be multiplied by this spatially-and-temporally varying function.");
  params.addRequiredParam<UserObjectName>("richardsVarNames_UO", "The UserObject that holds the list of Richards variable names.");
  return params;
}

BAHalfCubicSinkFlux::BAHalfCubicSinkFlux(const InputParameters & parameters) :
    SideIntegralVariablePostprocessor(parameters),
    _feproblem(dynamic_cast<FEProblem &>(_subproblem)),
    _maximum(getParam<Real>("max")),
    _cutoff(getFunction("cutoff")),
    _centre(getParam<Real>("centre")),
    _richards_name_UO(getUserObject<RichardsVarNames>("richardsVarNames_UO")),
    _pvar(_richards_name_UO.richards_var_num(coupled("variable"))),
    _m_func(getFunction("multiplying_fcn")),
    _pp(getMaterialProperty<std::vector<Real> >("porepressure"))
{}

Real
BAHalfCubicSinkFlux::computeQpIntegral()
{
  Real x = _pp[_qp][_pvar] - _centre;
  Real full_value = _maximum*_dt*_m_func.value(_t, _q_point[_qp]);

  if (x >= 0)
    return full_value;

  Real cutoff = _cutoff.value(_t, _q_point[_qp]);
  if (x <= cutoff)
    return 0;

  Real cutoff3 = cutoff*cutoff*cutoff;
  return full_value*(2*x + cutoff)*(x - cutoff)*(x - cutoff)/cutoff3;
}
