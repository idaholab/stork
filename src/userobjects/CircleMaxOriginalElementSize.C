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

#include "CircleMaxOriginalElementSize.h"

#include "libmesh/parallel_algebra.h"

template<>
InputParameters validParams<CircleMaxOriginalElementSize>()
{
  InputParameters params = validParams<ElementUserObject>();
  params.addCoupledVar("periodic_variable", "Use perodic boundary conditions of this variable to determine the distance to the function peak location");

  MultiMooseEnum setup_options(SetupInterface::getExecuteOptions());
  // the mapping needs to run at timestep begin, which is after the adaptivity
  // run of the previous timestep.
  setup_options = "timestep_begin";
  params.set<MultiMooseEnum>("execute_on") = setup_options;

  return params;
}

CircleMaxOriginalElementSize::CircleMaxOriginalElementSize(const InputParameters & parameters) :
    ElementUserObject(parameters),
    _periodic_var(isCoupled("periodic_variable") ? (int) coupled("periodic_variable") : -1),
    _mesh_changed(true),
    _rebuild_map(true),
    _mesh(_fe_problem.mesh())
{
}

Real
CircleMaxOriginalElementSize::value(const Point & p, const Real & radius) const
{
  Real max_element_size = 0.0;

  Real min_distance = std::numeric_limits<Real>::max();
  dof_id_type closest_element;

  // Loop over elements
  for (std::map<dof_id_type, Point>::const_iterator it = _centroids.begin();
      it != _centroids.end();
      ++it)
  {
    dof_id_type id = it->first;
    Point centroid = it->second;

    Real r = distance(p, centroid);

    // save closest element in case mesh too coarse
    if (r < min_distance)
    {
      min_distance = r;
      closest_element = id;
    }

    // check if distance between points is less than supplied radius
    if (r < radius)
    {
      if (_original_element_sizes.find(id) == _original_element_sizes.end())
        mooseError2("In CircleMaxOriginalElementSize, element id ", id, " not found.");
      else
      {
        Real element_size = _original_element_sizes.at(id);
        if (element_size > max_element_size)
          max_element_size = element_size;
      }
    }
  }

  if (min_distance == std::numeric_limits<Real>::max())
    return _original_element_sizes.at(closest_element);

  return max_element_size;
}

void
CircleMaxOriginalElementSize::initialize()
{
  if (_mesh_changed)
  {
    _rebuild_map = true;
    // clear maps of values
    _original_element_sizes.clear();
    _centroids.clear();
  }
  else
    _rebuild_map = false;

  _mesh_changed = false;
}

void
CircleMaxOriginalElementSize::execute()
{
  if (_rebuild_map)
  {
    // Get pointer to original element of this (possibly) refined element
    const Elem * original_element = _current_elem->top_parent();

    // Store the parent element size value
    _original_element_sizes[_current_elem->id()] = original_element->hmax();

    // Store the parent element centroid
    _centroids[_current_elem->id()] = original_element->centroid();
  }
}

void
CircleMaxOriginalElementSize::threadJoin(const UserObject & y)
{
  if (_rebuild_map)
  {
    // We are joining with another class like this one so do a cast so we can get to it's data
    const CircleMaxOriginalElementSize & uo = dynamic_cast<const CircleMaxOriginalElementSize &>(y);

    _original_element_sizes.insert(uo._original_element_sizes.begin(), uo._original_element_sizes.end());
    _centroids.insert(uo._centroids.begin(), uo._centroids.end());
  }
}

void
CircleMaxOriginalElementSize::finalize()
{
  if (_rebuild_map)
  {
    _communicator.set_union(_original_element_sizes);
    _communicator.set_union(_centroids);
  }
}

Real
CircleMaxOriginalElementSize::distance(Point p1, Point p2) const
{
  // distance between supplied point and element centroid depends on perodicity
  if (_periodic_var < 0)
    return (p1 - p2).norm();

  return _mesh.minPeriodicDistance(_periodic_var, p1, p2);
}

void
CircleMaxOriginalElementSize::meshChanged()
{
  _mesh_changed = true;
}

