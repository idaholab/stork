import pandas
import csv
import subprocess
import os
import numpy as np

# use a different matplotlib backend so we can do this without an X11 connection
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import matplotlib.patches as mpatches

### set some parameters here ###

# specify base file name (no extention)
input_basename = '2d_sink_map_with_refinement'

# specify output filename storing times
output_filename = 'timing_data.csv'

# specify which cpu counts to run
cpu_list = [1, 2, 4, 8, 16]

# write exodus files?
exodus = False

# how many events?
num_events = 20

# how many elements in each direction
nx = 20

# show console output?
console = False

### should not need to edit below this line ###

if not console:
  FNULL = open(os.devnull, 'w')

# write headers to output file
with open(output_filename, 'w') as outfile:
  csv_writer = csv.writer(outfile)
  csv_writer.writerow(['cpus', 'reinit', 'adaptivity', 'solve', 'other', 'total'])

# loop over cpu counts
for cpus in cpu_list:
  # build command line list
  cmd = ['mpiexec', '-np', str(cpus), '../../PRARIEDOG-opt', '-i', input_basename + '.i']

  # add terminator
  cmd = cmd + ['UserObjects/terminator/expression="num_past_events>=' + str(num_events) + '"']

  # set mesh size
  cmd = cmd + ['Mesh/nx=' + str(nx), 'Mesh/ny=' + str(nx), 'Mesh/nz=' + str(nx)]

  # allow exodus output if specified (it is enabled by default in input file)
  if not exodus:
    cmd = cmd + ['Outputs/exodus=false']

  # now run it
  print ' '.join(cmd)  # print out command line before running
  if console:
    subprocess.call(cmd)
  else:
    subprocess.call(cmd, stdout=FNULL)

  # parse datafile
  df = pandas.read_csv(input_basename + '_out.csv')

  # grab last line of timing columns, and find leftover
  total_time = df['total_time'].iloc[-1]
  adaptivity_time = df['adaptivity_time'].iloc[-1]
  solve_time = df['solve_time'].iloc[-1]
  reinit_time = df['reinit_time'].iloc[-1]
  other_time = total_time - adaptivity_time - solve_time - reinit_time

  # print info to screen
  print 'reinit time:', reinit_time, 'adaptivity time:', adaptivity_time, 'solve time:', solve_time, 'other:', other_time, 'total:', total_time

  # print info to file
  with open(output_filename, 'a') as outfile:
    csv_writer = csv.writer(outfile)
    csv_writer.writerow([cpus, reinit_time, adaptivity_time, solve_time, other_time, total_time])

# now that we're done, open the timings file and plot contributions to the total runtime as a function of cpu count
tdf = pandas.read_csv(output_filename)

# stack up data we are interested in
data = np.row_stack((tdf['reinit'], tdf['adaptivity'], tdf['solve'], tdf['other']))
print data

# plot it
plt.stackplot(tdf['cpus'], data, colors=['r','b','g','grey'])

# make it look nice
plt.xscale('log')
plt.xticks(tdf['cpus'])
plt.xlabel('cpus')
plt.ylabel('time (seconds)')
plt.gca().xaxis.set_major_formatter(ticker.ScalarFormatter())
plt.minorticks_off()

# creating the legend manually
plt.legend([mpatches.Patch(color='r'),  
            mpatches.Patch(color='b'), 
            mpatches.Patch(color='g'), 
            mpatches.Patch(color='grey')], 
           ['_eq.reinit()','refine_and_coarsen_elements()','solve','other'])

# output plot to file
plt.savefig('timing_breakdown.png')

# now make a plot of elements and memory from the last run
df = pandas.read_csv(input_basename + '_out.csv')
fig, ax1 = plt.subplots()
ax1.plot(df['time'], df['num_elems'], color='b')
ax1.set_ylabel('number of elements', color='b')
ax1.set_ylim(bottom=0)
ax1.tick_params(axis='y', colors='b')
ax2 = ax1.twinx()
ax2.plot(df['time'], df['memory']*1e-9/cpu_list[-1], color='r')
ax2.set_ylabel('memory in GB/cpu', color='r')
ax2.set_xlabel('simulation time')
ax2.set_ylim(bottom=0)
ax2.tick_params(axis='y', colors='r')
fig.tight_layout()
plt.savefig('elements_and_memory.png')
