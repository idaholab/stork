/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/


//  This post processor returns the mass due to a flux from the boundary of a volume.
//
#include "BAPiecewiseLinearSinkFlux.h"

template<>
InputParameters validParams<BAPiecewiseLinearSinkFlux>()
{
  InputParameters params = validParams<RichardsPiecewiseLinearSinkFlux>();
  params.addCoupledVar("aux_var", 1, "Fluxes will be multiplied by this variable");
  return params;
}

BAPiecewiseLinearSinkFlux::BAPiecewiseLinearSinkFlux(const InputParameters & parameters) :
    RichardsPiecewiseLinearSinkFlux(parameters),
    _aux_var(coupledValue("aux_var"))
{
}

Real
BAPiecewiseLinearSinkFlux::computeQpIntegral()
{
  return _aux_var[_qp]*RichardsPiecewiseLinearSinkFlux::computeQpIntegral();
}
