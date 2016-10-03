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

#ifndef SINKMAPAUX_H
#define SINKMAPAUX_H

#include "AuxKernel.h"

//Forward Declarations
class SinkMapAux;
class SinkMapUserObject;

template<>
InputParameters validParams<SinkMapAux>();

class SinkMapAux : public AuxKernel
{
public:
  SinkMapAux(const InputParameters & parameters);

  virtual ~SinkMapAux() {}

protected:
  virtual void precalculateValue();
  virtual Real computeValue();

  const SinkMapUserObject & _sink_map_uo;
  std::vector<Real> _element_sink_map;
};

#endif // SINKMAPAUX_H
