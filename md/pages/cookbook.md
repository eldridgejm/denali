# Cookbook

*Denali* can be used to visualize many different sources of data, but it
requires that its input be in the form of a [scalar
tree](tutorial.html#scalar-tree). *Denali* includes tools to extract tree-like
structure from your data so that it can be visualized as a landscape metaphor.

This document describes several ways to combine these tools to perform several
common analyses.

- [Visualize a scalar function on a tree](#visualize-a-scalar-function-on-a-tree)
- [Visualize a scalar function on a graph](#visualize-a-scalar-function-on-a-graph)
- [Visualize a scalar function on a point cloud](#visualize-a-scalar-function-on-a-point-cloud)
- [Visualize and interactively sample a probability distribution](#visualize-and-interactively-sample-a-probability-distribution)


## Scalar function on a tree
If your function is defined on a tree, then you may not need to do anything
extra to visualize it with denali. Simply provide your input as a `.tree` file
(see the [specification](formats.html#tree) or the [tutorial](tutorial.html) for
more information).

## Scalar function on a graph
If your function is defined on a graph, you'll need to extract tree-like
structure from the graph. One way to do this is to compute the contour tree of
the graph. The included *ctree* tool will do this: simply provide the vertex
values and edges of your graph to *ctree*, and it will output the contour tree
in a file *denali* can read.

## Scalar function on a point cloud
If your function is defined on a point cloud, you'll need to extract tree-like
structure. One way to do this is with a contour tree. The included *ctree* tool
and *pydenali*'s contour tree utilities can help do this. For more information,
see the [Visualizing function on point clouds](tutorial.html#visualizing-functions-on-point-clouds)
section of the tutorial.

## Visualize and interactively sample a probability distribution
Suppose you have many samples from a probability distribution in the form of
points with associated probabilities. You can visualize the distribution as any
other scalar function defined on a point cloud: by first building a
nearest-neighbor graph, then building the contour tree. See the [Visualize a
scalar function on a point cloud](#visualize-a-scalar-function-on-a-point-cloud)
section above for more.

*Denali*'s callback system can be used to interactively resample the probability
distribution. Simply write a [tree callback](callback.html) to do the following:

1. Resample points around the selection.
2. Recompute the neighbor graph.
3. Pass the recomputed neighbors and new vertices into *ctree*.
4. Print the computed tree to STDOUT.

Included in `examples/pdf` is a callback which does just this for a simple
mixture of Gaussians.


<div style="text-align: center; margin-top: 20px">
<a href="./examples.html" style="font-size: 20px">Previous: Examples</a>
<span style="font-size: 20px; opacity: .5"> | </span>
<a href="./related.html" style="font-size: 20px">Next: Related Work</a>
</div>
