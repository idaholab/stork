/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#include "EventInserter.h"
#include "MooseRandom.h"

#include <time.h> // for time function to seed random number generator

template<>
InputParameters validParams<EventInserter>()
{
  InputParameters params = validParams<GeneralUserObject>();
  params += validParams<RandomInterface>();

  MooseEnum distribution("uniform exponential", "uniform");

  params.addRequiredParam<Real>("mean", "Mean (time) of probability distribution");
  params.addRequiredParam<UserObjectName>("random_point_user_object", "Name of RandomPointUserObject to get random points on the mesh.");
  params.addParam<bool>("random_timing", false, "Use random timing (true) or uniform timing (false).");
  params.addParam<MooseEnum>("distribution", distribution, "Type of probability distribution to use in picking event times.");
  params.addParam<bool>("insert_initial", false, "Insert an event at the beginning of the simulation.");
  params.addParam<bool>("insert_test", false, "Insert a test event. If set 'test_time' and 'test_location' need to be set.");
  params.addParam<Real>("test_time", "Insert an event at this time in the simulation cell");
  params.addParam<Point>("test_location", "Insert an event at a point in the simulation cell");
  params.addParam<unsigned int>("seed", 0, "Feed the random number generator. If not supplied, system time is used.");
  params.addParam<bool>("verbose", false, "Print out extra information about when events are inserted and deleted.");
  params.addParam<Real>("time_tolerance", 1.0e-12, "Tolerance when comparing times.");
  MultiMooseEnum setup_options(SetupInterface::getExecuteOptions());
  setup_options = "timestep_begin";
  params.set<MultiMooseEnum>("execute_on") = setup_options;
  return params;
}

EventInserter::EventInserter(const InputParameters & parameters) :
    GeneralUserObject(parameters),
    RandomInterface(parameters, _fe_problem, _tid, false),
    _use_random_timing(getParam<bool>("random_timing")),
    _distribution(getParam<MooseEnum>("distribution")),
    _mean(getParam<Real>("mean")),
    _insert_initial(getParam<bool>("insert_initial")),
    _insert_test(getParam<bool>("insert_test")),
    _test_time(_insert_test ? getParam<Real>("test_time") : 0),
    _test_location(_insert_test ? getParam<Point>("test_location") : 0),
    _random_point_user_object(getUserObject<RandomPointUserObject>("random_point_user_object")),
    _seed(getParam<unsigned int>("seed")),
    _verbose(getParam<bool>("verbose")),
    _time_tol(getParam<Real>("time_tolerance")),
    _insert_first(true),
    _insert_second(true),
    _global_event_list(0)
{
  setRandomResetFrequency(EXEC_INITIAL);
  if (parameters.isParamSetByUser("seed"))
    MooseRandom::seed(_seed);
  else
    MooseRandom::seed(time(NULL));

  if ((parameters.isParamSetByUser("insert_test")) && (!parameters.isParamSetByUser("test_time")))
    mooseError("'test_time' parameter is required");
  if ((parameters.isParamSetByUser("insert_test")) && (!parameters.isParamSetByUser("test_location")))
    mooseError("'test_location' parameter is required");

  // populate global_event_list so TimeStepper can access it immediately

  // insert initial event once
  if (_insert_initial)
  {
    _global_event_list.push_back(Event(_t, _random_point_user_object.getRandomPoint()));
    _insert_initial = false;
    if (_verbose)
      _console << "*** inserting initial event at time " << _t << std::endl;
  }

  // insert test event once
  if (_insert_test)
  {
    _global_event_list.push_back(Event(_test_time, _test_location));
    _insert_test = false;
    if (_verbose)
      _console << "*** inserting test event at time " << _test_time << std::endl;
  }

  // insert first event
  if (_insert_first)
  {
    Real new_event_time = _t + getNewEventInterval(); // for debugging
    _global_event_list.push_back(Event(new_event_time, _random_point_user_object.getRandomPoint()));
    _insert_first = false;
    if (_verbose)
      _console << "*** inserting first event at time " << new_event_time << std::endl;
  }

  // insert second event so list won't be empty when removing an event
  if (_insert_second)
  {
    Real new_event_time = getMaxEventTime() + getNewEventInterval(); // for debugging
    _global_event_list.push_back(Event(new_event_time, _random_point_user_object.getRandomPoint()));
    _insert_second = false;
    if (_verbose)
      _console << "*** inserting second event at time " << new_event_time << std::endl;
  }
}

void
EventInserter::timestepSetup()
{
  // expire entries from the list (if the current timestep converged) then add the next one
  if (_fe_problem.converged())
  {
    bool add_event = false;
    unsigned int i = 0;
    while (i < _global_event_list.size())
    {
      if (_global_event_list[i].first < _t)
      {
        // remove entry (by replacing with last element and shrinking size by one)
        if (_verbose)
          _console << "*** removing entry at time " << _global_event_list[i].first << std::endl;
        if (_global_event_list[i].first != _test_time) // do not add event when removing test event, messes with statistics
          add_event = true;
        _global_event_list[i] = _global_event_list.back();
        _global_event_list.pop_back();
      }
      else
        ++i;
    }
    // if entries were removed, add a new event
    if (add_event)
    {
      Real new_event_time = getMaxEventTime() + getNewEventInterval(); // for debugging
      _global_event_list.push_back(Event(new_event_time, _random_point_user_object.getRandomPoint()));
      if (_verbose)
        _console << "*** inserting new event at time " << new_event_time << std::endl;
    }
  }
}

void
EventInserter::execute()
{
  // print out events for debugging
  if (_verbose)
  {
    _console << " *** time: " << _t << " list size: " << _global_event_list.size() << std::endl;
    for (unsigned int i=0; i<_global_event_list.size(); i++)
      _console << " *** i: " << i << " first: " << _global_event_list[i].first << " second: " << _global_event_list[i].second << std::endl;
  }
}

Real
EventInserter::getNewEventInterval()
{
  if (_use_random_timing)
  {
    if (_distribution == "uniform")
      return MooseRandom::rand()*_mean*2.0;
    else // exponential distribution
      return -std::log(MooseRandom::rand())*_mean;
  }

  return _mean;

}

bool
EventInserter::isEventActive(const Real t1, const Real t2) const
{
  for (unsigned int i=0; i<_global_event_list.size(); i++)
    if ((_global_event_list[i].first >= t1 - _time_tol) && (_global_event_list[i].first < t2))
      return true;
  return false;
}

Point
EventInserter::getActiveEventPoint(const Real t1, const Real t2) const
{
  for (unsigned int i=0; i<_global_event_list.size(); i++)
    if ((_global_event_list[i].first >= t1 - _time_tol) && (_global_event_list[i].first < t2))
      return _global_event_list[i].second;

  mooseError("EventInserter::getActiveEvent: Active event point requested but no active event found. Check with `isEventActive(t1, t2)` first.");
}

Real
EventInserter::getNextEventTime() const
{
  // Find the min event time that is greater than current time
  Real next_time = std::numeric_limits<Real>::max();
  for (unsigned int i=0; i<_global_event_list.size(); i++)
    if ((_global_event_list[i].first < next_time) && (_global_event_list[i].first > _t))
      next_time = _global_event_list[i].first;

  return next_time;
}

Real
EventInserter::getMaxEventTime() const
{
  // Find the max event time
  Real max_time = std::numeric_limits<Real>::min();
  for (unsigned int i=0; i<_global_event_list.size(); i++)
    if (_global_event_list[i].first > max_time)
      max_time = _global_event_list[i].first;

  return max_time;
}
