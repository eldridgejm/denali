#!/usr/bin/env python2

from multiprocessing.connection import Listener
from multiprocessing import Process
import argparse
import matplotlib.pyplot as plt
import numpy as np
import sys
import denali

def plot_digit(digit_image):
    """Plots a digit."""
    plt.figure()
    plt.imshow(digit_image)
    plt.show()

def main():

    parser = argparse.ArgumentParser(description="Callback server for "
        "hierarchical clustering example.")

    parser.add_argument("data_file", type=str, help="The numpy array file "
        "containing the digit features.")
    parser.add_argument("labels_file", type=str, help="The numpy array file "
        "containing the digit labels.")

    args = parser.parse_args()

    # load the data
    data = np.load(args.data_file)
    labels = np.load(args.labels_file)

    # set up the listener
    listener = Listener(("localhost", 6000))
    conn = listener.accept()

    while True:
        selection = conn.recv()

        if selection is None:
            conn.close()
            conn = listener.accept()
            continue


        if "subtree" in selection:
            members = selection['subtree'].nodes()
        else:
            members = [x[0] for x in selection['members']]

        # find the nodes which are leaf nodes, not cluster nodes
        singles = np.array([x for x in members if x < data.shape[0]], int)

        component_labels = labels[singles]

        distribution = np.bincount(component_labels, minlength=10)
        conn.send(distribution)

        if singles.shape[0] > 0:
            component_data = data[singles]
            component_mean = np.mean(component_data, axis=0)
            digit_image = np.reshape(component_mean, (28,28))

            Process(target=plot_digit, args=[digit_image]).start()
            # plot_digit(digit_image)

if __name__ == "__main__":
    main()
