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

#ifndef GAUSSIANFUNCTION_H
#define GAUSSIANFUNCTION_H

// MOOSE includes
#include "Function.h"
#include "Coupleable.h"
#include "MooseMesh.h"

// Forward declarations
class GaussianFunction;

template<>
InputParameters validParams<GaussianFunction>();

/**
 * A function generating Gaussian functions in any dimension.
 * Function will integrate to 1 provided sigma << L (width of domain) and mesh density is sufficient
 */
class GaussianFunction :
  public Function,
  public Coupleable
{
public:

  /**
   * Class constructor
   * @param parameters The parameters object holding data for the class to use.
   */
  GaussianFunction(const InputParameters & parameters);

  /**
   * Class destructor
   */
  virtual ~GaussianFunction();

  /**
   * Return the function value for the given point
   * @param t Time (unused)
   * @param p The point at which to get function value
   */
  virtual Real value(Real t, const Point & p);

  /**
   * Return the function value for the given point
   * @param p The point at which to get function value
   * @param center The peak of the Gaussian function
   */
  virtual Real value(const Point & p, const Point & center);

protected:
  /// Value to multiply function by
  Real _scale;

  /// Standard deviation of distribution
  Real _sigma;

  /// Variable number for checking periodicity
  int _periodic_var;

  /// Reference to mesh
  MooseMesh & _mesh;

  /// Problem dimension, based on MooseMesh::dimension()
  unsigned int _dim;

  /// Peak location of Gaussian Function
  Point _center;

};

#endif // GAUSSIANFUNCTION_H
