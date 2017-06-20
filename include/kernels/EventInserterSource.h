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

#ifndef EVENTINSERTERSOURCE_H
#define EVENTINSERTERSOURCE_H

#include "Kernel.h"

//Forward Declarations
class EventInserterSource;
class EventInserterBase;
class GaussianUserObject;

template<>
InputParameters validParams<EventInserterSource>();

/**
 * Define the Kernel for inserting events based on an
 * EventInserter UserObject. Shape of events are based on
 * a GaussianUserObject.
 */
class EventInserterSource : public Kernel
{
public:

  EventInserterSource(const InputParameters & parameters);

protected:
  /**
   * Checks for active event during this timestep and saves event location
   */
  virtual void timestepSetup();

  /**
   * Computes test function * forcing function.
   */
  virtual Real computeQpResidual();

  const EventInserterBase & _inserter;

  const GaussianUserObject * _gaussian_user_object_ptr;

  const bool _use_uniform_source;

  const Real _magnitude;

  const PostprocessorValue & _volume;

  bool _is_event_active;

  Point _active_point;
};

#endif //EVENTINSERTERSOURCE_H
