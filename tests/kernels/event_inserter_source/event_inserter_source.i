[Mesh]
  type = GeneratedMesh
  dim = 2
  xmin = -1
  xmax = 1
  ymin = -1
  ymax = 1
  nx = 20
  ny = 20
[]

[Variables]
  [./u]
    order = FIRST
    family = LAGRANGE

    [./InitialCondition]
      type = ConstantIC
      value = 0
    [../]
  [../]
[]

[Kernels]
  [./ie]
    type = TimeDerivative
    variable = u
  [../]

  [./diff]
    type = Diffusion
    variable = u
  [../]

  [./event_inserter_source]
    type = EventInserterSource
    variable = u
    inserter = inserter
    gaussian_user_object = gaussian_uo
  [../]
[]

[BCs]
  [./Periodic]
    [./all]
      variable = u
      auto_direction = 'x y'
    [../]
  [../]
[]

[UserObjects]
  [./random_point_uo]
    type = RandomPointUserObject
    seed = 1
  [../]
  [./inserter]
    type = EventInserter
    random_timing = true
    distribution = uniform
    mean = 0.1
    random_point_user_object = random_point_uo
    insert_test = true
    test_time = 0.015
    test_location = '0.1 0.1 0.0'
    seed = 1
    verbose = true
  [../]
  [./gaussian_uo]
    type = GaussianUserObject
    sigma = 0.1
    periodic_variable = u
  [../]
[]


[Executioner]
  type = Transient
  scheme = 'implicit-euler'

  # Preconditioned JFNK (default)
  solve_type = 'PJFNK'

  start_time = 0.0
  end_time = 0.03

  verbose = true

  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'

  [./TimeStepper]
    type = EventTimeStepper
    dt = 0.01
    event_inserter = inserter
  [../]
[]

[Postprocessors]
  [./dt]
    type = TimestepSize
  [../]
  [./integral]
    type = ElementIntegralVariablePostprocessor
    variable = u
  [../]
[]

[Outputs]
  exodus = true
  execute_on = 'initial timestep_end'
[]
