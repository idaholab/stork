import numpy as np
import matplotlib.pyplot as plt
import os

# postprocessor output should be the only csv file in directory
for file in os.listdir("."):
  if file.endswith(".csv"):
    csvfile=file

data=np.loadtxt(csvfile, delimiter=',', skiprows=1)

plt.plot(data[:,0], data[:,3], 'r-o')
plt.xlabel('time')
plt.ylabel('number of elements')

plt.show()
