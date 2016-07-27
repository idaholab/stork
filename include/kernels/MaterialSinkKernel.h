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
#ifndef MATERIALSINKKERNEL_H
#define MATERIALSINKKERNEL_H

#include "Reaction.h"

//Forward Declarations
class MaterialSinkKernel;

template<>
InputParameters validParams<MaterialSinkKernel>();

class MaterialSinkKernel : public Reaction
{
public:
  MaterialSinkKernel(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  const MaterialProperty<Real> & _diffusivity;
  const MaterialProperty<Real> & _sink_strength;
};

#endif //MATERIALSINKKERNEL_H
