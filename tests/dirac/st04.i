# checking that the BAQuantity works properly for single-processor
[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 1
  ny = 1
  nz = 1
  xmin = 0
  xmax = 1
  ymin = 0
  ymax = 1
  zmin = 0
  zmax = 1
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
    dens0 = 1000
    bulk_mod = 2E9
  [../]
  [./Seff1VG]
    type = RichardsSeff1VG
    m = 0.8
    al = 1E-5
  [../]
  [./RelPermPower]
    type = RichardsRelPermPower
    simm = 0.0
    n = 2
  [../]
  [./Saturation]
    type = RichardsSat
    s_res = 0
    sum_s_res = 0
  [../]
  [./SUPGstandard]
    type = RichardsSUPGstandard
    p_SUPG = 1E8
  [../]

  [./stream_total_outflow_mass]
    type = RichardsSumQuantity
  [../]
  [./stream_pp_uo]
    type = BAQuantity
  [../]
[]


[Variables]
  active = 'pressure'
  [./pressure]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[ICs]
  [./p_ic]
    type = RandomIC
    variable = pressure
    min = 0
    max = 2E6
    seed = 5
  [../]
[]



[Kernels]
  active = 'richardsf richardst'
  [./richardst]
    type = RichardsMassChange
    variable = pressure
  [../]
  [./richardsf]
    type = RichardsFlux
    variable = pressure
  [../]
[]

[DiracKernels]
  [./stream]
    type = BAPolyLineSink
    pressures = 0
    fluxes = 0
    point_file = st01.stream
    p0 = 1E9
    SumQuantityUO = stream_total_outflow_mass
    porepressure_val_UO = stream_pp_uo
    variable = pressure
  [../]
[]


[Postprocessors]
  [./should_be_zero]
    type = RichardsPlotQuantity
    uo = stream_total_outflow_mass
  [../]
  [./should_be_p0]
    type = BAPlotQuantity
    uo = stream_pp_uo
  [../]
  [./p0]
    type = PointValue
    variable = pressure
    point = '0 0 0'
    execute_on = timestep_end
  [../]
[]



[Materials]
  [./all]
    type = RichardsMaterial
    block = 0
    viscosity = 1E-3
    mat_porosity = 0.1
    mat_permeability = '1E-15 0 0  0 1E-15 0  0 0 1E-15'
    density_UO = DensityConstBulk
    relperm_UO = RelPermPower
    sat_UO = Saturation
    seff_UO = Seff1VG
    SUPG_UO = SUPGstandard
    gravity = '0 0 0'
    linear_shape_fcns = true
  [../]
[]



[Preconditioning]
  [./usual]
    type = SMP
    full = true
    petsc_options = '-snes_converged_reason'
    petsc_options_iname = '-ksp_type -pc_type -snes_rtol -snes_max_it -ksp_max_it'
    petsc_options_value = 'bcgs bjacobi 1E-8 10000 30'
  [../]
[]


[Executioner]
  type = Transient
  end_time = 10
  dt = 1
  solve_type = NEWTON

[]

[Outputs]
  execute_on = 'timestep_end'
  file_base = st04
  output_initial = false
  exodus = false
  csv = true
[]
