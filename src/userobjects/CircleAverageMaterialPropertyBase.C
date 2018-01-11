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

#include "CircleAverageMaterialPropertyBase.h"

template<>
InputParameters validParams<CircleAverageMaterialPropertyBase>()
{
  InputParameters params = validParams<ElementIntegralUserObject>();

  params.addRequiredParam<MaterialPropertyName>("mat_prop", "The name of the material property we are going to use.");
  params.addCoupledVar("periodic_variable", "Use perodic boundary conditions of this variable to determine the distance to the function peak location.");

  // The mapping needs to run at timestep begin, which is after the adaptivity
  // run of the previous timestep.
  params.set<ExecFlagEnum>("execute_on") = EXEC_TIMESTEP_BEGIN;

  return params;
}

CircleAverageMaterialPropertyBase::CircleAverageMaterialPropertyBase(const InputParameters & parameters) :
    ElementIntegralUserObject(parameters),
    _mat_prop(getMaterialProperty<Real>("mat_prop")),
    _periodic_var(isCoupled("periodic_variable") ? (int) coupled("periodic_variable") : -1),
    _mesh(_fe_problem.mesh())
{
}

Real
CircleAverageMaterialPropertyBase::computeQpIntegral()
{
  return _mat_prop[_qp];
}

Real
CircleAverageMaterialPropertyBase::distance(Point p1, Point p2) const
{
  // distance between supplied point and element centroid depends on perodicity
  if (_periodic_var < 0)
    return (p1 - p2).norm();

  return _mesh.minPeriodicDistance(_periodic_var, p1, p2);
}
