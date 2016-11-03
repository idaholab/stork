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

#ifndef RANDOMCIRCLEMARKER_H
#define RANDOMCIRCLEMARKER_H

#include "Marker.h"
#include "MooseRandom.h"

// libmesh includes
//#include "libmesh/mesh_tools.h"

class RandomCircleMarker;

template<>
InputParameters validParams<RandomCircleMarker>();

class RandomCircleMarker : public Marker
{
public:
  RandomCircleMarker(const InputParameters & parameters);
  virtual ~RandomCircleMarker(){};

protected:
  virtual void initialSetup();
  virtual void timestepSetup();
  virtual MarkerValue computeElementMarker();

  void setup();

  const  Real _r;
  Point _p;
  MarkerValue _marker_value;

  MooseRandom _random;
};

#endif /* RANDOMCIRCLEMARKER_H */
