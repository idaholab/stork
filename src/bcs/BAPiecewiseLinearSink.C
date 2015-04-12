/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/


#include "BAPiecewiseLinearSink.h"

template<>
InputParameters validParams<BAPiecewiseLinearSink>()
{
  InputParameters params = validParams<RichardsPiecewiseLinearSink>();
  params.addCoupledVar("aux_var", 1, "Fluxes will be multiplied by this variable");
  return params;
}

BAPiecewiseLinearSink::BAPiecewiseLinearSink(const std::string & name, InputParameters parameters) :
    RichardsPiecewiseLinearSink(name,parameters),
    _aux_var(coupledValue("aux_var"))
{
}




Real
BAPiecewiseLinearSink::computeQpResidual()
{
  return _aux_var[_qp]*RichardsPiecewiseLinearSink::computeQpResidual();
}


Real
BAPiecewiseLinearSink::computeQpJacobian()
{
  return _aux_var[_qp]*RichardsPiecewiseLinearSink::computeQpJacobian();
}


Real
BAPiecewiseLinearSink::computeQpOffDiagJacobian(unsigned int jvar)
{
  return _aux_var[_qp]*RichardsPiecewiseLinearSink::computeQpOffDiagJacobian(jvar);
}
