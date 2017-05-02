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

#ifndef NUMPASTEVENTS_H
#define NUMPASTEVENTS_H

#include "GeneralPostprocessor.h"
#include "EventInserter.h"

class NumPastEvents;

template<>
InputParameters validParams<NumPastEvents>();

/**
 * This PPS just retrieves the value from EventInserter User Object
 */
class NumPastEvents : public GeneralPostprocessor
{
public:
  NumPastEvents(const InputParameters & parameters);
  virtual ~NumPastEvents();

  virtual void initialize() {};  // not used
  virtual void execute() {};  // not used
  virtual Real getValue();

protected:
  const EventInserter & _inserter;
};

#endif /* NUMPASTEVENTS_H */
