Installation
============

`denali.py` is a cross-platform Python module, and doesn't need to be installed
as-such. However, it may be convenient to install `denali.py` to a location
where Python knows about it, making using `denali.py` as simple as writing
``import denali``.  In this regard, `denali.py` can be installed just as any
other python module, though an install script is not provided. It is simple to
install `denali.py`, neverthelless.

**No installation**:

You can use `denali.py` without installing it by changing your python path at
the beginning of your script. For example:

::

    import sys
    sys.path.append("/path/to/denali/python")
    import denali

will import `denali.py` from the location on disk. On Unix, you can also append
`/path/to/denali/python` to your ``PYTHONPATH`` environment variable.

**Add a .pth file**:

The preferred way to "install" `denali.py` to a system-wide location, or to a
virtualenv, for that matter, is to place a ``.pth`` file in your python's
``site-packages`` directory. A ``.pth`` file simply consists of the full path
to the module as a single line. Placing this file in your distributions
``site-packages`` folder will add the moudle to Python's path.
