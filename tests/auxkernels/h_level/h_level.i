[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 10
  nz = 0
  zmax = 0
  elem_type = QUAD4
[]

[Variables]
  [./u]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[AuxVariables]
  [./h_level]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = u
  [../]
[]

[AuxKernels]
  [./h_level]
    type = ElementHLevel
    variable = h_level
  [../]
[]

[BCs]
  [./right]
    type = DirichletBC
    variable = u
    boundary = right
    value = 1
  [../]
  [./top]
    type = DirichletBC
    variable = u
    boundary = top
    value = 0
  [../]
[]

[Executioner]
  type = Steady

  # Preconditioned JFNK (default)
  solve_type = 'PJFNK'
[]

[Adaptivity]
  steps = 2
  marker = box
  [./Markers]
    [./box]
      bottom_left = '0.3 0.3 0'
      inside = refine
      top_right = '0.6 0.6 0'
      outside = do_nothing
      type = BoxMarker
    [../]
  [../]
[]

[Outputs]
  exodus = true
[]
