import itertools as _itertools
from StringIO import StringIO as _StringIO
import networkx as _networkx


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
        A 2x2 nested list representing the nodes in the selection.

    **members**
        A nx2 nested list representing the members in the selection.

    If a colormap was specified in `denali`, the following entry is present:
        
    **reduction**
        The result of applying the reduction to the selected component and its
        members

    If the "Provide subtree" box has been checked in `denali`'s callback
    configuration dialog, then the additional entry is present:

    **subtree**
        A networkx tree object representing the selected subtree. The structure
        of the subtree is as described in :ref:`working-with-trees`.

    *component* and *members* are nested lists with two columns. Each
    row represents a single node or member. The first column contains the ids
    of the node or member, and the second contains the scalar value.

    *Example*:

    >>> denali.io.read_selection(open("selection_file"))
    {'component': [[4, 16.0], [5, 31.0]],
     'file': '/path/to/tree/file.tree',
     'members': [[0, 25.0], [4, 16.0], [5, 32.0]],
     'reduction': 1.89452,
     'subtree': <networkx.classes.digraph.DiGraph object at 0x379bd10>}
    """
    def _process_filename(data):
        return list(data)[0].strip()

    def _process_array(lines):
        data_string = "".join(lines)
        return _read_vertex_definitions(data_string)

    # members and components are both arrays
    _process_members = _process_component = _process_array 

    def _process_reduction(line):
        return float(next(line).strip())

    def _process_subtree(lines):
        """Read the subtree section into a networkx object."""
        return read_tree(_StringIO("".join(lines)))

    def _process_subtree_reduction(lines):
        """Reads the subtree reduction in."""
        reduction = {}
        for line in lines:
            parent, child, value = line.split()

            reduction[tuple([int(parent), int(child)])] = float(value)

        return reduction

    process_map = {
            "file": _process_filename,
            "component": _process_component,
            "members": _process_members,
            "reduction": _process_reduction,
            "subtree": _process_subtree,
            "subtree_reduction": _process_subtree_reduction
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

    # the tree was read as an undirected tree, but we know the root
    if "subtree" in selection_information:
        tree = selection_information["subtree"]
        root = selection_information["component"][0][0]

        directed_tree = _networkx.dfs_tree(tree, root)

        selection_information["subtree"] = directed_tree

        if "subtree_reduction" in selection_information:
            subtree_reduction = selection_information["subtree_reduction"]
            del selection_information["subtree_reduction"]

            subtree = selection_information["subtree"]

            for (u,v),value in subtree_reduction.iteritems():
                subtree.edge[u][v]['reduction'] = value

    return selection_information


def read_tree(fileobj):
    """Reads in a .tree file to a networkx tree object.

    **Note**: This function requires that the `networkx` package is installed.

    :param fileobj: A file-like object representing a tree in 
        ``.tree`` file format
    :type fileobj: File-like
    :returns: A `networkx` undirected graph.

    The returned graph's nodes have a `value` attribute that corresponds
    to the scalar value of the node in the tree. The edges have a `members`
    attribute which is a dictionary mapping member ids to their values.
    """
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

    **Note**: This function requires that the `networkx` package is installed.

    :param fileobj: A file-like object that will be written to.
    :type fileobj: File-like

    :param tree: The tree to be written.
    :type tree: `networkx` graph

    The ``tree`` must have the same attributes as described in `read_tree()`.
    That is, its nodes must have a `value` attribute, and the edges must
    have a `members` attribute.
    """
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
        fileobj.write("\n")


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
    for vertex_id, vertex_value in zip(ids, weights):
        fileobj.write("{}\t{}\n".format(vertex_id, vertex_value))


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
