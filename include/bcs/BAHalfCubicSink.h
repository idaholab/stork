/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/


#ifndef BAHALFCUBICSINK
#define BAHALFCUBICSINK

#include "IntegratedBC.h"
#include "RichardsVarNames.h"

// Forward Declarations
class BAHalfCubicSink;

template<>
InputParameters validParams<BAHalfCubicSink>();

/**
 * Applies a fluid sink to the boundary.
 * The sink has strength
 * Denote x = porepressure - centre.  Then
 * Then Flux out = (max/cutoff^3)*(2x + cutoff)(x - cutoff)^2 for cutoff < x < 0.
 * Flux out = max for x >= 0.
 * Flux out = 0 for x <= cutoff.  
 * If a function, _m_func, is used then the flux is multiplied by _m_func.
 * This is typically used for modelling evapotranspiration
 * from the top of a groundwater model
 */
class BAHalfCubicSink : public IntegratedBC
{
public:
  BAHalfCubicSink(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();

  virtual Real computeQpJacobian();

  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  /// Denote x = porepressure - centre.  Then Flux out = (max/cutoff^3)*(2x + cutoff)(x - cutoff)^2 for cutoff < x < 0.  Flux out = max for x >= 0.  Flux out = 0 for x <= cutoff.
  Real _maximum;

  /// Denote x = porepressure - centre.  Then Flux out = (max/cutoff^3)*(2x + cutoff)(x - cutoff)^2 for cutoff < x < 0.  Flux out = max for x >= 0.  Flux out = 0 for x <= cutoff.
  Real _cutoff;

  /// _cutoff*_cutoff*_cutoff
  Real _cutoff3;

  /// Denote x = porepressure - centre.  Then Flux out = (max/cutoff^3)*(2x + cutoff)(x - cutoff)^2 for cutoff < x < 0.  Flux out = max for x >= 0.  Flux out = 0 for x <= cutoff.
  Real _centre;

  /// multiplying function: all fluxes will be multiplied by this
  Function & _m_func;

  /**
   * holds info regarding the names of the Richards variables
   * and methods for extracting values of these variables
   */
  const RichardsVarNames & _richards_name_UO;

  /**
   * the index of this variable in the list of Richards variables
   * held by _richards_name_UO.  Eg
   * if richards_vars = 'pwater pgas poil' in the _richards_name_UO
   * and this kernel has variable = pgas, then _pvar = 1
   * This is used to index correctly into _viscosity, _seff, etc
   */
  unsigned int _pvar;

  /// porepressure (or porepressure vector for multiphase problems)
  const MaterialProperty<std::vector<Real> > & _pp;

  /// d(porepressure_i)/dvariable_j
  const MaterialProperty<std::vector<std::vector<Real> > > & _dpp_dv;
};

#endif //BAHALFCUBICSINK
