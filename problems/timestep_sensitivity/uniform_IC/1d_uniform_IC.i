# start with a constant initial condition and let a sink (reaction) act on it

[Mesh]
  type = GeneratedMesh
  dim = 1
  xmin = -0.5
  xmax = 0.5
  nx = 10
[]

[Variables]
  [./u]
    order = FIRST
    family = LAGRANGE

    [./InitialCondition]
      type = ConstantIC
      value = 1
    [../]
  [../]
[]

[Kernels]
  [./ie]
    type = TimeDerivative
    variable = u
  [../]

  [./diff]
    type = MaterialDiffusion
    variable = u
    diffusivity_name = diffusivity
  [../]

  [./sink]
    type = MaterialSinkKernel
    variable = u
    diffusivity_name = diffusivity
    sink_strength_name = sink_strength
  [../]
[]

[BCs]
  [./Periodic]
    [./all]
      variable = u
      auto_direction = 'x'
    [../]
  [../]
[]

[Materials]
  [./simple]
    type = GenericConstantMaterial
    block = 0
    prop_names = 'diffusivity sink_strength'
    prop_values = '2.0 0.05'
  [../]
[]

[Executioner]
  type = Transient
  scheme = 'implicit-euler'

  solve_type = 'NEWTON'

  start_time = 0.0
  end_time = 10.0
  dt = 0.1

  verbose = true

  timestep_tolerance = 1.0e-12

  # serial only!
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'

[]

[Preconditioning]
  [./smp]
    type = SMP
    full = true
  [../]
[]

[Postprocessors]
  [./average]
    type = ElementAverageValue
    variable = u
    execute_on = 'initial timestep_end'
  [../]
[]

[Outputs]
  [./csv]
    type = CSV
    execute_on = 'initial final'
  [../]
[]
