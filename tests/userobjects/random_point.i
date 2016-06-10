[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 50
  ny = 50
  #nz = 50
[]

[Variables]
  [./u]
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
      auto_direction = 'x y'
    [../]
  [../]
[]

[Functions]
  [./gaussian]
    type = GaussianFunction
    sigma = 0.05
    scale = 2.0
    periodic_variable = u
    use_random_point = true
    random_point_user_object = random_point_uo
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

[UserObjects]
  [./random_point_uo]
    type = RandomPointUserObject
    seed = 1
  [../]
[]

[Postprocessors]
  [./integral]
    type = ElementIntegralVariablePostprocessor
    variable = u
    execute_on = 'initial timestep_end'
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 2
  dt = 0.001
  solve_type = NEWTON
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
[]
