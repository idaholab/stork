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

#ifndef DIRACSINKMAPKERNEL_H
#define DIRACSINKMAPKERNEL_H

// Moose Includes
#include "DiracKernel.h"

//Forward Declarations
class DiracSinkMapKernel;
class SinkMapUserObject;

template<>
InputParameters validParams<DiracSinkMapKernel>();

class DiracSinkMapKernel : public DiracKernel
{
public:
  DiracSinkMapKernel(const InputParameters & parameters);

  virtual void addPoints();
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

protected:
  const SinkMapUserObject & _sink_map_uo;

  const MaterialProperty<Real> & _diffusivity;

  Real _num_points;

  Real _sink_strength;

  Real _volume;
};

#endif //DIRACSINKMAPKERNEL_H
