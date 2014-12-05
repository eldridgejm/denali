# Introduction

*Denali* is a tool for visualizing complex and possibly high-dimensional data
intuitively and efficiently. The central concept used by *denali* is that of the
*landscape metaphor*: a 2-d "mountainous terrain" embedded in 3-d which
preserves important features of the data and presents them to the user in an
intuitive fashion. This software package includes tools for computing,
visualizing, and manipulating landscape metaphors.


## Can I use *denali* to visualize my data?

Broadly-speaking, there are two pre-requisites for visualizing your data with
*denali*:

1. Each data point must have an associated scalar value, such as a probability,
   score, or cost.
2. You must have some method of extracting tree-like structure from your data.

Included with *denali* are tools for satisfying the second prerequisite for many
types of data that may not appear, at first glance, to exhibit tree-like
structure.

For example, suppose we have drawn many samples from a probability distribution.
Can we attempt to understand the structure of the distribution by visualizing the
samples with *denali*?  For each sample we have an associated probability, so
the first prerequisite is clearly met. However our data is in the form of a
point cloud, not a tree, and so it appears that the second requirement is not
satisfied. In fact, we *can* extract tree-like structure from this point cloud
in a very natural way using the concept of the *contour tree*. The contour tree
captures the important topological characteristics of a scalar function, and, as
its name implies, it exhibits tree-like structure. Hence our second requirement
is met, and we *can* visualize the probability distribution using *denali*.

*Denali* includes tools for quickly and easily computing contour trees, making
it simply and straightforward to visualize many different types of data as
landscape metaphors.

## What is included with *denali*?

Several tools are provided alongside *denali*. [*ctree*](./ctree.html) is tool
for computing contour trees from point clouds.
[*pydenali*](../pydoc/_build/html/index.html) is a package of Python utilities
for interacting with *denali* and *ctree*.


## Using this documentation

The suggested method of using this documentation is as follows: 

- First, if you have yet to install *denali*, see the [installation
  instructions](install.html).

- After installation, jump right into the [tutorial](tutorial.html). The
  tutorial is an in-depth practical discussion of how to use *denali*, including
  how to interpret the visualization, provide input, and use some of *denali*'s
  advanced features.

- For tips on how to use *denali* to visualize several different types of data,
  see the [cookbook](cookbook.html).

<div style="text-align: center; margin-top: 20px">
<a href="./install.html" style="font-size: 20px">Next: Installation</a>
</div>
