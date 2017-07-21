# Using a gaussian sigma that too small to be sufficiently be resolved on this mesh, so lots of adaptivity is needed
# Integral of variable over domain should be close to 1

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
    insert_initial = true
    distribution = exponential
    mean = 1.0
    random_point_user_object = random_point_uo
    seed = 3
    verbose = true
  [../]
  [./gaussian_uo]
    type = GaussianUserObject
    sigma = 0.05
    periodic_variable = u
  [../]
[]

[Adaptivity]
  initial_marker = event_marker
  initial_steps = 4
  marker = event_marker
  cycles_per_step = 4
  max_h_level = 2
  recompute_markers_during_cycles = true
  [./Markers]
    [./event_marker]
      type = EventMarker
      inserter = inserter
      gaussian_user_object = gaussian_uo
      marker_radius = 6.0
      periodic_variable = u
    [../]
  [../]
[]

[Executioner]
  type = Transient
  scheme = 'implicit-euler'

  solve_type = 'NEWTON'

  start_time = 0.0
  end_time = 10.0

  verbose = true

  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'

  nl_abs_tol = 1.0e-12

  [./TimeStepper]
    type = EventTimeStepper
    dt = 0.01
    event_inserter = inserter
    growth_factor = 2.0
  [../]
[]

[Preconditioning]
  [./smp]
    type = SMP
    full = true
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
  csv = true
  execute_on = 'initial timestep_end'
  [./console]
    type = Console
    print_mesh_changed_info = true
  [../]
[]
