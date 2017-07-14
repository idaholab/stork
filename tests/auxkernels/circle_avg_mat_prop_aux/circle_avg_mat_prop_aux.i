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

[AuxVariables]
  [./circle_avg_diffusivity]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Kernels]
  [./diff]
    type = MaterialDiffusion
    variable = u
    diffusivity_name = diffusivity
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
  type = Steady

  # Preconditioned JFNK (default)
  solve_type = 'PJFNK'

  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
[]
