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

  params.addRequiredParam<Point>("peak_location", "Center of Gaussian distribution");
  params.addParam<Real>("scale", 1.0, "Scaling factor");
  params.addParam<Real>("sigma", 1.0, "Standard deviation of the Gaussian curve");
  params.addCoupledVar("periodic_variable", "Use perodic boundary conditions of this variable to determine the distance to the function peak location");

  return params;
}

GaussianFunction::GaussianFunction(const InputParameters & parameters) :
    Function(parameters),
    Coupleable(this, false),
    _peak_location(getParam<Point>("peak_location")),
    _scale(getParam<Real>("scale")),
    _sigma(getParam<Real>("sigma")),
    _periodic_var(isCoupled("periodic_variable") ? (int) coupled("periodic_variable") : -1),
    _mesh(_ti_feproblem.mesh())
{
}

GaussianFunction::~GaussianFunction()
{
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
