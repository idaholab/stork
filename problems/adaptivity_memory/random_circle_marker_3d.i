[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 20
  ny = 20
  nz = 20
  xmin = 0
  xmax = 1
  ymin = 0
  ymax = 1
  zmin = 0
  zmax = 1
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
  num_steps = 1000
  dt = 1.0
  solve_type = PJFNK
[]

[Adaptivity]
  initial_marker = marker
  initial_steps = 100
  marker = marker
  cycles_per_step = 100
  max_h_level = 6
  recompute_markers_during_cycles = true
  [./Markers]
    [./marker]
      type = RandomCircleMarker
      radius = 0.01
    [../]
  [../]
[]

[Postprocessors]
  [./active_elems]
    type = NumElems
    elem_filter = active
    execute_on = 'initial timestep_end'
  [../]
  [./total_elems]
    type = NumElems
    elem_filter = total
    execute_on = 'initial timestep_end'
  [../]
[]

[Outputs]
  [./console]
    type = Console
    print_mesh_changed_info = true
  [../]
[]
