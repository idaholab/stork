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
  [./dirac_sink]
    type = MaterialDiracSinkKernel
    variable = u
    diffusivity_name = diffusivity
    sink_strength_name = sink_strength
    point = '0.1 0.2 0.0'
  [../]
[]

[Materials]
  [./uniform_diffusivity]
    type = GenericConstantMaterial
    prop_names = 'diffusivity sink_strength'
    prop_values = '0.5 2.0'
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
