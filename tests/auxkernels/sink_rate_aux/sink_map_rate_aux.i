[Mesh]
  type = GeneratedMesh
  dim = 2
  xmin = -0.5
  xmax = 0.5
  ymin = -0.5
  ymax = 0.5
  nx = 40
  ny = 40
[]

[Variables]
  [./u]
    initial_condition = 1
  [../]
[]

[AuxVariables]
  [./sink_strength]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./sink_rate]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[BCs]
  [./Periodic]
    [./all]
      variable = u
      auto_direction = 'x y'
    [../]
  [../]
[]

[Kernels]
  [./dt]
    type = TimeDerivative
    variable = u
  [../]
  [./diffusion]
    type = Diffusion
    variable = u
  [../]
  [./sink_map_loss]
    type = SinkMapKernel
    variable = u
    diffusivity_name = diffusivity
    sink_map_user_object = sink_map_uo
  [../]
[]

[AuxKernels]
  [./sink_map_aux]
    type = SinkMapAux
    variable = sink_strength
    sink_map_user_object = sink_map_uo
    execute_on = 'timestep_end'
  [../]
  [./sink_map_rate_aux]
    type = SinkMapRateAux
    variable = sink_rate
    sink_map_user_object = sink_map_uo
    diffusivity_name = diffusivity
    solution_variable = u
  [../]
[]

[Materials]
  [./simple]
    type = GenericConstantMaterial
    block = 0
    prop_names = diffusivity
    prop_values = 2.0
  [../]
[]

[UserObjects]
  [./sink_gaussian_uo]
    type = GaussianUserObject
    sigma = 0.05
  [../]
  [./sink_map_uo]
    type = SinkMapUserObject
    spacing = 1.0
    strength = 3.0
    gaussian_user_object = sink_gaussian_uo
    periodic_variable = u
    sink_placement = corner
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 3
  dt = 0.01
  solve_type = NEWTON
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Postprocessors]
  [./avg_sink_map_rate]
    type = ElementAverageValue
    variable = sink_rate
  [../]
[]

[Outputs]
  exodus = true
[]
