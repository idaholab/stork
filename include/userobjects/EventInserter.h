/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#ifndef EVENTINSERTER_H
#define EVENTINSERTER_H

#include "GeneralUserObject.h"
#include "RandomPointUserObject.h"
#include "MooseRandom.h"

class EventInserter;
class CircleAverageMaterialProperty;
class CircleMaxOriginalElementSize;

template<>
InputParameters validParams<EventInserter>();

/**
 * This UserObject manages the insertion and timing of events in the simulation
 * domain. It manages a list of events with their insertion times and their center
 * positions.
 */
class EventInserter : public GeneralUserObject
{
public:
  EventInserter(const InputParameters & parameters);

  virtual void initialSetup();

  virtual void initialize(){} // Not used
  virtual void execute();
  virtual void finalize(){} // Not used

  /// An event has an start time and a location
  typedef std::pair<Real, Point> Event;

  /// A list of future time/location pairs
  typedef std::vector<Event> EventList;

  /// Get the event list
  const EventList & getEventList() const { return _global_event_list; }

  /// Get the old event list
  const EventList & getOldEventList() const { return _old_event_list; }

  /// Get time tolerance
  const Real & getTimeTolerance() const { return _time_tol; }

  /// Get old event tracking flag
  const bool & areOldEventsBeingTracked() const { return _track_old_events; }

  /// Get old event removed flag
  const bool & wasOldEventRemoved() const { return _old_event_removed; }

  /// Calculate time to next event
  Real getNewEventInterval();

  /// Check if event is available between the two supplied times
  bool isEventActive(const Real t1, const Real t2) const;

  /// Return event between the two supplied times
  Point getActiveEventPoint(const Real t1, const Real t2) const;

  /// Returns time of next event in list
  Real getNextEventTime() const;

  /// Returns time of last event in list
  Real getMaxEventTime() const;

  /// Print out event lists
  void printEventLists() const;

protected:
  /// Timing to use between event
  const bool _use_random_timing;

  /// Type of probability distribution to use for picking next event time
  const MooseEnum _distribution;

  /// Mean value of probability distribution
  const Real _mean;

  /// Flag to add an event at the beginning of the simulation
  const bool _insert_initial;

  /// Flag to add an extra event at user-specified time and place
  const bool _insert_test;

  /// Time to insert test event
  Real _test_time;

  /// Location of test event
  Point _test_location;

  /// Reference to RandomPointUserObject
  const RandomPointUserObject & _random_point_user_object;

  /// Random number seed
  const unsigned int _seed;

  /// Verbose flag
  const bool _verbose;

  /// Tolerance when comparing times
  const Real _time_tol;

  /// Flag to track old events
  const bool _track_old_events;

  /// How to choose when old events should be removed from list
  const MooseEnum _removal_method;

  /// Age at which to expire an old event
  const Real _removal_time;

  /// Target Event sigma value to remove old event
  const Real _removal_sigma;

  /// Target Event sigma to original element size ratio to remove old event
  const Real _removal_ratio;

  /// Distance, in multiples of the Event sigma, around an old event, to sample the original mesh
  const Real _radius;

  /// Pointer to CircleAverageMaterialProperty UserObject
  const CircleAverageMaterialProperty * _circle_average_mat_prop_uo_ptr;

  /// Pointer to another CircleAverageMaterialProperty UserObject
  const CircleAverageMaterialProperty * _inserter_circle_average_mat_prop_uo_ptr;

  /// Pointer to CircleMaxOriginalElementSize UserObject
  const CircleMaxOriginalElementSize * _circle_max_elem_size_uo_ptr;

  /// Flag when Event has been removed from old list
  bool & _old_event_removed;

  /// the global list of all events
  EventList & _global_event_list;

  /// List of old events
  EventList & _old_event_list;

  /// List of sigma estimates for old events
  std::vector<Real> & _old_sigma_list;

  /// Random number generator instance, mutable so it can be used inside const functions
  MooseRandom & _random;

  /// Holds initial sigma from GaussianUserObject
  Real _initial_sigma;

  /// Flag for starting up after restart
  bool _first_time_after_restart;
};

#endif //EVENTINSERTER_H
