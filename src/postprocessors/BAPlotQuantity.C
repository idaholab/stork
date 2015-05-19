/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/


#include "BAPlotQuantity.h"
#include "BAQuantity.h"

template<>
InputParameters validParams<BAPlotQuantity>()
{
  InputParameters params = validParams<GeneralPostprocessor>();
  params.addRequiredParam<UserObjectName>("uo", "user object name that has the total mass value");

  return params;
}

BAPlotQuantity::BAPlotQuantity(const std::string & name, InputParameters parameters) :
    GeneralPostprocessor(name, parameters),
    _total_value(getUserObject<BAQuantity>("uo"))
{
}

BAPlotQuantity::~BAPlotQuantity()
{
}

void
BAPlotQuantity::initialize()
{
}

void
BAPlotQuantity::execute()
{
}

PostprocessorValue
BAPlotQuantity::getValue()
{
  return _total_value.getValue();
}
