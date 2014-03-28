from sklearn.neighbors import kneighbors_graph as _kneighbors_graph
import networkx as _networkx

def kneighbors_complex(data, k):
    """
    Builds a complex from the data by connecting each point to the k nearest
    neighbors. Returns a list of edges.
    """
    nn = _kneighbors_graph(data, k)
    
    # get an array of u indices and v indices
    u,v = np.nonzero(nn)
    
    # and zip them together
    edges = zip(u.tolist(), v.tolist())
    
    # make a graph complex and return it
    g = _networkx.Graph()
    g.add_edges_from(edges)
    g.remove_edges_from(g.selfloop_edges())

    return [e for e in g.edges_iter()]
