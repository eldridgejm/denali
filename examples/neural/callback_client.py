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

# close the connection
conn.send(None)
conn.close()
