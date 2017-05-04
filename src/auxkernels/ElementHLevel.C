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

#include "ElementHLevel.h"

template<>
InputParameters validParams<ElementHLevel>()
{
  InputParameters params = validParams<AuxKernel>();
  return params;
}

ElementHLevel::ElementHLevel(const InputParameters & parameters) :
    AuxKernel(parameters)
{
  if (isNodal())
    mooseError("Cannot use ElementHLevel AuxKernel with Nodal variables.");
}

Real
ElementHLevel::computeValue()
{
  return _current_elem->level();
}
