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

// MOOSE includes
#include "EventTimeStepper.h"

template<>
InputParameters validParams<EventTimeStepper>()
{
  InputParameters params = validParams<TimeStepper>();
  params.addClassDescription("Adjust the timestep based on EventInserter.");
  params.addRequiredParam<UserObjectName>("event_inserter", "Name of the EventInserter user object.");
  params.addRequiredParam<Real>("dt", "The initial timestep size.");
  params.addParam<Real>("growth_factor", 1.0, "Factor to apply to timestep if no events occur.");
  params.addParam<bool>("verbose", false, "Print out extra information about interactions with events.");
  return params;
}

EventTimeStepper::EventTimeStepper(const InputParameters & parameters) :
    TimeStepper(parameters),
    UserObjectInterface(this),
    _input_dt(getParam<Real>("dt")),
    _growth_factor(getParam<Real>("growth_factor")),
    _verbose(getParam<bool>("verbose")),
    _was_dt_cut(declareRestartableData<bool>("was_dt_cut",false)),
    _new_dt(declareRestartableData<Real>("new_dt",_input_dt)),
    _inserter_ptr(NULL)
{
}

void
EventTimeStepper::init()
{
  // called here because timesteppers are initialized before user objects
  _inserter_ptr = &getUserObject<EventInserterBase>("event_inserter");
}

Real
EventTimeStepper::computeInitialDT()
{
  if (_input_dt > _inserter_ptr->getNextEventTime())
  {
    // if proposed timestep will step over next event,
    // cut dt to line up with next event time
    _was_dt_cut = true;
    if (_verbose)
      _console << "computeInitialDT(): " <<  _inserter_ptr->getNextEventTime() - _time << std::endl;
    return _inserter_ptr->getNextEventTime() - _time;
  }
  return _input_dt;
}

Real
EventTimeStepper::computeDT()
{
  if (_dt < _new_dt)
  {
    _was_dt_cut = false; // technicaly dt _was_ cut, but not by this TimeStepper
    if (_verbose)
      _console << "EventTimeStepper detected dt was cut elsewhere, still waiting for event..." << std::endl;
  }

  // default is to apply growth factor to current DT
  _new_dt = _dt * _growth_factor;

  if (_was_dt_cut)
  {
    // reset timestep to initial after event has been inserted
    if (_verbose)
      _console << "Resetting dt to " << _input_dt << " following alignment with event." << std::endl;
    _new_dt = _input_dt;
    _was_dt_cut = false;
  }

  Real next_event_time = _inserter_ptr->getNextEventTime();
  if ((_new_dt + _time) > next_event_time)
  {
    // if proposed timestep will step over next event,
    // cut dt to line up with next event time
    if (_verbose)
      _console << "Next time step of " << _new_dt << " will overstep next event at " << next_event_time << ", cutting dt to " << next_event_time - _time << std::endl;
    if (std::abs(_time - next_event_time) > _inserter_ptr->getTimeTolerance())  // check to make sure we're not really close to an event
      _new_dt = next_event_time - _time;
    else if (_verbose)
      _console << "Nevermind, we're actually at an event" << std::endl;
    _was_dt_cut = true;
  }

  if (_verbose)
    _console << "*** next event: " << _inserter_ptr->getNextEventTime() << std::endl;

  return _new_dt;
}
