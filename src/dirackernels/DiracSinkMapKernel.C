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

#include "DiracSinkMapKernel.h"
#include "SinkMapUserObject.h"
#include "MooseMesh.h"

template<>
InputParameters validParams<DiracSinkMapKernel>()
{
  InputParameters params = validParams<DiracKernel>();
  params.addRequiredParam<UserObjectName>("sink_map_user_object", "The name of the SinkMapUserObject.");
  return params;
}

DiracSinkMapKernel::DiracSinkMapKernel(const InputParameters & parameters) :
    DiracKernel(parameters),
    _sink_map_uo(getUserObject<SinkMapUserObject>("sink_map_user_object")),
    _diffusivity(getMaterialProperty<Real>("diffusivity"))
{
  if (_sink_map_uo.getPlacement() == "corner")
    mooseError("Dirac sinks do not work at the corners, yet... Set 'sink_placement = inside' in SinkMapUserObject.");

  // get problem dimension to normalize the average strength
  // this assumes a CONSTANT, RECTANGULAR domain
  _volume = _mesh.dimensionWidth(0);

  if (_mesh.dimension() > 1)
    _volume *= _mesh.dimensionWidth(1);

  if (_mesh.dimension() > 2)
    _volume *= _mesh.dimensionWidth(2);
}

void
DiracSinkMapKernel::addPoints()
{
  // get list of sink points
  std::vector<Point> sink_location_list = _sink_map_uo.getSinkLocationList();

  // remember how many there are because we need to normalize the residual contribution by this
  _num_points = Real(sink_location_list.size());

  // strength of sinks
  _sink_strength = _sink_map_uo.getStrength();

  // add the points
  for (unsigned int i = 0; i < sink_location_list.size(); ++i)
    addPoint(sink_location_list[i]);
}

Real
DiracSinkMapKernel::computeQpResidual()
{
  return _volume*_sink_strength*_diffusivity[_qp]*_test[_i][_qp]*_u[_qp]/_num_points;
}
