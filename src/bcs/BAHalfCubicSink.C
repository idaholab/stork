/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/


#include "BAHalfCubicSink.h"

// MOOSE includes
#include "Material.h"
#include "Function.h"
#include "MooseVariable.h"

#include <iostream>


template<>
InputParameters validParams<BAHalfCubicSink>()
{
  InputParameters params = validParams<IntegratedBC>();
  params.addRequiredParam<Real>("max", "Maximum of the flux (measured in kg.m^-2.s^-1).  Denote x = porepressure - centre.  Then Flux out = (max/cutoff^3)*(2x + cutoff)(x - cutoff)^2 for cutoff < x < 0.  Flux out = max for x >= 0.  Flux out = 0 for x <= cutoff.  Note, to make this a source rather than a sink, let max<0");
  params.addRequiredParam<FunctionName>("cutoff", "Cutoff of the cubic (measured in Pa).  This needs to be less than zero.");
  params.addRequiredParam<Real>("centre", "Centre of the cubic (measured in Pa");
  params.addParam<FunctionName>("multiplying_fcn", 1.0, "If this function is provided, the flux will be multiplied by this function.  This is useful for spatially or temporally varying sinks");
  params.addRequiredParam<UserObjectName>("richardsVarNames_UO", "The UserObject that holds the list of Richards variable names.");
  return params;
}

BAHalfCubicSink::BAHalfCubicSink(const InputParameters & parameters) :
    IntegratedBC(parameters),
    _maximum(getParam<Real>("max")),
    _cutoff(getFunction("cutoff")),
    _centre(getParam<Real>("centre")),
    _m_func(getFunction("multiplying_fcn")),
    _richards_name_UO(getUserObject<RichardsVarNames>("richardsVarNames_UO")),
    _pvar(_richards_name_UO.richards_var_num(_var.number())),
    _pp(getMaterialProperty<std::vector<Real> >("porepressure")),
    _dpp_dv(getMaterialProperty<std::vector<std::vector<Real> > >("dporepressure_dv"))
{}

Real
BAHalfCubicSink::computeQpResidual()
{
  Real x = _pp[_qp][_pvar] - _centre;
  Real full_value = _maximum*_test[_i][_qp]*_m_func.value(_t, _q_point[_qp]);

  if (x >= 0)
    return full_value;

  Real cutoff = _cutoff.value(_t, _q_point[_qp]);
  if (x <= cutoff)
    return 0;

  Real cutoff3 = cutoff*cutoff*cutoff;
  return full_value*(2*x + cutoff)*(x - cutoff)*(x - cutoff)/cutoff3;
}

Real
BAHalfCubicSink::computeQpJacobian()
{
  Real x = _pp[_qp][_pvar] - _centre;
  Real full_value = _maximum*_test[_i][_qp]*_m_func.value(_t, _q_point[_qp]);

  if (x >= 0)
    return 0.0;

  Real cutoff = _cutoff.value(_t, _q_point[_qp]);
  if (x <= cutoff)
    return 0;

  Real cutoff3 = cutoff*cutoff*cutoff;
  return full_value*6*x*(x - cutoff)/cutoff3*_phi[_j][_qp]*_dpp_dv[_qp][_pvar][_pvar];
}

Real
BAHalfCubicSink::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (_richards_name_UO.not_richards_var(jvar))
    return 0.0;
  unsigned int dvar = _richards_name_UO.richards_var_num(jvar);

  Real x = _pp[_qp][_pvar] - _centre;
  Real full_value = _maximum*_test[_i][_qp]*_m_func.value(_t, _q_point[_qp]);

  if (x >= 0)
    return 0.0;

  Real cutoff = _cutoff.value(_t, _q_point[_qp]);
  if (x <= cutoff)
    return 0;

  Real cutoff3 = cutoff*cutoff*cutoff;
  return full_value*6*x*(x - cutoff)/cutoff3*_phi[_j][_qp]*_dpp_dv[_qp][_pvar][dvar];
}
