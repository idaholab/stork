[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 1
  ny = 1
  xmin = 0
  xmax = 1
  ymin = 0
  ymax = 1
[]


[GlobalParams]
  richardsVarNames_UO = PPNames
[]

[UserObjects]
  [./PPNames]
    type = RichardsVarNames
    richards_vars = pressure
  [../]
  [./DensityConstBulk]
    type = RichardsDensityConstBulk
    dens0 = 1
    bulk_mod = 1
  [../]
  [./SeffVG]
    type = RichardsSeff1VG
    m = 0.5
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
    sum_s_res = 0.2
  [../]
  [./SUPGstandard]
    type = RichardsSUPGstandard
    p_SUPG = 0.1
  [../]
[]

[Variables]
  [./pressure]
    order = FIRST
    family = LAGRANGE
    [./InitialCondition]
      type = FunctionIC
      function = initial_pressure
    [../]
  [../]
[]

[AuxVariables]
  [./left_flux]
  [../]
  [./right_flux]
  [../]
[]

[AuxKernels]
  [./left_flux]
    type = BAHalfCubicSinkAux
    variable = left_flux
    pressure_var = pressure
    centre = 1.5
    max = 2
    cutoff = -1
  [../]
  [./right_flux]
    type = BAHalfCubicSinkAux
    variable = right_flux
    pressure_var = pressure
    centre = 1.5
    max = 2
    cutoff = -1
  [../]
[]


[Functions]
  [./initial_pressure]
    type = ParsedFunction
    value = 2
  [../]

  [./mass_bal_fcn]
    type = ParsedFunction
    value = abs((mi-lfout-rfout-mf)/2/(mi+mf))
    vars = 'mi mf lfout rfout'
    vals = 'mass_init mass_fin left_flux_out right_flux_out'
  [../]
[]

[Postprocessors]
  [./mass_init]
    type = RichardsMass
    variable = pressure
    execute_on = timestep_begin
  [../]
  [./mass_fin]
    type = RichardsMass
    variable = pressure
    execute_on = timestep_end
  [../]
  [./left_flux_out]
    type = BAHalfCubicSinkFlux
    boundary = left
    variable = pressure
    centre = 1.5
    max = 2
    cutoff = -1
  [../]
  [./right_flux_out]
    type = BAHalfCubicSinkFlux
    boundary = right
    variable = pressure
    centre = 1.5
    max = 2
    cutoff = -1
  [../]
  [./one]
    type = FunctionValuePostprocessor
    function = 1
    outputs = ''
  [../]
  [./total_flux_out]
    type = BASumPostprocessor
    w = 'one one'
    postprocessors = 'left_flux_out right_flux_out'
  [../]
  [./p0]
    type = PointValue
    point = '0 0 0'
    variable = pressure
  [../]
  [./left_flux_val]
    type = PointValue
    point = '0 0.5 0'
    variable = left_flux
  [../]
  [./right_flux_val]
    type = PointValue
    point = '1 0.5 0'
    variable = right_flux
  [../]
  [./mass_bal]
    type = FunctionValuePostprocessor
    function = mass_bal_fcn
  [../]
[]

[BCs]
  [./left_flux]
    type = BAHalfCubicSink
    boundary = left
    variable = pressure
    centre = 1.5
    max = 2
    cutoff = -1
  [../]
  [./right_flux]
    type = BAHalfCubicSink
    boundary = right
    variable = pressure
    centre = 1.5
    max = 2
    cutoff = -1
  [../]
[]

[Kernels]
  active = 'richardst'
  [./richardst]
    type = RichardsMassChange
    variable = pressure
  [../]
[]

[Materials]
  [./rock]
    type = RichardsMaterial
    block = 0
    mat_porosity = 0.1
    mat_permeability = '1E-5 0 0  0 1E-5 0  0 0 1E-5'
    density_UO = DensityConstBulk
    relperm_UO = RelPermPower
    SUPG_UO = SUPGstandard
    sat_UO = Saturation
    seff_UO = SeffVG
    viscosity = 1E-3
    gravity = '-1 0 0'
    linear_shape_fcns = true
  [../]
[]


[Preconditioning]
  [./andy]
    type = SMP
    full = true
    #petsc_options = '-snes_test_display'
    petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it'
    petsc_options_value = 'bcgs bjacobi 1E-12 1E-10 10000'
  [../]
[]

[Executioner]
  type = Transient
  solve_type = Newton
  dt = 4E-3
  end_time = 0.4
[]

[Outputs]
  execute_on = 'timestep_end'
  hide = one
  file_base = s02
  output_initial = false
  csv = true
[]

[Problem]
  use_legacy_uo_initialization = true
[]
