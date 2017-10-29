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

#ifndef COUPLEDSINKRATEAUX_H
#define COUPLEDSINKRATEAUX_H

#include "AuxKernel.h"

//Forward Declarations
class CoupledSinkRateAux;

template<>
InputParameters validParams<CoupledSinkRateAux>();

class CoupledSinkRateAux : public AuxKernel
{
public:
  CoupledSinkRateAux(const InputParameters & parameters);

protected:
  virtual Real computeValue();

  const MaterialProperty<Real> & _diffusivity;
  const VariableValue & _v;
  const VariableValue & _sink_strength;
};

#endif // COUPLEDSINKRATEAUX_H
