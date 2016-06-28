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

#include "RandomPointUserObject.h"
#include "MooseRandom.h"

#include <time.h>  // for time function to seed random number generator

template<>
InputParameters validParams<RandomPointUserObject>()
{
  InputParameters params = validParams<GeneralUserObject>();
  params += validParams<RandomInterface>();

  params.addParam<unsigned int>("seed", 0, "The seed for the random number generator");
  return params;
}

RandomPointUserObject::RandomPointUserObject(const InputParameters & parameters) :
    GeneralUserObject(parameters),
    RandomInterface(parameters, _fe_problem, _tid, false),
    _mesh(_fe_problem.mesh())
{
  setRandomResetFrequency(EXEC_INITIAL);
  if (parameters.isParamSetByUser("seed"))
    MooseRandom::seed(getParam<unsigned int>("seed"));
  else
    MooseRandom::seed(time(NULL));

  //MooseRandom::seed(getParam<unsigned int>("seed"));
}

Point
RandomPointUserObject::getRandomPoint() const
{
  // Width of domain, some will be zero if mesh is less than 3D
  Real x_length = _mesh.dimensionWidth(0);
  Real y_length = _mesh.dimensionWidth(1);
  Real z_length = _mesh.dimensionWidth(2);

  // Lower coordinate
  Real x_min = _mesh.getMinInDimension(0);
  Real y_min = _mesh.getMinInDimension(1);
  Real z_min = _mesh.getMinInDimension(2);

  // loop until random point is on mesh
  Point random_point;
  while (1)
  {
    random_point = Point(x_min + MooseRandom::rand()*x_length, y_min + MooseRandom::rand()*y_length, z_min + MooseRandom::rand()*z_length);

    // check if point is valid
    std::vector<Point> point_vec(1, random_point);

    // Locate the element and store the id
    // We can't store the actual Element pointer here b/c PointLocatorBase returns a const Elem *
    UniquePtr<PointLocatorBase> pl = _mesh.getMesh().sub_point_locator();
    const Elem * elem = (*pl)(point_vec[0]);

    // make sure point is found, then break out of loop
    if (elem)
      break;
  }

  return random_point;
}
