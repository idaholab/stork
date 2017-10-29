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

#include "GaussianUserObject.h"
#include "RandomPointUserObject.h"

template<>
InputParameters validParams<GaussianUserObject>()
{
  // Define the general parameters
  InputParameters params = validParams<GeneralUserObject>();
  params.addClassDescription("Function for generating Gaussian distributions normalized to the mesh dimension.");

  params.addParam<Real>("scale", 1.0, "Scaling factor");
  params.addParam<Real>("sigma", 1.0, "Standard deviation of the Gaussian curve");
  params.addParam<Point>("peak_location", 0.0, "Center of Gaussian distribution");
  params.addParam<bool>("use_random_point", false, "Use a random point for the Gaussian peak location");
  params.addParam<UserObjectName>("random_point_user_object", "Name of the RandomPointUserObject to use");
  params.addCoupledVar("periodic_variable", "Use perodic boundary conditions of this variable to determine the distance to the function peak location");

  return params;
}

GaussianUserObject::GaussianUserObject(const InputParameters & parameters) :
    GeneralUserObject(parameters),
    Coupleable(this, false),
    _scale(getParam<Real>("scale")),
    _sigma(getParam<Real>("sigma")),
    _use_random(getParam<bool>("use_random_point")),
    _periodic_var(isCoupled("periodic_variable") ? (int) coupled("periodic_variable") : -1),
    _peak_location(getParam<Point>("peak_location")),
    _mesh(_fe_problem.mesh())
{
  // normalization constant so integral over domain (theoretically) equals 1
  unsigned int dim = _mesh.dimension();
  _norm = 1.0/std::pow(_sigma*std::sqrt(2.0*libMesh::pi), Real(dim));

  // check for correct input
  if ((parameters.isParamSetByUser("peak_location")) && (parameters.isParamSetByUser("use_random_point")))
    mooseError("In GaussianUserObject, both 'peak_location' and 'use_random_point' are set. Can only do one or the other.");

  if ((_use_random) && (!parameters.isParamSetByUser("random_point_user_object")))
    mooseError("In GaussianUserObject, the parameter 'random_point_user_object' needs to be set to use random points.");
}

void
GaussianUserObject::initialSetup()
{
  if (_use_random)
  {
    const RandomPointUserObject * _random_point_user_object_ptr = &getUserObject<RandomPointUserObject>("random_point_user_object");
    _peak_location = _random_point_user_object_ptr->getRandomPoint();
  }
}

Real
GaussianUserObject::value(const Point & p, const Point & center) const
{
  // distance to center depends on perodicity
  Real r;
  if (_periodic_var < 0)
    r = (p-center).norm();
  else
    r = _mesh.minPeriodicDistance(_periodic_var, p, center);

  Real f = exp(-r*r/2.0/_sigma/_sigma);

  return f*_norm*_scale;
}

Real
GaussianUserObject::value(const Real r) const
{
  Real f = exp(-r*r/2.0/_sigma/_sigma);

  return f*_norm*_scale;
}

Real
GaussianUserObject::spatialValue(const Point & p) const
{
  return value(p, _peak_location);
}
