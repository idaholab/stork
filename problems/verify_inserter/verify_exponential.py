#!/usr/bin/env python

num_events = 10000
mean_interval = 1.2
num_bins = 20
dt = 1.0
distribution = "exponential"

# command to run test case and parse output
command  = "../../PRARIEDOG-opt -i event_timestepper.i "
command += "UserObjects/inserter/mean=" + str(mean_interval) + " "
command += "UserObjects/inserter/distribution=" + distribution + " "
command += "UserObjects/inserter/verbose=true "
command += "Executioner/TimeStepper/dt=" + str(dt) + " "
command += "Executioner/end_time=" + str(num_events*mean_interval)
command += " | awk 'BEGIN {t=0} /inserting/ {print $NF-t; t=$NF}'"

# run command and read in output
import subprocess
p = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
result = p.communicate()[0]

# convert to numpy array
import numpy as np
data = np.fromstring(result,sep="\n")

# generate some random data for comparison
x = np.random.rand(num_events)
fake_data = -np.log(x)*mean_interval

# plot comparison historgram
import matplotlib.pyplot as plt
plt.hist(fake_data, normed=True, bins=num_bins, color="grey", label="simulated data for comparison")

# plot histogram
plt.hist(data, normed=True, bins=num_bins, color="blue", label="binned interval data")

# plot what the PDF should look like
x = np.linspace(0, max(data))
pdf = np.exp(-x/mean_interval)/mean_interval
plt.plot(x, pdf, color="red", label="exponential PDF")
plt.plot([np.mean(data), np.mean(data)], [0, 1.0/mean_interval], color="blue", label="calculated mean")
plt.plot([np.mean(fake_data), np.mean(fake_data)], [0, 1.0/mean_interval], color="grey", label="simulated mean")
plt.plot([mean_interval, mean_interval], [0, 1.0/mean_interval], color="red", label="exact mean")

# show plot
plt.legend()
plt.xlim([0, 5*mean_interval])
plt.show()
