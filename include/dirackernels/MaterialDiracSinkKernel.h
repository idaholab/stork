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

#ifndef MATERIALDIRACSINKKERNEL_H
#define MATERIALDIRACSINKKERNEL_H

// Moose Includes
#include "DiracKernel.h"

//Forward Declarations
class MaterialDiracSinkKernel;

template<>
InputParameters validParams<MaterialDiracSinkKernel>();

class MaterialDiracSinkKernel : public DiracKernel
{
public:
  MaterialDiracSinkKernel(const InputParameters & parameters);

  virtual void addPoints();
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

protected:
  const MaterialProperty<Real> & _diffusivity;

  const MaterialProperty<Real> & _sink_strength;

  const Point _point;

  const PostprocessorValue & _volume;
};

#endif //MATERIALDIRACSINKKERNEL_H
