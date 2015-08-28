/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/
#include "BATransverseDirectionAux.h"

template<>
InputParameters validParams<BATransverseDirectionAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addClassDescription("Produces a component of a normal vector.  This vector is normal to the top and bottom of each element of the mesh, assuming the element is a HEX8 or PRISM6, and that the mesh was created using sweeping in the vertical direction.  The normal vector is not normalised and points mostly upwards.");
  params.addRequiredRangeCheckedParam<unsigned int>("index", "index >= 0 & index <= 2", "The component of the normal vector to output");
  return params;
}

BATransverseDirectionAux::BATransverseDirectionAux(const InputParameters & parameters) :
    AuxKernel(parameters),
    _i(getParam<unsigned int>("index"))
{
}

Real
BATransverseDirectionAux::computeValue()
{
  RealVectorValue av_norm(0, 0, 1);

  if (_current_elem->type() == 13 || _current_elem->type() == 10)
  {
    // PRISM6 or HEX8 are the only thing i've coded at the moment

    Point p0 = _current_elem->point(0);
    Point p1 = _current_elem->point(1);
    Point p2 = _current_elem->point(2);
    RealVectorValue v1 = p1 - p0;
    RealVectorValue v2 = p2 - p0;
    RealVectorValue norm; // v2 x v1
    norm(0) = v2(1)*v1(2) - v2(2)*v1(1);
    norm(1) = v2(2)*v1(0) - v2(0)*v1(2);
    norm(2) = v2(0)*v1(1) - v2(1)*v1(0);
    Real len = std::pow(norm(0)*norm(0) + norm(1)*norm(1) + norm(2)*norm(2), 0.5);

    if (norm(2) > 0)
      av_norm = norm/len;
    else
      av_norm = -norm/len;
    
    unsigned int n = _current_elem->n_vertices();
    p0 = _current_elem->point(n-1);
    p1 = _current_elem->point(n-2);
    p2 = _current_elem->point(n-3);
    v1 = p2 - p0;
    v2 = p1 - p0;
    norm(0) = v2(1)*v1(2) - v2(2)*v1(1);
    norm(1) = v2(2)*v1(0) - v2(0)*v1(2);
    norm(2) = v2(0)*v1(1) - v2(1)*v1(0);
    len = std::pow(norm(0)*norm(0) + norm(1)*norm(1) + norm(2)*norm(2), 0.5);

    if (norm(2) > 0)
      av_norm += norm/len;
    else
      av_norm -= norm/len;
  }

  return av_norm(_i);
}

