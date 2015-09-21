[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 2
  xmin = -1
  xmax = 1
  ny = 2
  ymin = -1
  ymax = 1
[]


[GlobalParams]
  richardsVarNames_UO = PPNames
  density_UO = DensityConstBulk
  relperm_UO = RelPermPower
  SUPG_UO = SUPGnone
  sat_UO = Saturation
  seff_UO = SeffVG
  viscosity = 1E-3
[]

[UserObjects]
  [./PPNames]
    type = RichardsVarNames
    richards_vars = pressure
  [../]
  [./DensityConstBulk]
    type = RichardsDensityConstBulk
    dens0 = 1
    bulk_mod = 1.0E3
  [../]
  [./SeffVG]
    type = RichardsSeff1VG
    m = 0.8
    al = 1 # same deal with PETSc's "constant state"
  [../]
  [./RelPermPower]
    type = RichardsRelPermPower
    simm = 0.0
    n = 2
  [../]
  [./Saturation]
    type = RichardsSat
    s_res = 0.1
    sum_s_res = 0.1
  [../]
  [./SUPGnone]
    type = RichardsSUPGnone
  [../]
[]

[Variables]
  [./pressure]
  [../]
[]

[ICs]
  [./pressure]
    type = ConstantIC
    variable = pressure
    value = 1
  [../]
[]


[Kernels]
  [./richardst]
    type = RichardsMassChange
    variable = pressure
  [../]
[]

[AuxVariables]
  [./i_zone]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./ch_zone]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./por_zone]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  [./i_zone]
    type = ConstantAux
    variable = i_zone
    value = -1
  [../]
  [./ch_zone]
    type = ConstantAux
    variable = ch_zone
    value = -1
  [../]
  [./por_zone]
    type = ConstantAux
    variable = por_zone
    value = 0
  [../]
[]

[Materials]
  [./rock]
    type = BAMaterial
    block = 0
    mat_porosity = 0.5
    mat_permeability = '0 0 0  0 0 0  0 0 0'
    gravity = '0 0 0'
    insitu_perm_zone = i_zone
    kh = '1 2'
    kv = 1
    insitu_por_zone = por_zone
    por = 0.1
    change_perm_zone = ch_zone
    change_kh = 1
    change_kv = 1
    linear_shape_fcns = true
  [../]
[]


[Preconditioning]
  [./andy]
    type = SMP
    full = true
    #petsc_options = '-snes_test_display'
    petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it'
    petsc_options_value = 'bcgs bjacobi 1E-15 1E-10 10000'
  [../]
[]

[Executioner]
  type = Transient
  solve_type = Newton
[]

[Outputs]
  execute_on = 'timestep_end'
  exodus = true
[]
