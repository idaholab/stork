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
#ifndef MATERIALSINKRATEAUX_H
#define MATERIALSINKRATEAUX_H

#include "AuxKernel.h"

//Forward Declarations
class MaterialSinkRateAux;

template<>
InputParameters validParams<MaterialSinkRateAux>();

class MaterialSinkRateAux : public AuxKernel
{
public:
  MaterialSinkRateAux(const InputParameters & parameters);

protected:
  virtual Real computeValue();

  const MaterialProperty<Real> & _diffusivity;
  const MaterialProperty<Real> & _sink_strength;
  const VariableValue & _v;
};

#endif //MATERIALSINKRATEAUX_H
