## Examples

*Denali* comes complete with a variety of examples. All of these examples are
located in the `/examples` folder, directly under the project root. Most of
these examples are designed to show how the various features of *denali* can be
used for various purposes. In particular, the callback system is demonstrated.
The examples use the included python module, and may require that several python
packages be installed.


### `./tutorial`

**Description**: This is the example referenced throughout the
[tutorial](./tutorial.html). It demonstrates color maps, weight maps, and a
simple callback. The data being visualized is a toy tree; there isn't much to
interpret.


**Files**:

- `tree.tree`: A contour tree used throughout the tutorial.
- `tree.colors`: A color map for the above tree.
- `tree.weights`: A weight map for the above tree.
- `callback.py`: A simple info callback which prints the nodes in the selected
  component to the status box.

The following files are referred to in the section on computing a contour tree
from a point cloud:

- `data.txt`: A data file containing 10000 samples of a probability density in
  four dimensions.
- `makenngraph.py`: Builds a nearest neighbor graph for the data in the
  `data.txt` file and prints the vertices and edges to files. Requires the
  `numpy` package.
- `vertices.txt`: A vertex definition file describing points in a density function
  consisting of three Gaussians.
- `edges.txt`: The edges made between vertices by using the `makenngraph.py`
  script.
- `gendata.py`: Generates more data, like that in `data.txt`.


**Usage**: See the [tutorial](./tutorial)


### `./clustering`

**Description**: 647 images of handwritten digits (consisting of zeros, ones,
and sevens) are hierarchically clustered using single-linkage clustering. The
resulting tree is visualized. This demo shows the rich callback system by
displaying an average image of the digits in the selected component in a new
window.

**Files**:

- `mnist_017.npy`: A *numpy* archive containing the digit images.
- `mnist_017_labels.npy`: A *numpy* archive containing the labels of the images
  in `mnist_017.npy`. The label for an image is either 0, 1, or 7.
- `mnist_017.tree`: The tree resulting from running single-linkage clustering on
  the data.
- `callback.py`: The callback which shows the digits in the component when the
  selection is made. Requires *numpy* and *matplotlib*.

**Usage**: Load the `mnist_017.tree` file in *denali*. The three classes of
digits are represented by "plateaus" in the landscape. To explore the landscape
more thoroughly, we'll use the callback. Select **File → Configure Callbacks**
to open the callbacks dialog. We'll use an Async callback. Browse for the
callback file, and select **Run on selection** and **Supply subtree**. Click
**Ok**, and right click to select a component of the landscape. An image will
appear, showing the average of all of the digits in the subtree induced by your
selection.


### `./comments`

**Description**: An comment thread from a website which features hierarchical
discussions was scraped, and 954 comments were downloaded. Each comment has an
unbounded integer score rating its popularity. This naturally yields a scalar
tree, but for the ease of visualization, we make a slight adjustment to the
scalar value of each node. The scalar value associated with a comment is taken
to be its score, plus the score of its parent comment. This causes the landscape
to grow upward.

Highlighted by this example are the color map and callback features. The
callback prints the selected comment to the status box.

**Files**:

- `comments.pickle`: A `networkx` tree representing the discussion serialized
  using the Python 2 `pickle` module.
- `comments.tree`: The tree in *denali* format.
- `vote_ratio.colors`: A color map describing the ratio of upvotes to downvotes
  for every comment.
- `callback.py`: The python callback which prints the comments to the status
  box.


**Usage**: Open the tree in *denali* and set the root node to be "0". This node
is artificially inserted into the tree to be a parent of all of the top-level
comments. Next, set the color map by selecting **File → Configure Color Map**.
Browse for the color map file. Set the "Contributors" to **Child** and the
reduction to **Maximum**. The particular reduction used doesn't matter in this
case, as we're only using the child node in the reduction set.

To see the content of individual comments, we'll use the callback function. As
before, load the callback, but as an *Info* callback rather than an *Async*
callback. Right click to select a component. The comment's score and content
will be printed to the status box.


### `./neural`

**Description**: A neural network model's parameter space. Several neural nets
are trained on a sine function, and the parameter space is sampled around some
of the local minima found.
