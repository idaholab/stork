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
#ifndef MATERIALDIFFUSION_H
#define MATERIALDIFFUSION_H

#include "Kernel.h"
#include "MaterialProperty.h"

// Forward Declaration
class MaterialDiffusion;

template<>
InputParameters validParams<MaterialDiffusion>();


class MaterialDiffusion : public Kernel
{
public:
  MaterialDiffusion(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();

  virtual Real computeQpJacobian();

  std::string _prop_name;
  const MaterialProperty<Real> * _diff;
};

#endif //MATERIALDIFFUSION_H
