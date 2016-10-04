/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#ifndef SINKMAPUSEROBJECT_H
#define SINKMAPUSEROBJECT_H

#include "ElementUserObject.h"

class SinkMapUserObject;
class GaussianUserObject;

template<>
InputParameters validParams<SinkMapUserObject>();

/**
 * This UserObject maintains a per QP map that holds the sink strength.
 */
class SinkMapUserObject : public ElementUserObject
{
public:
  SinkMapUserObject(const InputParameters & parameters);

  virtual void initialSetup();
  virtual void initialize();
  virtual void execute();
  virtual void threadJoin(const UserObject & y);
  virtual void finalize() {}

  virtual void meshChanged();

  const std::vector<Real> & getLocalSinkMap(const Elem *) const;

  std::vector<Point> getSinkLocationList () const { return _sink_location_list; }

  Real getDistanceToNearestSink(const Point & p) const;

  Real getStrength() const { return _strength; }

  const MooseEnum & getPlacement() const { return _sink_placement; }

protected:
  /// Distance between sink centers
  const Real _spacing;

  /// Average overall strength of entire map
  const Real _strength;

  /// Pointer to GaussianUserObject
  const GaussianUserObject * _gaussian_user_object_ptr;

  /// How to place sinks on domain, either "corner" or "inside"
  const MooseEnum _sink_placement;

  /// Shape to use for sinks when running a 3D problem, either "spheres" or "lines"
  const MooseEnum _sink_shape_3d;

  /// variable number to use for minPeriodicDistance calls (i.e. use the periodicity of this variable)
  int _periodic_var;

  /// holds the dimension of the mesh
  const unsigned int _dim;

  /// Did the mesh change since the last execution of this User Object?
  bool _mesh_changed;

  /// Do we need to rebuild the map during this timestep?
  bool _rebuild_map;

  /// Buffer for building the per QP map
  std::vector<Real> _elem_map;

  /// Dummy map for elements without nuclei
  std::vector<Real> _zero_map;

  /// list of sink locations
  std::vector<Point>  _sink_location_list;

  ///@{
  /// Per element list of sink strength at qp's
  typedef LIBMESH_BEST_UNORDERED_MAP<dof_id_type, std::vector<Real> > SinkMap;
  SinkMap _sink_strength_map;
  ///@}

  /// Sigma of sink Gaussian
  Real _sigma;

  /// Normalization constant for spaced sinks
  Real _norm;
};

#endif //SINKMAPUSEROBJECT_H
