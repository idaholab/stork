import numpy as np
import matplotlib.pyplot as plt

# define some constants
dim = 2
xmin = -1.0
xmax = 1.0
ymin = -2.0
ymax = 2.0
spacing = 1.0
sigma = 0.05
dx = 0.025
dy = 0.025
Q_avg = 3.0

# set y range to zero in case you forgot
if dim == 1:
  ymin = 0.0
  ymax = 0.0

# points to compute map
Lx = xmax-xmin
Ly = ymax-ymin
x_array = np.linspace(xmin, xmax, num=Lx/dx+1,endpoint=True)
y_array = np.linspace(ymin, ymax, num=Ly/dy+1,endpoint=True)

# corner sinks
#x_centers = np.linspace(xmin, xmax, num=Lx/spacing+1,endpoint=True)
#y_centers = np.linspace(ymin, ymax, num=Ly/spacing+1,endpoint=True)
# or, centered sinks
x_centers = np.linspace(xmin+spacing/2.0, xmax-spacing/2.0, num=Lx/spacing,endpoint=True)
y_centers = np.linspace(ymin+spacing/2.0, ymax-spacing/2.0, num=Ly/spacing,endpoint=True)

Q=np.zeros((np.size(x_array),np.size(y_array)))

# centered sinks in 1D can screw up in the y dimension
if np.size(y_centers) == 0:
  y_centers = np.array([0.0]);

for i in range (0,np.size(x_array)):
  for j in range (0,np.size(y_array)):
    for x_center in x_centers:
      for y_center in y_centers:
        d_squared = np.power(x_array[i] - x_center, 2) + np.power(y_array[j] - y_center, 2)
        Q[i,j] += Q_avg*np.power(spacing/sigma/np.sqrt(2.0*np.pi),dim)*np.exp(-d_squared/2.0/sigma/sigma)

if dim == 1:
  Qint = np.trapz(Q,x_array,axis=0)
  Qavg = Qint/Lx
  plt.plot(x_array,Q)
if dim == 2:
  X, Y = np.meshgrid(y_array,x_array)
  plt.contourf(X, Y, Q)
  plt.xlim((xmin,xmax))
  plt.ylim((ymin,ymax))
  plt.axis('equal')
  Qint = np.trapz(np.trapz(Q,y_array,axis=1),x_array,axis=0)
  Qavg = Qint/Lx/Ly

print "integral: ", Qint
print "average: ", Qavg


plt.show()
