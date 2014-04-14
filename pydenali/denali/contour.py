try:
    from sklearn.neighbors import kneighbors_graph as _kneighbors_graph
except ImportError:
    _has_sklearn = False
else:
    _has_sklearn = True

try:
    import networkx as _networkx
except ImportError:
    _has_networkx = False
else:
    _has_networkx = True

try:
    import numpy as _numpy
except ImportError:
    _has_numpy = False
else:
    _has_numpy = True

def kneighbors_complex(data, k):
    """
    Builds a complex from the data by connecting each point to the k nearest
    neighbors. Returns a list of edges.

    **Note**: To use this function, the `numpy`, `scikit-learn`, and `networkx`
    packages must be installed.

    :param data: An array of *d*-dimensional data.
    :type data: Numpy array

    :param k: The number of neighbors to use.
    :type k: int

    This function is useful for generating the input to the ``ctree`` program
    which generates contour trees. Given a data array, making the input
    for ``ctree`` is as simple as (assuming ``values`` is a list containing
    values for each vertex):

    >>> edges = denali.contour.kneighbors_complex(data, 10)
    >>> denali.io.write_vertices(open("values_file", 'w'), values)
    >>> denali.io.write_edges(open("edges_file", 'w'), edges)

    Here, 10 neighbors have been found for each vertex. Experiment to find
    what is best for your application -- the only requirement is that the
    resulting graph is connected.
    """
    if not _has_sklearn:
        raise ImportError("sklearn is required to build a complex.")

    if not _has_numpy:
        raise ImportError("numpy is required to build a complex.")

    if not _has_networkx:
        raise ImportError("networkx is required to build a complex.")

    nn = _kneighbors_graph(data, k)
    
    # get an array of u indices and v indices
    u,v = _numpy.nonzero(nn)
    
    # and zip them together
    edges = zip(u.tolist(), v.tolist())
    
    # make a graph complex and return it
    g = _networkx.Graph()
    g.add_edges_from(edges)
    g.remove_edges_from(g.selfloop_edges())

    return [e for e in g.edges_iter()]
