import sys
import denali

with open(sys.argv[1]) as f:
    selection = denali.io.read_selection(f)
    u,v = selection['component'][:,0]
    print "The selection component was {} --> {}.".format(int(u), int(v))
