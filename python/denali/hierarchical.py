import networkx as _networkx
import numpy as _numpy 

def build_cluster_tree(slc, height_function):
    """Given a clustering and a function that maps the clusters to heights, 
    produces a cluster tree."""
    tree = _networkx.DiGraph()
    
    n_points = slc.shape[0] + 1
    
    # we create a node for each of the data points
    tree.add_nodes_from(range(n_points))
    
    # now we loop through the clustering and add nodes for each cluster
    for i,cluster in enumerate(slc):
        node_id = i + n_points
        tree.add_node(node_id)
        
        u,v = cluster[:2]
        tree.add_edge(node_id, int(u))
        tree.add_edge(node_id, int(v))
        
        height = height_function(cluster)
        
        tree.node[u]['value'] = tree.node[v]['value'] = height
        
    tree.node[len(tree)-1]['value'] = 0.
    
    return tree


def exp_height_factory(alpha, beta, gamma=0.1):
    a = -_numpy.log(gamma)/(alpha - beta)
    
    def exp_height(cluster):
        x = cluster[2]
        return _numpy.exp(-a*(x - beta))

    return exp_height
