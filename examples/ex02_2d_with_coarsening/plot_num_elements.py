import numpy as np
import matplotlib.pyplot as plt

data=np.loadtxt('2d_with_coarsening_out.csv', delimiter=',', skiprows=1)

plt.plot(data[:,0], data[:,3], 'r-o')
plt.xlabel('time')
plt.ylabel('number of elements')

plt.show()
