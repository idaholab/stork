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

#ifndef CIRCLEAVERAGEMATERIALPROPERTYBASE_H
#define CIRCLEAVERAGEMATERIALPROPERTYBASE_H

#include "ElementIntegralUserObject.h"
#include "MooseMesh.h"

//Forward Declarations
class CircleAverageMaterialPropertyBase;

template<>
InputParameters validParams<CircleAverageMaterialPropertyBase>();

/**
 * Computes the average value of a material property in side a circle
 */
class CircleAverageMaterialPropertyBase : public ElementIntegralUserObject
{
public:
  CircleAverageMaterialPropertyBase(const InputParameters & parameters);

  /**
   * Compute the material property at the quadrature point for the integral
   */
  virtual Real computeQpIntegral();

protected:
  /// Calculate periodic distance between points
  Real distance(Point p1, Point p2) const;

  /// This will hold the material property of interest
  const MaterialProperty<Real> & _mat_prop;

  /// Variable number for checking periodicity
  const int _periodic_var;

  /// Reference to the mesh
  const MooseMesh & _mesh;
};

#endif
