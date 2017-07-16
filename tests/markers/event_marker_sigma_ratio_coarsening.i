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
  [./one_plus_x]
    type = ParsedFunction
    value = '1+x'
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
  [./ffn]
    type = UserForcingFunction
    variable = u
    function = forcing_fn
  [../]
[]

[BCs]
  [./all]
    type = FunctionDirichletBC
    variable = u
    boundary = '0 1 2 3'
    function = exact_fn
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
  [./inserter_circle_average]
    type = InserterPointCircleAverageMaterialProperty
    mat_prop = diffusivity
    periodic_variable = u
    inserter = inserter
    radius = 0.2
  [../]
  [./random_point_uo]
    type = RandomPointUserObject
    seed = 1
  [../]
  [./gaussian_uo]
    type = GaussianUserObject
    sigma = 0.1
    periodic_variable = u
  [../]
  [./circle_max_original_element_size_uo]
    type = CircleMaxOriginalElementSize
    periodic_variable = u
  [../]
  [./inserter]
    type = EventInserter
    distribution = 'uniform'
    mean = 1.0
    insert_initial = true
    random_point_user_object = random_point_uo
    verbose = true
    seed = 3
    track_old_events = true
    removal_method = sigma_element_size_ratio
    removal_sigma_element_size_ratio = 2.0
    radius = 3.0
    gaussian_user_object = gaussian_uo
    inserter_circle_average_material_property_user_object = inserter_circle_average
    circle_max_original_element_size_user_object = circle_max_original_element_size_uo
  [../]
[]

[Adaptivity]
  initial_marker = event_marker
  initial_steps = 2
  marker = event_marker
  cycles_per_step = 2
  max_h_level = 2
  recompute_markers_during_cycles = true
  [./Markers]
    [./event_marker]
      type = EventMarker
      inserter = inserter
      gaussian_user_object = gaussian_uo
      marker_radius = 3.0
      verbose = true
      coarsen_events = true
    [../]
  [../]
[]

[Executioner]
  type = Transient
  scheme = 'implicit-euler'

  solve_type = 'PJFNK'

  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'

  start_time = 0.0
  end_time = 0.07

  verbose = true
  [./TimeStepper]
    type = EventTimeStepper
    dt = 0.01
    event_inserter = inserter
    verbose = true
  [../]
[]

[Postprocessors]
  [./dt]
    type = TimestepSize
  [../]
[]

[Outputs]
  exodus = true
  [./console]
    type = Console
    print_mesh_changed_info = true
  [../]
[]
