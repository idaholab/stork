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

#ifndef CIRCLEAVERAGEMATERIALPROPERTY_H
#define CIRCLEAVERAGEMATERIALPROPERTY_H

#include "ElementIntegralUserObject.h"
#include "MooseMesh.h"

//Forward Declarations
class CircleAverageMaterialProperty;
class EventInserter;

template<>
InputParameters validParams<CircleAverageMaterialProperty>();

/**
 * Computes the average value of a material property in side a circle
 */
class CircleAverageMaterialProperty : public ElementIntegralUserObject
{
public:
  CircleAverageMaterialProperty(const InputParameters & parameters);

  /**
   * Given a Point and a radius, return the average value for a material property in that circle
   *
   * Note that accessor functions on UserObjects like this _must_ be const.
   * That is because the UserObject system returns const references to objects
   * trying to use UserObjects.  This is done for parallel correctness.
   *
   * @return The average value of a material property in that circle
   */
  Real averageValue(const Point & p, const Real & radius) const;
  Real averageValue(const unsigned int i) const;

  /**
   * Compute the material property at the quadrature point for the integral
   */
  virtual Real computeQpIntegral();

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
  /// Calculate periodic distance between points
  Real distance(Point p1, Point p2) const;

  /// This will hold the material property of interest
  const MaterialProperty<Real> & _mat_prop;

  /// Variable number for checking periodicity
  const int _periodic_var;

  /// Flag that we're getting a list of points from EventInserter
  const bool _use_inserter_points;

  /// Reference to EventInserter UserObject
  const EventInserter * _inserter;

  /// Distance to search around inserter points
  const Real _radius;

  /// Reference to the mesh
  const MooseMesh & _mesh;

  /// An event has an start time and a location
  typedef std::pair<Real, Point> Event;

  /// A list of future time/location pairs
  typedef std::vector<Event> EventList;

  /// This map will hold the integral of each element
  std::map<dof_id_type, Real> _integral_values;

  /// This map will hold the volume of each element
  std::map<dof_id_type, Real> _volume_values;

  /// This map will hold the centroid of each element
  std::map<dof_id_type, Point> _centroids;

  /// This vector will hold integral around inserter points
  std::vector<Real> _integral_sum;

  /// This vector will hold volume around inserter points
  std::vector<Real> _volume_sum;

  /// List of old events
  EventList _old_event_list;

  /// Length of old events
  unsigned int _Npoints;

};

#endif
