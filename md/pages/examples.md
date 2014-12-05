## Examples

*Denali* comes complete with a variety of examples. All of these examples are
located in the `/examples` folder, directly under the project root. Most of
these examples are designed to show how the various features of *denali* can be
used for various purposes. In particular, the callback system is demonstrated.
The examples use the included `pydenali` python module, and may require that
several python packages be installed. Note that `pydenali` is included with your
download, but not automatically installed. To install `pydenali`, see the
[pydenali](../pydoc/_build/html/index.html) documentation.

To run these examples, you first need to know where they are. This depends on
your operating system:

- **Windows**: `C:\Program Files (x86)\denali\examples`
- **Linux**: `/usr/share/denali/examples` or `/usr/local/share/denali/examples`
- **OS X**: The examples aren't included with the default installation. You can
  download them separately from the [project
  page](http://denali.cse.ohio-state.edu).

----

- [`./tutorial`](#tutorial)
- [`./clustering`](#clustering)
- [`./comments`](#comments)
- [`./neural`](#neural)
- [`./pdf`](#pdf)

----


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
are trained on a noisy sampling of the sine function, and the parameter space is
sampled around some of the local minima found. This example demonstrates the
callback system.  Whenever the user selects a component of the landscape, the
corresponding model is evaluated and plotted in comparison to the ideal sine
function and the training data. The callback requires the `neurolab` python
package.

**Files**:

- `all_params.npy`: A numpy archive containing samples of the neural network's
  parameter space.
- `vertices`: A file containing each of the parameter samples in the above file.
  A vertex's scalar value is the loss of the model when evaluated with the
  corresponding parameters on the training data.
- `edges`: A set of nearest neighbor edges connecting the vertices into a graph.
- `tree.tree`: A contour tree generated from `vertices` and `edges` by using
  `ctree`.
- `targets.npy`: A numpy archive containing the training examples.
- `topology.pickle`: The topology of the neural network, serialize into a python
  pickle file.
- `nnsample.py`: Utility functions used by the callback to read and write
  serialized neural networks.
- `callback.py`: The python callback script. Requires `neurolab` to function.


**Usage**: First, load the `tree.tree` file into *denali*. By default, *denali*
chooses the node with the minimum scalar value to be the root. Since this tree
visualizes the cost function, it is more natural to set the root to be the node
with the maximum scalar value. Click **Choose Root**, then select the **Maximum
node** radio button, then hit **Ok**.

Next, we'll use the callback to gain an understanding of the cost landscape.
Select **File → Configure Callbacks**. We'll run this callback asynchronously.
Click **Browse** in the *Async Callback* section, and select the `callback.py`
file. Next, select **Run on selection**, then hit **Ok**.

Right clicking on a component will extract the parameter associated with the
child node of the arc, evaluate the network using that parameter vector, and
plot the model on the interval $[0, 2 \pi]$. Also shown in the plot are the
ideal sine function (dashed line), and the noisy training samples (blue dots).
By exploring, we see that the global minimum tends to overfit the data, while
two plateaus corresponding to local minima then to underfit the data.


### `./pdf`

**Description**: A mixture of three Gaussians in four dimensions is sampled.
Each sample is associated with the density at that point in space. The contour
tree of the density is computed, and the result is visualized as a landscape. A
callback is provided which resamples the pdf locally whenever the user selects a
region. Upon resampling, the contour tree is recomputed and the landscape is
seamlessly updated.

**Files**: 

- `initialize.py`: Generate samples from the mixture and save them to
  `data.npz`, as well as create a neighbor graph, saving the vertices and edges
  in `vertices` and `edges`, and generating a contour tree in `tree.tree` by
  calling `ctree`.
- `vertices`: The vertices generated by `initialize.py`
- `edges`: The edges generated by `initialize.py`
- `data.npz`: A numpy archive containing all of the data generated by
  `initialize.py`
- `tree.tree`: A contour tree generated by `ctree` with `vertices` and `edges`
  as input.
- `info_callback.py`: Prints the coordinates of the two nodes of the arc
  selected by the user.
- `sampling_callback.py`: Callback which resamples the pdf in the region around
  the user's selection.

**Usage**: As an optional first step, we'll generate some random data. The
`vertices`, `edges`, and `tree.tree` files should already be included in the
folder, but if you'd like to generate new random samples, simply run
`./initialize.py`. This will overwrite the previously mentioned files.

Next, load `tree.tree` in *denali*. You can load the `info_callback.py` script
as a *info* callback. The `sampling_callback.py` script can be used as a *tree*
callback. Neither of these callbacks require that the subtree be provided.

With both callbacks active, selecting a component of the landscape will print
some information about the selection to the status box, and automatically
resample the pdf and redraw the landscape.


<div style="text-align: center; margin-top: 20px">
<a href="./tutorial.html" style="font-size: 20px">Previous: Tutorial</a>
<span style="font-size: 20px; opacity: .5"> | </span>
<a href="./cookbook.html" style="font-size: 20px">Next: Cookbook</a>
</div>
