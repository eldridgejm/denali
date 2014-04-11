import numpy as np

# the locations of our three Gaussians
locations = np.array(
                    [[-1,-1,-1, 0],
                     [-1,-1,-1,-2],
                     [-1,-1,-1, 2]])

# the spread of each Gaussian
sigma = 0.5

# generate the points in our cloud uniformly
samples = np.random.uniform(-4, 4, (10000, 4))

# now we evaluate the density at each sample point
density = np.zeros(samples.shape[0])

for location in locations:
    density += np.exp(-np.sum((samples - location)**2, axis=1)/sigma)

np.savetxt("data.txt", np.column_stack((samples, density)), fmt="%5.5e")
