## Introduction
*Denali* is a graphical interface for visualizing scalar functions as 
mountainous terrains. Included alongside *denali* are several tools which aid in
the creation, manipulation, and interpretation of such landscapes: *ctree* is a
command-line tool for computing contour trees, and *denali.py* is a python
module with many diverse utility functions.

## What can be visualized with *denali*?
Strictly-speaking, *denali* requires as input a tree whose nodes are mapped to
scalar values. However, by using the included tools, scalar functions defined on
point clouds and graphs are also able to be visualized.

Some examples of things we have visualized with *denali*:

- The parameter space of a neural network model
- A hierarchical clustering
- High dimensional probability distributions
- Comment threads on internet forums
- Decision trees
- Protein interaction networks

## What is a landscape metaphor?
A landscape metaphor is a representation of a scalar tree as a 2-d surface
embedded in 3-d. The height of points on the landscape corresponds to values of
the function on the tree. The volume of components in the landscape corresponds
to the number of vertices in the associated part of the tree.

