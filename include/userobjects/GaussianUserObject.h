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

#ifndef GAUSSIANUSEROBJECT_H
#define GAUSSIANUSEROBJECT_H

// MOOSE includes
#include "GeneralUserObject.h"
#include "Coupleable.h"
#include "MooseMesh.h"

// Forward declarations
class GaussianUserObject;

template<>
InputParameters validParams<GaussianUserObject>();

/**
 * An object for evaluating Gaussian functions
 * Function will integrate to 1 provided sigma << L (width of domain) and mesh density is sufficient
 * Doing this as an *actual* Function proved to be too complicated due to lack of systems available to Functions.
 * Plus now I don't feel so bad about creating custom interaces to get at paramters.
 */
class GaussianUserObject : public GeneralUserObject, public Coupleable
{
public:

  /**
   * Class constructor
   * @param parameters The parameters object holding data for the class to use.
   */
  GaussianUserObject(const InputParameters & parameters);

  /**
   * Class destructor
   */
  virtual ~GaussianUserObject();

  virtual void initialize(){}  // Not used
  virtual void execute(){}  // Not used
  virtual void finalize(){}  // Not used

  /**
   * Return the function value for the given point
   * @param p The point at which to get function value
   * @param center The peak of the Gaussian function
   */
  Real value(const Point & p, const Point & center) const;

  /**
   * Return the function value for the given distance to center
   * @param r The distance to the function center
   */
  Real value(const Real r) const;

  const Real getSigma() const { return _sigma; }

protected:
  /// Value to multiply function by
  const Real _scale;

  /// Standard deviation of distribution
  const Real _sigma;

  /// Variable number for checking periodicity
  const int _periodic_var;

  /// Reference to mesh
  const MooseMesh & _mesh;

  /// Normalization constant
  Real _norm;
};

#endif // GAUSSIANUSEROBJECT_H
