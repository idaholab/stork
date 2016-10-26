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

#include "MaterialDiracSinkKernel.h"
#include "MooseMesh.h"

template<>
InputParameters validParams<MaterialDiracSinkKernel>()
{
  InputParameters params = validParams<DiracKernel>();
  params.addRequiredParam<MaterialPropertyName>("diffusivity_name", "Name of the material property for diffusion coefficient.");
  params.addRequiredParam<MaterialPropertyName>("sink_strength_name", "Name of the material property for sink strength.");
  params.addRequiredParam<Point>("point", "Location of the dirac sink.");
  params.addRequiredParam<PostprocessorName>("volume_pps", "The name of the VolumePostprocessor.");
  return params;
}

MaterialDiracSinkKernel::MaterialDiracSinkKernel(const InputParameters & parameters) :
    DiracKernel(parameters),
    _diffusivity(getMaterialProperty<Real>("diffusivity_name")),
    _sink_strength(getMaterialProperty<Real>("sink_strength")),
    _point(getParam<Point>("point")),
    _volume(getPostprocessorValue("volume_pps"))
{
}

void
MaterialDiracSinkKernel::addPoints()
{
  addPoint(_point);
}

Real
MaterialDiracSinkKernel::computeQpResidual()
{
  return _volume*_sink_strength[_qp]*_diffusivity[_qp]*_test[_i][_qp]*_u[_qp];
}

Real
MaterialDiracSinkKernel::computeQpJacobian()
{
  return _volume*_sink_strength[_qp]*_diffusivity[_qp]*_test[_i][_qp]*_phi[_j][_qp];
}
