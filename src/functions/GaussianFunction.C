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

#include "GaussianFunction.h"
#include "MooseMesh.h"

template<>
InputParameters validParams<GaussianFunction>()
{
  // Define the general parameters
  InputParameters params = validParams<Function>();
  params.addClassDescription("UserObject for generating Gaussian distributions based on the dimensions of the mesh.");

  params.addParam<Real>("scale", 1.0, "Scaling factor");
  params.addParam<Real>("sigma", 1.0, "Standard deviation of the Gaussian curve");
  params.addParam<Point>("peak_location", "Center of Gaussian distribution");
  params.addParam<bool>("use_random_point", false, "Use a random point for the Gaussian peak location");
  params.addParam<UserObjectName>("random_point_user_object", "Name of the RandomPointUserObject to use");
  params.addCoupledVar("periodic_variable", "Use perodic boundary conditions of this variable to determine the distance to the function peak location");

  return params;
}

GaussianFunction::GaussianFunction(const InputParameters & parameters) :
    Function(parameters),
    Coupleable(this, false),
    _scale(getParam<Real>("scale")),
    _sigma(getParam<Real>("sigma")),
    _use_random(getParam<bool>("use_random_point")),
    _random_point_user_object_ptr(NULL),
    _periodic_var(isCoupled("periodic_variable") ? (int) coupled("periodic_variable") : -1),
    _mesh(_ti_feproblem.mesh()),
    _peak_location(0),
    _use_center(false)
{
  // check for correct input
  if ((parameters.isParamSetByUser("peak_location")) && (parameters.isParamSetByUser("use_random_point")))
    mooseError("Both 'peak_location' and 'use_random_point' are set. Can only do one or the other.");

  if ((_use_random) && (!parameters.isParamSetByUser("random_point_user_object")))
    mooseError("The parameter 'random_point_user_object' needs to be set.");

  if (parameters.isParamSetByUser("peak_location"))
    _peak_location = getParam<Point>("peak_location");

  // If all else fails, put peak in the center of domain
  if ((!parameters.isParamSetByUser("peak_location")) && (!parameters.isParamSetByUser("use_random_point")))
    _use_center = true;
}

GaussianFunction::~GaussianFunction()
{
}

void
GaussianFunction::initialSetup()
{
  // pick a point if not already specified
  if (_use_center) // use "center" of domain, assuming it is rectangular
  {
    Real x_low = _mesh.getMinInDimension(0);
    Real y_low = _mesh.getMinInDimension(1);
    Real z_low = _mesh.getMinInDimension(2);

    Real x_width = _mesh.dimensionWidth(0);
    Real y_width = _mesh.dimensionWidth(1);
    Real z_width = _mesh.dimensionWidth(2);

    Real x_center = x_low + x_width/2.0;
    Real y_center = y_low + y_width/2.0;
    Real z_center = z_low + z_width/2.0;

    _peak_location = Point(x_center, y_center, z_center);
  }
  else if (_use_random)
  {
    // Using a pointer because apparently user objects are not available during function construction
    _random_point_user_object_ptr = &getUserObject<RandomPointUserObject>("random_point_user_object");
    _peak_location = _random_point_user_object_ptr->getRandomPoint();
  }
}

Real
GaussianFunction::value(Real /*t*/, const Point & p)
{
  return value(p, _peak_location);
}

Real
GaussianFunction::value(const Point & p, const Point & center)
{
  // normalization constant so integral over domain (theoretically) equals 1
  unsigned int dim = _mesh.dimension();
  Real norm = 1.0/(pow(_sigma,dim)*pow(pow(2.0*pi,dim),0.5));

  // distance to center depends on perodicity
  Real r;
  if (_periodic_var < 0)
    r = (p-center).norm();
  else
    r = _mesh.minPeriodicDistance(_periodic_var, p, center);

  Real f = exp(-r*r/2.0/_sigma/_sigma);

  return f*norm*_scale;
}
