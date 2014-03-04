#ifndef DENALI_FOLDED_H
#define DENALI_FOLDED_H

#include <denali/contour_tree.h>
#include <denali/graph_maps.h>
#include <denali/graph_mixins.h>
#include <denali/graph_structures.h>
#include <denali/mappable_list.h>

#include <stdexcept>

namespace denali {

/// \brief A folded tree.
/// \ingroup fold_tree
class FoldTree :
        public
        EdgeObservableMixin <UndirectedGraph,
        NodeObservableMixin <UndirectedGraph,
        ReadableUndirectedGraphMixin <UndirectedGraph,
        BaseGraphMixin <UndirectedGraph> > > >
{
public:

    class NodeFold;
    class EdgeFold;

private:

    typedef
    EdgeObservableMixin <UndirectedGraph,
    NodeObservableMixin <UndirectedGraph,
    ReadableUndirectedGraphMixin <UndirectedGraph,
    BaseGraphMixin <UndirectedGraph> > > >
    Mixin;

    typedef boost::shared_ptr<NodeFold> NodeFoldPtr;
    typedef boost::shared_ptr<EdgeFold> EdgeFoldPtr;

    typedef UndirectedGraph GraphType;

    GraphType _graph;

    ObservingNodeMap<GraphType, NodeFoldPtr> _node_to_fold;
    ObservingEdgeMap<GraphType, EdgeFoldPtr> _edge_to_fold;

    unsigned int _number_of_node_folds;
    unsigned int _edge_fold_identifier;

    Node restoreNode(NodeFoldPtr v_fold)
    {
        Node v = _graph.addNode();
        v_fold->_node = v;
        _node_to_fold[v] = v_fold;
        return v;
    }

    Edge restoreEdge(EdgeFoldPtr uv_fold)
    {
        Node u = uv_fold->_u_fold->_node;
        Node v = uv_fold->_v_fold->_node;

        Edge uv = _graph.addEdge(u,v);
        uv_fold->_edge = uv;
        _edge_to_fold[uv] = uv_fold;
        return uv;
    }

    NodeFoldPtr oppositeNodeFold(NodeFoldPtr u_fold, EdgeFoldPtr uv_fold)
    {
        return uv_fold->_u_fold->_id == u_fold->_id ? 
                uv_fold->_v_fold : uv_fold->_u_fold;
    }

public:
    typedef GraphType::Node Node;
    typedef GraphType::Edge Edge;

    class NodeFold
    {
        friend class FoldTree;

        size_t _id;
        std::vector<EdgeFoldPtr> _collapsed;
        EdgeFoldPtr _uv_fold;
        EdgeFoldPtr _vw_fold;
        Node _node;

        NodeFold(Node node, unsigned int id) : _node(node), _id(id) {}

    public:
        
        size_t numberOfCollapsedEdgeFolds() const {
            return _collapsed.size();
        }

        const EdgeFold& getCollapsedEdgeFold(size_t i) const {
            return *_collapsed[i];
        }

        const EdgeFold& uvFold() const {
            return *_uv_fold;
        }

        const EdgeFold& vwFold() const {
            return *_vw_fold;
        }

        Node getNode() const {
            return _node;
        }

        bool isReduced() const {
            return _uv_fold;
        }

        size_t getIdentifier() const {
            return _id;
        }

    };

    class EdgeFold
    {
        friend class FoldTree;

        NodeFoldPtr _u_fold;
        NodeFoldPtr _v_fold;
        NodeFoldPtr _reduced_fold;
        Edge _edge;
        size_t _id;

        EdgeFold(size_t id, Edge edge, NodeFoldPtr u_fold, NodeFoldPtr v_fold)
            : _id(id), _edge(edge), _u_fold(u_fold), _v_fold(v_fold) {}

    public:

        const NodeFold& uFold() const {
            return *_u_fold;
        }

        const NodeFold& vFold() const {
            return *_v_fold;
        }

        const NodeFold& reducedFold() const {
            return *_reduced_fold;
        }

        Edge getEdge() const {
            return _edge;
        }

        bool hasReduced() const {
            return _reduced_fold;
        }

        size_t getIdentifier() const {
            return _id;
        }
    };


    FoldTree() 
        : Mixin(_graph), _node_to_fold(_graph), _edge_to_fold(_graph),
          _number_of_node_folds(0), _edge_fold_identifier(0) {}

    /// \brief Add a node to the tree.
    Node addNode()
    {
        // create the node
        Node node = _graph.addNode();

        // create the node fold
        NodeFoldPtr node_fold = NodeFoldPtr(new NodeFold(node, _number_of_node_folds));
        _number_of_node_folds++;

        // link the node to the node fold
        _node_to_fold[node] = node_fold;

        return node;
    }

    /// \brief Add an edge to the tree.
    Edge addEdge(Node u, Node v)
    {
        // create the edge
        Edge edge = _graph.addEdge(u,v);

        // get the node folds of u and v
        NodeFoldPtr u_fold = _node_to_fold[u];
        NodeFoldPtr v_fold = _node_to_fold[v];

        // make an edge fold
        EdgeFoldPtr edge_fold = EdgeFoldPtr(
                new EdgeFold(_edge_fold_identifier, edge, u_fold, v_fold));
        _edge_fold_identifier++;

        // link it to the edge
        _edge_to_fold[edge] = edge_fold;

        return edge;
    }

    /// \brief Retrieve a node's fold.
    const NodeFold& getNodeFold(Node node) const {
        if (!isNodeValid(node)) {
            throw std::runtime_error("Invalid node has no fold.");
        }
        return *_node_to_fold[node];
    }

    /// \brief Retrieve an edge's fold.
    const EdgeFold& getEdgeFold(Edge edge) const {
        if (!isEdgeValid(edge)) {
            throw std::runtime_error("Invalid edge has no fold.");
        }

        return *_edge_to_fold[edge];
    }

    /// \brief Get the number of nodes that would exist in the fully unfolded tree.
    size_t numberOfNodeFolds() const
    {
        return _number_of_node_folds;
    }

    /// \brief Collapse the child node into the parent.
    void collapse(Edge edge)
    {
        Node parent = _graph.degree(_graph.u(edge)) == 1 ? 
                _graph.v(edge) : _graph.u(edge);

        Node child = _graph.opposite(parent, edge);

        assert(_graph.degree(child) == 1);

        // add the fold edge to the parent node fold's collapse list
        _node_to_fold[parent]->_collapsed.push_back(_edge_to_fold[edge]);

        // remove the child from the tree
        _graph.removeNode(child);
    }

    /// \brief Reduce the node, connecting its neighbors.
    Edge reduce(Node v)
    {
        // u <---> **v** <---> w
        assert(_graph.degree(v) == 2);

        // get the two neighbors
        UndirectedNeighborIterator<GraphType> it(_graph, v);
        Node u = it.neighbor(); Edge uv = it.edge(); ++it;
        Node w = it.neighbor(); Edge vw = it.edge();

        // make an edge between them
        Edge uw = this->addEdge(u,w);

        // update v's reduced edge folds
        NodeFoldPtr v_fold = _node_to_fold[v];
        v_fold->_uv_fold = _edge_to_fold[uv];
        v_fold->_vw_fold = _edge_to_fold[vw];

        // update the new edge's reduced node
        _edge_to_fold[uw]->_reduced_fold = v_fold;

        // remove the node
        _graph.removeNode(v);

        return uw;
    }

    /// \brief Uncollapses the collapsed edge at the index in the node's collapse list.
    /*!
     *  An index less than zero indicates that the last collapsed edge should be
     *  uncollapsed.
     */
    Edge uncollapse(Node u, int index=-1)
    {
        // get the folded node
        NodeFoldPtr u_fold = _node_to_fold[u];

        // if the index is less than one, uncollapse the last collapsed edge
        if (index < 0) {
            index = u_fold->_collapsed.size()-1;
        }

        // pop the collapsed edge fold
        EdgeFoldPtr uv_fold = u_fold->_collapsed[index];
        u_fold->_collapsed.erase(u_fold->_collapsed.begin() + index);

        // get the collapsed node fold at the other end of the edge
        NodeFoldPtr v_fold = oppositeNodeFold(u_fold, uv_fold);

        // restore the node
        Node v = restoreNode(v_fold);

        // restore the edge
        Edge uv = restoreEdge(uv_fold);

        return uv;
    }

    /// \brief Unreduces the edge.
    Node unreduce(Edge uw)
    {
        // u <---> **v** <---> w

        // get the reduced node
        EdgeFoldPtr uw_fold = _edge_to_fold[uw];
        NodeFoldPtr v_fold = uw_fold->_reduced_fold;

        // restore it to the tree
        Node v = restoreNode(v_fold);

        // get the reduced edges
        EdgeFoldPtr uv_fold = v_fold->_uv_fold;
        EdgeFoldPtr vw_fold = v_fold->_vw_fold;

        // restore the edges
        Edge uv = restoreEdge(uv_fold);
        Edge vw = restoreEdge(vw_fold);

        // remove the old edge
        _graph.removeEdge(uw);

        return v;
    }

};

////////////////////////////////////////////////////////////////////////////////
//
// FoldedContourTree
//
////////////////////////////////////////////////////////////////////////////////

class FoldedMembers
{
public:
    virtual ~FoldedMembers() { }
    virtual size_t size() const = 0;
};

class FoldedNodeMembers : public FoldedMembers
{
public:
    virtual size_t size() const { return 0; }

};

class FoldedEdgeMembers : public FoldedMembers
{
public:
    virtual size_t size() const
    {
        return 0;
    }
};

template <typename ContourTree>
class FoldedContourTree :
        public
        ReadableUndirectedGraphMixin <FoldTree,
        BaseGraphMixin <FoldTree> >
{
public:
    typedef FoldTree::Node Node;
    typedef FoldTree::Edge Edge;

private:
    typedef
    ReadableUndirectedGraphMixin <FoldTree,
    BaseGraphMixin <FoldTree> >
    Mixin;

    const ContourTree& _contour_tree;
    FoldTree _fold_tree;

    FoldedEdgeMembers _members;

    // map the contour tree nodes to the fold tree nodes so we can later make
    // the correct edges.
    StaticNodeMap<ContourTree, Node> _ct_to_fold_node;

    std::vector<typename ContourTree::Node> _fold_id_to_ct_node;
    std::vector<typename ContourTree::Edge> _fold_id_to_ct_edge;

    typename ContourTree::Node getContourTreeNode(Node node) const
    {
        size_t id = _fold_tree.getNodeFold(node).getIdentifier();
        return _fold_id_to_ct_node[id];
    }

    typename ContourTree::Edge getContourTreeEdge(Edge edge) const
    {
        size_t id = _fold_tree.getEdgeFold(edge).getIdentifier();
        return _fold_id_to_ct_edge[id];
    }


public:

    typedef FoldedMembers Members;

    FoldedContourTree(const ContourTree& contour_tree)
            : Mixin(_fold_tree), _contour_tree(contour_tree), 
              _ct_to_fold_node(_contour_tree)
    {
        // we need to initialize the fold tree with the structure of the 
        // contour tree. We also want to map the folds to their corresponding
        // contour tree nodes and edges.

        // add each node in turn
        for (NodeIterator<ContourTree> it(_contour_tree); !it.done(); ++it) {
            // add the node
            Node node = _fold_tree.addNode();

            // map the node fold id to the contour tree node
            _fold_id_to_ct_node.push_back(it.node());

            // map the contour tree node to the fold tree node
            _ct_to_fold_node[it.node()] = node;
        }

        for (EdgeIterator<ContourTree> it(_contour_tree); !it.done(); ++it) {
            // get the nodes in the contour tree
            typename ContourTree::Node ct_u = _contour_tree.u(it.edge());
            typename ContourTree::Node ct_v = _contour_tree.v(it.edge());

            // get the corresponding nodes in the fold tree
            Node u = _ct_to_fold_node[ct_u];
            Node v = _ct_to_fold_node[ct_v];

            // add the edge
            Edge edge = _fold_tree.addEdge(u,v);

            // map the contour tree edge to this fold edge
            _fold_id_to_ct_edge.push_back(it.edge());
        }
    }

    /// \brief Retrieve the node's scalar value.
    double getValue(Node node) const {
        return _contour_tree.getValue(getContourTreeNode(node));
    }

    /// \brief Get the ID of a node.
    unsigned int getID(Node node) const {
        return _contour_tree.getID(getContourTreeNode(node));
    }

    /// \brief Retrieve a node by ID.
    Node getNode(unsigned int id) const {
        return _ct_to_fold_node[_contour_tree.getNode(id)];
    }

    /// \brief Collapse an edge.
    void collapse(Edge edge) {
        return _fold_tree.collapse(edge);
    }

    /// \brief Reduced a node, connecting its neighbors.
    Edge reduce(Node v) {
        return _fold_tree.reduce(v);
    }

    /// \brief Checks if the node has collapsed edges inside.
    bool hasCollapsed(Node node) const {
        return _fold_tree.getNodeFold(node).numberOfCollapsedEdgeFolds() > 0;
    }

    /// \brief Checks if the edge contains a reduced node.
    bool hasReduced(Edge edge) const {
        return _fold_tree.getEdgeFold(edge).hasReduced();
    }

    /// \brief Uncollapse a collapsed edge in the node.
    Edge uncollapse(Node u, int index=-1)
    {
        Edge edge = _fold_tree.uncollapse(u, index);

        // we have to be careful: we have a new node in the tree now,
        // and we have to map the corresponding contour tree node to it
        Node node = _fold_tree.opposite(u, edge);
        _ct_to_fold_node[getContourTreeNode(node)] = node;

        return edge;
    }

    /// \brief Unreduce the edge.
    Node unreduce(Edge uw)
    {
        // unreduce and get the newly created node
        Node node = _fold_tree.unreduce(uw);

        // map the contour tree node to the new node
        _ct_to_fold_node[getContourTreeNode(node)] = node;

        return node;
    }

    const Members& getNodeMembers(Node node) const {
        return _members;
    }

    const Members& getEdgeMembers(Edge edge) const 
    {
        typename ContourTree::Node ct_u = getContourTreeNode(_fold_tree.u(edge));
        typename ContourTree::Node ct_v = getContourTreeNode(_fold_tree.v(edge));

        std::cout << "Computing members for " << _contour_tree.getID(ct_u) << " <---> " <<
                                                 _contour_tree.getID(ct_v) << std::endl;

        FoldTree::EdgeFold edge_fold = _fold_tree.getEdgeFold(edge);

        if (edge_fold.hasReduced())
        {
            std::cout << "\tThe edge was reduced." << std::endl;

            // get the reduced node
            FoldTree::NodeFold reduced_node = edge_fold.reducedFold();

            typename ContourTree::Node reduced_ct_node = _fold_id_to_ct_node[reduced_node.getIdentifier()];

            std::cout << "\tThe reduced node had id: " << _contour_tree.getID(reduced_ct_node) << std::endl;

            // now, get the reduced edges
            FoldTree::EdgeFold uvfold = reduced_node.uvFold();
            FoldTree::EdgeFold vwfold = reduced_node.vwFold();

        }


        return _members;
    }
};

////////////////////////////////////////////////////////////////////////////////
//
// Helpers
//
////////////////////////////////////////////////////////////////////////////////


/// \brief Expands the subtree
template <typename FoldedTree>
void expandSubtree(
        FoldedTree& tree,
        typename FoldedTree::Node parent,
        typename FoldedTree::Node child)
{
    typedef typename FoldedTree::Node Node;
    typedef typename FoldedTree::Edge Edge;

    std::queue<Edge> expand_queue;

    // add all of the edges in the subtree to the queue
    expand_queue.push(tree.findEdge(parent,child));

    for (UndirectedBFSIterator<FoldedTree> it(tree, parent, child);
            !it.done(); ++it) 
    {
        expand_queue.push(it.edge());
    }

    // now we expand each edge
    while (expand_queue.size() > 0)
    {
        Edge edge = expand_queue.front();
        expand_queue.pop();

        // if the edge has a reduced node, unreduce it
        if (tree.hasReduced(edge))
        {
            Node node = tree.unreduce(edge);

            // add each of the node's neighbor edges to the expand list
            for (UndirectedNeighborIterator<FoldedTree> it(tree, node); 
                    !it.done(); ++it)
            {
                expand_queue.push(it.edge());
            }

            // uncollapse the node
            while (tree.hasCollapsed(node))
            {
                Edge collapsed_edge = tree.uncollapse(node);
                expand_queue.push(collapsed_edge);
            }
        }
    }
}


} // namespace denali


#endif
