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
    initial_condition = 1.0
  [../]
[]

[AuxVariables]
  [./sink_strength]
    order = CONSTANT
    family = MONOMIAL
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

[Kernels]
  [./dt]
    type = TimeDerivative
    variable = u
  [../]
  [./matdiffusion]
    type = MaterialDiffusion
    variable = u
    diffusivity_name = diffusivity
  [../]
[]

[DiracKernels]
  [./sinkmap]
    type = DiracSinkMapKernel
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
[]

[Materials]
  [./uniform_diffusivity]
    type = GenericConstantMaterial
    prop_names = diffusivity
    prop_values = 0.5
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
    sink_placement = inside
  [../]
[]

[Postprocessors]
  [./integral_sink_strength]
    type = ElementIntegralVariablePostprocessor
    variable = sink_strength
    execute_on = 'initial timestep_end'
  [../]
  [./average_sink_strength]
    type = ElementAverageValue
    variable = sink_strength
    execute_on = 'initial timestep_end'
  [../]
  [./average_solution]
    type = ElementAverageValue
    variable = u
    execute_on = 'initial timestep_end'
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 3
  dt = 0.001
  solve_type = NEWTON
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
[]
