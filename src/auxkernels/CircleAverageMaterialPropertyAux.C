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

#include "CircleAverageMaterialPropertyAux.h"
#include "CircleAverageMaterialProperty.h"

template<>
InputParameters validParams<CircleAverageMaterialPropertyAux>()
{
  InputParameters params = validParams<AuxKernel>();

  params.addRequiredParam<UserObjectName>("circle_average_material_property_user_object", "The CircleAverageMaterialProperty to retrieve values from.");
  params.addRequiredParam<Real>("radius", "Distance to search around for material properties.");

  return params;
}

CircleAverageMaterialPropertyAux::CircleAverageMaterialPropertyAux(const InputParameters & parameters) :
    AuxKernel(parameters),
    _uo(getUserObject<CircleAverageMaterialProperty>("circle_average_material_property_user_object")),
    _radius(getParam<Real>("radius"))
{
}

Real
CircleAverageMaterialPropertyAux::computeValue()
{
  return _uo.averageValue(_q_point[_qp], _radius);
}
