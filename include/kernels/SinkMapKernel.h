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
#ifndef SINKMAPKERNEL_H
#define SINKMAPKERNEL_H

#include "Reaction.h"

//Forward Declarations
class SinkMapKernel;
class SinkMapUserObject;

template<>
InputParameters validParams<SinkMapKernel>();

class SinkMapKernel : public Reaction
{
public:
  SinkMapKernel(const InputParameters & parameters);

protected:
  virtual void precalculateResidual();
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  const SinkMapUserObject & _sink_map_uo;
  const MaterialProperty<Real> & _diffusivity;
  std::vector<Real> _element_sink_map;
};

#endif //SINKMAPKERNEL_H
