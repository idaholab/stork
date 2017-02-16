/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/


#include "BAMaterial.h"
#include "RotationMatrix.h"
#include "Function.h"

// libmesh includes
#include "libmesh/quadrature.h"



template<>
InputParameters validParams<BAMaterial>()
{
  InputParameters params = validParams<RichardsMaterial>();
  params.addRequiredCoupledVar("insitu_perm_zone", "An auxillary variable the defines the zone for insitu permeability.  For each element, the zone must be a nonnegative integer");
  params.addRequiredParam<std::vector<Real> >("kh", "A vector of horizontal insitu permeability values.  For insitu_perm_zone=0, the first of these is used.  For insitu_perm_zone=1, the second is used.  Etc.  You must ensure that for each insitu_perm_zone there is a value!");
  params.addRequiredParam<std::vector<Real> >("kv", "A vector of vertical insitu permeability values.  For insitu_perm_zone=0, the first of these is used.  For insitu_perm_zone=1, the second is used.  Etc.  You must ensure that for each insitu_perm_zone there is a value!");

  params.addRequiredCoupledVar("insitu_por_zone", "An auxillary variable the defines the zone for insitu porosity.  For each element, the zone must be a nonnegative integer");
  params.addRequiredParam<std::vector<Real> >("por", "A vector of horizontal insitu pporosity values.  For insitu_perm_zone=0, the first of these is used.  For insitu_perm_zone=1, the second is used.  Etc.  You must ensure that for each insitu_por_zone there is a value!");


  params.addCoupledVar("depth", 0, "An auxillary variable the defines depth of an element.  This is used to modify the insitu permeabilities.  If not given then it defaults to zero");
  params.addParam<Real>("decayh", 0.0, "The insitu horizontal permeabilities will be multiplied by Exp(-decayh*depth)");
  params.addParam<Real>("decayv", 0.0, "The insitu vertical permeabilities will be multiplied by Exp(-decayv*depth)");
  params.addParam<Real>("decayp", 0.0, "The insitu porosities will be multiplied by Exp(-decayp*depth)");

  params.addRequiredCoupledVar("change_perm_zone", "An auxillary variable the defines the zone for changes in permeability.  For each element, the zone must be a nonnegative integer");
  params.addRequiredParam<std::vector<FunctionName> >("change_kh", "A vector of changes in horizontal permeability.  For change_perm_zone=0, the first of these is used.  For change_perm_zone=1, the second is used.  Etc. permeability = kh*Exp(-decayh*depth)*10**change_kh.   You must ensure that for each change_perm_zone there is a value!");
  params.addRequiredParam<std::vector<FunctionName> >("change_kv", "A vector of changes in vertical permeability.  For change_perm_zone=0, the first of these is used.  For change_perm_zone=1, the second is used.  Etc. permeability = kv*Exp(-decayv*depth)*10**change_kv.   You must ensure that for each change_perm_zone there is a value!");

  params.addParam<Real>("max_kh_change", 15.0, "The maximum change in horizontal permeabilities allowed");
  params.addParam<Real>("max_kv_change", 15.0, "The maximum change in vertical permeabilities allowed");

  params.addParam<Real>("max_kh", 1.0E30, "The maximum horizontal permeability allowed");
  params.addParam<Real>("max_kv", 1.0E30, "The maximum vertical permeability allowed");
  params.addParam<Real>("max_por", 1.0, "The maximum porosity allowed");

  params.addParam<Real>("min_kh", 0, "The maximum horizontal permeability allowed");
  params.addParam<Real>("min_kv", 0, "The maximum vertical permeability allowed");
  params.addParam<Real>("min_por", 0, "The maximum porosity allowed");

  params.addCoupledVar("transverse_directions", 0, "List of AuxVariables (of type BATransverseDirectionAux) that calculate the transverse direction for each element");

  params.addClassDescription("Material designed to work well with the BA project which has a lot of different zones, so is not suitable for dividing into blocks");
  return params;
}

BAMaterial::BAMaterial(const InputParameters & parameters) :
    RichardsMaterial(parameters),
    _insitu_perm_zone(coupledValue("insitu_perm_zone")),
    _kh(getParam<std::vector<Real> >("kh")),
    _kv(getParam<std::vector<Real> >("kv")),

    _insitu_por_zone(coupledValue("insitu_por_zone")),
    _por(getParam<std::vector<Real> >("por")),

    _depth(coupledValue("depth")),
    _decayh(getParam<Real>("decayh")),
    _decayv(getParam<Real>("decayv")),
    _decayp(getParam<Real>("decayp")),

    _max_kh_change(getParam<Real>("max_kh_change")),
    _max_kv_change(getParam<Real>("max_kv_change")),

    _max_kh(getParam<Real>("max_kh")),
    _max_kv(getParam<Real>("max_kv")),
    _max_por(getParam<Real>("max_por")),

    _min_kh(getParam<Real>("min_kh")),
    _min_kv(getParam<Real>("min_kv")),
    _min_por(getParam<Real>("min_por")),

    _change_perm_zone(coupledValue("change_perm_zone")),

    _use_transverse_direction(false)
{
  if (_kh.size() != _kv.size())
    mooseError("BAMaterial: For clarity kh and kv must have the same size\n");

  const std::vector<FunctionName> & fcn_namesh(getParam<std::vector<FunctionName> >("change_kh"));
  const std::vector<FunctionName> & fcn_namesv(getParam<std::vector<FunctionName> >("change_kv"));

  const unsigned num = fcn_namesh.size();
  if (num != fcn_namesv.size())
    mooseError("BAMaterial: For clarity change_kh and change_kv must have the same size\n");

  _change_kh.resize(num);
  _change_kv.resize(num);
  for (unsigned i = 0 ; i < num ; ++i)
  {
    _change_kh[i] = &getFunctionByName(fcn_namesh[i]);
    _change_kv[i] = &getFunctionByName(fcn_namesv[i]);
  }

  _trans_dir.resize(3);
  if (isParamValid("transverse_directions") && (coupledComponents("transverse_directions") == 3))
  {
    _use_transverse_direction = true;
    for (unsigned i = 0 ; i < 3 ; ++i)
      _trans_dir[i] = &coupledValue("transverse_directions", i);
  }
}

void
BAMaterial::computeProperties()
{

  // compute porepressures and effective saturations
  // with algorithms depending on the _richards_name_UO.var_types()
  computePandSeff();

  // all of the following are assumed to be constant throughout the material
  unsigned int i_zone = (unsigned) _insitu_perm_zone[0] + 0.5;
  unsigned int ipor_zone = (unsigned) _insitu_por_zone[0] + 0.5;
  unsigned int ch_zone = (unsigned) _change_perm_zone[0] + 0.5;

  // error checking
  if (_insitu_perm_zone[0] < 0)
    mooseError2("BAMaterial: insitu_perm_zone is ", _insitu_perm_zone[0], " which is negative!\n");
  if (i_zone >= _kh.size())
    mooseError2("BAMaterial: insitu_perm_zone is ", i_zone, " which is not smaller than the size of kh!\n");
  if (_insitu_por_zone[0] < 0)
    mooseError2("BAMaterial: insitu_por_zone is ", _insitu_por_zone[0], " which is negative!\n");
  if (ipor_zone >= _por.size())
    mooseError2("BAMaterial: insitu_por_zone is ", ipor_zone, " which is not smaller than the size of por!\n");
  if (_change_perm_zone[0] < 0)
    mooseError2("BAMaterial: change_perm_zone is ", _change_perm_zone[0], " which is negative!\n");
  if (ch_zone >= _change_kh.size())
    mooseError2("BAMaterial: change_perm_zone is ", ch_zone, " which is not smaller than the size of change_kh!\n");

  // all of the following are assumed to be constant throughout the material
  // This is a requirement in other parts of Richards
  Real depth = _depth[0];
  Real change_kh = _change_kh[ch_zone]->value(_t, _q_point[0]);
  change_kh = std::min(change_kh, _max_kh_change);
  Real change_kv = _change_kv[ch_zone]->value(_t, _q_point[0]);
  change_kv = std::min(change_kv, _max_kv_change);
  Real porosity = _por[ipor_zone]*std::exp(-_decayp*depth);
  Real permh = _kh[i_zone]*std::exp(-_decayh*depth)*std::pow(10, change_kh);
  Real permv = _kv[i_zone]*std::exp(-_decayv*depth)*std::pow(10, change_kv);

  permh = std::min(std::max(permh, _min_kh), _max_kh);
  permv = std::min(std::max(permv, _min_kv), _max_kv);
  porosity = std::min(std::max(porosity, _min_por), _max_por);

  // porosity, permeability, and gravity
  for (unsigned int qp = 0; qp < _qrule->n_points(); qp++)
  {
    _porosity[qp] = porosity;
    _porosity_old[qp] = porosity;

    _permeability[qp] = RealTensorValue();
    _permeability[qp](0, 0) = _permeability[qp](1, 1) = permh;
    _permeability[qp](2, 2) = permv;

    _gravity[qp] = _material_gravity;
  }

  if (_use_transverse_direction)
  {
    RealVectorValue av_norm((*_trans_dir[0])[0], (*_trans_dir[1])[0], (*_trans_dir[2])[0]); // evaluate at the zeroth qp because these should be CONSTANT MONOMIAL
    if (!(av_norm(0) == 0 && av_norm(1) == 0 && av_norm(2) == 0))
    {
      RealTensorValue rot = RotationMatrix::rotVecToZ(av_norm);
      for (unsigned int qp = 0; qp < _qrule->n_points(); qp++)
        _permeability[qp] = (rot.transpose()*_permeability[qp])*rot;
    }
  }


  // compute "derived" quantities -- those that depend on P and Seff --- such as density, relperm
  for (unsigned int qp = 0; qp < _qrule->n_points(); qp++)
    computeDerivedQuantities(qp);


  // compute certain second derivatives of the derived quantities
  // These are needed in Jacobian calculations if doing SUPG
  for (unsigned int qp = 0; qp < _qrule->n_points(); qp++)
    compute2ndDerivedQuantities(qp);


  // Now for SUPG itself
  for (unsigned int qp = 0; qp < _qrule->n_points(); qp++)
    zeroSUPG(qp);

  // the following saves computational effort if all SUPG is trivial
  bool trivial_supg = true;
  for (unsigned int i = 0; i < _num_p; ++i)
    trivial_supg = trivial_supg && (*_material_SUPG_UO[i]).SUPG_trivial();
  if (trivial_supg)
    return;
  else
    computeSUPG();

}
