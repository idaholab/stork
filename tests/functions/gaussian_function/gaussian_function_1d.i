[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 50
  #ny = 50
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

[Functions]
  [./gaussian]
    type = GaussianFunction
    sigma = 0.05
    peak_location = '0.4 0.0 0.0'
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
