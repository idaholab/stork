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
#ifndef BASUMPOSTPROCESSOR_H
#define BASUMPOSTPROCESSOR_H

#include "GeneralPostprocessor.h"

class BASumPostprocessor;

template<>
InputParameters validParams<BASumPostprocessor>();

/**
 * Computes the difference between two postprocessors
 *
 * result = value1 - value2
 */
class BASumPostprocessor : public GeneralPostprocessor
{
public:
  BASumPostprocessor(const InputParameters & parameters);
  virtual ~BASumPostprocessor();

  virtual void initialize();
  virtual void execute();
  virtual PostprocessorValue getValue();
  virtual void threadJoin(const UserObject & uo);

protected:

  std::vector<Real> _w;
  unsigned int _num_w;
  std::vector<const PostprocessorValue *> _pps_vals;
};


#endif /* BASUMPOSTPROCESSOR_H */
