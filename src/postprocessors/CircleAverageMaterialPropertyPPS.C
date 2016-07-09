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

#include "CircleAverageMaterialPropertyPPS.h"

template<>
InputParameters validParams<CircleAverageMaterialPropertyPPS>()
{
  InputParameters params = validParams<GeneralPostprocessor>();
  params.addRequiredParam<UserObjectName>("user_object", "The name of the user object");
  params.addRequiredParam<Point>("location", "Point of the center of the circle.");
  params.addRequiredParam<Real>("radius", "Distance to search around the specified location.");

  return params;
}

CircleAverageMaterialPropertyPPS::CircleAverageMaterialPropertyPPS(const InputParameters & parameters) :
    GeneralPostprocessor(parameters),
    _uo(getUserObject<CircleAverageMaterialProperty>("user_object")),
    _p(getParam<Point>("location")),
    _radius(getParam<Real>("radius")),
    _value(0.)
{
}

CircleAverageMaterialPropertyPPS::~CircleAverageMaterialPropertyPPS()
{
}

void
CircleAverageMaterialPropertyPPS::initialize()
{
  _value = 0;
}

void
CircleAverageMaterialPropertyPPS::execute()
{
  _value = _uo.averageValue(_p, _radius);
}

Real
CircleAverageMaterialPropertyPPS::getValue()
{
  return _value;
}
