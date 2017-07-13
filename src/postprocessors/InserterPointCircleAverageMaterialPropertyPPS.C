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

#include "InserterPointCircleAverageMaterialPropertyPPS.h"
#include "InserterPointCircleAverageMaterialProperty.h"

template<>
InputParameters validParams<InserterPointCircleAverageMaterialPropertyPPS>()
{
  InputParameters params = validParams<GeneralPostprocessor>();

  params.addRequiredParam<UserObjectName>("user_object", "The name of the InserterPointCircleAverageMaterialProperty user object");
  params.addParam<unsigned int>("entry", 0, "Which entry when multiple InserterPointCircleAverageMaterialProperty values are being computed. Default is 0.");

  return params;
}

InserterPointCircleAverageMaterialPropertyPPS::InserterPointCircleAverageMaterialPropertyPPS(const InputParameters & parameters) :
    GeneralPostprocessor(parameters),
    _uo(getUserObject<InserterPointCircleAverageMaterialProperty>("user_object")),
    _entry(getParam<unsigned int>("entry"))
{
}

Real
InserterPointCircleAverageMaterialPropertyPPS::getValue()
{
  return _uo.averageValue(_entry);
}
