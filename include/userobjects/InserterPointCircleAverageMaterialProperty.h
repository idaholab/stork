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

#ifndef INSERTERPOINTCIRCLEAVERAGEMATERIALPROPERTY_H
#define INSERTERPOINTCIRCLEAVERAGEMATERIALPROPERTY_H

#include "CircleAverageMaterialPropertyBase.h"
#include "MooseMesh.h"

//Forward Declarations
class InserterPointCircleAverageMaterialProperty;
class EventInserterBase;

template<>
InputParameters validParams<InserterPointCircleAverageMaterialProperty>();

/**
 * Computes the average value of a material property in side a circle
 */
class InserterPointCircleAverageMaterialProperty : public CircleAverageMaterialPropertyBase
{
public:
  InserterPointCircleAverageMaterialProperty(const InputParameters & parameters);

  /**
   * Look up average material property value by Point if it is in an Event list.
   */
  Real averageValue(const Point & p) const;

  /**
   * This is called at the beginning of the simulation.
   */
  virtual void initialSetup();

  /**
   * This is called before execute so you can reset any internal data.
   */
  virtual void initialize();

  /**
   * Called on every "object" (like every element or node).
   * In this case, it is called at every quadrature point on every element.
   */
  virtual void execute();

  /**
   * Called when using threading.  You need to combine the data from "y"
   * into _this_ object.
   */
  virtual void threadJoin(const UserObject & y);

  /**
   * Called _once_ after execute has been called all all "objects".
   */
  virtual void finalize();

protected:
  /// An event has an start time and a location
  typedef std::pair<Real, Point> Event;

  /// A list of future time/location pairs
  typedef std::vector<Event> EventList;

  /// Reference to EventInserter UserObject
  const EventInserterBase * _inserter;

  /// Distance to search around inserter points
  const Real _radius;

  /// This vector will hold integral around inserter points
  std::vector<Real> _integral_sum;

  /// This vector will hold volume around inserter points
  std::vector<Real> _volume_sum;

  /// List of events
  EventList _event_list;

  /// Number of events
  unsigned int _Npoints;

};

#endif
