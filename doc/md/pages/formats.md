## File Formats

1. [`.tree` - Scalar trees](#tree)
2. [`.weights` - Weight maps](#weights)
2. [`.colors` - Color maps](#colors)

### `.tree`
Denali uses tab-delimited `.tree` files to represent scalar trees. A `.tree`
file consists of three parts:
    
1. The number of vertices
2. Vertex definitions
3. Edge definitions

#### Number of vertices
The first line of a `.tree` file must contain the number of vertices in the
scalar tree as an unsigned integer.

#### Vertex definitions 
Next, each vertex in the scalar tree is defined, one vertex per line. Each
vertex has an associated ID and a scalar value. A vertex's ID must be a unique
unsigned integer, but the IDs do not need to be contiguous. The scalar value can
be any real number that can be represented by your machine's double-precision
floating point type.

To define a vertex, place the vertex's ID and value on a line, separated by a
tab. For example, to define a vertex with ID 42 and value 3.14:

    42  3.14

#### Edge definitions
An edge definition consists of the IDs of the vertices which define the edge, and
an optional list of "members".

At the minimum, an edge must reference two vertices which define its endpoints.
These vertices must have been defined previously in the vertex definition part
of the file. For example, to specify an edge between vertices 0 and 17:

    0   17

The order of the IDs is inconsequential: the above is equivalent to `17    0`.

An edge can also have a list of "members": pseudo-vertices which do not show up
in the structure of the tree, but still have an associated ID and value. Each
member is defined on the same line as the edge it is contained within. The
syntax for defining a member is the same as defining a vertex: first comes the
ID, then the value.

For example, to define an edge between vertices 0 and 17, with a member having
ID 99 and value 10.2, and another member with ID 110 and value -4.2:

    0   17  99  10.2    110 -4.2

#### Example
The following file represents a tree with 9 vertices and 8 edges. Note that the
vertex IDs are not contiguous.

~~~~
9
11	30
10	53
9	51
8	58
7	39
5	32
4	16
3	66
1	62
11	7
10	3	6	64
10	7
9	10
8	7
5	4	0	25
5	1	2	45
5	7
~~~~


### `.weights`
A `.weights` file is used to map vertex and member IDs to positive weight
values. Each line of a weight file takes the following form:

    <ID>    <Weight>

The separation is done with a tab. The vertex weight must be a real number
greater than zero. It is not necessary to specify a weight for each vertex in the
tree: vertex IDs which do not appear in the weight map are assumed to have unit
weight.

#### Example
The following weight map assigns a weight of 2 to vertex 1, a weight of 4 to
vertex 2, and a weight of 99.9 to vertex 7:

~~~~~
1   2
2   4
7   99.9
~~~~~

### `.colors`
A `.colors` file is used to define a color map on a tree. It must assign each
vertex **and** member a real-valued number. This is in contrast to a weight map,
which need not specify a weight for every vertex or member.

Each line of a `.colors` file takes the form:

    <ID>    <Value>

#### Example
The following color map file assigns a second scalar value to each vertex and
member in the tree provided in the `.tree` file example:

~~~~~
11	30
10	53
9	51
8	58
7	39
5	32
4	16
3	66
1	62
6	64
0	25
2	45
~~~~~
