[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 20
  ny = 20
[]

[Variables]
  [./u]
  [../]
[]

[AuxVariables]
  [./circle_avg_diffusivity]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = u
  [../]
  [./time]
    type = TimeDerivative
    variable = u
  [../]
[]

[AuxKernels]
  [./circle_avg_aux]
    type = CircleAverageMaterialPropertyAux
    variable = circle_avg_diffusivity
    circle_average_material_property_user_object = circle_average
    radius = 0.2
    execute_on = 'timestep_end'
  [../]
[]

[Problem]
  solve = false
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
    outputs = 'all'
  [../]
[]

[UserObjects]
  [./circle_average]
    type = CircleAverageMaterialProperty
    mat_prop = diffusivity
  [../]
[]

[Executioner]
  # Preconditioned JFNK (default)
  type = Transient
  num_steps = 1
  dt = 0.1
  solve_type = PJFNK
[]

[Adaptivity]
  initial_marker = marker
  initial_steps = 10
  marker = marker
  cycles_per_step = 10
  max_h_level = 2
  recompute_markers_during_cycles = true
  [./Markers]
    [./marker]
      type = RandomCircleMarker
      radius = 0.1
    [../]
  [../]
[]

[Outputs]
  exodus = true
[]

