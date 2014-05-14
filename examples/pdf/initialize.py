#!/usr/bin/env python2

import sys
import subprocess

import numpy as np

sys.path.append("../../../code/python/")
import denali

print "Sampling from probability distribution..."

locations = np.array(
                    [[-1,-1,-1, 0],
                     [-1,-1,-1,-2],
                     [-1,-1,-1, 2]])

samples = np.random.uniform(-4, 4, (1000, 4))
density = np.zeros(samples.shape[0])
sigma = 1

for location in locations:
    density += np.exp(-np.sum((samples - location)**2, axis=1)/sigma)

edges = denali.contour.kneighbors_complex(samples, 10)

denali.io.write_vertices(open("vertices", 'w'), density)
denali.io.write_edges(open("edges", 'w'), edges)

# save to disk for the callback's use
np.savez("data", samples=samples, density=density, locations=locations, 
         sigma=sigma)

print "Computing contour tree..."
subprocess.call(["ctree", "vertices", "edges", "tree.tree"])
