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
#include "BASumPostprocessor.h"

template<>
InputParameters validParams<BASumPostprocessor>()
{
  InputParameters params = validParams<GeneralPostprocessor>();
  params.addRequiredParam<std::vector<PostprocessorName> >("postprocessors", "This postprocessor will return Sum_over_i(postprocessors_i/w_i)");
  params.addRequiredParam<std::vector<PostprocessorName> >("w", "This postprocessor will return Sum_over_i(postprocessors_i/w_i)");
  params.addClassDescription("Returns the linear combination of the postprocessors");

  return params;
}

BASumPostprocessor::BASumPostprocessor(const InputParameters & parameters) :
    GeneralPostprocessor(parameters)
{
  std::vector<PostprocessorName> pps_names(getParam<std::vector<PostprocessorName> >("postprocessors"));
  std::vector<PostprocessorName> w_names(getParam<std::vector<PostprocessorName> >("w"));

  _num_w = w_names.size();
  if (pps_names.size() != _num_w)
    mooseError("BASumPostprocessor: number of postprocessors must equal the number of items in w");

  for (unsigned i = 0 ; i < _num_w ; ++i)
  {
    _w.push_back(&getPostprocessorValueByName(w_names[i]));
    _pps_vals.push_back(&getPostprocessorValueByName(pps_names[i]));
  }
}

BASumPostprocessor::~BASumPostprocessor()
{
}

void
BASumPostprocessor::initialize()
{
}

void
BASumPostprocessor::execute()
{
}

PostprocessorValue
BASumPostprocessor::getValue()
{
  Real val = 0;
  for (unsigned i = 0 ; i < _num_w ; ++i)
    if ((*_pps_vals[i]) != 0)
      val += (*_pps_vals[i])/(*_w[i]);
  return val;
}
