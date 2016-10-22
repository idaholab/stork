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

#include "SinkMapAux.h"
#include "SinkMapUserObject.h"

template<>
InputParameters validParams<SinkMapAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredParam<UserObjectName>("sink_map_user_object", "The SinkMapUserObject to retrieve values from.");
  return params;
}

SinkMapAux::SinkMapAux(const InputParameters & parameters) :
    AuxKernel(parameters),
    _sink_map_uo(getUserObject<SinkMapUserObject>("sink_map_user_object")),
    _element_sink_map(0)
{
  if (isNodal())
    mooseError("Cannot use SinkMapAux with Nodal variables");
}

void
SinkMapAux::precalculateValue()
{
  _element_sink_map = _sink_map_uo.getLocalSinkMap(_current_elem);
}

Real
SinkMapAux::computeValue()
{
  return _element_sink_map[_qp];
}
