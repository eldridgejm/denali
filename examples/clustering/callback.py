#!/usr/bin/env python2

import matplotlib.pyplot as plt
import numpy as np
import sys
import os

import denali

# make the directory of this script the current working directory
os.chdir(os.path.dirname(__file__))

# the names of the data and label files
DATA_FILE  = "mnist_017.npy"
LABEL_FILE = "mnist_017_labels.npy"


def plot_digit(digit_image):
    """Plots a digit."""
    plt.figure()
    plt.imshow(digit_image)
    plt.show()

def main():
    # load the data
    data = np.load(DATA_FILE)
    labels = np.load(LABEL_FILE)

    selection = denali.io.read_selection_file(sys.argv[1])

    if "subtree" in selection:
        members = selection['subtree'].nodes()
    else:
        members = [x[0] for x in selection['members']]

    # find the nodes which are leaf nodes, not cluster nodes
    singles = np.array([x for x in members if x < data.shape[0]], int)

    component_labels = labels[singles]

    distribution = np.bincount(component_labels, minlength=10)

    if singles.shape[0] > 0:
        component_data = data[singles]
        component_mean = np.mean(component_data, axis=0)
        digit_image = np.reshape(component_mean, (28,28))

        plot_digit(digit_image)

if __name__ == "__main__":
    main()
