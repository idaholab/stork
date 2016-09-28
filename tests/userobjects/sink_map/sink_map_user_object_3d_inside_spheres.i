[Mesh]
  type = GeneratedMesh
  dim = 3
  xmin = -1
  xmax = 1
  ymin = -1
  ymax = 1
  zmin = -1
  zmax = 1
  nx = 50
  ny = 50
  nz = 50
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

[ICs]
  [./gaussianIC]
    type = FunctionIC
    variable = u
    function = gaussian
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

[Functions]
  [./gaussian]
    type = GaussianFunction
    sigma = 0.05
    scale = 2.0
    peak_location = '0.05 0.95 0.0'
    periodic_variable = u
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
[]

[AuxKernels]
  [./sink_map_aux]
    type = SinkMapAux
    variable = sink_strength
    sink_map_user_object = sink_map_uo
    execute_on = 'timestep_end'
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
    sink_shape_3d = spheres
  [../]
[]

[Postprocessors]
  [./integral]
    type = ElementIntegralVariablePostprocessor
    variable = sink_strength
    execute_on = 'initial timestep_end'
  [../]
  [./average]
    type = ElementAverageValue
    variable = sink_strength
    execute_on = 'initial timestep_end'
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 1
  dt = 0.001
  solve_type = NEWTON
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
[]
