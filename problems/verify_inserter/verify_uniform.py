#!/usr/bin/env python

num_events = 10000
mean_interval = 1.2
num_bins = 20
dt = 1.0
distribution = "uniform"

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
fake_data = np.random.rand(num_events)*2.0*mean_interval

# plot comparison historgram
import matplotlib.pyplot as plt
plt.hist(fake_data, normed=True, bins=num_bins, color="grey", label="simulated data for comparison")

# plot histogram
plt.hist(data, normed=True, bins=num_bins, label="binned interval data")

# plot what the PDF should look like
x = np.linspace(0, 2.0*mean_interval)
pdf = np.ones(x.size)/mean_interval/2.0
plt.plot(x, pdf, color="red", label="uniform PDF")
plt.plot([np.mean(data), np.mean(data)], [0, 1.0/mean_interval], color="blue", label="calculated mean")
plt.plot([np.mean(fake_data), np.mean(fake_data)], [0, 1.0/mean_interval], color="grey", label="simulated mean")
plt.plot([mean_interval, mean_interval], [0, 1.0/mean_interval], color="red", label="exact mean")

# show plot
plt.legend()
plt.ylim([0, 1.0/mean_interval])
plt.show()
