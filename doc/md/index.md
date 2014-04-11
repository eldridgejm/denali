Welcome to the documentation for denali, ctree, and related tools.

----

- [Introduction](pages/intro.html)
- [Installation](pages/install.html)
- [Tutorial](pages/tutorial.html)
- [Interpreting Landscapes](pages/landscape.html)
- [Cookbook](pages/cookbook.html)

----

## denali
*Denali* is the graphical interface for visualizing landscapes. For a general
information on how to use *denali*, see the [tutorial](pages/tutorial).
For detailed information about supplying input to *denali*, see:

- [File format specifications](pages/formats.html)
- [Callback system](pages/callback.html)


----

## ctree
*Ctree* is a command line interface for creating contour trees from graph input.

- [ctree documentation](pages/ctree.html)

----

## denali.py

*denali.py* is a Python module containing useful utility functions for
interacting with *denali* and *ctree*. These functions help read files in
*denali* formats, create callbacks, and generate contour trees. Read the full
documentation here:

- [denali.py documentation](pydoc/_build/html/index.html)

----

## developer documentation

The developer documentation is useful for those who are interested in modifying
or extending *denali*.
The documentation is managed by Doxygen. To create it, run `doxygen`
in the `doc/dev` directory. Once generated, it can be found
[here](dev/html/index.html).
