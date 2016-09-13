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

#ifndef EVENTMARKER_H
#define EVENTMARKER_H

#include "Marker.h"
#include "EventInserter.h"
#include "GaussianUserObject.h"
#include "Coupleable.h"

// libmesh includes
#include "libmesh/mesh_tools.h"

class EventMarker;

template<>
InputParameters validParams<EventMarker>();

class EventMarker : public Marker, public Coupleable
{
public:
  EventMarker(const InputParameters & parameters);
  virtual ~EventMarker(){};

  // copying these from EventInserter.h

  /// An event has an start time and a location
  typedef std::pair<Real, Point> Event;

  /// A list of future time/location pairs
  typedef std::vector<Event> EventList;

protected:
  virtual void initialSetup();

  virtual void timestepSetup();

  virtual MarkerValue computeElementMarker();

  void checkForEvent();

  const EventInserter & _inserter;

  const GaussianUserObject & _gaussian_uo;

  const Real _marker_radius;

  const bool _verbose;

  const unsigned int _periodic_var;

  const bool _coarsen_events;

  const Real _sigma_mesh_ratio;

  const Real _refine_distance;

  const Real _minimum_element_size;

  const bool _refine_by_ratio;

  bool _event_incoming;

  Point _event_location;

  unsigned int _input_cycles_per_step;

  bool _coarsening_needed;

  EventList _old_event_list;
};

#endif /* EVENTMARKER_H */
