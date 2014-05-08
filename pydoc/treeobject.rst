.. _working-with-trees:

Working with denali tree objects
================================

`denali` defines its own format for files which represent scalar trees, and
`pydenali` offers several utilities for working with this format. `pydenali`
uses the [`networkx`](https://networkx.github.io/) package to represent trees
and graphs.

`pydenali` stores information about each of the nodes and edges of a tree in the
`networkx` object itself. The following conventions always hold true:

- A node's ID is it's `networkx` identifier. That is, for a tree `t`,
  ``t.nodes()`` lists all of the node IDs of the nodes in the tree.

- A node's scalar value is located in the `networkx` node's data dictionary
  under the "value" key. For example, to access the scalar value associated with
  node 0 in a tree `t`, we write:

  >>> t.node[0]['value']

- An edge's members are included as a dictionary under the "members" key in the
  edge's data dictionary. In other words, each edge has a "members" dictionary
  associated with it. Each key in the members dict is the integer ID of the
  member node, and each value is the scalar value associated with the member.

  For example, to get the members of the edge between 0 and 2:

  >>> t.edge[0][2]['members']

  And if we knew, for example, that 1 was a member of this edge, we could get
  its associated scalar value by writing:

  >>> t.edge[0][2]['members'][1]

If a colormap has been supplied to `denali`, there may be a "reduction" entry in
the data dictionary for each edge, representing the value of the reduction of
the second color function on that edge. To obtain the reduction value of edge 0
to 2, we would write

    >>> t.edge[0][2]['reduction']
