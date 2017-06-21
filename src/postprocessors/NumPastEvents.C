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

#include "NumPastEvents.h"

template<>
InputParameters validParams<NumPastEvents>()
{
  InputParameters params = validParams<GeneralPostprocessor>();

  params.addRequiredParam<UserObjectName>("inserter", "The name of the Inserter user object");

  return params;
}

NumPastEvents::NumPastEvents(const InputParameters & parameters) :
    GeneralPostprocessor(parameters),
    _inserter(getUserObject<EventInserterBase>("inserter"))
{
}

NumPastEvents::~NumPastEvents()
{
}

Real
NumPastEvents::getValue()
{
  return _inserter.getNumPastEvents();
}
