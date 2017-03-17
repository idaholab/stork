[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 20
  ny = 20
  xmin = 0
  xmax = 1
  ymin = 0
  ymax = 1
[]

[Variables]
  [./u]
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = u
  [../]
  [./time]
    type = TimeDerivative
    variable = u
  [../]
[]

[Problem]
  solve = false
[]

[Executioner]
  # Preconditioned JFNK (default)
  type = Transient
  num_steps = 1
  dt = 0.1
  solve_type = PJFNK
[]

[Adaptivity]
  initial_marker = marker
  initial_steps = 10
  max_h_level = 3
  recompute_markers_during_cycles = true
  [./Markers]
    [./marker]
      type = RandomCircleMarker
      radius = 0.1
    [../]
  [../]
[]

[Outputs]
  exodus = true
  [./console]
    type = Console
    print_mesh_changed_info = true
  [../]
[]

