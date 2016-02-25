/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/


#ifndef BAPIECEWISELINEARSINK
#define BAPIECEWISELINEARSINK

#include "RichardsPiecewiseLinearSink.h"

// Forward Declarations
class BAPiecewiseLinearSink;

template<>
InputParameters validParams<BAPiecewiseLinearSink>();

/**
 * Multiplies RichardsPiecewiseLinearSink by an aux variable
 */
class BAPiecewiseLinearSink : public RichardsPiecewiseLinearSink
{
public:

  BAPiecewiseLinearSink(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();

  virtual Real computeQpJacobian();

  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

private:

  const VariableValue & _aux_var;

};

#endif //BAPIECEWISELINEARSINK
