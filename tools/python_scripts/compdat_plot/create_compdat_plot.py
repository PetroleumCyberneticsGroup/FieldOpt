#!/usr/bin/env python3
'''
Script that creates plots wellpaths, including a colormapping
of the well indices over a grid.

It takes as input a CSV file containing information about the
well (ijk-coordinates and WI for each block), the (ij) grid size,
(optionally) the xyz coordinates for the well heel and toe, as
well as the path to the output file to be written.

It stores plots of the well path in a pdf file at a given location.

The CSV files used are created by the WellIndexCalculator executable.

Note that this script assumes that the input from the CSV file is
1-indexed. These will be shifted to 0-indexing
'''
import argparse
import sys
import numpy as np
from matplotlib import pyplot as plt
from matplotlib import cm as cm
from matplotlib.ticker import MultipleLocator, FormatStrFormatter

parser = argparse.ArgumentParser(description='Save plots of well paths.')
parser.add_argument('infile', metavar='INFILE', type=argparse.FileType('r'),
    help='path to CSV file')
parser.add_argument('outfile', metavar='OUTFILE', type=argparse.FileType('w'),
    help='path to output PDF file')
parser.add_argument('dimi', metavar='DIMI', type=int,
    help="Number of blocks in i-direction")
parser.add_argument('dimj', metavar='DIMJ', type=int,
    help="Number of blocks in j-direction")
args = parser.parse_args()

# Read the well data from the CSV file
lines = args.infile.readlines()
lines = [line.strip() for line in lines]  # Strip leading/trailing whitespace
lines = [line.replace('\t', '') for line in lines]  # Remove tabs
lines = lines[2:]  # Remove header line
lines = lines[:-1]  # Remove tail line

# Create empty grid
grid = np.zeros([args.dimi, args.dimj])
x_vals = [i for i in range(0, args.dimi, 10)]
y_vals = [j for j in range(0, args.dimj, 10)]
grid_xlabels = [x + 1 for x in x_vals]
grid_ylabels = [y + 1 for y in y_vals]

# Extract well block data and fill the WI values into the grid
for line in lines:
    elems = line.split(',')
    # print(elems)
    i = int(elems[0]) - 1
    j = int(elems[1]) - 1
    k = int(elems[2]) - 1
    wi = float(elems[4])
    grid[j, i] = wi  # Flip the indices (because imgplot does it)


# Plot the well blocks
fig, ax = plt.subplots()
imgplot = plt.imshow(grid, vmin=0, vmax=0.00001, cmap=cm.Blues, interpolation='nearest')
plt.xticks(x_vals, grid_xlabels)
plt.yticks(y_vals, grid_xlabels)
# plt.colorbar()

# Set grid
minorLocator = MultipleLocator(1)
ax.xaxis.set_minor_locator(minorLocator)
ax.yaxis.set_minor_locator(minorLocator)
plt.grid(which='minor', color='grey', linestyle='-')

# plt.show()
plt.savefig(args.outfile.name, format='pdf')
