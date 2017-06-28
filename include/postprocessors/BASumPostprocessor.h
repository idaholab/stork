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
 * This postprocessor will return Sum_over_i(postprocessors_i/w_i)
 */
class BASumPostprocessor : public GeneralPostprocessor
{
public:
  BASumPostprocessor(const InputParameters & parameters);
  virtual ~BASumPostprocessor();

  virtual void initialize();
  virtual void execute();
  virtual PostprocessorValue getValue();

protected:

  unsigned int _num_w;
  std::vector<const PostprocessorValue *> _pps_vals;
  std::vector<const PostprocessorValue *> _w;
};


#endif /* BASUMPOSTPROCESSOR_H */
