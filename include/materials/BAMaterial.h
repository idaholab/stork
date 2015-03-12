/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/


#ifndef BAMATERIAL_H
#define BAMATERIAL_H

#include "RichardsMaterial.h"

//Forward Declarations
class BAMaterial;

template<>
InputParameters validParams<BAMaterial>();

/*
 * Material designed to work well with the BA project which has a lot of different zones,
 * so is not suitable for dividing into blocks.
 *
 * permeability = insitu_value * Exp(-decay*depth) * 10**change
 *
 * where "insitu" and "change" depend on the zones.  The zones are
 * defined through AuxVariables.
 */
class BAMaterial : public RichardsMaterial
{
public:
  BAMaterial(const std::string & name, InputParameters parameters);

protected:

  virtual void computeProperties();


private:

  VariableValue & _insitu_perm_zone;
  std::vector<Real> _kh;
  std::vector<Real> _kv;

  VariableValue & _depth;
  Real _decayh;
  Real _decayv;

  VariableValue & _change_perm_zone;
  std::vector<Function *> _change_kh;
  std::vector<Function *> _change_kv;

};

#endif //BAMATERIAL_H
