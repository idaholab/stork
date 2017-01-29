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

#ifndef CIRCLEMAXELEMENTSIZE_H
#define CIRCLEMAXELEMENTSIZE_H

#include "ElementUserObject.h"
#include "MooseMesh.h"

//Forward Declarations
class CircleMaxOriginalElementSize;

template<>
InputParameters validParams<CircleMaxOriginalElementSize>();

/**
 * Computes the max element size inside a circle on the *original* mesh
 */
class CircleMaxOriginalElementSize : public ElementUserObject
{
public:
  CircleMaxOriginalElementSize(const InputParameters & parameters);

  /**
   * Given a Point and a radius, return the max element size inside a circle on
   * the *original* mesh
   *
   * Note that accessor functions on UserObjects like this _must_ be const.
   * That is because the UserObject system returns const references to objects
   * trying to use UserObjects.  This is done for parallel correctness.
   *
   * @return The max element size inside a circle on the *original* mesh
   */
  Real value(const Point & p, const Real & radius) const;

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

  virtual void meshChanged();

protected:
  /// Calculate periodic distance between points
  Real distance(Point p1, Point p2) const;

  /// Variable number for checking periodicity
  const int _periodic_var;

  /// Flag for changed mesh
  bool _mesh_changed;

  /// Flag to re-calculate maps
  bool _rebuild_map;

  /// Reference to the mesh
  const MooseMesh & _mesh;

  /// This map will hold the element sizes
  std::map<dof_id_type, Real> _original_element_sizes;

  /// This map will hold the centroid of each element
  std::map<dof_id_type, Point> _centroids;
};

#endif
