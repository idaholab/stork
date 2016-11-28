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

#include "RandomCircleMarker.h"
#include "MooseMesh.h"

template<>
InputParameters validParams<RandomCircleMarker>()
{
  InputParameters params = validParams<Marker>();
  params.addRequiredParam<Real>("radius", "Distance from the center of the circle to mark elements");
  params.addParam<unsigned int>("seed", 3, "Seed the random number generator.");

  return params;
}


RandomCircleMarker::RandomCircleMarker(const InputParameters & parameters) :
    Marker(parameters),
    _r(getParam<Real>("radius")),
    _p(0),
    _marker_value(REFINE)
{
  _random.seed(_tid, getParam<unsigned int>("seed"));
}

void
RandomCircleMarker::initialSetup()
{
  setup();
}

void
RandomCircleMarker::timestepSetup()
{
  setup();
}

Marker::MarkerValue
RandomCircleMarker::computeElementMarker()
{
  // get element center
  Point centroid = _current_elem->centroid();

  // refine element if we are near element and it is time to refine
  if ((((centroid-_p).size() < _r) || (_current_elem->contains_point(_p))) && (_marker_value == REFINE))
    return _marker_value;

  // otherwise COARSEN
  return COARSEN;
}

void
RandomCircleMarker::setup()
{
  if (_fe_problem.timeStep() % 2 == 0) // even time step
    _marker_value = REFINE;
  else  // odd time step
    _marker_value = COARSEN;

  Real x, y, z = 0.0;

  x = _random.rand(_tid);

  if (_mesh.dimension() > 1)
    y = _random.rand(_tid);

  if (_mesh.dimension() > 2)
    z = _random.rand(_tid);

  _p = Point(x, y, z);
}
