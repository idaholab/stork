/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/


//  van-Genuchten effective saturation as a function of single pressure, and its derivs wrt to that pressure
//
#include "BASeff1VG.h"

template<>
InputParameters validParams<BASeff1VG>()
{
  InputParameters params = validParams<RichardsSeff>();
  params.addRequiredRangeCheckedParam<Real>("al", "al > 0", "van-Genuchten alpha parameter.  Must be positive.  Single-phase VG seff = (1 + (-al*(p-s))^(1/(1-m)))^(-m)");
  params.addRequiredRangeCheckedParam<Real>("m", "m > 0 & m < 1", "van-Genuchten m parameter.  Must be between 0 and 1, and optimally should be set to >0.5   Single-phase VG seff = (1 + (-al*(p-s))^(1/(1-m)))^(-m)");
  params.addParam<Real>("modflow_shift", 0, "Shift (s).  Typically this is not positive.  Single-phase VG seff = (1 + (-al*(p-s))^(1/(1-m)))^(-m)");
  params.addClassDescription("van-Genuchten effective saturation as a function of pressure suitable for use in single-phase simulations..  seff = (1 + (-al*(p-s))^(1/(1-m)))^(-m), where s is the modflow_shift");
  return params;
}

BASeff1VG::BASeff1VG(const InputParameters & parameters) :
    RichardsSeff(parameters),
    _al(getParam<Real>("al")),
    _m(getParam<Real>("m")),
    _modflow_shift(getParam<Real>("modflow_shift"))
{
}


Real
BASeff1VG::seff(std::vector<const VariableValue *> p, unsigned int qp) const
{
  return RichardsSeffVG::seff((*p[0])[qp] - _modflow_shift, _al, _m);
}

void
BASeff1VG::dseff(std::vector<const VariableValue *> p, unsigned int qp, std::vector<Real> & result) const
{
  result[0] = RichardsSeffVG::dseff((*p[0])[qp] - _modflow_shift, _al, _m);
}

void
BASeff1VG::d2seff(std::vector<const VariableValue *> p, unsigned int qp, std::vector<std::vector<Real> > & result) const
{
  result[0][0] = RichardsSeffVG::d2seff((*p[0])[qp] - _modflow_shift, _al, _m);
}

