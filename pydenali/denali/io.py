import itertools as _itertools
from StringIO import StringIO as _StringIO

try:
    import networkx as _networkx
except ImportError:
    _has_networkx = False
else:
    _has_networkx = True


def _read_vertex_definitions(string):
    """Reads vertex definitions from a string.

    Vertex definitions have the following form:

        <vertex id> <vertex value>

    This function reads them into a list of 2-lists, where the first entry in 
    each sublist is an integer id, and the second is a float value.
    """
    defs = []
    split = string.strip().split()
    pairs = zip(*([iter(split)] * 2))

    for str_id, str_value in pairs:
        defs.append([int(str_id), float(str_value)])

    return defs


def read_selection(fileobj):
    """Read the selection information from a file-like object.

    :param fileobj: A file-like object containing the selection file.
    :type fileobj: File-like
    :returns: A dictionary containing information about the selection.

    This function is used to parse a selection file into a python dict. 
    The input is a file-like object, like that returned by the `open()`
    function.

    The returned dictionary has the following entries:

    **file**
        A string containing the full path to the file from which the
        visualization was generated.

    **component**
        A 2x2 numpy array representing the nodes in the selection.

    **members**
        A nx2 numpy array representing the members in the selection.

    **subtree**
        (Optional) Contains the members and nodes in the selected subtree.

    Each of *component*, *members*, and *subtree* is a numpy array with two
    columns. Each row represents a single node or member. The first column
    contains the ids of the node or member, and the second contains the scalar
    value.

    *Example*:

    >>> denali.io.read_selection(open("selection_file"))
    {'component': array([[  4.,  16.],
            [  5.,  32.]]),
     'file': '/home/eldridge/tree.tree',
     'members': array([[  0.,  25.],
            [  4.,  16.],
            [  5.,  32.]]),
     'subtree': array([[  1.,  62.],
            [  2.,  45.],
            [  3.,  66.],
            [  6.,  64.],
            [  7.,  39.],
            [  8.,  58.],
            [  9.,  51.],
            [ 10.,  53.],
            [ 11.,  30.]])}

    """

    def _process_filename(data):
        return list(data)[0].strip()

    def _process_array(lines):
        data_string = "".join(lines)
        return _read_vertex_definitions(data_string)

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


def read_tree(fileobj):
    """Reads in a .tree file to a networkx tree object.

    :param fileobj: A file-like object representing a tree in 
        ``.tree`` file format
    :type fileobj: File-like
    :returns: A networkx undirected graph.

    The returned graph's nodes have a `value` attribute that corresponds
    to the scalar value of the node in the tree. The edges have a `members`
    attribute which is a dictionary mapping member ids to their values.
    """
    if not _has_networkx:
        raise RuntimeError("The networkx package is required to read a tree.")

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


def write_tree(fileobj, tree):
    """Writes a tree in denali format.

    :param fileobj: A file-like object that will be written to.
    :type fileobj: File-like

    :param tree: The tree to be written.
    :type tree: Networkx graph

    The ``tree`` must have the same attributes as described in `read_tree()`.
    That is, its nodes must have a `value` attribute, and the edges must
    have a `members` attribute.
    """
    if not _has_networkx:
        raise RuntimeError("The networkx package is required to read a tree.")

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


def write_weights(fileobj, ids, weights):
    """Writes a weight map from two arrays, one containing the ids, and 
    another with their corresponding weights.

    :param fileobj: The file-like object where the output will be written.
    :type fileobj: File-like

    :param ids: A list of the ids in the map.
    :type ids: List-like

    :param weights: A list of the weights in the map.
    :type weights: List-like
    """


    data = _numpy.column_stack((ids, weights))
    _numpy.savetxt(fileobj, data, fmt="%d\t%f")


def write_colors(fileobj, ids, values):
    """Writes a color map in denali ``.colors`` format. 

    :param fileobj: A file-like object that will be written to.
    :type fileobj: File-like

    :param ids: A list of the integer ids in the color map.
    :type ids: List-like

    :param values: A list of the scalar values associated with each id.
    :type values: List-like

    Input is specified as two arrays or python lists: the first being the list
    of IDs, the second being the scalar values corresponding to the IDs in the
    first array.""" 
    for vertex_id, vertex_value in zip(ids, values):
        fileobj.write("{}\t{}\n".format(vertex_id, vertex_value))

def write_vertices(fileobj, vertex_values):
    """Writes the contiguous vertex values to the file.

    :param fileobj: The file-like object where the output will be written.
    :type fileobj: File-like

    :param vertex_values: The values of the vertices.
    :type vertex_values: List-like

    The ``vertex_values`` list of vertices implicitly defines the id of the
    vertex so that ``vertex_values[i]`` represents the value of the vertex
    with id ``i``.
    """
    fileobj.write("\n".join([str(x) for x in vertex_values]))


def write_edges(fileobj, edges):
    """Writes the edges to the file.

    :param fileobj: The file-like object where the output will be written.
    :type fileobj: File-like

    :param edges: The edges as pairs of vertex ids.
    :type edges: List-like

    ``edges`` can be a python list of 2-tuples or 2-lists, or a numpy array
    with 2 columns.
    """
    for u,v in edges:
        fileobj.write("{}\t{}\n".format(u,v))
