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

#ifndef ELEMENTHLEVEL_H
#define ELEMENTHLEVEL_H

#include "AuxKernel.h"

//Forward Declarations
class ElementHLevel;

template<>
InputParameters validParams<ElementHLevel>();

class ElementHLevel : public AuxKernel
{
public:
  ElementHLevel(const InputParameters & parameters);

protected:
  virtual Real computeValue();
};

#endif // ELEMENTHLEVEL_H
