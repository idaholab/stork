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
  params.addRequiredParam<MaterialPropertyName>("diffusivity_name", "Name of the material property for diffusion coefficient.");
  params.addRequiredParam<PostprocessorName>("volume_pps", "The name of the VolumePostprocessor.");
  return params;
}

DiracSinkMapKernel::DiracSinkMapKernel(const InputParameters & parameters) :
    DiracKernel(parameters),
    _sink_map_uo(getUserObject<SinkMapUserObject>("sink_map_user_object")),
    _diffusivity(getMaterialProperty<Real>("diffusivity_name")),
    _volume(getPostprocessorValue("volume_pps"))
{
  if (_sink_map_uo.getPlacement() == "corner")
    mooseError("Dirac sinks do not work at the corners, yet... Set 'sink_placement = inside' in SinkMapUserObject.");
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

Real
DiracSinkMapKernel::computeQpJacobian()
{
  return _volume*_sink_strength*_diffusivity[_qp]*_test[_i][_qp]*_phi[_j][_qp]/_num_points;
}
