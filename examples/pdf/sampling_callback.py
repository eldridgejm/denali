#!/usr/bin/env python2

N_NEW_SAMPLES = 100
SAMPLING_SIGMA = .1

import sys
import subprocess

import numpy as np

import denali

# read the selection information
selection = denali.io.read_selection_file(sys.argv[1])

# extract the component
u,v = (x[0] for x in selection['component'])

# find the input points corresponding to these vertices
with np.load("data.npz") as data:
    samples = data['samples']
    density = data['density']
    locations = data['locations']
    sigma = data['sigma']

# the dimensionality of the data
d = samples.shape[1]

# find the center of the selected samples
selected_samples = samples[[u,v]]

# use the child as the centroid of the samples
centroid = selected_samples[1]

# make some new samples around the centroid
new_samples = centroid + np.random.normal(0, SAMPLING_SIGMA, (N_NEW_SAMPLES, d))

# compute the new densities
new_density = np.zeros(new_samples.shape[0])

for location in locations:
    new_density += np.exp(-np.sum((new_samples - location)**2, axis=1)/sigma)

# update the data on disk
samples = np.vstack((samples, new_samples))
density = np.concatenate((density, new_density))

# save to disk for the callback's use
np.savez("data", samples=samples, density=density, locations=locations, 
         sigma=sigma)

# compute the new complex
edges = denali.contour.kneighbors_complex(samples, 10)

denali.io.write_vertices(open("vertices", 'w'), density)
denali.io.write_edges(open("edges", 'w'), edges)

subprocess.call(["ctree", "vertices", "edges", "tree.tree"])
sys.stdout.write(open("tree.tree").read())
