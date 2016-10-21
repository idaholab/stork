# start with an initial gaussian in 1D and let it broaden by diffusion

[Mesh]
  type = GeneratedMesh
  dim = 1
  xmin = -0.5
  xmax = 0.5
  nx = 400
[]

[Variables]
  [./u]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[ICs]
  [./gaussian_ic]
    type = FunctionIC
    variable = u
    function = gaussian
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
[]

[BCs]
  [./Periodic]
    [./all]
      variable = u
      auto_direction = 'x'
    [../]
  [../]
[]

[Functions]
  [./gaussian]
    type = ParsedFunction
    vars = sigma
    vals = 0.02
    value = 1.0/sigma/sqrt(2*pi)*exp(-x*x/2/sigma/sigma)
  [../]
[]

[Materials]
  [./simple]
    type = GenericConstantMaterial
    block = 0
    prop_names = 'diffusivity'
    prop_values = '0.001'
  [../]
[]

[Executioner]
  type = Transient
  scheme = 'implicit-euler'

  solve_type = 'NEWTON'

  start_time = 0.0
  end_time = 1.0
  dt = 0.01

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
  [./max]
    type = NodalMaxValue
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
