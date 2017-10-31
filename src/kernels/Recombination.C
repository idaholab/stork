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

#include "Recombination.h"

template <> InputParameters validParams<Recombination>()
{
  InputParameters params = validParams<Kernel>();

  params.addRequiredCoupledVar("other_variable", "The coupled variable.");

  return params;
}

Recombination::Recombination(const InputParameters & parameters) :
    Kernel(parameters),
    _v_var(coupled("other_variable")),
    _v(coupledValue("other_variable"))
{
}

Real
Recombination::computeQpResidual()
{
  return _test[_i][_qp] * (_v[_qp] * _u[_qp]);
}

Real
Recombination::computeQpJacobian()
{
  return _test[_i][_qp] * (_v[_qp] * _phi[_j][_qp]);
}

Real
Recombination::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _v_var)
    return _test[_i][_qp] * (_u[_qp] * _phi[_j][_qp]);

  // else
  return 0.0;
}
