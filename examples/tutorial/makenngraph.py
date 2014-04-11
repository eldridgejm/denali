import numpy as np
import denali

data = np.loadtxt("data.txt")

# separate the samples from the density
samples = data[:,:3]
density = data[:,4]

# find the edges in a 10-nearest-neighbor graph
edges = denali.contour.kneighbors_complex(samples, 10)

# write the vertices and edges to a file
denali.io.write_vertices(open("vertices.txt", 'w'), density)
denali.io.write_edges(open("edges.txt", 'w'), edges)
