[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 20
  ny = 20
[]

[Variables]
  [./u]
    initial_condition = 1.0
  [../]
  [./v]
  [../]
[]

[BCs]
  [./Periodic]
    [./u_periodic]
      variable = u
      auto_direction = 'x y'
    [../]
    [./v_periodic]
      variable = v
      auto_direction = 'x y'
    [../]
  [../]
[]

[ICs]
  [./initial_v]
    type = FunctionIC
    variable = v
    function = gaussian
  [../]
 []

[Kernels]
  [./dt_u]
    type = TimeDerivative
    variable = u
  [../]
  [./dt_v]
    type = TimeDerivative
    variable = v
  [../]
  [./recombination_u]
    type = Recombination
    variable = u
    other_variable = v
  [../]
  [./recombination_v]
    type = Recombination
    variable = v
    other_variable = u
  [../]
[]

[Functions]
  [./gaussian]
    type = GaussianFunction
    sigma = 0.08
    peak_location = '0.4 0.6 0.0'
  [../]
[]

[Preconditioning]
  [./smp]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 3
  dt = 0.1
  solve_type = NEWTON
[]

[Outputs]
  exodus = true
[]
