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

#ifndef CIRCLEAVERAGEMATERIALPROPERTY_H
#define CIRCLEAVERAGEMATERIALPROPERTY_H

#include "ElementIntegralUserObject.h"
#include "MooseMesh.h"

//Forward Declarations
class CircleAverageMaterialProperty;

template<>
InputParameters validParams<CircleAverageMaterialProperty>();

/**
 * Computes the average value of a material property in side a circle
 */
class CircleAverageMaterialProperty : public ElementIntegralUserObject
{
public:
  CircleAverageMaterialProperty(const InputParameters & parameters);

  /**
   * Given a Point and a radius, return the average value for a material property in that circle
   *
   * Note that accessor functions on UserObjects like this _must_ be const.
   * That is because the UserObject system returns const references to objects
   * trying to use UserObjects.  This is done for parallel correctness.
   *
   * @return The average value of a material property in that circle
   */
  Real averageValue(const Point & p, const Real & radius) const;

  /**
   * Compute the material property at the quadrature point for the integral
   */
  virtual Real computeQpIntegral();

  /**
   * This is called before execute so you can reset any internal data.
   */
  virtual void initialize();

  /**
   * Called on every "object" (like every element or node).
   * In this case, it is called at every quadrature point on every element.
   */
  virtual void execute();

  /**
   * Called when using threading.  You need to combine the data from "y"
   * into _this_ object.
   */
  virtual void threadJoin(const UserObject & y);

  /**
   * Called _once_ after execute has been called all all "objects".
   */
  virtual void finalize();

protected:
  /// This map will hold the integral of each element
  std::map<dof_id_type, Real> _integral_values;

  /// This map will hold the volume of each element
  std::map<dof_id_type, Real> _volume_values;

  /// This map will hold the centroid of each element
  std::map<dof_id_type, Point> _centroids;

  /// This will hold the material property of interest
  const MaterialProperty<Real> & _mat_prop;

  /// Variable number for checking periodicity
  const int _periodic_var;

  /// Reference to the mesh
  const MooseMesh & _mesh;
};

#endif
