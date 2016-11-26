[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 20
  ny = 20
  xmin = -1
  xmax = 1
  ymin = -1
  ymax = 1
[]

[Variables]
  [./u]
  [../]
[]

[AuxVariables]
  [./v]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Kernels]
  [./diffusion]
    type = Diffusion
    variable = u
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

[AuxKernels]
  [./gaussian_aux]
    type = SpatialUserObjectAux
    variable = v
    user_object = gaussian_uo
  [../]
[]

[UserObjects]
  [./random_point_uo]
    type = RandomPointUserObject
    seed = 1
  [../]
  [./gaussian_uo]
    type = GaussianUserObject
    sigma = 0.2
    use_random_point = true
    random_point_user_object = random_point_uo
  [../]
[]

[Executioner]
  type = Steady
  solve_type = NEWTON
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
[]
