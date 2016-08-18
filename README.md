Prariedog
=====

As shown in [J. Carter, "A Rate-Theory Approach to Irradiation Damage Modeling with Random Cascades in Space and Time", Metall and Mat Trans A (2015) 46: 93](http://link.springer.com/article/10.1007/s11661-014-2409-8), the "ODE" being solved here (with spatial resolution) is:

dC/dt = P - QDC(t)

where:
- P is production rate (source term)
- D is diffusion coeff
- Q is sink strength (loss term)
- C is the nonlinear variable (concentration in this case)

which has an analytical solution:

C(t) = P/QD(1-exp(-QDt)).

In terms of upscaling this to something with a spatial extent and randomly inserted sources, we have the expression

mean = (VP/eta)^-1

where:
- mean is the mean time between events and set in EventInserter
- V is "volume" material being modeled. Use 1.0 for "unmodeled" dimensions (e.g. z direction on a 2d mesh)
- P is the source term above
- eta is the scale of the source term (integral of Gaussian source over domain)

"Fork Prariedog" to create a new MOOSE-based application.

For more information see: [http://mooseframework.org/create-an-app/](http://mooseframework.org/create-an-app/)
