/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/


#include "BAQuantity.h"

template<>
InputParameters validParams<BAQuantity>()
{
  InputParameters params = validParams<GeneralUserObject>();

  return params;
}

BAQuantity::BAQuantity(const InputParameters & parameters) :
    GeneralUserObject(parameters),
    _quant(0)
{
}

BAQuantity::~BAQuantity()
{
}

void
BAQuantity::zero()
{
  _quant = 0;
}

void
BAQuantity::record(Real contrib)
{
  _quant = contrib;
}

void
BAQuantity::initialize()
{
}

void
BAQuantity::execute()
{
}

void
BAQuantity::finalize()
{
  Real themax = _quant;
  Real themin = _quant;
  gatherMax(themax);
  gatherMin(themin);
  if (themax == themin)
    _quant = themax; // single-processor, or all processors own the point
  else if (themax > 0 && themin == 0.0)
    _quant = themax; // _quant must actually be a positive number
  else if (themax == 0.0 && themin < 0)
    _quant = themin; // _quant must actually be a negative number
  else
    mooseError("BAQuantity has themax = ", themax, " and themin = ", themin, "\n");
}

Real
BAQuantity::getValue() const
{
  return _quant;
}
