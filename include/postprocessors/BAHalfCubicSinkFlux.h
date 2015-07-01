/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/


#ifndef BAHALFCUBICSINKFLUX_H
#define BAHALFCUBICSINKFLUX_H

#include "SideIntegralVariablePostprocessor.h"
#include "RichardsVarNames.h"

class Function;

//Forward Declarations
class BAHalfCubicSinkFlux;

template<>
InputParameters validParams<BAHalfCubicSinkFlux>();

/**
 * Postprocessor that records the mass flux from porespace to
 * a half-cubic sink.  (Positive if fluid is being removed from porespace.)
 * Denote x = porepressure - centre.  Then
 * Denote x = porepressure - centre.  Then
 * Then Flux out = (max/cutoff^3)*(2x + cutoff)(x - cutoff)^2 for cutoff < x < 0.
 * Flux out = max for x >= 0.
 * Flux out = 0 for x <= cutoff.  
 * If a function, _m_func, is used then the flux is multiplied by _m_func.
 * The result is the flux integrated over the specified sideset.
 */
class BAHalfCubicSinkFlux: public SideIntegralVariablePostprocessor
{
public:
  BAHalfCubicSinkFlux(const std::string & name, InputParameters parameters);

protected:
  virtual Real computeQpIntegral();

  FEProblem & _feproblem;

  /// Denote x = porepressure - centre.  Then Flux out = (max/cutoff^3)*(2x + cutoff)(x - cutoff)^2 for cutoff < x < 0.  Flux out = max for x >= 0.  Flux out = 0 for x <= cutoff.
  Real _maximum;

  /// Denote x = porepressure - centre.  Then Flux out = (max/cutoff^3)*(2x + cutoff)(x - cutoff)^2 for cutoff < x < 0.  Flux out = max for x >= 0.  Flux out = 0 for x <= cutoff.
  Real _cutoff;

  /// _cutoff*_cutoff*_cutoff
  Real _cutoff3;

  /// Denote x = porepressure - centre.  Then Flux out = (max/cutoff^3)*(2x + cutoff)(x - cutoff)^2 for cutoff < x < 0.  Flux out = max for x >= 0.  Flux out = 0 for x <= cutoff.
  Real _centre;

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

  /// the multiplier function: the flux out will be multiplied by this function
  Function & _m_func;

  /// porepressure (or porepressure vector for multiphase problems)
  const MaterialProperty<std::vector<Real> > & _pp;

};

#endif
