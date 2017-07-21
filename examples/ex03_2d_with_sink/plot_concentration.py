import numpy as np
import matplotlib.pyplot as plt

# enter values from simulation here
diffusivity = 2.0
sink_strength = 0.05
mean = 1.2  # mean of EventInserter distribution
Lx = 2.0    # length of problem in x
Ly = 2.2    # length of problem in y
scale = 3.0 # scale of Gaussian source term

data=np.loadtxt('2d_with_sink_out.csv', delimiter=',', skiprows=1)

plt.plot(data[:,0], data[:,1], 'r-', label='solution')
plt.xlabel('time')
plt.ylabel('average concentration')

analytic = scale/diffusivity/sink_strength/mean/Lx/Ly*(1.0 - np.exp(-sink_strength*diffusivity*data[:,0]))
plt.plot(data[:,0], analytic, 'k-', label='analytic')

plt.legend(loc="lower right")

plt.show()
