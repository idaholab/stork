import pandas
import csv
import subprocess
import os
import numpy as np
import math
from PIL import Image

# use a different matplotlib backend so we can do this without an X11 connection
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import matplotlib.patches as mpatches

### set some parameters here ###

# 2d or 3d?
dim = 2

# specify base file name (no extention)
input_filename_base = str(dim) + 'd_sink_map_with_refinement'

# specify output filename storing times (no extention)
output_filename_base = 'timing_data'

# how many elements in each direction
nx_list = [20, 40, 80, 160]

# specify which cpu counts to run
cpu_list = [1, 2, 4, 8, 16, 32]

# write exodus files?
exodus = False

# how many events?
num_events = 20

# show console output?
console = False

### should not need to edit below this line ###

def stitch_images(image_list, output_filename, scale=0.7):
  # make a composite image of all the input images
  total_width = 0
  max_height = 0
  for image in image_list:
    im = Image.open(image)
    width, height = im.size
    width = int(width*scale)  # resize a bit
    height = int(height*scale)  # resize a bit
    total_width += width
    max_height = max(max_height, height)

  # make a big canvas and paste each image into it
  location = 0
  canvas = Image.new('RGB', (total_width, max_height))
  for image in image_list:
    im = Image.open(image)
    width, height = im.size
    width = int(width*scale)  # resize a bit
    height = int(height*scale)  # resize a bit
    canvas.paste(im.resize((width, height), resample=Image.LANCZOS), (location, 0))
    location += width

  canvas.save(output_filename, 'PNG')


if not console:
  FNULL = open(os.devnull, 'w')

timing_image_filename_list = []
elements_image_filename_list = []

for nx in nx_list:
  # append output filename with problem info
  filename_append_string = '-' + str(dim) + 'd-nx-' + str(nx)
  output_filename = output_filename_base + filename_append_string + '.csv'

  # write headers to output file
  with open(output_filename, 'w') as outfile:
    csv_writer = csv.writer(outfile)
    csv_writer.writerow(['cpus', 'reinit', 'adaptivity', 'solve', 'other', 'total'])

  # loop over cpu counts
  for cpus in cpu_list:
    # build command line list
    cmd = ['mpiexec', '-np', str(cpus), '../../PRARIEDOG-opt', '-i', input_filename_base + '.i']

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
    df = pandas.read_csv(input_filename_base + '_out.csv')

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

  # plot it
  fig, ax = plt.subplots()
  plt.stackplot(tdf['cpus'], data, colors=['r','b','g','grey'])

  # make it look nice
  plt.xscale('log')
  plt.xticks(tdf['cpus'])
  plt.xlabel('cpus')
  plt.ylabel('time (seconds)')
  plt.gca().xaxis.set_major_formatter(ticker.ScalarFormatter())
  plt.minorticks_off()
  plt.text(0.8, 0.1, 'nx=' + str(nx), transform=ax.transAxes, bbox=dict(facecolor='white', edgecolor='black'))

  # creating the legend manually
  plt.legend([mpatches.Patch(color='r'),
              mpatches.Patch(color='b'),
              mpatches.Patch(color='g'),
              mpatches.Patch(color='grey')],
             ['_eq.reinit()','refine_and_coarsen_elements()','solve','other'])

  # output plot to file
  timing_filename = 'timing_breakdown' + filename_append_string + '.png'
  plt.savefig(timing_filename)
  timing_image_filename_list.append(timing_filename)

  # now make a plot of elements and memory from the last run
  df = pandas.read_csv(input_filename_base + '_out.csv')
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
  plt.text(0.8, 0.1, 'nx=' + str(nx), transform=ax.transAxes, bbox=dict(facecolor='white', edgecolor='black'))
  fig.tight_layout()
  elements_filename = 'elements_and_memory' + filename_append_string + '.png'
  plt.savefig(elements_filename)
  elements_image_filename_list.append(elements_filename)

  # clear the plot for the next one
  plt.close(fig)

stitch_images(timing_image_filename_list, str(dim) + 'd_timing.png')
stitch_images(elements_image_filename_list, str(dim) + 'd_elements.png')
