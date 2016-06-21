/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#ifndef EVENTINSERTER_H
#define EVENTINSERTER_H

#include "GeneralUserObject.h"
#include "RandomInterface.h"
#include "RandomPointUserObject.h"

class EventInserter;

template<>
InputParameters validParams<EventInserter>();

/**
 * This UserObject manages the insertion and timing of events in the simulation
 * domain. It manages a list of events with their insertion times and their center
 * positions.
 */
class EventInserter :
  public GeneralUserObject,
  public RandomInterface
{
public:
  EventInserter(const InputParameters & parameters);

  virtual void timestepSetup();

  virtual void initialize(){} // Not used
  virtual void execute();
  virtual void finalize(){} // Not used

  /// An event has an start time and a location
  typedef std::pair<Real, Point> Event;

  /// A list of future time/location pairs
  typedef std::vector<Event> EventList;

  /// Get the event list
  const EventList & getEventList() const { return _global_event_list; }

  /// Get time tolerance
  const Real & getTimeTolerance() const { return _time_tol; }

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

protected:
  /// Timing to use between event
  bool _use_random_timing;

  /// Type of probability distribution to use for picking next event time
  MooseEnum _distribution;

  /// Mean value of probability distribution
  Real _mean;

  /// Flag to add an event at the beginning of the simulation
  bool _insert_initial;

  /// Flag to add an extra event at user-specified time and place
  bool _insert_test;

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

  /// Flag to insert the first event
  bool _insert_first;

  /// Flag to insert the second event
  bool _insert_second;

  /// the global list of all events
  EventList _global_event_list;
};

#endif //EVENTINSERTER_H
