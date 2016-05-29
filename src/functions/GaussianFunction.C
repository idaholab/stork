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
  params.addClassDescription("Function for generating Gaussian distributions based on the dimensions of the mesh.");

  params.addParam<Real>("scale", 1.0, "Scaling factor");
  params.addParam<Real>("sigma", 1.0, "Standard deviation of the Gaussian curve");
  params.addParam<Point>("center", "Center of Gaussian distribution");
  params.addCoupledVar("periodic_variable", "Use perodic boundary conditions of this variable to determine the distance to the function center");

  return params;
}

GaussianFunction::GaussianFunction(const InputParameters & parameters) :
    Function(parameters),
    Coupleable(this, false),
    _scale(getParam<Real>("scale")),
    _sigma(getParam<Real>("sigma")),
    _periodic_var(isCoupled("periodic_variable") ? (int) coupled("periodic_variable") : -1),
    _mesh(_ti_feproblem.mesh()),
    _dim(_mesh.dimension())
{
  _console << "*** GaussianFunction constructor: " << std::endl;
  if (parameters.isParamSetByUser("center"))
    _center = getParam<Point>("center");
  else // put center of distribution at center of domain
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

    _center = Point(x_center, y_center, z_center);
  }
}

GaussianFunction::~GaussianFunction()
{
}

Real
GaussianFunction::value(Real /*t*/, const Point & p)
{
  return value(p, _center);
}

Real
GaussianFunction::value(const Point & p, const Point & center)
{
  // normalization constant so integral over domain (theoretically) equals 1
  Real norm = 1.0/(pow(_sigma,_dim)*pow(pow(2.0*pi,_dim),0.5));

  // distance to center depends on perodicity
  Real r;
  if (_periodic_var < 0)
    r = (p-center).norm();
  else
    r = _mesh.minPeriodicDistance(_periodic_var, p, center);

  Real f = exp(-r*r/2.0/_sigma/_sigma);

  return f*norm*_scale;
}

