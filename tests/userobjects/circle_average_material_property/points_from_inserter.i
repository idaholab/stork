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

  [./mat_diff]
    type = MaterialDiffusion
    variable = u
    prop_name = diffusivity
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

[Functions]
  [./one_plus_x]
    type = ParsedFunction
    value = '1+x'
  [../]
[]

[Materials]
  [./gfm]
    type = GenericFunctionMaterial
    prop_names = diffusivity
    prop_values = one_plus_x
    block = 0
  [../]
[]

[UserObjects]
  [./random_point_uo]
    type = RandomPointUserObject
    seed = 1
  [../]
  [./inserter]
    type = EventInserter
    mean = 1.0
    random_point_user_object = random_point_uo
    insert_initial = true
    seed = 3
    verbose = true
    track_old_events = true
    removal_time = 10.0
  [../]
  [./gaussian_uo]
    type = GaussianUserObject
    sigma = 0.1
    use_random_points = true
    random_point_user_object = random_point_uo
    periodic_variable = u
  [../]
  [./circle_average]
    type = CircleAverageMaterialProperty
    mat_prop = diffusivity
    periodic_variable = u
    inserter = inserter
    radius = 0.2
  [../]
[]


[Executioner]
  type = Transient
  scheme = 'implicit-euler'

  # Preconditioned JFNK (default)
  solve_type = 'PJFNK'

  start_time = 0.0
  end_time = 0.05

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
  [./circle_average_pps]
    type = CircleAverageMaterialPropertyPPS
    method = inserter
    user_object = circle_average
    entry = 0
  [../]
[]

[Outputs]
  csv = true
  execute_on = 'initial timestep_end'
[]
