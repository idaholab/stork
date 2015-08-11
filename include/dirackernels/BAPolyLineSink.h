#ifndef BAPOLYLINESINK_H
#define BAPOLYLINESINK_H

#include "RichardsPolyLineSink.h"
#include "BAQuantity.h"
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
  BAPolyLineSink(const InputParameters & parameters);

  virtual void addPoints();

  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  virtual Real computeQpOffDiagJacobian(unsigned int jvar);


protected:

  Function & _p0;

private:

  // only for _start_time <= t <= _end_time will the BAPolyLineSink be active
  Real _start_time;

  // only for _start_time <= t <= _end_time will the BAPolyLineSink be active
  Real _end_time;

  bool _cache_here;

  bool _have_added_points;

  /// This is where the Dirac point's porepressure is recorded
  BAQuantity & _point_pp;

  std::vector<const Elem *> _the_elems;


};

#endif //BAPOLYLINESINK_H
