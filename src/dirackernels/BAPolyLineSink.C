#include "BAPolyLineSink.h"

template<>
InputParameters validParams<BAPolyLineSink>()
{
  InputParameters params = validParams<RichardsPolyLineSink>();
  params.addRequiredParam<FunctionName>("p0", "Base porepressure.  SinkFlux = _sink_func(p - p0)");
  params.addParam<Real>("start_time", -1.0E36, "Only if t>=start_time will the polyline sink be active");
  params.addParam<Real>("end_time", 1.0E36, "Only if t<=end_time will the polyline sink be active");
  params.addRequiredParam<UserObjectName>("porepressure_val_UO", "The porepressure at the Dirac point will be recorded into this user object.  Must be a UserObject of type=BAQuantity");
  params.addParam<bool>("cache_here", true, "Cache elements: only needed until DiracKernel is OK");
  params.addClassDescription("Approximates a polyline sink in the mesh by using a number of point sinks whose positions are read from a file.  This extends RichardsPolyLineSink by adding a function for the base porepressure p0, as well as start_time and end_time.");
  return params;
}

BAPolyLineSink::BAPolyLineSink(const InputParameters & parameters) :
    RichardsPolyLineSink(parameters),
    _p0(getFunction("p0")),
    _start_time(getParam<Real>("start_time")),
    _end_time(getParam<Real>("end_time")),
    _cache_here(getParam<bool>("cache_here")),
    _have_added_points(false),
    _point_pp(const_cast<BAQuantity &>(getUserObject<BAQuantity>("porepressure_val_UO")))
{
  _the_elems.resize(0);
}

void
BAPolyLineSink::addPoints()
{
  _total_outflow_mass.zero();
  _point_pp.zero();

  // For the _cache_here=true case I'm assuming:
  // (1) the points aren't moving around
  // (2) we're not using mesh adaptivity.
  // then i can cache the elements until DiracKernel's caching gets fixed

  if (!_have_added_points || !_cache_here)
  {
    _the_elems.resize(_zs.size());
    for (unsigned int i = 0; i < _zs.size(); i++)
      _the_elems[i] = addPoint(Point(_xs[i], _ys[i], _zs[i]), i);
  }
  else
    // we are caching the elements in this class (cache_here == true)
    // and we've added the points already in a previous call to this function
    for (unsigned int i = 0; i < _zs.size(); i++)
      addPoint(_the_elems[i], Point(_xs[i], _ys[i], _zs[i]), i);

  _have_added_points = true;
}

Real
BAPolyLineSink::computeQpResidual()
{
  Real test_fcn = _test[_i][_qp];
  Real flow = 0;
  if ((_t >= _start_time) && (_t <= _end_time))
    flow = test_fcn*_sink_func.sample(_pp[_qp][_pvar] - _p0.value(_t, _q_point[_qp]));
  _total_outflow_mass.add(flow*_dt);
  _point_pp.record(_pp[_qp][_pvar]);
  return flow;
}

Real
BAPolyLineSink::computeQpJacobian()
{
  Real test_fcn = _test[_i][_qp];
  if ((_t >= _start_time) && (_t <= _end_time))
    return test_fcn*_sink_func.sampleDerivative(_pp[_qp][_pvar] - _p0.value(_t, _q_point[_qp]))*_dpp_dv[_qp][_pvar][_pvar]*_phi[_j][_qp];
  else
    return 0.0;
}

Real
BAPolyLineSink::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (_richards_name_UO.not_richards_var(jvar))
    return 0.0;
  unsigned int dvar = _richards_name_UO.richards_var_num(jvar);
  Real test_fcn = _test[_i][_qp];
  if ((_t >= _start_time) && (_t <= _end_time))
    return test_fcn*_sink_func.sampleDerivative(_pp[_qp][_pvar] - _p0.value(_t, _q_point[_qp]))*_dpp_dv[_qp][_pvar][dvar]*_phi[_j][_qp];
  else
    return 0.0;
}
