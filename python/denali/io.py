import numpy as _numpy
import itertools as _itertools
import networkx as _networkx
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
    fileobj.write("{}\n".format(len(tree)))

    # now write each node to the file, along with its value
    for node in tree:
        fileobj.write("{}\t{}\n".format(node, tree.node[node]['value']))

    # and write each edge
    for u,v in tree.edges_iter():
        fileobj.write("{}\t{}".format(u,v))

        for member_id, member_value in tree.edge[u][v]['members'].iteritems():
            fileobj.write("\t{}\t{}".format(member_id, member_value))


def write_colors_from_arrays(fileobj, ids, values):
    """Writes a color map in denali format. 
    
    Input is specified as two arrays: the first being the list of IDs, the
    second being the scalar values corresponding to the IDs in the first
    array."""
    data = _numpy.column_stack((ids, values))
    _numpy.savetxt(fileobj, data, fmt="%d\t%f")


def write_vertices(fileobj, vertex_values):
    """Writes the contiguous vertex values to the file."""
    fileobj.write("\n".join([str(x) for x in vertex_values]))


def write_edges(fileobj, edges):
    """Writes the edges to the file."""
    for u,v in edges:
        fileobj.write("{}\t{}\n".format(u,v))


def write_weights_from_arrays(fileobj, ids, weights):
    """Writes a weight map from two arrays, one containing the ids, and 
    another with their corresponding weights."""
    data = _numpy.column_stack((ids, weights))
    _numpy.savetxt(fileobj, data, fmt="%d\t%f")


def read_tree(fileobj):
    """Reads in a .tree file to a networkx tree object."""
    tree = _networkx.Graph()

    for i,line in enumerate(fileobj):
        if i == 0:
            n_vertices = int(line)
        elif i <= n_vertices:
            split_line = line.split()
            vertex_id = int(split_line[0])
            vertex_value = float(split_line[1])
            tree.add_node(vertex_id, value=vertex_value)
        else:
            split_line = line.split()

            # extract the endpoints of the edge
            u,v = [int(x) for x in split_line[:2]]
            tree.add_edge(u,v)

            # extract the members
            tree.edge[u][v]['members'] = {}
            member_pairs = zip(*([iter(split_line[2:])]*2))
            for member_id,member_value in member_pairs:
                tree.edge[u][v]['members'][int(member_id)] = float(member_value)

    return tree
