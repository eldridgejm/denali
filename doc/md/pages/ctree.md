## ctree
ctree is a command line tool for generating contour trees from scalar functions
defined on a simplicial complex.

1. [Usage](#usage)
0. [Input Formats](#input-formats)

### Usage

~~~~
ctree <vertex value file> <edge file> <tree file> 
      [--join <filename>] [--split <filename>]
~~~~

ctree is called from the command line. It takes three required arguments:
a vertex value file, an edge file, and a tree file. The vertex value and
edge files are used as input, and the tree file is used as output. The format
of the input files are described below, while the format of the output
file is described in the [`.tree` format](./formats.html#tree) page.

By default, ctree outputs only the contour tree. The join and split trees are
computed in the process, but not output in order to save memory. To output the
join or split tree, provide the `--join` or `--split` tag, followed by the
output file. For example, to output both the contour tree and the join tree,
invoke:

    ctree vertex_file edge_file contour.tree --join join.tree

This will place the contour tree in `contour.tree` and the join tree in 
`join.tree`.


### Input Formats
The input to ctree is the 1-skeleton of a simplicial complex. In other words, ctree
requires a list of vertices and a list of connections between these vertices.
Furthermore, each vertex in the simplicial complex has an associated scalar
value.

The vertices and edges are defined in separate files. The vertex file consists
of a series of scalar values, one per line. The ID of the vertices is implicit
from their order in this file. For example, the file:

~~~
3.1
-3.4
-2.1
42.1
~~~~

Defines four vertices. Vertex 0 has value 3.1, vertex 1 has value -3.4, and so
on. Note that vertices are 0-indexed.

The edge file lists the edges in the simplicial complex, one per line. An edge
is a tab-separated pair of vertex IDs. For example, to define an edge between 0
and 1:

    0   1

When specifying an edge, the order of the nodes does not matter: `1 0` is the 
same as `0  1`. Note, however, that edges should be uniquely specified.
