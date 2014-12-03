# Tutorial

This tutorial serves as an in-depth first introduction to denali. Along the way,
you will gain an understanding for how to interpret the visualization produced
by *denali*, how to use the graphical interface, and how to use *denali*'s tools
to visualize other sources of data, such as a scalar functions defined on point
clouds.

The files we will use in this tutorial are located in the `examples/tutorial`
directory.

---

**Table of contents**:

- [Introduction to landscape metaphors](#introduction-to-landscape-metaphors)
    - [Scalar trees](#scalar-trees)
    - [Interpreting landscapes](#interpreting-landscapes)
- [Basic usage](#basic-usage)
    - [The input tree](#the-input-tree)
    - [Loading the tree](#loading-the-tree)
    - [Exploring the landscape](#exploring-the-landscape)
    - [Selecting a component](#selecting-a-component)
    - [Simplifying the visualization](#simplifying-the-visualization)
    - [Choosing a root](#choosing-a-root)
    - [Rebasing the landscape](#rebasing-the-landscape)
- [Specifying custom behavior](#specifying-custom-behavior)
    - [Specifying a weight map](#specifying-a-weight-map)
    - [Specifying a color map](#specifying-a-color-map)
    - [Printing special information about a
      selection](#printing-special-information-about-a-selection)
- [Visualizing functions on point
  clouds](#visualizing-functions-on-point-clouds)
    - [Introduction to contour trees](#introduction-to-contour-trees)
    - [Building a neighbor graph](#building-a-neighbor-graph)
    - [Computing the contour tree](#computing-the-contour-tree)
- [Other sources of hierarchical structure](#other-sources-of-hierarchical-structure)


---

## Introduction to landscape metaphors

As described in the [introduction](intro.html), *denali* is a tool for
visualizing data that meets two prerequisites. First, each data point must have
an associated scalar value, whether it be a probability, score, cost, etc.
Second, it must be possible to extract some tree-like structure from the data.
[Later](#visualizing-functions-on-point-clouds) in this tutorial, we'll see how
to extract tree-like structure from such things as scalar functions defined on
point clouds. *Denali* can also be used in other useful cases, such as
visualizing the structure of a tree-like manifold, and interpreting hierachical
structure in a dataset through agglomerative clustering. For now, we'll assume
that a scalar tree has already been extracted from our data, and we'd now
like to visualize the structure using *denali*.

### Scalar trees

The fundamental unit of input in *denali* is the *scalar tree*. A scalar tree is
a tree for which every node has an associated integer ID and scalar value. An
example of a (directed) scalar tree is shown below.

<center>
<img style="margin:20pt 20pt" width=400 src="../resources/tree.png">
</center>

Note that while the above tree is directed, in general, input to *denali* is an
undirected scalar graph. A direction is imposed whenever a root node is chosen.
This can be done using *denali*'s interface.

A **node** is a vertex of the scalar tree. It has an integer ID and a scalar
value.

A **member** is like a node, but it is contained in the edges of the tree. It
also has an integer ID and a scalar value.

In the above image, nodes are contained within the ellipses, and members are
printed next to the edges in which they are contained. 

Each node and member can be assigned a positive real number as a **weight**. By
default, this weight is simply one. Later, we'll see how to specify a weight map.

Each arc of a directed scalar tree may also be referred to as a **component** of
the tree. Since each component may have members, we can define the **component
weight** to be the sum of all member weights. Since each member by default has
unit weight, this usually means that the default component weight is simply the
number of members it contains. For example, in the above scalar tree, the 4 → 5
component has weight 1, while the 5 → 7 component has weight zero.

We'll also want to define the notion of a **total weight** on the nodes of the
directed tree.  If no custom weight map is specified, meaning that each node and
member has unit weight, the total weight of a node *v* is simply the number of
nodes and members in the subtree rooted at *v*, including *v* itself.  

More specifically, the total weight of a node is defined to be the sum of:

- the total weights of the node's children,
- the weight of the node itself,
- and the weights of all outgoing arcs.

Assuming that the weight of each node and member is one, as it is by default,
the total weight of node 3 in the above directed tree is simply one. The total
weight of node 10 is 4. To see this, note that each of its two children have
unit weight.  It has two outgoing arcs, but only one of these arcs has nonzero
weight: the one that contains member 6. Node 10 itself has weight 1, bringing
the total weight to 4. Lastly, the total weight of the root of the tree, node 4,
is 12. This is equal to the total number of nodes and members in the tree as a
whole.


### Interpreting landscapes

*Denal* visualizes scalar trees like those described above as landscape
metaphors. To understand a landscape metaphor, let's build one from the simple
scalar tree show below:

<center>
<img style="margin:20pt 20pt" width=300 src="../resources/interpret_tree.png">
</center>

The exact scalar values associated with each node aren't important, but note
that the tree is laid out so that the scalar values increase from bottom to top.
That is, the scalar value associated with node 0, denoted by *f(0)*, is less
than the scalar value associated with node 1.  Node 7 has the highest scalar
value.

Also note that each edge has zero or more members, denoted by the black dots
placed on the lines between nodes. We'll assume that all nodes and members have
the default weight of one.

The first step in drawing this tree as a landscape is to pick a root node. Much
of the time, the node with the minimum scalar value is the natural choice to be
the root node. Let's make node 0 the root of our landscape.

Next, we draw the rectangular treemap representing the tree. The figure below
shows what this looks like:

<center>
<img style="margin:20pt 20pt" width=300 src="../resources/interpret_map.png">
</center>

You'll notice that each node in the tree has been mapped to a shape in the
treemap. Branch nodes (and the root node) have been mapped to rectangles. Leaf
nodes have been mapped to points. In what follows, the 2-d shape to which a node
is mapped to is referred to as its **contour**.

The nested relationship of the contours is important. If a contour *u* contains
another contour *v*, then *v* is in the subtree rooted at node *u*. If *v*'s
contour is directly nested within *u*'s, then *v* is a child of *u*. For
example, node 0's contour is the outermost contour, which is to be expected
because every node in the tree is in the subtree rooted at node 0. Node 1 is the
child of node 0, and so its contour is nested directly within node 0's.

A branch node's contour is split into several pieces: one for each of its
children. The contour of each child is embedded within its corresponding piece.
Consider, for example, branch contour 2. It is split into two pieces: one for
node 3, and another for node 4. The region between the boundary of a split piece
and the inner nested contour represents an arc. The following figure makes this
clear:

<center>
<img style="margin:20pt 20pt" width=450 src="../resources/interpret_correspondence.png">
</center>

Some of the arcs of the tree and their corresponding regions in the map have
been colored. Note, for example, the orange region between contour 1 and contour
2. This corresponds to the orange arc in the tree between node 1 and node 2.

Since arcs in the tree correspond to regions in the map, it makes sense to
choose the areas of these regions so that they reflect the **component weights**
of their corresponding arcs. For example, look at the arcs 2 → 3 and 2 → 4. Arc
2 → 3 has two members, and therefore has component weight 2. Arc 2 → 4, on the
other hand, has no members, and so it has weight 0. This is reflected in the
treemap: the region corresponding to arc 2 → 3 is larger than the region
corresponding to 2 → 4. In general, the area between two contours is a function
of the weight of the arc connecting the corresponding nodes in the tree.

Likewise, the area of a contour itself encodes information about the **total
weight** of the corresponding node. Notice, for example, that the area of
contour 5 is larger than that of contour 2. This implies that the total weight
of node 5 is larger than that of node 2, which is in fact the case: node 5 has
total weight of 8, while node 2 has total weight 5. In general, the area of
contour 5 is greater than the area of contour 2 by a ratio that will be
(approximately) 8:5.

The last step in creating the landscape metaphor is to "lift" each contour to a
height in the third dimension that corresponds to it's nodes scalar value. This
looks like the following:

<center>
<img style="margin:20pt 20pt" width=400 src="../resources/interpret_landscape.png">
</center>

Viewed from above, the landscape metaphor is exactly the treemap described
above. From the side, however, the height of points on the landscape convey
information about the scalar function defined on the tree. Node 7, for example,
has the highest scalar value in our tree, and it is therefore the tallest point
in our landscape metaphor.

Next we will see how to use the software to build and visualize a simple tree.

## Basic usage

### The input tree
The tree we will be visualizing is shown below:

<center>
<img style="margin:20pt 20pt" width=400 src="../resources/tree.png">
</center>

Of course, this tree is simpler than the usual sort of input we'll want to
visualize with *denali*, but it is useful for demonstration purposes.

Each node has a positive integer id and a scalar value. In this case, the values
are also positive integers, but in general they can be arbitrary real numbers.
We also see that some edges have labels next to them. These represent the edges
*members*: lists of vertices that are hidden within each edge. Each member has
an unique id, distinct from the ids of the vertices in the tree, and an
associated scalar value.

The undirected version of the above tree is represented in
`examples/tutorial/tree.tree`.

If we look inside the file, this is what we see:

~~~~~
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
~~~~~

In brief, the format of the `tree.tree` file is as follows: the first line
states the number of vertices in the tree. The next nine lines define the
vertices: the first number is the id, and the second is the scalar value. The
two numbers are separated by a tab. The remainder of the lines list the edges in
the undirected tree. The first two numbers on each line are the ids of the
vertices defining the edge. The trailing pairs of numbers on the line (if there
are any) define the edge's members: the first number in the pair is the member's
id, the second number is the member's value. 

For more detailed information on the `.tree` format, see the [format
specification](formats.html#tree).

### Loading the tree
With *denali* running, click **File→Open Tree** and select the `tree.tree` file
in `examples/tutorial`. You'll see the following (*click to enlarge*):

<center>
<a href="../resources/scrot_open.png">
<img class="screenshot" src="../resources/scrot_open.png">
</a>
</center>

*Denali* opens with with a birds-eye view of the landscape. By default, the node
in the tree with the minimum scalar value is used as the base of the landscape.
We'll see how to change this shortly.

### Exploring the landscape
To explore the visualization, use the mouse:

- **Hold left mouse**: Click and drag to rotate the landscape
- **Scroll mouse wheel**: Zoom in and out
- **Hold mouse wheel**: Translate the landscape
- **Click right mouse**: Select a component

### Selecting a component

Right clicking selects a component of the landscape. Each component of the
landscape is mapped to an arc of the tree. When a component is selected, general
information about the selection is printed in the status box in the lower left
of the display.

Click the large blue component at the base of the landscape. The status box
prints the following information:

~~~~
Component Selected: 4 → 5
Parent value: 16
Child value: 32
Persistence: 16
Component weight: 1
Parent total weight: 12
Child total weight: 10
~~~~

For a refresher on what some of these terms mean, see the above section on
[scalar trees](#scalar-trees). In summary:

- **Component selected**: The ids of the nodes in the selected component are shown
  here. When the file was opened, the node with the minimum scalar value was
  automatically assumed to be the root of the tree. This induces a direction on
  the edges of the tree. Here, we see that node 4 is the parent of node 5 in the
  directed tree.

- **Parent value**: The scalar value associated with the parent node. Here, this
  is the scalar value associated with node 4.

- **Child value**: The scalar value associated with the child node. Here, this
  is the scalar value associated with node 5.

- **Persistence**: The *persistence* of the arc is the absolute difference
  between the parent value and the child value. This is used in simplification,
  as we'll see shortly.

- **Component weight**: The "weight" of a component is the sum of the weights of
  the members contained within the component. By default, a node or member's
  weight is simply one. If we look at the tree we used as input, we see that the
  edge between nodes 4 and 5 had a single member.  Therefore, the weight of this
  component is one. Later, we'll see how to specify a positive real number to
  the vertices and members in the graph, effectively weighting components of the
  landscape. Note that edges which have no members have a component weight of
  zero.

- **Parent total weight**: The weight of the parent node, plus the weight of all
  nodes and members in the subtree rooted at the parent node.

- **Child total weight**: The weight of the child node, plus the weight of all
  nodes and members in the subtree rooted at the child node.

### Simplifying the visualization
The tree we are visualizing now isn't very complicated, but sometimes,
especially with noisy data, the tree may have many spurious features. These
present as small, spiky features in the landscape, and can be distracting.

*Denali* provides a way of suppressing these small features using
*simplification by persistence*. As described above, *persistence* is the height
of component in the landscape: the absolute difference between the parent's
scalar value and the child's. A reasonable assumption is that leafy arcs with
small persistence aren't very important, while arcs with high persistence are
more likely to be of interested.

Simplification in *denali* is an operation on an entire subtree. First, we'll
simplify the entire tree:

1. Right click the base of the landscape to select component 4 → 5.
2. Adjust the simplification threshold slider to about 10.
3. Click **Refine Subtree**.

Your landscape should look like the following:

<center>
<a href="../resources/scrot_simplify.png">
<img class="screenshot" src="../resources/scrot_simplify.png">
</a>
</center>

*Denali* has iteratively collapsed leafy arcs with persistence less than the
threshold. Along the way, new leafy arcs are created. For example, exploring
the simplified landscape shows that the arc 7 → 3 now exists in our simplified
tree. This arc wasn't present in the original tree, but simplification has
connected the two. Note that the arc has a component weight of 3: this is
because the arc "contains" nodes 9 and 10, as well as the member 6 which was
originally in the arc from 10 → 3. In this way, the total weight of node 7 has
remained the same. That is to say: simplification of a subtree preserves the
subtree's total weight.

A careful reader will note that node 11 is no longer in the landscape and it
wasn't listed as a member of the 7 → 3 arc. In actuality, node 11 is internally
represented as a member of node 7 and contributes to its total weight.

Simplifications can be made in series, and they apply only to the subtree of the
selected component. For example, suppose we wanted to see the structure of the
simplified 7 → 3 component in more detail.

1. Right click the 7 → 3 component.
2. Adjust the simplification slider to about 5.
3. Click **Refine Subtree**.

You'll see the following:

<center>
<a href="../resources/scrot_simplify2.png">
<img class="screenshot" src="../resources/scrot_simplify2.png">
</a>
</center>

The 7 → 3 component was completely expanded, and the subtree was then simplified
down using the new threshold of 5. However, other parts of the landscape, which
were previously simplified with a threshold of 10, were left untouched.

Lastly, we may wish to start over and view the entire, unsimplified tree. To do
this, select the **Expand** button in the simplification pane.


### Choosing a root
When a new landscape is opened in *denali* the node with the minimum scalar
value is used as the base of the landscape. Depending on what the tree
represents, however, it may be useful to select a different node as the root.

To select a root, click the **Choose Root** button in the *Root Selection* pane.
To use the maximum node as the root, select the **Maximum node** option. Any
vertex in the tree may be used as the root. For example, to set node 7 to be the
root, click the radio button next to **Other** and insert 7 in the box, then
click **Ok**.

As you see, not all root selections lead to interesting or
aesthetically-pleasing visualizations. More often than not, the maximum or
minimum nodes are the best choices.

Before we continue, set the root of the landscape to be the minimum node.


### Rebasing the landscape
Suppose we are interested in a particular subtree or section of the landscape.
We can visualize this section alone by *rebasing* the tree.

For example, suppose we are only interested in the subtree rooted at node 7,
and we'd like to get rid of the rest of the landscape:

1. Right click to select the 5 → 7 component. In the context of rebasing, this
   is equivalent to selecting the 7 node.
2. Click the **Rebase** button in the *Root Selection* pane.

The rebased landscape now appears in the visualization window. To return to
visualizing the full landscape, you'll need to open the tree again by selecting
**File → Open Tree**. Before proceeding with the tutorial, make sure that you've
done so.

## Specifying Custom Behavior

### Specifying a weight map

**Important**: Before continuing, make sure that the full tree is being
visualized by selecting **File → Open Tree** and choosing
`examples/tutorial/tree.tree`.

Each vertex and member in the scalar tree has an associated weight. By default,
this weight is simply one. If we'd like to draw attention to a part of the
landscape, or represent a second scalar function using the volume of the
landscape components, we can specify a mapping from vertex or member id to a
positive scalar value.

In this example, we'll re-weight node 8 to have weight 40, and member 0 to have
weight 20. If you open the file `examples/tutorial/tree.weights`, this is what
you'll see:

~~~~
0   20
8   40
~~~~

These two lines specify the weights for the node and member. Note that not every
node or member in the tree appears in the file: a partial mapping is
sufficient. Any node or member not listed in the mapping is assumed to have unit
weight. For more information on the weight map file format, see the
[specification](formats.html#weights).

Now we will load the weight map:

1. Click **File → Load Weight Map**.
2. Select `examples/tutorial/tree.weights`.

This is what you'll see:

<center>
<a href="../resources/scrot_weight.png">
<img class="screenshot" src="../resources/scrot_weight.png">
</a>
</center>

Node 8 is a leaf node at the center of the landscape. If we select component 7 → 8,
we see that the child total weight is 40.

Member 0 is a member of the 4 → 5 arc. If we select this component, we see that
the component weight is 20, as expected.

You can make changes to the weight map while *denali* is running, but to
recompute the landscape to reflect the changes, you'll need to reload the weight
file by following the steps above. If you'd like to go back to visualizing the
unweighted landscape, click **File → Clear Weight Map**.


### Specifying a color map
By default, the color of the landscape is a function of the height (i.e, the
scalar value of the vertices). It is possible, however, to choose a custom color
function. This is useful, for example, to visualize a second scalar function on
top of the landscape.

As noted above, interactively simplifying the tree may create and destroy arcs.
Having the user specify a color map by associating each arc to a color is
therefore infeasible. Instead, *denali* has the user map each node and member of
the tree to a second scalar value. The user then selects from a set of rules
which assign to each arc of the tree a single scalar value. This value is then
used to give the arc a color. 

Consider, for example, the arc from 4 → 5 in the example tree above. This arc
has has a member. In general, an arc may have many members, or none at all.
The user will provide a color map associated to the two nodes and member a
second scalar value.

There are many ways to reduce these three values associated with the arc to a
single value, thereby mapping the arc to a color. For example, we may simply
take the mean of the three values. Alternatively, we might want only the parent
and child nodes to contribute to the mean. Even simpler, we might just set the
value of the child node to be the representative value of the arc.

In each case, we define a set of *contributors* and a *reduction*. The set of
contributors includes either the parent, child, or both, and optionally the
members in the arc. *Denali* also provides many reductions, such as max, min,
mean, and variance, which reduce the set of scalar values associated with the
contributors to a single scalar value.

Let's look at a concrete example. First, we must assign each node and member in
the tree a second scalar value. Let's just assign each node and member the same
value it has in our original tree. This has already been done for you in
`examples/tutorial/tree.colors`. If you open the file, this is what you'll see:

~~~~
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
~~~~

Each node and member in the tree has it's own line. The first number is the id
of the node or member, and the second is the value. For more information on the
format of `.colors` files, see the [specification](formats.html#colors).

To use this color map:

1. Select **File → Configure Color Map**.
2. Click **Browse** and select the `examples/tutorial/tree.colors` color map.
3. In the *Contributors* pane, select **Child** and *de*select **Include
   members**.
4. In the *Reduction* drop-down menu, select **Maximum**.

This is what you'll see:

<center>
<a href="../resources/scrot_color.png">
<img class="screenshot" src="../resources/scrot_color.png">
</a>
</center>

What we've done is set the color of each component to be the scalar value
assigned to the child node. The color map we've specified isn't very
interesting, though: each node and member is mapped to the same scalar value it
had in the original tree. 

Two of the reductions provided by *denali* are a bit different: the covariance
and correlation reductions. These reductions compute the covariance and
correlation between the color scalar function and the original scalar function,
useful for comparing the two mappings.


### Printing special information about a selection 
*Denali* includes a powerful and general callback system. It can be used to
invoke external commands whenever a component of the landscape is selected.
Communication with the invoked process is done via a simple flat file and
STDIN/STDOUT, meaning that callbacks may be written in virtually any programming
language without any dependencies on a message-passing library.

There are three flavors of callbacks:

- *Information* callbacks provide a string the *Denali*, which is then printed
  in the status box.

- *Async* callbacks functions run asynchronously, so that they do not block 
  *denali*'s interface while they are running.

- *Tree* callbacks provide as output a tree in `.tree` format. This tree
  replaces the one that was previously being visualized.

A full treatment of the callback system is outside of the scope of this tutorial
&mdash; for that, see the [callback system section](callback.html). For now,
we'll cover the creation of a simple callback in Python. Whenever a component is
selected, the callback will print some information about the selection to the
status box. We'll therefore be implementing an *info* callback.

When *denali* invokes a callback, it prints useful information about the
selected component to a temporary file. The location of this file is provided as
the first argument to the callback process. Detailed information about the
structure of this "selection file" is in the [callback section](callback.html).
For now, we don't need to understand exactly how this file is formatted, just
know that it contains the ids of the nodes in the selected components.

Included with *denali* is the *denali.py* Python module. Among other things, it
contains useful functions for interacting with *denali* data formats. One such
function is `denali.io.read_selection`. This function takes a file-like object
representing a selection file and parses it into a dictionary.

The dictionary contains a "component" key representing the selected component.
Its value is a 2x2 numpy array whose first column has the ids of the two nodes
at either end of the component, and whose second column has the corresponding
scalar values of the node.

This is all the information we'll need to write our simple callback. If you look
at the `examples/tutorial/callback.py` file, you'll see:

~~~~{.python}
import sys
import denali

selection = denali.io.read_selection_file(sys.argv[1])
u,v = [x[0] for x in selection['component']]
~~~~

The callback gets the path of the selection file from its first argument. It
then reads the selection and prints a simple message to the screen. The standard
output of the callback is captured, however, and redirected to the status box in
the *denali*. Therefore, printing information to *denali*'s status box is as simple as
printing to the screen, no matter what language you choose to use.

Now, let's try testing this callback. First you'll need the *denali* python
module in your path. There are several ways of doing this. You can pick your
favorite, or simply add the line:

~~~~{.python}
sys.path.append("/path/to/denali/python")
~~~~

under `import sys` in the above script.

Now we'll notify *denali* to use this script as a callback. Click on **File →
Configure Callbacks**. You should see the following:

<center>
<a href="../resources/scrot_callback.png">
<img class="screenshot" src="../resources/scrot_callback.png">
</a>
</center>

There are three sections, one for each type of callback. Since we'll be
specifying an info callback, we are interested in the first section. Click the
**Browse** button and find the `examples/tutorial/callback.py` script. This will
place the path to the callback script in the text box. Click inside the textbox
and add `python2` in front of this path, so that the whole line reads something
like:

~~~~
python2 path/to/denali/examples/tutorial/callback.py
~~~~

Note that under Unix you could just as well place `#!/usr/bin/env python2` or similar at
the top of `callback.py` and made the file executable. Then you could omit
`python2` from the text box. 

Below the text box are two options: **Run on selection** and **Supply subtree**.
When the **Run on selection** box is marked, the callback will automatically be
invoked when a selection is made. When the **Supply subtree** box is checked,
the callback will be provided with a list of all of the nodes and members in the
subtree induced by the selection. Check the **Supply subtree** box, and click
**Ok**.

Now, right click to select a component of the landscape, and in the status box
in the lower left corner, you'll see the text: "The selection component was: ",
followed by the ids of the nodes in your selection.

After a selection is made, you can manually invoke the callback again by
clicking the **Info** button in the *Callbacks* pane at the lower right of the
interface.

Note that we could have done *anything* in our Python callback, such as plotted
relevant data in a new window, download information from the internet, or even
spawn a new *denali* process. And while the included utility functions make
parsing the selection information very easy, any other language is also capable
of reading and interaction with *denali* in this way.


## Visualizing Functions on Point Clouds

### Introduction to contour trees
As we have seen, *denali*'s input is a scalar tree: an undirected graph whose
nodes each have an associated real number. We can use the concept of a *contour
tree* to visualize scalar functions defined on other structures, including point
clouds.

Consider a scalar function $f$ defined on a subset of $\mathbb{R}^d$. We can
define the *level set* $f^{-1}(a) = \{ x | f(x) = a \}$. For any given $a$, the
level set of $f$ may have several connected components. Intuitively, as we
increase or decrease $a$, these level sets may appear, disappear, merge, or
split. This creates a tree, called the *contour tree*.

Informally speaking, the contour tree captures the "skeleton" of the scalar
function. Nodes in the contour tree correspond to critical points of the
function. The scalar value of a node in the contour tree is equal to the
function's value at the corresponding point in the input space.

It is often the case that we have many samples of a scalar function defined on a
potentially high-dimensional space, and we'd like to visualize it somehow. For
example, we may want to visualize a probability distribution of many variables.
In this case, our dataset is a point cloud. To visualize this data, we'll adopt
the following strategy:

1. Build a neighbor graph by connecting each point in the dataset to its $k$
   nearest neighbors. *denali.py* provides a function to do this.

2. Compute the contour tree using *ctree*.

3. Visualize with *denali*.


### Building a neighbor graph

First, we need some data to visualize. Included in `examples/tutorial/data.txt`
are 10000 samples of a probability density in 4 dimensions. If you'd like
to see how the data is generated, you can look at
`examples/tutorial/gendata.py`.

The format of the file is as follows: the first four columns specify the
location of the sample. The fifth column denotes the (unnormalized) density of
the function at that point.

The script contained in `examples/tutorial/makenngraph.py` reads this file and
computes the 10-nearest-neighbor graph. It then outputs this graph in a format
that `ctree` can use. If you were to open it, you'd find:

~~~~{.python}
import numpy as np
import denali

data = np.loadtxt("data.txt")

# separate the samples from the density
samples = data[:,:3]
density = data[:,4]

# find the edges in a 10-nearest-neighbor graph
edges = denali.contour.kneighbors_complex(samples, 10)

# write the vertices and edges to a file
denali.io.write_vertices(open("vertices.txt", 'w'), density)
denali.io.write_edges(open("edges.txt", 'w'), edges)
~~~~

Three utility functions included in *denali.py* are used here:

- `kneighbors_complex` takes an array of points and the number of neighbors
  and builds a *k*nn graph, returning the edges as a list.

- `write_vertices` takes an open file-like object and an array of vertex values
  and writes the values to the file.

- `write_edges` takes an open file-like object and a list of edge pairs, such as
  those returned by `kneighbors_complex`, and writes them to the file.

For more information on these functions, see the [python
documentation](../pydoc/_build/html/index.html).

Running the above script will output two files: `vertices.txt` and `edges.txt`.
We've included them in `examples/tutorial`, so you don't have to run the script
above to proceed to the next step.

### Computing the contour tree

*ctree* is a command-line tool for computing contour trees. It should have been
installed alongside *denali*.

*ctree* has three required arguments:

~~~~
ctree <vertex value file> <edge file> <tree file> 
~~~~

The *vertex file* and *edge file* are the files we just created in the last
step. The *tree file* will be overwritten by *ctree*: it's where the output will
be placed.

In a terminal, navigate to the `examples/tutorial` directory and type

~~~~
ctree vertices.txt edges.txt tutorial.tree
~~~~

Now, start *denali* and load the `tutorial.tree` file. You should see a large
peak with several other, smaller peaks surrounding it. If so, you've
successfully visualized a scalar function defined on a point cloud.

For more information on how to use *ctree*, see the [ctree
documentation](ctree.html).


## Other Sources of Hierarchical Structure

There are many ways of extracting hierarchical structure from a data set.
Computing the contour tree of a scalar function defined on the data is one
approach -- this is the method described above. In other situations, however, a
different technique may be more applicable.

A general approach to extracting hierarchical structure from data is via
[hierarchical clustering](http://en.wikipedia.org/wiki/Hierarchical_clustering).
Most hierarchical clustering algorithms accept a dissimilarity matrix as input
and produce a scalar tree as output. This tree is commonly represented as a
[dendrogram](http://en.wikipedia.org/wiki/Dendrogram). *Denali* offers an
alternative way of visualizing the hierarchical clustering which is more
suitable for large datasets. An example of this use case can be seen
[here](http://denali.cse.ohio-state.edu/doc/pages/examples.html#clustering).

In other cases, the data may lie near a tree-like manifold embedded in a high
dimensional space. A natural way of visualizing the structure of
the manifold is to visualize the level sets of the geodesic distance from an
arbitrary data point. This is the approach taken in the mouse cytometry example
included in the *denali* distribution.
