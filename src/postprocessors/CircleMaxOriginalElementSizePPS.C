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

#include "CircleMaxOriginalElementSizePPS.h"

template<>
InputParameters validParams<CircleMaxOriginalElementSizePPS>()
{
  InputParameters params = validParams<GeneralPostprocessor>();

  params.addRequiredParam<UserObjectName>("user_object", "The name of the CircleMaxOriginalElementSize user object");
  params.addRequiredParam<Point>("location", "Point of the center of the circle.");
  params.addRequiredParam<Real>("radius", "Distance to search around the specified location.");

  return params;
}

CircleMaxOriginalElementSizePPS::CircleMaxOriginalElementSizePPS(const InputParameters & parameters) :
    GeneralPostprocessor(parameters),
    _uo(getUserObject<CircleMaxOriginalElementSize>("user_object")),
    _p(getParam<Point>("location")),
    _radius(getParam<Real>("radius"))
{
}

CircleMaxOriginalElementSizePPS::~CircleMaxOriginalElementSizePPS()
{
}

Real
CircleMaxOriginalElementSizePPS::getValue()
{
  return _uo.value(_p, _radius);
}
