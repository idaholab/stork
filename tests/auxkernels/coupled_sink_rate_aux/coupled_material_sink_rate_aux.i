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
    initial_condition = 1.0
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
  [./sink]
    type = MaterialSinkKernel
    variable = u
    diffusivity_name = diffusivity
    sink_strength_name = sink_strength
  [../]
[]

[AuxKernels]
  [./sink_map_aux]
    type = MaterialRealAux
    variable = sink_strength
    property = sink_strength
    execute_on = 'timestep_end'
  [../]
  [./sink_rate_aux]
    type = CoupledSinkRateAux
    variable = sink_rate
    diffusivity_name = diffusivity
    sink_strength_variable = sink_strength
    solution_variable = u
    execute_on = 'timestep_end'
  [../]
[]

[Functions]
  [./gaussian_2d]
    # 2D gaussian function, centered at origin, sigma=0.05, integrates to 3
    type = ParsedFunction
    value = 3.0/0.05/0.05/2.0/pi*exp(-(x*x+y*y)/2/0.05/0.05)
  [../]
[]

[Materials]
  [./uniform_diffusivity]
    type = GenericConstantMaterial
    prop_names = diffusivity
    prop_values = 0.5
  [../]
  [./gaussian_sink]
    type = GenericFunctionMaterial
    block = 0
    prop_names = sink_strength
    prop_values = gaussian_2d
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

[Postprocessors]
  [./avg_sink_map_rate]
    type = ElementAverageValue
    variable = sink_rate
  [../]
[]

[Outputs]
  exodus = true
[]
