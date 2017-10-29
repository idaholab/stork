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

#ifndef CIRCLEMAXORIGINALELEMENTSIZEPPS_H
#define CIRCLEMAXORIGINALELEMENTSIZEPPS_H

#include "GeneralPostprocessor.h"
#include "CircleMaxOriginalElementSize.h"

class CircleMaxOriginalElementSizePPS;

template<>
InputParameters validParams<CircleMaxOriginalElementSizePPS>();

/**
 * This PPS just retrieves the value from CircleMaxOriginalElementSize User Object
 */
class CircleMaxOriginalElementSizePPS : public GeneralPostprocessor
{
public:
  CircleMaxOriginalElementSizePPS(const InputParameters & parameters);

  virtual void initialize() {};  // not used
  virtual void execute() {};  // not used
  virtual Real getValue();

protected:
  const CircleMaxOriginalElementSize & _uo;
  const Point _p;
  const Real _radius;
};

#endif /* CIRCLEMAXORIGINALELEMENTSIZEPPS_H */
