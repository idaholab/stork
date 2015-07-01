/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/


//  This post processor returns the mass due to a half-gaussian sink flux from the boundary of a volume.
//
#include "BAHalfCubicSinkFlux.h"
#include "Function.h"

template<>
InputParameters validParams<BAHalfCubicSinkFlux>()
{
  InputParameters params = validParams<SideIntegralVariablePostprocessor>();
  params.addRequiredParam<Real>("max", "Maximum of the flux (measured in kg.m^-2.s^-1).  Denote x = porepressure - centre.  Then Flux out = (max/cutoff^3)*(2x + cutoff)(x - cutoff)^2 for cutoff < x < 0.  Flux out = max for x >= 0.  Flux out = 0 for x <= cutoff.  Note, to make this a source rather than a sink, let max<0");
  params.addRequiredRangeCheckedParam<Real>("cutoff", "cutoff < 0", "Cutoff of the cubic (measured in Pa).  This needs to be less than zero.");
  params.addRequiredParam<Real>("centre", "Centre of the cubic (measured in Pa");
  params.addParam<FunctionName>("multiplying_fcn", 1.0, "The flux will be multiplied by this spatially-and-temporally varying function.");
  params.addRequiredParam<UserObjectName>("richardsVarNames_UO", "The UserObject that holds the list of Richards variable names.");
  return params;
}

BAHalfCubicSinkFlux::BAHalfCubicSinkFlux(const std::string & name, InputParameters parameters) :
    SideIntegralVariablePostprocessor(name, parameters),
    _feproblem(dynamic_cast<FEProblem &>(_subproblem)),
    _maximum(getParam<Real>("max")),
    _cutoff(getParam<Real>("cutoff")),
    _cutoff3(_cutoff*_cutoff*_cutoff),
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
  else if (x <= _cutoff)
    return 0;
  else
    return full_value*(2*x + _cutoff)*(x - _cutoff)*(x - _cutoff)/_cutoff3;
}
