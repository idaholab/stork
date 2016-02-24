/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/


#ifndef BAPIECEWISELINEARSINKFLUX_H
#define BAPIECEWISELINEARSINKFLUX_H

#include "RichardsPiecewiseLinearSinkFlux.h"

//Forward Declarations
class BAPiecewiseLinearSinkFlux;

template<>
InputParameters validParams<BAPiecewiseLinearSinkFlux>();

/**
 * This postprocessor computes the fluid flux to a BAPiecewiseLinearSink.
 */
class BAPiecewiseLinearSinkFlux:
  public RichardsPiecewiseLinearSinkFlux
{
public:
  BAPiecewiseLinearSinkFlux(const InputParameters & parameters);

protected:
  virtual Real computeQpIntegral();

private:

  const VariableValue & _aux_var;


};

#endif
