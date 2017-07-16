[Mesh]
  type = GeneratedMesh
  dim = 3
  xmin = -1
  xmax = 1
  ymin = -1
  ymax = 1
  zmin = -1
  zmax = 1
  nx = 20
  ny = 20
  nz = 20
[]

[Variables]
  [./u]
  [../]
[]

[AuxVariables]
  [./sink_strength]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Functions]
  [./forcing_fn]
    type = ParsedFunction
    value = 3*t*t*((x*x)+(y*y))-(4*t*t*t)
  [../]
  [./exact_fn]
    type = ParsedFunction
    value = t*t*t*((x*x)+(y*y))
  [../]
[]

[Kernels]
  active = 'diff ie'
  [./ie]
    type = TimeDerivative
    variable = u
  [../]
  [./diff]
    type = Diffusion
    variable = u
  [../]
  [./ffn]
    type = UserForcingFunction
    variable = u
    function = forcing_fn
  [../]
[]

[AuxKernels]
  [./sink_aux]
    type = SinkMapAux
    variable = sink_strength
    sink_map_user_object = sink_map_user_object
    execute_on = 'timestep_end'
  [../]
[]

[BCs]
  active = ''
  [./all]
    type = FunctionDirichletBC
    variable = u
    boundary = '0 1 2 3'
    function = exact_fn
  [../]
  [./left]
    type = DirichletBC
    variable = u
    boundary = 3
    value = 0
  [../]
  [./right]
    type = DirichletBC
    variable = u
    boundary = 1
    value = 1
  [../]
[]

[UserObjects]
  [./random_point_uo]
    type = RandomPointUserObject
    seed = 1
  [../]
  [./inserter]
    type = EventInserter
    distribution = 'uniform'
    mean = 0.4
    insert_test = true
    test_time = 0.15
    test_location = '0.1 0.2 0.0'
    random_point_user_object = random_point_uo
    verbose = true
  [../]
  [./gaussian_uo]
    type = GaussianUserObject
    sigma = 0.1
    periodic_variable = u
  [../]
  [./sink_gaussian_uo]
    type = GaussianUserObject
    sigma = 0.05
  [../]
  [./sink_map_user_object]
    type = SinkMapUserObject
    spacing = 1.0
    strength = 3.0
    gaussian_user_object = sink_gaussian_uo
    periodic_variable = u
  [../]
[]

[Adaptivity]
  initial_marker = event_marker
  initial_steps = 10
  max_h_level = 1
  recompute_markers_during_cycles = true
  [./Markers]
    [./event_marker]
      type = EventMarker
      inserter = inserter
      gaussian_user_object = gaussian_uo
      marker_radius = 3.0
      verbose = true
      refine_sinks = true
      sink_marker_radius = 6.0
      sink_map_user_object = sink_map_user_object
      sink_gaussian_user_object = sink_gaussian_uo
    [../]
  [../]
[]

[Executioner]
  type = Transient
  scheme = 'implicit-euler'

  solve_type = NEWTON
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'

  #start_time = 0.0
  #end_time = 0.39
  num_steps = 1

  verbose = true
  [./TimeStepper]
    type = EventTimeStepper
    dt = 0.1
    event_inserter = inserter
    verbose = true
  [../]
[]

[Postprocessors]
  [./dt]
    type = TimestepSize
  [../]
  [./sink_strength_integral]
    type = ElementIntegralVariablePostprocessor
    variable = sink_strength
  [../]
  [./sink_strength_average]
    type = ElementAverageValue
    variable = sink_strength
  [../]
[]

[Outputs]
  exodus = true
  [./console]
    type = Console
    print_mesh_changed_info = true
  [../]
[]
