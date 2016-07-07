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
  InputParameters params = validParams<ElementIntegralUserObject>();
  params.addRequiredParam<MaterialPropertyName>("mat_prop", "the name of the material property we are going to use");
  params.addCoupledVar("periodic_variable", "Use perodic boundary conditions of this variable to determine the distance to the function peak location");

  return params;
}

CircleAverageMaterialProperty::CircleAverageMaterialProperty(const InputParameters & parameters) :
    ElementIntegralUserObject(parameters),
    _mat_prop(getMaterialProperty<Real>("mat_prop")),
    _periodic_var(isCoupled("periodic_variable") ? (int) coupled("periodic_variable") : -1),
    _mesh(_fe_problem.mesh())
{
}

Real
CircleAverageMaterialProperty::averageValue(const Point & p, const Real & radius) const
{
  // Note that we can't use operator[] for a std::map in a const function!

  Real integral_sum = 0.0;
  Real volume_sum = 0.0;

  for (std::map<dof_id_type, Point>::const_iterator it = _centroids.begin();
      it != _centroids.end();
      ++it)
  {
    dof_id_type id = it->first;
    Point centroid = it->second;

    // distance between supplied point and element centroid depends on perodicity
    Real r;
    if (_periodic_var < 0)
      r = (p-centroid).norm();
    else
      r = _mesh.minPeriodicDistance(_periodic_var, p, centroid);

    // check if distance between points is less than supplied radius
    if (r < radius)
    {
      integral_sum += _integral_values.find(id)->second;
      volume_sum += _volume_values.find(id)->second;
    }
  }

  if (volume_sum > 0.0)
    return integral_sum/volume_sum;

  return 0.0;
}

Real
CircleAverageMaterialProperty::computeQpIntegral()
{
  return _mat_prop[_qp];
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
  // Compute the integral on this element
  Real integral_value = computeIntegral();

  // Store that value
  _integral_values[_current_elem->id()] = integral_value;

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

  for (std::map<dof_id_type, Real>::const_iterator it = uo._integral_values.begin();
      it != uo._integral_values.end();
      ++it)
    _integral_values[it->first] += it->second;

  for (std::map<dof_id_type, Real>::const_iterator it = uo._volume_values.begin();
      it != uo._volume_values.end();
      ++it)
    _volume_values[it->first] += it->second;

  _centroids.insert(uo._centroids.begin(), uo._centroids.end());
}

void
CircleAverageMaterialProperty::finalize()
{
  // Loop over the integral values and sum them up over the processors
  for (std::map<dof_id_type, Real>::iterator it = _integral_values.begin();
      it != _integral_values.end();
      ++it)
    gatherSum(it->second);

  // Loop over the volume_values and sum them up over the processors
  for (std::map<dof_id_type, Real>::iterator it = _volume_values.begin();
      it != _volume_values.end();
      ++it)
    gatherSum(it->second);

  // Join up centroid values over processors
  _communicator.set_union(_centroids);
}
