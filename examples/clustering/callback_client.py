#!/usr/bin/env python2
from multiprocessing.connection import Client
import socket
import sys

sys.path.append("../../../code/python/")
import denali

# process the input
with open(sys.argv[1]) as f:
    selection = denali.io.read_selection(f)

try:
    conn = Client(("localhost", 6000))
except socket.error:
    print "Error: Couldn't connect to the visualization server. Is it running?"
    sys.exit()

conn.send(selection)

distribution = conn.recv()
print "<b>Distribution of digits in selection:</b><br>"
print ("&nbsp;"*4).join(
        ["<b>{}</b>: {}".format(x, d) for x,d in enumerate(distribution)])

# close the connection
conn.send(None)
conn.close()
