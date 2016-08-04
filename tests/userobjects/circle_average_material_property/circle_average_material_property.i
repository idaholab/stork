[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 10
[]

[Variables]
  [./u]
  [../]
[]

[Kernels]
  [./diff]
    type = MaterialDiffusion
    variable = u
    prop_name = diffusivity
  [../]
[]

[BCs]
  [./left]
    type = DirichletBC
    variable = u
    boundary = left
    value = 0
  [../]
  [./right]
    type = DirichletBC
    variable = u
    boundary = right
    value = 1
  [../]
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
  [../]
[]

[UserObjects]
  [./circle_average]
    type = CircleAverageMaterialProperty
    mat_prop = diffusivity
    periodic_variable = u
  [../]
[]

[Executioner]
  type = Steady

  # Preconditioned JFNK (default)
  solve_type = 'PJFNK'

  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Postprocessors]
  [./circle_average_pps]
    type = CircleAverageMaterialPropertyPPS
    user_object = circle_average
    location = '0.5 0.5 0.0'
    radius = 0.2
  [../]
[]

[Outputs]
  csv = true
[]
