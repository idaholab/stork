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

  MooseEnum method("point inserter", "point");

  params.addRequiredParam<UserObjectName>("user_object", "The name of the CircleAverageMaterialProperty user object");
  params.addRequiredParam<MooseEnum>("method", method, "Method for getting the point. Options are 'point' for a single point and radius, or 'inserter' for a pre-computed value from EventInserter. Default is 'point'.");
  params.addParam<Point>("location", "Point of the center of the circle.");
  params.addParam<Real>("radius", "Distance to search around the specified location.");
  params.addParam<unsigned int>("entry", 0, "Which entry when multiple CircleAverageMaterialProperty values are being computed. Default is 0.");

  return params;
}

CircleAverageMaterialPropertyPPS::CircleAverageMaterialPropertyPPS(const InputParameters & parameters) :
    GeneralPostprocessor(parameters),
    _uo(getUserObject<CircleAverageMaterialProperty>("user_object")),
    _method(getParam<MooseEnum>("method")),
    _p((_method == "point") && (parameters.isParamSetByUser("location")) ? getParam<Point>("location") : 0.0),
    _radius((_method == "point") && (parameters.isParamSetByUser("radius")) ? getParam<Real>("radius") : 0.0),
    _entry((_method == "inserter") && (parameters.isParamSetByUser("entry")) ? getParam<unsigned int>("entry") : 0)
{
  // check input logic
  if (_method == "point")
  {
    if (!parameters.isParamSetByUser("location"))
      mooseError("In CircleAverageMaterialPropertyPPS, user requested to use a point but did not specify a location. Set the 'location' parameter to a Point on the mesh.");
    if (!parameters.isParamSetByUser("radius"))
      mooseError("In CircleAverageMaterialPropertyPPS, user requested to use a point but did not specify a radius. Set the 'radius' parameter.");
  }

  if (_method == "inserter")
    if (!parameters.isParamSetByUser("entry"))
      mooseError("In CircleAverageMaterialPropertyPPS, user requested to use inserter points but did not specify which entry. Set the 'entry' parameter.");
}

CircleAverageMaterialPropertyPPS::~CircleAverageMaterialPropertyPPS()
{
}

Real
CircleAverageMaterialPropertyPPS::getValue()
{
  if (_method == "point")
    return _uo.averageValue(_p, _radius);

  // use inserter points
  return _uo.averageValue(_entry);
}
