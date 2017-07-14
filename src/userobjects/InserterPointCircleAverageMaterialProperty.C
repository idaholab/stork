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

#include "InserterPointCircleAverageMaterialProperty.h"
#include "EventInserterBase.h"

#include "libmesh/parallel_algebra.h"

template<>
InputParameters validParams<InserterPointCircleAverageMaterialProperty>()
{
  InputParameters params = validParams<CircleAverageMaterialPropertyBase>();

  params.addRequiredParam<UserObjectName>("inserter", "Name of the EventInserter UserObject.");
  params.addRequiredParam<Real>("radius", "Radius of circle to average material property in.");

  return params;
}

InserterPointCircleAverageMaterialProperty::InserterPointCircleAverageMaterialProperty(const InputParameters & parameters) :
    CircleAverageMaterialPropertyBase(parameters),
    _inserter(NULL),
    _radius(getParam<Real>("radius")),
    _integral_sum(0),
    _volume_sum(0),
    _event_list(0),
    _Npoints(0)
{
}

void
InserterPointCircleAverageMaterialProperty::initialSetup()
{
  // Doing this here because all UserObjects will be constructed and order won't matter in the input file
  _inserter = &getUserObject<EventInserterBase>("inserter");
}

Real
InserterPointCircleAverageMaterialProperty::averageValue(const Point & p) const
{
  // loop through the event list and find which index the incoming Point corresponds to
  for (unsigned int i=0; i<_event_list.size(); i++)
    if (p.absolute_fuzzy_equals(_event_list[i].second))
      return _integral_sum[i]/_volume_sum[i];
    else
      return 0.0;  // in case volume postprocessor hasn't been run

  // if we made it here, the point wasn't found
  mooseError("In InserterPointCircleAverageMaterialProperty::averageValue(), Point", p, "not found");
}

void
InserterPointCircleAverageMaterialProperty::initialize()
{
  // get old and event lists and put them together
  _event_list = _inserter->getOldEventList();
  EventList upcoming_events = _inserter->getEventList();
  _event_list.insert(_event_list.end(), upcoming_events.begin(), upcoming_events.end());

  // keep track of number of points
  _Npoints = _event_list.size();

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

void
InserterPointCircleAverageMaterialProperty::execute()
{
  for (unsigned int i=0; i < _Npoints; i++)
  {
    // get distance from event point to element centroid
    Real r = distance(_current_elem->centroid(), _event_list[i].second);

    // check if distance between points is less than supplied radius
    // or if point is in element
    if ((r < _radius) || (_current_elem->contains_point(_event_list[i].second)))
    {
      _integral_sum[i] += computeIntegral();
      _volume_sum[i] += _current_elem_volume;
    }
  }
}

void
InserterPointCircleAverageMaterialProperty::threadJoin(const UserObject & y)
{
  // We are joining with another class like this one so do a cast so we can get to it's data
  const InserterPointCircleAverageMaterialProperty & uo = dynamic_cast<const InserterPointCircleAverageMaterialProperty &>(y);

  // put together values from threads
  for (unsigned int i=0; i < _Npoints; i++)
  {
    _integral_sum[i] += uo._integral_sum[i];
    _volume_sum[i] += uo._volume_sum[i];
  }
}

void
InserterPointCircleAverageMaterialProperty::finalize()
{
  // put together values from processors
  for (unsigned int i=0; i < _Npoints; i++)
  {
    gatherSum(_integral_sum[i]);
    gatherSum(_volume_sum[i]);
  }
}
