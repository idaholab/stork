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
#ifndef EVENTTIMESTEPPER_H
#define EVENTTIMESTEPPER_H

#include "TimeStepper.h"
#include "EventInserter.h"
#include "UserObjectInterface.h"

class EventTimeStepper;

template<>
InputParameters validParams<EventTimeStepper>();

/**
 * Adjust the timestep to line up with events determined in an EventInserter UserObject.
 * This time stepper allows the user to specify a factor to apply to the previous timestep.
 */
class EventTimeStepper :
  public TimeStepper,
  public UserObjectInterface
{
public:
  EventTimeStepper(const InputParameters & parameters);
  virtual ~EventTimeStepper();

protected:
  virtual void init();
  virtual Real computeInitialDT();
  virtual Real computeDT();

  /// The dt from the input file.
  const Real _input_dt;

  /// grow the timestep by this factor
  const Real _growth_factor;

  /// Print out extra information about time stepping
  const bool _verbose;

  /// Flag if dt was cut
  bool & _was_dt_cut;

  /// timestep computed by computeDT
  Real & _new_dt;

  /// Pointer to EventInserter UserObject
  const EventInserter * _inserter_ptr;
};

#endif /* EVENTTIMESTEPPER_H */
