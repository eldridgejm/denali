Installation
============

`pydenali` is a cross-platform Python package, and doesn't need to be installed
as-such. However, it may be convenient to install `pydenali` to a location
where Python knows about it, so that using `pydenali` as simple as writing
``import denali``.  In this regard, `pydenali` can be installed just as any
other python module, though an install script is not provided. It is simple to
install `pydenali`, neverthelless.

Requirements
------------

`pydenali` does not depend on other packages for much of its functionality, but 
several features have additional requirements. If you'd like to use `pydenali`
to read and write tree files, you'll need the `networkx` package. If you'd like
to create knn graphs for input to `ctree`, you'll need `networkx`, `numpy`, and
`scikit-learn`.

Using `pip`
-----------

`pydenali` is compatible with pip. To install to a system-wide location (or to
a virtualenv), simply run `pip` on the `pydenali` directory:

    pip install ./pydenali

You may need root priviledges to run the above command.

No installation
---------------

You can use `pydenali` without installing it by changing your python path at
the beginning of your script. For example:

::

    import sys
    sys.path.append("/path/to/denali/python")
    import denali

will import `pydenali` from the location on disk. On a Unix-like system, you
can also append `/path/to/denali/python` to your ``PYTHONPATH`` environment
variable.
