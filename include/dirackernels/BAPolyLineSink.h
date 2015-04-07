#ifndef BAPOLYLINESINK_H
#define BAPOLYLINESINK_H

#include "RichardsPolyLineSink.h"
#include "Function.h"

//Forward Declarations
class BAPolyLineSink;

template<>
InputParameters validParams<BAPolyLineSink>();

/**
 * Approximates a polyline by a sequence of Dirac Points
 * the mass flux from each Dirac Point is _sink_func as a
 * function of porepressure at the Dirac Point.
 * This extends RichardsPolyLineSink by adding a function for the base porepressure p0:
 * flux = _sink_func(p - p0)
 */
class BAPolyLineSink : public RichardsPolyLineSink
{
public:
  BAPolyLineSink(const std::string & name, InputParameters parameters);

  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  virtual Real computeQpOffDiagJacobian(unsigned int jvar);


protected:

  Function & _p0;
};

#endif //BAPOLYLINESINK_H
