#!/usr/bin/env python2

import sys
import subprocess

import numpy as np

import denali

# read the selection information
selection = denali.io.read_selection(open(sys.argv[1]))

# extract the component
u,v = (x[0] for x in selection['component'])

# find the input points corresponding to these vertices
with np.load("data.npz") as data:
    samples = data['samples']
    density = data['density']
    locations = data['locations']
    sigma = data['sigma']

# find the center of the selected samples
selected_samples = samples[[u,v]]

print "<b>Selected samples:</b><br>"
print "u, ", selected_samples[0]
print "<br>"
print "v: ", selected_samples[1]
