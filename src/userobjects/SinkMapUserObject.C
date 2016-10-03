/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "SinkMapUserObject.h"
#include "MooseMesh.h"
#include "GaussianUserObject.h"

// libmesh includes
#include "libmesh/quadrature.h"

template<>
InputParameters validParams<SinkMapUserObject>()
{
  MooseEnum sink_placement("corner inside", "corner");
  MooseEnum sink_shape_3d("spheres lines", "lines");

  InputParameters params = validParams<ElementUserObject>();
  params.addRequiredParam<Real>("spacing", "Distance to space sink centers");
  params.addRequiredParam<Real>("strength", "Average strength of the overall sink map.");
  params.addRequiredParam<UserObjectName>("gaussian_user_object", "Name of the GaussianUserObject to use for sink shapes.");
  params.addParam<MooseEnum>("sink_placement", sink_placement, "How to place sinks on domain, choices are 'corner' to place them in the corners (and wrap around), and 'inside' to keep them away from edges.");
  params.addParam<MooseEnum>("sink_shape_3d", sink_shape_3d, "Shape to use for sinks in 3D, choices are 'spheres' or 'lines'.");
  params.addCoupledVar("periodic_variable", "Use the periodicity settings of this variable to populate the sink map");

  MultiMooseEnum setup_options(SetupInterface::getExecuteOptions());
  // the mapping needs to run at timestep begin, which is after the adaptivity
  // run of the previous timestep.
  setup_options = "timestep_begin";
  params.set<MultiMooseEnum>("execute_on") = setup_options;
  return params;
}

SinkMapUserObject::SinkMapUserObject(const InputParameters & parameters) :
    ElementUserObject(parameters),
    _spacing(getParam<Real>("spacing")),
    _strength(getParam<Real>("strength")),
    _gaussian_user_object_ptr(NULL),
    _sink_placement(getParam<MooseEnum>("sink_placement")),
    _sink_shape_3d(getParam<MooseEnum>("sink_shape_3d")),
    _periodic_var(isCoupled("periodic_variable") ? coupled("periodic_variable") : -1),
    _dim(_mesh.dimension()),
    _mesh_changed(true)
{
  _zero_map.assign(_fe_problem.getMaxQps(), 0.0);
}

void
SinkMapUserObject::initialSetup()
{
  // get sink sigma
  _gaussian_user_object_ptr = &getUserObject<GaussianUserObject>("gaussian_user_object");
  _sigma = _gaussian_user_object_ptr->getSigma();

  // calculate sink locations
  if (_sink_placement == "corner")
  {
    // yes this is dumb, but it works
    for (Real x_center = _mesh.getMinInDimension(0); x_center <= _mesh.getMinInDimension(0) + _mesh.dimensionWidth(0); x_center += _spacing)
      for (Real y_center = _mesh.getMinInDimension(1); y_center <= _mesh.getMinInDimension(1) + _mesh.dimensionWidth(1); y_center += _spacing)
        if ((_dim == 3) && (_sink_shape_3d == "spheres"))
          for (Real z_center = _mesh.getMinInDimension(2); z_center <= _mesh.getMinInDimension(2) + _mesh.dimensionWidth(2); z_center += _spacing)
            _sink_location_list.push_back(Point(x_center, y_center, z_center));
        else
          _sink_location_list.push_back(Point(x_center, y_center, 0.0));
  }
  else  // centered sink placement
  {
    if (_dim == 1)
      for (Real x_center = _mesh.getMinInDimension(0) + _spacing/2.0; x_center <= _mesh.getMinInDimension(0) + _mesh.dimensionWidth(0) - _spacing/2.0; x_center += _spacing)
        _sink_location_list.push_back(Point(x_center, 0.0, 0.0));
    else
      for (Real x_center = _mesh.getMinInDimension(0) + _spacing/2.0; x_center <= _mesh.getMinInDimension(0) + _mesh.dimensionWidth(0) - _spacing/2.0; x_center += _spacing)
        for (Real y_center = _mesh.getMinInDimension(1) + _spacing/2.0; y_center <= _mesh.getMinInDimension(1) + _mesh.dimensionWidth(1) - _spacing/2.0; y_center += _spacing)
          if ((_dim == 3) && (_sink_shape_3d == "spheres"))
            for (Real z_center = _mesh.getMinInDimension(2) + _spacing/2.0; z_center <= _mesh.getMinInDimension(2) + _mesh.dimensionWidth(2) - _spacing/2.0; z_center += _spacing)
              _sink_location_list.push_back(Point(x_center, y_center, z_center));
          else
            _sink_location_list.push_back(Point(x_center, y_center, 0.0));
  }

  // compute normalization constant
  _norm = _strength*std::pow(_spacing, (double) _dim);

  // normalization constant from GaussianUserObject has an additional 1/(sigma*sqrt(2*pi)) since mesh is in 3D but lines are in 2D
  if ((_dim == 3) && (_sink_shape_3d == "lines"))
    _norm *= _sigma*std::sqrt(2.0*libMesh::pi);

  // print sink centers
  for (unsigned int i=0; i<_sink_location_list.size(); i++)
    _console << _sink_location_list[i] << std::endl;
}

void
SinkMapUserObject::initialize()
{
  if (_mesh_changed)
  {
    _rebuild_map = true;
    _sink_strength_map.clear();
  }
  else
    _rebuild_map = false;

  _mesh_changed = false;
}

void
SinkMapUserObject::execute()
{
  if (_rebuild_map)
  {
    // reserve space for each quadrature point in the element
    _elem_map.assign(_qrule->n_points(), 0);

    // loop over quadrature points in this element
    for (unsigned int qp = 0; qp < _qrule->n_points(); ++qp)
    {
      // find distance to nearest sink from this point
      Real rmin = getDistanceToNearestSink(_q_point[qp]);

      // compute sink strength at this location
      _elem_map[qp] = _norm*_gaussian_user_object_ptr->value(rmin);
    }

    // insert vector into map
    _sink_strength_map.insert(std::pair<dof_id_type, std::vector<Real> >(_current_elem->id(), _elem_map));
  }
}

void
SinkMapUserObject::threadJoin(const UserObject &y)
{
  // if the map needs to be updated we merge the maps from all threads
  if (_rebuild_map)
  {
    const SinkMapUserObject & uo = static_cast<const SinkMapUserObject &>(y);
    _sink_strength_map.insert(uo._sink_strength_map.begin(), uo._sink_strength_map.end());
  }
}

void
SinkMapUserObject::meshChanged()
{
  _mesh_changed = true;
}

const std::vector<Real> &
SinkMapUserObject::getLocalSinkMap(const Elem * elem) const
{
  SinkMap::const_iterator i = _sink_strength_map.find(elem->id());

  // if no entry in the map was found then something went wrong
  if (i == _sink_strength_map.end())
    mooseError("no sinks found in element " << elem->id());

  return i->second;
}

Real
SinkMapUserObject::getDistanceToNearestSink(const Point & p) const
{
  Real r, rmin = std::numeric_limits<Real>::max();

  // to do 3D lines, need to change z-component of point to that of a sink
  // so the distance to the line is based on the xy-plane distance
  Point new_p;
  if ((_dim == 3) && (_sink_shape_3d == "lines"))
    new_p = Point(p(0), p(1), 0.0);
  else
    new_p = p;

  // find the distance to the closest sink location
  for (unsigned i = 0; i < _sink_location_list.size(); ++i)
  {
    // use a non-periodic or periodic distance
    r = _periodic_var < 0 ?
          (new_p - _sink_location_list[i]).norm() :
          _mesh.minPeriodicDistance(_periodic_var, new_p, _sink_location_list[i]);
    if (r < rmin)
      rmin = r;
  }
  return rmin;
}
