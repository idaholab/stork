# Seff User object give the correct value
#
# Here pressure is x where x runs between -5E6 and 5E6

[UserObjects]
  [./Seff1VG]
    type = BASeff1VG
    m = 0.8
    al = 1E-6
  [../]
  [./BASeff1VG]
    type = BASeff1VG
    m = 0.8
    al = 1E-6
    modflow_shift = -1E6
  [../]

  # following are unimportant in this test
  [./PPNames]
    type = RichardsVarNames
    richards_vars = pressure
  [../]
  [./DensityConstBulk]
    type = RichardsDensityConstBulk
    dens0 = 1000
    bulk_mod = 2E6
  [../]
  [./RelPermPower]
    type = RichardsRelPermPower
    simm = 0.10101
    n = 2
  [../]
  [./Saturation]
    type = RichardsSat
    s_res = 0.054321
    sum_s_res = 0.054321
  [../]
  [./SUPGstandard]
    type = RichardsSUPGstandard
    p_SUPG = 1E5
  [../]
[]

[Functions]
  [./initial_pressure]
    type = ParsedFunction
    value = x
  [../]

  [./answer_Seff1VG]
    type = ParsedFunction
    value = (1+max((-x)*al,0)^(1/(1-m)))^(-m)
    vars = 'al m'
    vals = '1E-6 0.8'
  [../]
  [./answer_dSeff1VG]
    type = GradParsedFunction
    direction = '1 0 0'
    value = (1+max((-x)*al,0)^(1/(1-m)))^(-m)
    vars = 'al m'
    vals = '1E-6 0.8'
  [../]
  [./answer_d2Seff1VG]
    type = Grad2ParsedFunction
    direction = '1 0 0'
    value = (1+max((-x)*al,0)^(1/(1-m)))^(-m)
    vars = 'al m'
    vals = '1E-6 0.8'
  [../]

  [./answer_BASeff1VG]
    type = ParsedFunction
    value = (1+max((-x-s)*al,0)^(1/(1-m)))^(-m)
    vars = 'al m s'
    vals = '1E-6 0.8 1E6'
  [../]
  [./answer_dBASeff1VG]
    type = GradParsedFunction
    direction = '1 0 0'
    value = (1+max((-x-s)*al,0)^(1/(1-m)))^(-m)
    vars = 'al m s'
    vals = '1E-6 0.8 -1E6'
  [../]
  [./answer_d2BASeff1VG]
    type = Grad2ParsedFunction
    direction = '1 0 0'
    value = (1+max((-x-s)*al,0)^(1/(1-m)))^(-m)
    vars = 'al m s'
    vals = '1E-6 0.8 -1E6'
  [../]
[]

[AuxVariables]
  [./Seff1VG_Aux]
  [../]
  [./dSeff1VG_Aux]
  [../]
  [./d2Seff1VG_Aux]
  [../]

  [./BASeff1VG_Aux]
  [../]
  [./dBASeff1VG_Aux]
  [../]
  [./d2BASeff1VG_Aux]
  [../]

  [./check_Aux]
  [../]
[]

[AuxKernels]
  [./Seff1VG_AuxK]
    type = RichardsSeffAux
    variable = Seff1VG_Aux
    seff_UO = Seff1VG
    pressure_vars = pressure
  [../]
  [./dSeff1VG_AuxK]
    type = RichardsSeffPrimeAux
    variable = dSeff1VG_Aux
    seff_UO = Seff1VG
    pressure_vars = pressure
    wrtnum = 0
  [../]
  [./d2Seff1VG_AuxK]
    type = RichardsSeffPrimePrimeAux
    variable = d2Seff1VG_Aux
    seff_UO = Seff1VG
    pressure_vars = pressure
    wrtnum1 = 0
    wrtnum2 = 0
  [../]

  [./BASeff1VG_AuxK]
    type = RichardsSeffAux
    variable = BASeff1VG_Aux
    seff_UO = BASeff1VG
    pressure_vars = pressure
  [../]
  [./dBASeff1VG_AuxK]
    type = RichardsSeffPrimeAux
    variable = dBASeff1VG_Aux
    seff_UO = BASeff1VG
    pressure_vars = pressure
    wrtnum = 0
  [../]
  [./d2BASeff1VG_AuxK]
    type = RichardsSeffPrimePrimeAux
    variable = d2BASeff1VG_Aux
    seff_UO = BASeff1VG
    pressure_vars = pressure
    wrtnum1 = 0
    wrtnum2 = 0
  [../]

  [./check_AuxK]
    type = FunctionAux
    variable = check_Aux
    function = answer_BASeff1VG
  [../]
[]

[Postprocessors]
  [./cf_Seff1VG]
    type = NodalL2Error
    function = answer_Seff1VG
    variable = Seff1VG_Aux
  [../]
  [./cf_dSeff1VG]
    type = NodalL2Error
    function = answer_dSeff1VG
    variable = dSeff1VG_Aux
  [../]
  [./cf_d2Seff1VG]
    type = NodalL2Error
    function = answer_d2Seff1VG
    variable = d2Seff1VG_Aux
  [../]

  [./cf_BASeff1VG]
    type = NodalL2Error
    function = answer_BASeff1VG
    variable = BASeff1VG_Aux
  [../]
  [./cf_dBASeff1VG]
    type = NodalL2Error
    function = answer_dBASeff1VG
    variable = dBASeff1VG_Aux
  [../]
  [./cf_d2BASeff1VG]
    type = NodalL2Error
    function = answer_d2BASeff1VG
    variable = d2BASeff1VG_Aux
  [../]
[]



#############################################################################
#
# Following is largely unimportant as we are not running an actual similation
#
#############################################################################
[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 100
  xmin = -5E6
  xmax = 5E6
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

[Kernels]
  active = 'richardsf richardst'
  [./richardst]
    type = RichardsMassChange
    richardsVarNames_UO = PPNames
    variable = pressure
  [../]
  [./richardsf]
    type = RichardsFlux
    richardsVarNames_UO = PPNames
    variable = pressure
  [../]
[]

[Materials]
  [./unimportant_material]
    type = RichardsMaterial
    block = 0
    mat_porosity = 0.1
    mat_permeability = '1E-20 0 0  0 1E-20 0  0 0 1E-20'
    richardsVarNames_UO = PPNames
    density_UO = DensityConstBulk
    relperm_UO = RelPermPower
    sat_UO = Saturation
    seff_UO = Seff1VG
    SUPG_UO = SUPGstandard
    viscosity = 1E-3
    gravity = '0 0 -10'
    linear_shape_fcns = true
  [../]
[]

[Preconditioning]
  [./does_nothing]
    type = SMP
    full = true
    petsc_options = '-snes_converged_reason'
    petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it'
    petsc_options_value = 'bcgs bjacobi 1E50 1E50 10000'
  [../]
[]

[Executioner]
  type = Transient
  solve_type = Newton
  num_steps = 1
  dt = 1E-100
[]

[Outputs]
  execute_on = 'timestep_end'
  active = 'csv'
  file_base = uo3
  [./csv]
    type = CSV
    [../]
  [./exodus]
    type = Exodus
    hide = pressure
  [../]
[]
