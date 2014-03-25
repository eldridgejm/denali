import numpy as _numpy
import itertools as _itertools
from StringIO import StringIO as _StringIO

def read_selection(fileobj):
    """Read the selection information from a file-like object."""

    def _process_filename(data):
        return list(data)[0].strip()

    def _process_array(lines):
        data_string = "".join(lines)
        return _numpy.loadtxt(_StringIO(data_string))

    process_map = {
            "file": _process_filename,
            "component": _process_array,
            "members": _process_array,
            "subtree": _process_array
            }

    # group the lines of the selection file, breaking on #
    grouping = _itertools.groupby(fileobj, lambda x: x.startswith('#'))

    # zip together the keys with the data groups
    flags = (list(g)[0].lstrip('# ').rstrip('\n') for k,g in grouping if k)
    data = (g for k,g in grouping if not k)

    selection_information = {}

    for flag, data in _itertools.izip(flags, data):
        if flag in process_map:
            selection_information[flag] = process_map[flag](data)

    return selection_information


def write_tree(fileobj, tree):
    """Writes a tree in denali format."""
    # write the number of vertices
    f.write("{}\n".format(len(tree)))

    # now write each node to the file, along with its value
    for node in tree:
        f.write("{}\t{}\n".format(node, tree.node[node]['value']))

    # and write each edge
    for u,v in tree.edges_iter():
        f.write("{}\t{}\n".format(u,v))


def write_colors_from_arrays(fileobj, ids, values):
    """Writes a color map in denali format. 
    
    Input is specified as two arrays: the first being the list of IDs, the
    second being the scalar values corresponding to the IDs in the first
    array."""
    data = _numpy.column_stack((ids, values))
    _numpy.savetxt(fileobj, data, fmt="%d\t%f")
