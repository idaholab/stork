/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#ifndef BATRANSVERSEDIRECTIONAUX_H
#define BATRANSVERSEDIRECTIONAUX_H

#include "AuxKernel.h"

class BATransverseDirectionAux;

template<>
InputParameters validParams<BATransverseDirectionAux>();

/**
 * BATransverseDirectionAux is produces a component of
 * a normal vector.  This vector is normal to the top
 * and bottom of each element of the mesh, assuming the
 * element is a HEX8 or PRISM6, and that the mesh was
 * created using sweeping in the vertical direction
 * The normal is not normalised, and points mostly upwards
 */
class BATransverseDirectionAux : public AuxKernel
{
public:
  BATransverseDirectionAux(const InputParameters & parameters);
  virtual ~BATransverseDirectionAux() {}

protected:
  virtual Real computeValue();

private:
  const unsigned int _i;
};

#endif //BATRANSVERSEDIRECTIONAUX_H
