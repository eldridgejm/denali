#!/usr/bin/env python2

from multiprocessing import Process
from multiprocessing.connection import Listener
import pickle
import sys

import matplotlib.pyplot as plt
import numpy as np

import nnsample

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
    parameters = np.load(sys.argv[1])

    with open(sys.argv[2]) as f:
        topology = pickle.load(f)

    targets = np.load(sys.argv[3])

    # set up the listener
    listener = Listener(("localhost", 6000))
    conn = listener.accept()

    while True:
        selection = conn.recv()

        if selection is None:
            conn.close()
            conn = listener.accept()
            continue

        # get the id of the child of the component
        child = int(selection['component'][1,0])

        selected_param = parameters[child]
        net = nnsample.unpack_network(selected_param, topology)

        Process(target=plot_net, args=[net, targets]).start()

if __name__ == "__main__":
    main()
