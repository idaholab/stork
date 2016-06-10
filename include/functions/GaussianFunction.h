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
#include "RandomPointUserObject.h"

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

  virtual void initialSetup();

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
  const Real _scale;

  /// Standard deviation of distribution
  const Real _sigma;

  /// Flag to use random point as Gaussian center
  const bool _use_random;

  /// Pointer to RandomPointUserObject
  const RandomPointUserObject * _random_point_user_object_ptr;

  /// Variable number for checking periodicity
  const int _periodic_var;

  /// Reference to mesh
  const MooseMesh & _mesh;

  /// Peak location of Gaussian Function
  Point _peak_location;

  /// Flag to use to Gaussian peak at center of domain
  bool _use_center;
};

#endif // GAUSSIANFUNCTION_H
