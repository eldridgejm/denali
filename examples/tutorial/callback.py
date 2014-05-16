import sys
import denali

selection = denali.io.read_selection_file(sys.argv[1])
u,v = [x[0] for x in selection['component']]
print "The selection component was {} --> {}.".format(int(u), int(v))
