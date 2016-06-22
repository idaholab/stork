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

// libmesh includes
#include "libmesh/mesh_tools.h"

class EventMarker;

template<>
InputParameters validParams<EventMarker>();

class EventMarker : public Marker
{
public:
  EventMarker(const InputParameters & parameters);
  virtual ~EventMarker(){};

protected:
  virtual void initialSetup();

  virtual void timestepSetup();

  virtual MarkerValue computeElementMarker();

  void checkForEvent();

  const EventInserter & _inserter;

  const GaussianUserObject & _gaussian_uo;

  const Real _marker_radius;

  const bool _verbose;

  const Real _refine_distance;

  bool _event_incoming;

  Point _event_location;

  unsigned int _input_cycles_per_step;
};

#endif /* EVENTMARKER_H */
