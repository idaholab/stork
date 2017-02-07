/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#include "EventInserter.h"
#include "GaussianUserObject.h"
#include "CircleAverageMaterialProperty.h"
#include "CircleMaxOriginalElementSize.h"

#include <time.h> // for time function to seed random number generator

template<>
InputParameters validParams<EventInserter>()
{
  InputParameters params = validParams<GeneralUserObject>();

  MooseEnum distribution("uniform exponential", "uniform");
  MooseEnum removal_method("time sigma sigma_element_size_ratio", "time");

  params.addRequiredParam<Real>("mean", "Mean (time) of probability distribution");
  params.addRequiredParam<UserObjectName>("random_point_user_object", "Name of RandomPointUserObject to get random points on the mesh.");
  params.addParam<bool>("random_timing", false, "Use random timing (true) or constistent timing (false).");
  params.addParam<MooseEnum>("distribution", distribution, "Type of probability distribution to use in picking random event times.");
  params.addParam<bool>("insert_initial", false, "Insert an event at the beginning of the simulation.");
  params.addParam<bool>("insert_test", false, "Insert a test event. If set 'test_time' and 'test_location' need to be set.");
  params.addParam<Real>("test_time", "Insert an event at this time in the simulation cell");
  params.addParam<Point>("test_location", "Insert an event at a point in the simulation cell");
  params.addParam<unsigned int>("seed", 0, "Feed the random number generator. If not supplied, system time is used.");
  params.addParam<bool>("verbose", false, "Print out extra information about when events are inserted and deleted.");
  params.addParam<Real>("time_tolerance", 1.0e-12, "Tolerance when comparing times.");
  params.addParam<bool>("track_old_events", false, "Enable this to allow mesh from previous events to be coarsened. If true, choose removal method with 'removal_method'.");
  params.addParam<MooseEnum>("removal_method", removal_method, "How to decide when to remove old cascade events. Choices are 'time' or 'sigma'.");
  params.addParam<Real>("removal_time", "Time to wait after an event to remove from the list.");
  params.addParam<Real>("removal_sigma", "Target (approx.) sigma value at which to remove the event from the list.");
  params.addParam<Real>("removal_sigma_element_size_ratio", "Target sigma to original element size ratio at which to remove the event from the list.");
  params.addParam<Real>("radius", "Distance, in multiples of the Event sigma, around an old event, to sample the original mesh.");
  params.addParam<UserObjectName>("gaussian_user_object", "Name of the GaussianUserObject for initial sigma value when coarsening by sigma values.");
  params.addParam<UserObjectName>("circle_average_material_property_user_object", "Name of the CircleAverageMaterialProperty UserObject for arbitrary circles and radii.");
  params.addParam<UserObjectName>("inserter_circle_average_material_property_user_object", "Name of the CircleAverageMaterialProperty UserObject that gets points from EventInserter.");
  params.addParam<UserObjectName>("circle_max_original_element_size_user_object", "Name of the CircleMaxOriginalElementSize UserObject for arbitrary circles and radii.");

  MultiMooseEnum setup_options(SetupInterface::getExecuteOptions());
  setup_options = "timestep_begin";
  params.set<MultiMooseEnum>("execute_on") = setup_options;

  return params;
}

EventInserter::EventInserter(const InputParameters & parameters) :
    GeneralUserObject(parameters),
    _use_random_timing(getParam<bool>("random_timing")),
    _distribution(getParam<MooseEnum>("distribution")),
    _mean(getParam<Real>("mean")),
    _insert_initial(getParam<bool>("insert_initial")),
    _insert_test(getParam<bool>("insert_test")),
    _test_time(((_insert_test) && (parameters.isParamSetByUser("test_time"))) ? getParam<Real>("test_time") : -std::numeric_limits<Real>::max()),
    _test_location(((_insert_test) && (parameters.isParamSetByUser("test_location"))) ? getParam<Point>("test_location") : 0),
    _random_point_user_object(getUserObject<RandomPointUserObject>("random_point_user_object")),
    _seed(getParam<unsigned int>("seed")),
    _verbose(getParam<bool>("verbose")),
    _time_tol(getParam<Real>("time_tolerance")),
    _track_old_events(getParam<bool>("track_old_events")),
    _removal_method(getParam<MooseEnum>("removal_method")),
    _removal_time(((_track_old_events) && (_removal_method == "time") && (parameters.isParamSetByUser("removal_time"))) ? getParam<Real>("removal_time") : std::numeric_limits<Real>::max()),
    _removal_sigma(((_track_old_events) && (_removal_method == "sigma") && (parameters.isParamSetByUser("removal_sigma"))) ? getParam<Real>("removal_sigma") : std::numeric_limits<Real>::max()),
    _removal_ratio(((_track_old_events) && (_removal_method == "sigma_element_size_ratio") && (parameters.isParamSetByUser("removal_sigma_element_size_ratio"))) ? getParam<Real>("removal_sigma_element_size_ratio") : std::numeric_limits<Real>::max()),
    _radius(((_track_old_events) && (_removal_method == "sigma_element_size_ratio") && (parameters.isParamSetByUser("radius"))) ? getParam<Real>("radius") : std::numeric_limits<Real>::max()),
    _circle_average_mat_prop_uo_ptr((parameters.isParamSetByUser("circle_average_material_property_user_object") && (_removal_method == "sigma" || _removal_method == "sigma_element_size_ratio")) ? &getUserObject<CircleAverageMaterialProperty>("circle_average_material_property_user_object") : NULL),
    _inserter_circle_average_mat_prop_uo_ptr((parameters.isParamSetByUser("inserter_circle_average_material_property_user_object") && (_removal_method == "sigma" || _removal_method == "sigma_element_size_ratio")) ? &getUserObject<CircleAverageMaterialProperty>("inserter_circle_average_material_property_user_object") : NULL),
    _circle_max_elem_size_uo_ptr((parameters.isParamSetByUser("circle_max_original_element_size_user_object") && (_removal_method == "sigma" || _removal_method == "sigma_element_size_ratio")) ? &getUserObject<CircleMaxOriginalElementSize>("circle_max_original_element_size_user_object") : NULL),
    _old_event_removed(declareRestartableData<bool>("old_event_removed",false)),
    _global_event_list(declareRestartableData<EventList>("global_event_list")),
    _old_event_list(declareRestartableData<EventList>("old_event_list")),
    _old_sigma_list(declareRestartableData<std::vector<Real> >("old_sigma_list")),
    _random(declareRestartableData<MooseRandom>("event_inserter_generator")),
    _first_time_after_restart(_app.isRestarting() || _app.isRecovering())
{
  if (parameters.isParamSetByUser("seed"))
    _random.seed(0,_seed);
  else
    _random.seed(0,time(NULL));

  if ((_insert_test) && (!parameters.isParamSetByUser("test_time")))
    mooseError("'test_time' parameter is required");
  if ((_insert_test) && (!parameters.isParamSetByUser("test_location")))
    mooseError("'test_location' parameter is required");

  // Check input logic for removing old events
  if (_track_old_events)
  {
    if ((_removal_method == "time") && (!parameters.isParamSetByUser("removal_time")))
      mooseError("User requested to remove old events by time but 'removal_time' was not set.");
    if ((_removal_method == "sigma") && (!parameters.isParamSetByUser("removal_sigma")))
      mooseError("User requested to remove old events by sigma but 'removal_sigma' was not set.");
    if ((_removal_method == "sigma_element_size_ratio") && (!parameters.isParamSetByUser("removal_sigma_element_size_ratio")))
      mooseError("User requested to remove old events by sigma ratio but 'removal_sigma_element_size_ratio' was not set.");
    if ((_removal_method == "sigma_element_size_ratio") && (!parameters.isParamSetByUser("radius")))
      mooseError("User requested to remove old events by sigma ratio but 'radius' was not set.");
    if (((_removal_method == "sigma") || (_removal_method == "sigma_element_size_ratio")) && (!parameters.isParamSetByUser("gaussian_user_object")))
      mooseError("User requested to remove old events by a sigma method but 'gaussian_user_object' was not set.");
    if (((_removal_method == "sigma") || (_removal_method == "sigma_element_size_ratio")) && (!parameters.isParamSetByUser("circle_average_material_property_user_object")))
      mooseError("User requested to remove old events by a sigma method but 'circle_average_material_property_user_object' was not set.");
    if (((_removal_method == "sigma") || (_removal_method == "sigma_element_size_ratio")) && (!parameters.isParamSetByUser("inserter_circle_average_material_property_user_object")))
      mooseError("User requested to remove old events by a sigma method but 'inserter_circle_average_material_property_user_object' was not set.");
    if ((_removal_method == "sigma_element_size_ratio") && (!parameters.isParamSetByUser("circle_max_original_element_size_user_object")))
      mooseError("User requested to remove old events by sigma ratio but 'circle_max_original_element_size_user_object' was not set.");
  }
}

void
EventInserter::initialSetup()
{
  // get initial sigma for initializing old event sigmas
  if ((_removal_method == "sigma") || (_removal_method == "sigma_element_size_ratio"))
  {
    const GaussianUserObject & gaussian_user_object = getUserObject<GaussianUserObject>("gaussian_user_object");
    _initial_sigma = gaussian_user_object.getSigma();
  }

  // populate global_event_list so TimeStepper can access it immediately

  // insert initial event once
  if ((_insert_initial) && (!_app.isRestarting()) && (!_app.isRecovering()))
  {
    _global_event_list.push_back(Event(_t, _random_point_user_object.getRandomPoint()));
    if (_verbose)
      _console << "*** inserting initial event at time " << _t << std::endl;
  }

  // insert test event once
  if ((_insert_test) && (!_app.isRestarting()) && (!_app.isRecovering()))
  {
    _global_event_list.push_back(Event(_test_time, _test_location));
    if (_verbose)
      _console << "*** inserting test event at time " << _test_time << std::endl;
  }

  // insert first event
  if ((!_app.isRestarting()) && (!_app.isRecovering()))
  {
    Real new_event_time = _t + getNewEventInterval(); // for debugging
    _global_event_list.push_back(Event(new_event_time, _random_point_user_object.getRandomPoint()));
    if (_verbose)
      _console << "*** inserting first event at time " << new_event_time << std::endl;
  }

  // insert second event so list won't be empty when removing an event
  if ((!_app.isRestarting()) && (!_app.isRecovering()))
  {
    Real new_event_time = getMaxEventTime() + getNewEventInterval(); // for debugging
    _global_event_list.push_back(Event(new_event_time, _random_point_user_object.getRandomPoint()));
    if (_verbose)
      _console << "*** inserting second event at time " << new_event_time << std::endl;
  }
}

void
EventInserter::execute()
{
  // print out events for debugging
  if (_verbose)
  {
    _console << "event lists at beginning of EventInserter::execute() for time step " << _t_step << " and time " << _t << std::endl;
    printEventLists();
  }

  // expire entries from the list (if the current timestep converged) then add the next one
  if ((_fe_problem.converged()) || (_first_time_after_restart))
  {
    // update sigma list before adding any entries to it
    if ((_removal_method == "sigma") || (_removal_method == "sigma_element_size_ratio"))
    {
      if (_verbose)
        _console << "updating sigma list values..." << std::endl;
      for (unsigned int i=0; i<_old_sigma_list.size(); i++)
      {
        // get average diffusion coefficient around the old event point
        Real D = _inserter_circle_average_mat_prop_uo_ptr->averageValue(i);

        // in case D changed from the previous step, calculate new t_star (described below) based on
        // last time step (which when is when sigma would have been affected by the change)
        if (D <= 0.0)
          mooseError("Tried to calculate effective time for sigma evolution but received a diffusion coefficient of " << D);
        Real t_star = (_t - _dt) - _old_sigma_list[i]*_old_sigma_list[i]/2.0/D;

        // calculate increase in sigma over this step
        Real dsigma = std::sqrt(2.0*D*(_t - t_star)) - std::sqrt(2.0*D*(_t - _dt - t_star));

        // update sigma
        _old_sigma_list[i] += dsigma;
      }
    }

    unsigned int i = 0;
    while (i < _global_event_list.size())
    {
      // look for past Events
      if (_global_event_list[i].first < _t)
      {
        // add past Event to old list if requested
        if (_track_old_events)
        {
          _old_event_list.push_back(_global_event_list[i]);

          // initialize entry in sigma array
          if ((_removal_method == "sigma") || (_removal_method == "sigma_element_size_ratio"))
          {
            // assume D has been constant from when event occured until now (have to assume something!)
            Real D = _circle_average_mat_prop_uo_ptr->averageValue(_global_event_list[i].second, _initial_sigma);

            // calculate fictitious time that event with sigma=0 (dirac delta) would need to occur to grow to
            // be initial sigma when actually inserted, measure time relative to this
            if (D <= 0.0)
              mooseError("Tried to calculate effective time for sigma evolution but received a diffusion coefficient of " << D);
            Real t_star = _global_event_list[i].first - _initial_sigma*_initial_sigma/2.0/D;

            // estimate current value of sigma because the old events are not updated right away
            _old_sigma_list.push_back(std::sqrt(2.0*D*(_t - t_star)));
          }
        }

        // remove past events and add new ones
        if (std::abs(_global_event_list[i].first - _test_time) > _time_tol) // do not add event when removing test event, messes with statistics
        {
          // doing it this way so information can printed out if requested
          Real new_event_interval = getNewEventInterval();
          Real new_event_time = getMaxEventTime() + new_event_interval;

          if (_verbose)
          {
            _console << "*** new event interval: " << new_event_interval << std::endl;
            _console << "*** inserting new event at time " << new_event_time << std::endl;
            _console << "*** removing event at time " << _global_event_list[i].first << std::endl;
          }

          // replace old event with new one to maintain a non-empty list
          _global_event_list[i] = Event(new_event_time, _random_point_user_object.getRandomPoint());
        }
        else
        {
          // remove test Event by replacing it with the last event and shrinking size of list by one
          _global_event_list[i] = _global_event_list.back();
          _global_event_list.pop_back();

          if (_verbose)
            _console << "*** removing test event at time " << _test_time << std::endl;
        }
      }
      else
        ++i;
    }

    // expire entries from old list
    if (_track_old_events)
    {
      // check if old events need to be removed
      _old_event_removed = false;
      for (unsigned int i=0; i<_old_event_list.size(); i++)
      {
        if (_removal_method == "time")
        {
          if (_old_event_list[i].first < _fe_problem.time() - _removal_time) // check if event is old enough
          {
            _old_event_removed = true;
            if (_verbose)
              _console << "event needs to be coarsened: (old) event time: " << _old_event_list[i].first << " location: " << _old_event_list[i].second << std::endl;
          }
        }
        else if (_removal_method == "sigma") // sigma removal method
        {
          if (_old_sigma_list[i] > _removal_sigma) // check if sigma has widened enough
          {
            _old_event_removed = true;
            if (_verbose)
              _console << "event needs to be coarsened: (old) event time: " << _old_event_list[i].first << " location: " << _old_event_list[i].second << " estimated sigma: " << _old_sigma_list[i] << std::endl;
          }
        }
        else // sigma ratio removal method
        {
          // get max element size from the original mesh in the vicinity of the old event
          Real max_element_size = _circle_max_elem_size_uo_ptr->value(_old_event_list[i].second, _radius);

          if (_old_sigma_list[i]/max_element_size > _removal_ratio) // check if sigma on the *original* mesh is sampled enough
          {
            _old_event_removed = true;
            if (_verbose)
              _console << "event needs to be coarsened: (old) event time: " << _old_event_list[i].first << " location: " << _old_event_list[i].second << " estimated sigma: " << _old_sigma_list[i] << " local max original element size: " << max_element_size << " ratio: " << _old_sigma_list[i]/max_element_size << std::endl;
          }
        }

        // if an Event is active *now* and an old event needs to be removed,
        // then EventMarker will be triggered to both REFINE and COARSEN, but REFINE takes
        // precedence so we leave the old event list alone so COARSEN will take place on
        // the next step
        if ((isEventActive(_t - _time_tol, _t + _time_tol)) && (_old_event_removed))
        {
          _old_event_removed = false;
          if (_verbose)
            _console << "Wait, an event is incoming so delay coarsening until the next step..." << std::endl;
        }

        if (_old_event_removed)
        {
          // remove old event from list as we will signal the marker to coarsen everywhere where events are active
          if (_verbose)
            _console << "removing old event at time " << _old_event_list[i].first << std::endl;
          _old_event_list[i] = _old_event_list.back();
          _old_event_list.pop_back();

          if ((_removal_method == "sigma") || (_removal_method == "sigma_element_size_ratio"))
          {
            _old_sigma_list[i] = _old_sigma_list.back();
            _old_sigma_list.pop_back();
          }

          break;

        }
      }
    }
  }

  // print out events for debugging
  if (_verbose)
  {
    _console << "event lists at end of EventInserter::execute() for time step " << _t_step << " and time " << _t << std::endl;
    printEventLists();
  }

  // done with the first time through
  _first_time_after_restart = false;
}

Real
EventInserter::getNewEventInterval()
{
  if (_use_random_timing)
  {
    if (_distribution == "uniform")
      return _random.rand(0)*_mean*2.0;
    else // exponential distribution
      return -std::log(_random.rand(0))*_mean;
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

void
EventInserter::printEventLists() const
{
  _console << " *** printing event lists at time " << _t << "..." << std::endl;
  _console << " global event list, size: " << _global_event_list.size() << std::endl;
  for (unsigned int i=0; i<_global_event_list.size(); i++)
    _console << " i: " << i << " time: " << _global_event_list[i].first << " location: " << _global_event_list[i].second << std::endl;

  if (_track_old_events)
  {
    _console << " old event list, size: " << _old_event_list.size() << std::endl;
    for (unsigned int i=0; i<_old_event_list.size(); i++)
      _console << " i: " << i << " time: " << _old_event_list[i].first << " location: " << _old_event_list[i].second << std::endl;
  }

  if ((_track_old_events) && ((_removal_method == "sigma") || (_removal_method == "sigma_element_size_ratio")))
  {
    _console << " old sigma list, size: " << _old_sigma_list.size() << std::endl;
    for (unsigned int i=0; i<_old_sigma_list.size(); i++)
      _console << " i: " << i << ": sigma: " << _old_sigma_list[i] << std::endl;
    _console << " printing CircleAverageMaterialProperty values for old events..." << std::endl;
    for (unsigned int i=0; i<_old_event_list.size(); i++)
      _console << " old event list, i: " << i << ": location: " << _old_event_list[i].second << " average value: " << _inserter_circle_average_mat_prop_uo_ptr->averageValue(i) << std::endl;
  }
}
