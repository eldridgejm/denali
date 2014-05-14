#!/usr/bin/env python2

import pickle
import sys
import matplotlib.pyplot as plt
import numpy as np
import os

import denali

# change to the directory of this file
os.chdir(os.path.dirname(__file__))

import nnsample

PARAMS_FILE     = "all_params.npy"
TOPOLOGY_FILE   = "topology.pickle"
TARGETS_FILE    = "targets.npy"


def plot_net(net, targets):
    """Plots the neural network's predictions for sine on [0,2*pi]"""
    x = np.linspace(0, 2*np.pi, 100)

    y_sim = net.sim(x.reshape(-1,1))
    y_true = np.sin(x)

    plt.plot(x,y_sim)

    plt.plot(x,y_true, color='black', linestyle='--')

    plt.scatter(*targets.T, alpha=0.5)
    plt.show()


def main():

    # load the data
    parameters = np.load(PARAMS_FILE)

    with open(TOPOLOGY_FILE) as f:
        topology = pickle.load(f)

    targets = np.load(TARGETS_FILE)

    selection = denali.io.read_selection_file(sys.argv[1])

    # get the id of the child of the component
    child = int(selection['component'][1][0])

    selected_param = parameters[child]
    net = nnsample.unpack_network(selected_param, topology)

    # Process(target=plot_net, args=[net, targets]).start()
    plot_net(net, targets)

if __name__ == "__main__":
    main()
