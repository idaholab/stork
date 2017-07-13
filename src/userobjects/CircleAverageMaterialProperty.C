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

#include "CircleAverageMaterialProperty.h"

#include "libmesh/parallel_algebra.h"

template<>
InputParameters validParams<CircleAverageMaterialProperty>()
{
  InputParameters params = validParams<CircleAverageMaterialPropertyBase>();

  return params;
}

CircleAverageMaterialProperty::CircleAverageMaterialProperty(const InputParameters & parameters) :
    CircleAverageMaterialPropertyBase(parameters)
{
}

Real
CircleAverageMaterialProperty::averageValue(const Point & p, const Real & radius) const
{
  // Note that we can't use operator[] for a std::map in a const function!

  Real integral_sum = 0.0;
  Real volume_sum = 0.0;

  Real min_distance = std::numeric_limits<Real>::max();
  dof_id_type closest_element;

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
      if ((_integral_values.find(id) == _integral_values.end()) || (_volume_values.find(id) == _volume_values.end()))
        mooseError("In CircleAverageMaterialProperty, element id ", id, " not found.");
      else
      {
        // c++11!!
        integral_sum += _integral_values.at(id);
        volume_sum += _volume_values.at(id);
      }
    }
  }

  if (volume_sum > 0.0)
    return integral_sum/volume_sum;
  else
    return _integral_values.at(closest_element)/_volume_values.at(closest_element);  // return at least the information of the closest element

  return 0.0;
}

void
CircleAverageMaterialProperty::initialize()
{
  // clear maps of values
  _integral_values.clear();
  _volume_values.clear();
  _centroids.clear();
}

void
CircleAverageMaterialProperty::execute()
{
  // Store the integral of the material property over this element
  _integral_values[_current_elem->id()] = computeIntegral();

  // Keep track of the volume of this element
  _volume_values[_current_elem->id()] = _current_elem_volume;

  // Keep track of the centroid of this element
  _centroids[_current_elem->id()] = _current_elem->centroid();
}

void
CircleAverageMaterialProperty::threadJoin(const UserObject & y)
{
  // We are joining with another class like this one so do a cast so we can get to it's data
  const CircleAverageMaterialProperty & uo = dynamic_cast<const CircleAverageMaterialProperty &>(y);

  _integral_values.insert(uo._integral_values.begin(), uo._integral_values.end());
  _volume_values.insert(uo._volume_values.begin(), uo._volume_values.end());
  _centroids.insert(uo._centroids.begin(), uo._centroids.end());
}

void
CircleAverageMaterialProperty::finalize()
{
  // put together values from processors
  _communicator.set_union(_integral_values);
  _communicator.set_union(_volume_values);
  _communicator.set_union(_centroids);
}
