#include "BAPolyLineSink.h"

template<>
InputParameters validParams<BAPolyLineSink>()
{
  InputParameters params = validParams<RichardsPolyLineSink>();
  params.addParam<FunctionName>("p0", "Base porepressure.  SinkFlux = _sink_func(p - p0)");
  params.addClassDescription("Approximates a polyline sink in the mesh by using a number of point sinks whose positions are read from a file.  This extends RichardsPolyLineSink by adding a function for the base porepressure p0.");
  return params;
}

BAPolyLineSink::BAPolyLineSink(const std::string & name, InputParameters parameters) :
    RichardsPolyLineSink(name, parameters),
    _p0(getFunction("p0"))
{
}

Real
BAPolyLineSink::computeQpResidual()
{
  Real test_fcn = _test[_i][_qp];
  Real flow = test_fcn*_sink_func.sample(_pp[_qp][_pvar] - _p0.value(_t, _q_point[_qp]));
  _total_outflow_mass.add(flow*_dt);
  return flow;
}

Real
BAPolyLineSink::computeQpJacobian()
{
  Real test_fcn = _test[_i][_qp];
  return test_fcn*_sink_func.sampleDerivative(_pp[_qp][_pvar] - _p0.value(_t, _q_point[_qp]))*_dpp_dv[_qp][_pvar][_pvar]*_phi[_j][_qp];
}

Real
BAPolyLineSink::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (_richards_name_UO.not_richards_var(jvar))
    return 0.0;
  unsigned int dvar = _richards_name_UO.richards_var_num(jvar);
  Real test_fcn = _test[_i][_qp];
  return test_fcn*_sink_func.sampleDerivative(_pp[_qp][_pvar] - _p0.value(_t, _q_point[_qp]))*_dpp_dv[_qp][_pvar][dvar]*_phi[_j][_qp];
}
