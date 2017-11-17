[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 20
  ny = 20
  nz = 20
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

[AuxVariables]
  [./sink_map_aux]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./sink_map_rate_aux]
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
  [./sink_map_rate]
    type = SinkMapRateAux
    variable = sink_map_rate_aux
    diffusivity_name = diffusivity
    sink_map_user_object = sink_map_uo
    solution_variable = u
    execute_on = timestep_end
  [../]
[]

[BCs]
  [./Periodic]
    [./all]
      variable = u
      auto_direction = 'x y z'
    [../]
  [../]
[]

[Materials]
  [./simple]
    type = GenericConstantMaterial
    block = 0
    prop_names = 'diffusivity'
    prop_values = '0.5'
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
    sigma = 0.01
    periodic_variable = u
    scale = 3
  [../]
  [./sink_gaussian_uo]
    type = GaussianUserObject
    sigma = 0.01
  [../]
  [./circle_max_original_element_size_uo]
    type = CircleMaxOriginalElementSize
    periodic_variable = u
  [../]
  [./inserter]
    type = EventInserter
    insert_initial = true
    random_timing = true
    distribution = exponential
    mean = 0.02
    random_point_user_object = random_point_uo
    seed = 3
    verbose = true
    track_old_events = true
    removal_method = sigma_element_size_ratio
    removal_sigma_element_size_ratio = 1.0
    radius = 3.0
    gaussian_user_object = gaussian_uo
    inserter_circle_average_material_property_user_object = inserter_circle_average
    circle_max_original_element_size_user_object = circle_max_original_element_size_uo
  [../]
  [./sink_map_uo]
    type = SinkMapUserObject
    spacing = 1.0
    strength = 0.9
    sink_placement = inside
    gaussian_user_object = sink_gaussian_uo
    sink_shape_3d = spheres
  [../]
  [./terminator]
    type = Terminator
    expression = 'num_past_events>=5'
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
      marker_radius = 6.0
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
  # num_steps = 20

  verbose = true

  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'

  nl_abs_tol = 1.0e-12

  [./TimeStepper]
    type = EventTimeStepper
    dt = 0.001
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
  [./num_elems]
    type = NumElems
    execute_on = 'initial timestep_end'
  [../]
  [./sink_rate_average]
    type = ElementAverageValue
    variable = sink_map_rate_aux
  [../]
  [./num_past_events]
    type = NumPastEvents
    inserter = inserter
  [../]
  [./memory]
    type = MemoryUsage
    mem_type = physical_memory
    value_type = total
    # by default MemoryUsage reports the peak value for the current timestep
    # out of all samples that have been taken (at linear and non-linear iterations)
    execute_on = 'INITIAL TIMESTEP_END NONLINEAR LINEAR'
  [../]
  [./total_time]
    type = PerformanceData
    event = 'ACTIVE'
  [../]
  [./adaptivity_time]
    type = PerformanceData
    column = total_time_with_sub
    category = 'Mesh Adaptivity'
    event = refine_and_coarsen_elements()
  [../]
  [./reinit_time]
    type = PerformanceData
    column = total_time_with_sub
    category = 'Mesh Adaptivity'
    event = _eq.reinit()
  [../]
  [./solve_time]
    type = PerformanceData
    column = total_time_with_sub
    event = solve()
  [../]
[]

[Outputs]
  exodus = true
  csv = true
  execute_on = 'initial timestep_end'
  print_perf_log = true
  [./console]
    type = Console
    print_mesh_changed_info = true
  [../]
[]
