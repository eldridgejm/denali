I/O Utilities
=============

`pydenali` includes several functions useful for reading and writing output 
in *denali* formats.

.. contents::

Reading selection files
-----------------------

.. autofunction:: denali.io.read_selection


Reading and writing tree files
------------------------------

.. autofunction:: denali.io.read_tree
.. autofunction:: denali.io.write_tree


Writing weight and color files
------------------------------

.. autofunction:: denali.io.write_weights
.. autofunction:: denali.io.write_colors

Writing vertex and edge files
-----------------------------

Vertex and edge files are used to specify graphs as input to the ``ctree`` program.

.. autofunction:: denali.io.write_vertices
.. autofunction:: denali.io.write_edges
