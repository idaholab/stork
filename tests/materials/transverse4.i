# using transverse_directions in a more complicated mesh works as planned (compare this with transverse3)
[Mesh]
  type = FileMesh
  file = solid4.e
[]

[MeshModifiers]
  [./xmin]
    type = SideSetsFromNormals
    fixed_normal = true
    new_boundary = xmin
    normals = '-1 0 0'
  [../]
[]

[GlobalParams]
  richardsVarNames_UO = PPNames
  density_UO = DensityConstBulk
  sat_UO = Saturation
  seff_UO = SeffVG
  relperm_UO = RelPermPower
  SUPG_UO = SUPGnone
[]

[UserObjects]
  [./PPNames]
    type = RichardsVarNames
    richards_vars = pressure
  [../]
  [./DensityConstBulk]
    type = RichardsDensityConstBulk
    dens0 = 1
    bulk_mod = 1.0E3
  [../]
  [./SeffVG]
    type = RichardsSeff1VG
    m = 0.8
    al = 1
  [../]
  [./RelPermPower]
    type = RichardsRelPermPower
    simm = 0.0
    n = 2
  [../]
  [./Saturation]
    type = RichardsSat
    s_res = 0.1
    sum_s_res = 0.1
  [../]
  [./SUPGnone]
    type = RichardsSUPGnone
  [../]
[]

[Variables]
  [./pressure]
  [../]
[]

[ICs]
  [./p]
    type = FunctionIC
    function = 'if(x<-0.99,z,0)'
    variable = pressure
  [../]
[]

[BCs]
  [./p]
    type = FunctionPresetBC
    function = 'z'
    variable = pressure
    boundary = xmin
  [../]
[]

[Kernels]
  [./richardst]
    type = RichardsMassChange
    variable = pressure
  [../]
  [./richardsf]
    type = RichardsFullyUpwindFlux
    variable = pressure
  [../]
[]

[AuxVariables]
  [./dummy_zone]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./n0]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./n1]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./n2]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  [./dummy_zone]
    type = ConstantAux
    variable = dummy_zone
    value = 0
  [../]
  [./n0]
    type = BATransverseDirectionAux
    execute_on = initial
    variable = n0
    index = 0
  [../]
  [./n1]
    type = BATransverseDirectionAux
    execute_on = initial
    variable = n1
    index = 1
  [../]
  [./n2]
    type = BATransverseDirectionAux
    execute_on = initial
    variable = n2
    index = 2
  [../]
[]

[Materials]
  [./rock]
    type = BAMaterial

    transverse_directions = 'n0 n1 n2'

    block = '2 3 4 5 6 7 8 9 10 11'
    mat_porosity = 0.1
    mat_permeability = '0 0 0  0 0 0  0 0 0'
    viscosity = 1E-3
    gravity = '0 0 0'
    linear_shape_fcns = true

    insitu_perm_zone = dummy_zone
    kh = 1E-5
    kv = 0
    insitu_por_zone = dummy_zone
    por = 0.1
    change_perm_zone = dummy_zone
    change_kh = 0
    change_kv = 0
  [../]
[]


[Preconditioning]
  [./andy]
    type = SMP
    full = true
    petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it'
    petsc_options_value = 'bcgs bjacobi 1E-14 1E-10 10000'
  [../]
[]

[Executioner]
  type = Transient
  dt = 1E-2
  end_time = 10E-2
  solve_type = Newton
[]

[Outputs]
  file_base = transverse4
  exodus = true
  output_initial = true
  print_perf_log = true
[]
