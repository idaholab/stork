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

#ifndef CIRCLEAVERAGEMATERIALPROPERTYAUX_H
#define CIRCLEAVERAGEMATERIALPROPERTYAUX_H

#include "AuxKernel.h"

//Forward Declarations
class CircleAverageMaterialPropertyAux;
class CircleAverageMaterialProperty;

template<>
InputParameters validParams<CircleAverageMaterialPropertyAux>();

class CircleAverageMaterialPropertyAux : public AuxKernel
{
public:
  CircleAverageMaterialPropertyAux(const InputParameters & parameters);

protected:
  virtual Real computeValue();

  const CircleAverageMaterialProperty & _uo;
  const Real _radius;
};

#endif // CIRCLEAVERAGEMATERIALPROPERTYAUX_H
