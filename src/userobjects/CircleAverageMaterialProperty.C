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
#include "EventInserter.h"

#include "libmesh/parallel_algebra.h"

template<>
InputParameters validParams<CircleAverageMaterialProperty>()
{
  InputParameters params = validParams<ElementIntegralUserObject>();
  params.addRequiredParam<MaterialPropertyName>("mat_prop", "the name of the material property we are going to use");
  params.addCoupledVar("periodic_variable", "Use perodic boundary conditions of this variable to determine the distance to the function peak location");
  params.addParam<UserObjectName>("inserter", "Name of the EventInserter UserObject.");
  params.addParam<Real>("radius", "Radius of circle to average material property in.");

  MultiMooseEnum setup_options(SetupInterface::getExecuteOptions());
  // The mapping needs to run at timestep begin, which is after the adaptivity
  // run of the previous timestep. Also run at timestep end to have updated values
  // from EventInserter for Postprocessors. The latter is really just for debugging
  // purposes and could be removed for efficiency.
  setup_options = "timestep_begin timestep_end";
  params.set<MultiMooseEnum>("execute_on") = setup_options;

  return params;
}

CircleAverageMaterialProperty::CircleAverageMaterialProperty(const InputParameters & parameters) :
    ElementIntegralUserObject(parameters),
    _mat_prop(getMaterialProperty<Real>("mat_prop")),
    _periodic_var(isCoupled("periodic_variable") ? (int) coupled("periodic_variable") : -1),
    _use_inserter_points(parameters.isParamSetByUser("inserter")),
    _inserter(NULL),
    _radius((parameters.isParamSetByUser("inserter") && (parameters.isParamSetByUser("radius"))) ? getParam<Real>("radius") : 0.0),
    _mesh(_fe_problem.mesh()),
    _integral_sum(0),
    _volume_sum(0),
    _old_event_list(0),
    _Npoints(0)
{

  if ((_use_inserter_points) && (!parameters.isParamSetByUser("radius")))
    mooseError("In CircleAverageMaterialProperty, when using points supplied by EventInserter, a radius must be set. Set 'radius = <value>'.");
}

void
CircleAverageMaterialProperty::initialSetup()
{
  // Doing this here because all UserObjects will be constructed and order won't matter in the input file
  if (_use_inserter_points)
  {
    _inserter = &getUserObject<EventInserter>("inserter");

    if (!_inserter->areOldEventsBeingTracked())
      mooseError("CircleAverageMaterialProperty cannot be used on inserter points unless old events are being tracked. Set 'track_old_events = true' in EventInserter block.");
  }
}

Real
CircleAverageMaterialProperty::averageValue(const unsigned int i) const
{
  if (i < _volume_sum.size())
    if (_volume_sum[i] > 0.0)
      return _integral_sum[i]/_volume_sum[i];

  return 0.0;
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

Real
CircleAverageMaterialProperty::computeQpIntegral()
{
  return _mat_prop[_qp];
}

void
CircleAverageMaterialProperty::initialize()
{
  if (_use_inserter_points)
  {
    // get old event list to see how many points we are averaging
    _old_event_list = _inserter->getOldEventList();
    _Npoints = _old_event_list.size();

    // resize vectors to number of points
    _integral_sum.resize(_Npoints);
    _volume_sum.resize(_Npoints);

    // initialize values
    for (unsigned int i=0; i < _Npoints; i++)
    {
      _integral_sum[i] = 0.0;
      _volume_sum[i] = 0.0;
    }
  }
  else
  {
    // clear maps of values
    _integral_values.clear();
    _volume_values.clear();
    _centroids.clear();
  }
}

void
CircleAverageMaterialProperty::execute()
{
  if (_use_inserter_points)
  {
    for (unsigned int i=0; i < _Npoints; i++)
    {
      // get distance from event point to element centroid
      Real r = distance(_current_elem->centroid(), _old_event_list[i].second);

      // check if distance between points is less than supplied radius
      // or if point is in element
      if ((r < _radius) || (_current_elem->contains_point(_old_event_list[i].second)))
      {
        _integral_sum[i] += computeIntegral();
        _volume_sum[i] += _current_elem_volume;
      }
    }
  }
  else
  {
    // Store that value
    _integral_values[_current_elem->id()] = computeIntegral();

    // Keep track of the volume of this element
    _volume_values[_current_elem->id()] = _current_elem_volume;

    // Keep track of the centroid of this element
    _centroids[_current_elem->id()] = _current_elem->centroid();
  }
}

void
CircleAverageMaterialProperty::threadJoin(const UserObject & y)
{
  // We are joining with another class like this one so do a cast so we can get to it's data
  const CircleAverageMaterialProperty & uo = dynamic_cast<const CircleAverageMaterialProperty &>(y);

  // put together values from threads
  if (_use_inserter_points)
  {
    for (unsigned int i=0; i < _Npoints; i++)
    {
      _integral_sum[i] += uo._integral_sum[i];
      _volume_sum[i] += uo._volume_sum[i];
    }
  }
  else
  {
    _integral_values.insert(uo._integral_values.begin(), uo._integral_values.end());
    _volume_values.insert(uo._volume_values.begin(), uo._volume_values.end());
    _centroids.insert(uo._centroids.begin(), uo._centroids.end());
  }
}

void
CircleAverageMaterialProperty::finalize()
{
  // put together values from processors
  if (_use_inserter_points)
  {
    for (unsigned int i=0; i < _Npoints; i++)
    {
      gatherSum(_integral_sum[i]);
      gatherSum(_volume_sum[i]);
    }
  }
  else
  {
    _communicator.set_union(_integral_values);
    _communicator.set_union(_volume_values);
    _communicator.set_union(_centroids);
  }
}

Real
CircleAverageMaterialProperty::distance(Point p1, Point p2) const
{
  // distance between supplied point and element centroid depends on perodicity
  if (_periodic_var < 0)
    return (p1 - p2).norm();

  return _mesh.minPeriodicDistance(_periodic_var, p1, p2);
}
