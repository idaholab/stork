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

#ifndef INSERTERPOINTCIRCLEAVERAGEMATERIALPROPERTYPPS_H
#define INSERTERPOINTCIRCLEAVERAGEMATERIALPROPERTYPPS_H

#include "GeneralPostprocessor.h"

class InserterPointCircleAverageMaterialPropertyPPS;
class InserterPointCircleAverageMaterialProperty;

template<>
InputParameters validParams<InserterPointCircleAverageMaterialPropertyPPS>();

/**
 * This PPS just retrieves the value from InserterPointCircleAverageMaterialProperty User Object
 */
class InserterPointCircleAverageMaterialPropertyPPS : public GeneralPostprocessor
{
public:
  InserterPointCircleAverageMaterialPropertyPPS(const InputParameters & parameters);

  virtual void initialize() {};  // not used
  virtual void execute() {};  // not used
  virtual Real getValue();

protected:
  const InserterPointCircleAverageMaterialProperty & _uo;
  const Point _p;
};

#endif /* INSERTERPOINTCIRCLEAVERAGEMATERIALPROPERTYPPS_H */
