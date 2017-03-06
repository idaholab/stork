import yt
import numpy as np
import matplotlib.pyplot as plt

# load up exodus files
# NOTE: as of 3/4/2017 yt does not close files so you can hit a file limit
sim = yt.simulation('exodus', 'ExodusII')
sim.get_time_series()

# initialize some things
step_counter = 0
step_map = {}
simulation_min=1e99
simulation_max=-1e99

# loop over all files and look for min/max values, then map step number to file and step within file
for ds in sim:
    reg = ds.r['u']
    plot_max = float(reg.max())
    plot_min = float(reg.min())
    print 'filename:',ds.index_filename,'step:',ds.step,'time:',ds.current_time.value[()],'steps:',ds.num_steps,'max value:',plot_max,'min value:',plot_min
    step_map.update({step_counter : (ds.index_filename, ds.step)})
    step_counter = step_counter + 1
    if plot_max > simulation_max:
        simulation_max = plot_max
    if plot_min < simulation_min:
        simulation_min = plot_min

# total number of steps
num_steps = step_counter - 1

# print it all out as a check
for item in step_map:
    print item, step_map[item]
print 'simulation max:',simulation_max,'simulation min:',simulation_min,'num_steps:',num_steps

# read in CSV with average values
data = np.genfromtxt('2d_sink_map_with_refinement_out.csv', delimiter=',', skip_header=1, names=['time','dt','num_elems','sink_average','sink_integral','sink_rate_average','solution_average','solution_integral'])

# loop over all time steps and make a plot
for i in range(0,num_steps+1):
    filename, step = step_map[i]
    ds = yt.load(filename, step=step)
    slc = yt.SlicePlot(ds,'z','u',origin='native')
    slc.set_log('u',False) # turn off log scale
    slc.set_zlim('u',zmin=simulation_min,zmax=simulation_max) # set color limits
    slc.zoom(1.2) # fill the plot
    slc.set_xlabel('x ($\mu$m)') # set x label
    slc.set_ylabel('y ($\mu$m)') # set y label
    slc.set_colorbar_label('u', r'defect concentration (defects/$\mu$m$^3$)') # set solution label
    #slc.annotate_mesh_lines(plot_args={'color':'black'}) # use this line to color element edges

    # now make a little figure in the corner for the average values from the CSV file
    fig = slc.plots['u'].figure
    ax = slc.plots['u'].axes
    rect = (0.17,0.16,0.2,0.2)
    new_ax = fig.add_axes(rect)
    new_ax.tick_params(colors='white')
    lax = new_ax.set_xlabel('time (s)')
    lay = new_ax.set_ylabel(r'avg concentration (defects/$\mu$m$^3$)')
    lax.set_color('white')
    lay.set_color('white')
    new_ax.plot(data['time'],data['solution_average'])
    new_ax.plot(data['time'][i],data['solution_average'][i], marker='o')

    # save the image
    slc.save('figures/concentration-' + format(i,'04') + '.png')
