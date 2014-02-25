#ifndef DENALI_FOLDED_H
#define DENALI_FOLDED_H

#include <denali/graph_structures.h>
#include <denali/graph_mixins.h>
#include <denali/graph_maps.h>
#include <denali/contour_tree.h>

namespace denali {

/// \brief A folded tree.
/// \ingroup fold_tree
class FoldTree :
        public
        ReadableUndirectedGraphMixin <UndirectedGraph,
        BaseGraphMixin <UndirectedGraph> >
{
public:

    class NodeFold;
    class EdgeFold;

private:

    typedef
    ReadableUndirectedGraphMixin <UndirectedGraph,
    BaseGraphMixin <UndirectedGraph> >
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
        return *_node_to_fold[node];
    }

    /// \brief Retrieve an edge's fold.
    const EdgeFold& getEdgeFold(Edge edge) const {
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
// FoldedContourTreeContext
//
////////////////////////////////////////////////////////////////////////////////

/// \brief A simplification context that results in a folded tree.
template <typename ContourTree>
class FoldedContourTreeContext :
        public
        ReadableUndirectedGraphMixin <FoldTree,
        BaseGraphMixin <FoldTree> >
{
public:
    typedef FoldTree::Node Node;
    typedef FoldTree::Edge Edge;
    typedef typename ContourTree::Members Members;

private:
    typedef
    ReadableUndirectedGraphMixin <FoldTree,
    BaseGraphMixin <FoldTree> >
    Mixin;

    const ContourTree& _contour_tree;
    FoldTree _fold_tree;

    Members _members;

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

    typename ContourTree::Node getContourTreeEdge(Node node) const
    {
        size_t id = _fold_tree.getNodeFold(node).getIdentifier();
        return _fold_id_to_ct_node[id];
    }


public:
    FoldedContourTreeContext(const ContourTree& contour_tree)
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

    Members getNodeMembers(Node node) {
        return _members;
    }

    Members getEdgeMembers(Edge edge) {
        return _members;
    }
};


////////////////////////////////////////////////////////////////////////////////
//
// FoldedContourTree
//
////////////////////////////////////////////////////////////////////////////////

/// \brief A folded contour tree.
/// \ingroup contour_tree
template <typename ContourTree>
class FoldedContourTree :
        public
        ContourTreeMixin <FoldedContourTreeContext<ContourTree>,
        BaseGraphMixin <FoldedContourTreeContext<ContourTree> > >
{
    typedef
    ContourTreeMixin <FoldedContourTreeContext<ContourTree>,
    BaseGraphMixin <FoldedContourTreeContext<ContourTree> > >
    Mixin;

    FoldedContourTreeContext<ContourTree> _context;

    FoldedContourTree(const ContourTree& contour_tree)
        : Mixin(_context), _context(contour_tree) {}

public:

    template <typename FoldingAlgorithm>
    static FoldedContourTree<ContourTree> 
    fold(
            const ContourTree& contour_tree,
            FoldingAlgorithm algorithm)
    {
        // Make a new folded contour tree
        FoldedContourTree folded_tree(contour_tree);

        // pass its context to the folder
        algorithm.run(folded_tree._context);

        // return the result
        return folded_tree;
    }
};


} // namespace denali


#endif
