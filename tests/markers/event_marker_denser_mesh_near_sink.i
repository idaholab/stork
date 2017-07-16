[Mesh]
  type = GeneratedMesh
  dim = 2
  xmin = -0.5
  xmax = 0.5
  ymin = -0.5
  ymax = 0.5
  nx = 20
  ny = 20
[]

[Variables]
  [./u]
  [../]
[]

[AuxVariables]
  [./sink_map_aux]
    order = CONSTANT
    family = MONOMIAL
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
  [./sink_map]
    type = SinkMapKernel
    variable = u
    sink_map_user_object = sink_map_uo
    diffusivity_name = diffusivity
  [../]
  [./event_inserter_source]
    type = EventInserterSource
    variable = u
    inserter = inserter
    gaussian_user_object = gaussian_uo
  [../]
[]

[AuxKernels]
  [./sink_map]
    type = SinkMapAux
    variable = sink_map_aux
    sink_map_user_object = sink_map_uo
    execute_on = timestep_end
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

[Materials]
  [./simple]
    type = GenericConstantMaterial
    block = 0
    prop_names = 'diffusivity'
    prop_values = '2.0'
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
    sigma = 0.02
    periodic_variable = u
    scale = 3
  [../]
  [./sink_gaussian_uo]
    type = GaussianUserObject
    sigma = 0.05
  [../]
  [./circle_max_original_element_size_uo]
    type = CircleMaxOriginalElementSize
    periodic_variable = u
  [../]
  [./inserter]
    type = EventInserter
    insert_initial = true
    random_timing = false
    mean = 1.2
    random_point_user_object = random_point_uo
    seed = 3
    verbose = true
    track_old_events = true
    removal_method = sigma_element_size_ratio
    removal_sigma_element_size_ratio = 2.0
    radius = 3.0
    gaussian_user_object = gaussian_uo
    inserter_circle_average_material_property_user_object = inserter_circle_average
    circle_max_original_element_size_user_object = circle_max_original_element_size_uo
  [../]
  [./sink_map_uo]
    type = SinkMapUserObject
    spacing = 1.0
    strength = 0.05
    sink_placement = inside
    gaussian_user_object = sink_gaussian_uo
  [../]
[]

[Adaptivity]
  initial_marker = event_marker
  initial_steps = 10
  marker = event_marker
  cycles_per_step = 10
  max_h_level = 0
  recompute_markers_during_cycles = true
  [./Markers]
    [./event_marker]
      type = EventMarker
      inserter = inserter
      gaussian_user_object = gaussian_uo
      marker_radius = 8.0
      coarsen_events = true
      verbose = true
      periodic_variable = u
      event_sigma_mesh_ratio = 1.0
      refine_sinks = true
      sink_marker_radius = 6.0
      sink_map_user_object = sink_map_uo
      sink_gaussian_user_object = sink_gaussian_uo
      sink_sigma_mesh_ratio = 1.0
    [../]
  [../]
[]

[Executioner]
  type = Transient
  scheme = 'implicit-euler'

  solve_type = 'NEWTON'

  start_time = 0.0
  num_steps = 4

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
  [./solution_integral]
    type = ElementIntegralVariablePostprocessor
    variable = u
  [../]
  [./solution_average]
    type = ElementAverageValue
    variable = u
  [../]
  [./sink_integral]
    type = ElementIntegralVariablePostprocessor
    variable = sink_map_aux
  [../]
  [./sink_average]
    type = ElementAverageValue
    variable = sink_map_aux
  [../]
[]

[Outputs]
  exodus = true
[]
