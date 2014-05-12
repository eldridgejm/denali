import neurolab as nl
import numpy as np

def layer_starting_indices(net, ci, cn):
    # the total number of parameters in each layer of the model
    n_params_per_layer = cn + (cn * ci)
    
    # the position of the beginning of the i-th layer in the param
    # vector. The last entry is the total number of params
    layer_starts = np.zeros(1 + len(net.layers), int)
    layer_starts[1:] = np.cumsum(n_params_per_layer)
    
    return layer_starts


def pack_network(net):
    """Packs the parameters of the feedforward net into an array.
    
    Returns:
        params - a numpy array of parameters
        topology - a description of the network's topology needed to construct
                   a network of the same size
    """
    cn = np.array([layer.cn for layer in net.layers])
    ci = np.array([layer.ci for layer in net.layers])
    
    # compute the starting indices of each layer
    layer_starts = layer_starting_indices(net, ci, cn)
    
    params = np.empty(layer_starts[-1])
    
    # flatten each layer's parameters
    for i,layer in enumerate(net.layers):
        flat_layer = np.concatenate((layer.np['b'],
                                     layer.np['w'].flatten()))
        begin = layer_starts[i]
        end = layer_starts[i+1]
        
        params[begin:end] = flat_layer
        
    # make a dictionary of the network's size and input information
    # this is necessary in order to construct a new network
    topology = dict(inp_minmax=net.inp_minmax,
                    cn=cn)
        
    return params, topology


def unpack_network(parameters, topology):
    """Builds a network from a packed parameter array."""
    # make a network with the same topology
    net = nl.net.newff(topology['inp_minmax'], topology['cn'])
    
    cn = np.array([layer.cn for layer in net.layers])
    ci = np.array([layer.ci for layer in net.layers])
    
    # the total number of parameters in each layer of the model
    layer_starts = layer_starting_indices(net, ci, cn)
    
    for i,layer in enumerate(net.layers):
        begin = layer_starts[i]
        end = layer_starts[i+1]
        
        layer_params = parameters[begin:end]
        
        layer.np['b'] = layer_params[:layer.cn]
        layer.np['w'] = layer_params[layer.cn:].reshape((-1,layer.ci))
        
    return net
