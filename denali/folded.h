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

    for (EdgeIterator<FoldedTree> it(tree); !it.done(); ++it) 
    {
        typename FoldedTree::Node u = tree.u(it.edge());
        typename FoldedTree::Node v = tree.v(it.edge());
        std::cout << tree.getID(u) << " <----> " << tree.getID(v) << std::endl;
    }

    // add all of the edges in the subtree to the queue
    expand_queue.push(tree.findEdge(parent,child));

    assert(tree.isEdgeValid(tree.findEdge(parent, child)));

    for (UndirectedBFSIterator<FoldedTree> it(tree, parent, child);
            !it.done(); ++it) 
    {
        assert(tree.isEdgeValid(it.edge()));
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

        // uncollapse the nodes at the ends of the edge
        Node u = tree.u(edge);
        Node v = tree.v(edge);

        while (tree.hasCollapsed(u))
        {
            Edge collapsed_edge = tree.uncollapse(u);
            expand_queue.push(collapsed_edge);
        }

        while (tree.hasCollapsed(v))
        {
            Edge collapsed_edge = tree.uncollapse(v);
            expand_queue.push(collapsed_edge);
        }
    }
}


class MappableFoldTree : public
        EdgeObservableMixin <UndirectedGraph,
        NodeObservableMixin <UndirectedGraph,
        ReadableUndirectedGraphMixin <UndirectedGraph,
        BaseGraphMixin <UndirectedGraph> > > >
{
    typedef
    EdgeObservableMixin <UndirectedGraph,
    NodeObservableMixin <UndirectedGraph,
    ReadableUndirectedGraphMixin <UndirectedGraph,
    BaseGraphMixin <UndirectedGraph> > > >
    Mixin;

    typedef UndirectedGraph GraphType;

public:

    typedef GraphType::Node Node;
    typedef GraphType::Edge Edge;

    class FoldObserver 
    {
    public:
        virtual void notify() = 0;
    };

    class NodeFold
    {
        friend class MappableFoldTree;
        int _index;
        NodeFold(int i) : _index(i) {}
    public:
        NodeFold() : _index(-1) {}

        bool operator==(const NodeFold& node_fold) const {
            return _index == node_fold._index;
        }

        bool operator!=(const NodeFold& node_fold) const {
            return _index != node_fold._index;
        }

        bool operator<(const NodeFold& node_fold) const {
            return _index < node_fold._index;
        }
    };

    class EdgeFold
    {
        friend class MappableFoldTree;
        int _index;
        EdgeFold(int i) : _index(i) {}

    public:
    
        EdgeFold() : _index(-1) {}

        bool operator==(const EdgeFold& edge_fold) const {
            return _index == edge_fold._index;
        }

        bool operator!=(const EdgeFold& edge_fold) const {
            return _index != edge_fold._index;
        }

        bool operator<(const EdgeFold& edge_fold) const {
            return _index < edge_fold._index;
        }
    };

    MappableFoldTree() :
            Mixin(_graph), _node_to_fold(_graph), _edge_to_fold(_graph),
            _master_node_fold_observer(_node_fold_observers), 
            _master_edge_fold_observer(_edge_fold_observers)
    {
        _node_folds.attachObserver(_master_node_fold_observer); 
        _edge_folds.attachObserver(_master_edge_fold_observer); 
    }

    ~MappableFoldTree()
    {
        _node_folds.detachObserver(_master_node_fold_observer);
        _edge_folds.detachObserver(_master_edge_fold_observer);
    }

    /// \brief Add a node to the tree.
    Node addNode()
    {
        // create the node
        Node node = _graph.addNode();

        // create the node fold representation
        int n = _node_folds.insert(NodeFoldRep());
        _node_folds[n].node = node;

        // link the node to the fold
        _node_to_fold[node] = NodeFold(n);

        return node;
    }

    Edge addEdge(Node u, Node v)
    {
        // create the edge
        Edge edge = _graph.addEdge(u,v);

        // get the node folds of u and v
        NodeFold u_fold = _node_to_fold[u];
        NodeFold v_fold = _node_to_fold[v];

        // make an edge fold
        int n = _edge_folds.insert(EdgeFoldRep());
        _edge_folds[n].edge = edge;
        _edge_folds[n].u_fold = u_fold;
        _edge_folds[n].v_fold = v_fold;

        // link it to the edge
        _edge_to_fold[edge] = EdgeFold(n);

        return edge;
    }

    NodeFold getNodeFold(Node node) const {
        return _node_to_fold[node];
    }

    EdgeFold getEdgeFold(Edge edge) const {
        return _edge_to_fold[edge];
    }

    size_t numberOfNodeFolds() const {
        return _node_folds.size();
    }

    size_t numberOfEdgeFolds() const {
        return _edge_folds.size();
    }

    void collapse(Edge edge)
    {
        Node parent = _graph.degree(_graph.u(edge)) == 1 ?
                _graph.v(edge) : _graph.u(edge);

        Node child = _graph.opposite(parent, edge);

        assert(_graph.degree(child) == 1);

        // add the fold edge to the parent node folds collapse list
        NodeFold parent_node_fold = _node_to_fold[parent];
        int n = parent_node_fold._index;
        _node_folds[n].collapsed.push_back(_edge_to_fold[edge]);

        // remove the child from the tree
        _graph.removeNode(child);
    }

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
        NodeFold v_fold = _node_to_fold[v];
        NodeFoldRep& v_fold_rep = _node_folds[v_fold._index];
        v_fold_rep.uv_fold = _edge_to_fold[uv];
        v_fold_rep.vw_fold = _edge_to_fold[vw];

        // update the new edge's reduced node
        EdgeFold uw_fold = _edge_to_fold[uw];
        EdgeFoldRep& uw_fold_rep = _edge_folds[uw_fold._index];
        uw_fold_rep.reduced_fold = v_fold;

        // remove the node
        _graph.removeNode(v);

        return uw;
    }

    Edge uncollapse(Node u, int index=-1)
    {
        // get the folded node
        NodeFold u_fold = _node_to_fold[u];
        NodeFoldRep& u_fold_rep = _node_folds[u_fold._index];

        // if the index is less than one, uncollapse the last collapsed edge
        if (index < 0) {
            index = u_fold_rep.collapsed.size()-1;
        }

        // pop the collapsed edge fold
        EdgeFold uv_fold = u_fold_rep.collapsed[index];
        EdgeFoldRep& uv_fold_rep = _edge_folds[uv_fold._index];
        u_fold_rep.collapsed.erase(u_fold_rep.collapsed.begin() + index);

        // get the collapsed node fold at the other end of the edge
        NodeFold v_fold = oppositeNodeFold(u_fold, uv_fold);

        // restore the node
        Node v = restoreNode(v_fold);

        // restore the edge
        Edge uv = restoreEdge(uv_fold);

        return uv;
    }

    Node unreduce(Edge uw)
    {
        // get the reduced node
        EdgeFold uw_fold = _edge_to_fold[uw];
        EdgeFoldRep& uw_fold_rep = _edge_folds[uw_fold._index];
        NodeFold v_fold = uw_fold_rep.reduced_fold;

        // restore it to the tree
        Node v = restoreNode(v_fold);

        // get the reduced edges
        NodeFoldRep& v_fold_rep = _node_folds[v_fold._index];
        EdgeFold uv_fold = v_fold_rep.uv_fold;
        EdgeFold vw_fold = v_fold_rep.vw_fold;

        // restore the edges
        Edge uv = restoreEdge(uv_fold);
        Edge vw = restoreEdge(vw_fold);

        // delete the edge fold
        _edge_folds.remove(uw_fold._index);

        // delete the edge
        _graph.removeEdge(uw);

        return v;
    }

    size_t numberOfCollapsedEdgeFolds(NodeFold nf) const {
        return _node_folds[nf._index].collapsed.size();
    }

    EdgeFold getCollapsedEdgeFold(NodeFold nf, size_t i) const
    {
        const NodeFoldRep& nf_rep = _node_folds[nf._index];
        return nf_rep.collapsed[i];
    }

    EdgeFold uvFold(NodeFold nf) const {
        return _node_folds[nf._index].uv_fold;
    }

    EdgeFold vwFold(NodeFold nf) const {
        return _node_folds[nf._index].vw_fold;
    }

    Node getNodeFromFold(NodeFold nf) const {
        return _node_folds[nf._index].node;
    }

    NodeFold uFold(EdgeFold ef) const {
        return _edge_folds[ef._index].u_fold;
    }

    NodeFold vFold(EdgeFold ef) const {
        return _edge_folds[ef._index].v_fold;
    }

    NodeFold reducedFold(EdgeFold ef) const {
        return _edge_folds[ef._index].reduced_fold;
    }

    bool hasReduced(EdgeFold ef) const {
        return _edge_folds[ef._index].reduced_fold._index != -1;
    }

    Edge getEdgeFromFold(EdgeFold ef) const {
        return _edge_folds[ef._index].edge;
    }

    size_t getNodeFoldIdentifier(NodeFold nf) const {
        return nf._index;
    }

    size_t getMaxNodeFoldIdentifier() const {
        return _node_folds.getMaxIdentifier();
    }

    size_t getEdgeFoldIdentifier(EdgeFold ef) const {
        return ef._index;
    }

    size_t getMaxEdgeFoldIdentifier() const {
        return _node_folds.getMaxIdentifier();
    }

    void attachNodeFoldObserver(FoldObserver& observer) {
        _node_fold_observers.push_back(&observer);
    }

    void detachNodeFoldObserver(FoldObserver& observer) {
        _node_fold_observers.remove(&observer);
    }

    void attachEdgeFoldObserver(FoldObserver& observer) {
        _edge_fold_observers.push_back(&observer);
    }

    void detachEdgeFoldObserver(FoldObserver& observer) {
        _edge_fold_observers.remove(&observer);
    }


private:

    struct NodeFoldRep
    {
        std::vector<EdgeFold> collapsed;
        EdgeFold uv_fold;
        EdgeFold vw_fold;
        Node node;
    };

    struct EdgeFoldRep
    {
        NodeFold u_fold;
        NodeFold v_fold;
        NodeFold reduced_fold;
        Edge edge;
    };

    GraphType _graph;

    ObservingNodeMap<GraphType, NodeFold> _node_to_fold;
    ObservingEdgeMap<GraphType, EdgeFold> _edge_to_fold;

    MappableList<NodeFoldRep> _node_folds;
    MappableList<EdgeFoldRep> _edge_folds;

    typedef std::list<FoldObserver*> FoldObservers;
    FoldObservers _node_fold_observers;
    FoldObservers _edge_fold_observers;

    template <typename Rep>
    class MasterFoldObserver : public MappableList<Rep>::Observer
    {
        FoldObservers& _observers;
    public:
        MasterFoldObserver(FoldObservers& observers) :
                _observers(observers) 
        {}

        virtual void notify() const
        {
            for (FoldObservers::const_iterator it = _observers.begin();
                    it != _observers.end(); ++it)
            {
                (*it)->notify();
            }
        }
    };

    MasterFoldObserver<NodeFoldRep> _master_node_fold_observer;
    MasterFoldObserver<EdgeFoldRep> _master_edge_fold_observer;

    NodeFold oppositeNodeFold(NodeFold u_fold, EdgeFold uv_fold)
    {
        NodeFoldRep& u_fold_rep  = _node_folds[u_fold._index];
        EdgeFoldRep& uv_fold_rep = _edge_folds[uv_fold._index];

        return uv_fold_rep.u_fold == u_fold ?
                uv_fold_rep.v_fold : uv_fold_rep.u_fold;
    }

    Node restoreNode(NodeFold v_fold)
    {
        Node v = _graph.addNode();

        NodeFoldRep& v_fold_rep = _node_folds[v_fold._index];

        v_fold_rep.node = v;
        _node_to_fold[v] = v_fold;
        return v;
    }

    Edge restoreEdge(EdgeFold uv_fold)
    {
        EdgeFoldRep& uv_fold_rep = _edge_folds[uv_fold._index];
        NodeFoldRep& u_fold_rep = _node_folds[uv_fold_rep.u_fold._index];
        NodeFoldRep& v_fold_rep = _node_folds[uv_fold_rep.v_fold._index];
        
        Node u = u_fold_rep.node;
        Node v = v_fold_rep.node;

        Edge uv = _graph.addEdge(u,v);
        uv_fold_rep.edge = uv;
        _edge_to_fold[uv] = uv_fold;
        return uv;
    }


};


template <typename NodeFoldObservable, typename ValueType>
class ObservingNodeFoldMap : public NodeFoldObservable::FoldObserver
{
    NodeFoldObservable& _graph;
    std::vector<ValueType> _values;

public:
    ObservingNodeFoldMap(NodeFoldObservable& graph)
        : _graph(graph), _values(_graph.getMaxNodeFoldIdentifier())
    {
        _graph.attachNodeFoldObserver(*this);
    }

    ~ObservingNodeFoldMap()
    {
        _graph.detachNodeFoldObserver(*this);
    }

    void notify()
    {
        _values.resize(_graph.getMaxNodeFoldIdentifier());
    }

    typename std::vector<ValueType>::reference
    operator[](typename NodeFoldObservable::NodeFold node_fold)
    {
        return _values[_graph.getNodeFoldIdentifier(node_fold)];
    }

    typename std::vector<ValueType>::const_reference
    operator[](typename NodeFoldObservable::NodeFold node_fold) const
    {
        return _values[_graph.getNodeFoldIdentifier(node_fold)];
    }

};


template <typename EdgeFoldObservable, typename ValueType>
class ObservingEdgeFoldMap : public EdgeFoldObservable::FoldObserver
{
    EdgeFoldObservable& _graph;
    std::vector<ValueType> _values;

public:
    ObservingEdgeFoldMap(EdgeFoldObservable& graph)
        : _graph(graph), _values(_graph.getMaxEdgeFoldIdentifier())
    {
        _graph.attachEdgeFoldObserver(*this);
    }

    ~ObservingEdgeFoldMap()
    {
        _graph.detachEdgeFoldObserver(*this);
    }

    void notify()
    {
        _values.resize(_graph.getMaxEdgeFoldIdentifier());
    }

    typename std::vector<ValueType>::reference
    operator[](typename EdgeFoldObservable::EdgeFold edge_fold)
    {
        return _values[_graph.getEdgeFoldIdentifier(edge_fold)];
    }

    typename std::vector<ValueType>::const_reference
    operator[](typename EdgeFoldObservable::EdgeFold edge_fold) const
    {
        return _values[_graph.getEdgeFoldIdentifier(edge_fold)];
    }

};


template <typename ContourTree>
class MappableFoldedContourTree :
        public
        ReadableUndirectedGraphMixin <MappableFoldTree,
        BaseGraphMixin <MappableFoldTree> >
{
public:
    typedef MappableFoldTree::Node Node;
    typedef MappableFoldTree::Edge Edge;
    typedef MappableFoldTree::NodeFold NodeFold;
    typedef MappableFoldTree::EdgeFold EdgeFold;

    typedef typename ContourTree::Members Members;

private:
    typedef
    ReadableUndirectedGraphMixin <MappableFoldTree,
    BaseGraphMixin <MappableFoldTree> >
    Mixin;

    const ContourTree& _contour_tree;
    MappableFoldTree _fold_tree;

    Members _members;

    StaticNodeMap<ContourTree, NodeFold> _ct_to_fold_node;

    ObservingNodeFoldMap<MappableFoldTree, typename ContourTree::Node>
            _fold_to_ct_node;

    ObservingEdgeFoldMap<MappableFoldTree, typename ContourTree::Edge>
            _fold_to_ct_edge;

    typename ContourTree::Node getContourTreeNode(Node node) const {
        return _fold_to_ct_node[_fold_tree.getNodeFold(node)];
    }

    typename ContourTree::Edge getContourTreeEdge(Edge edge) const {
        return _fold_to_ct_edge[_fold_tree.getEdgeFold(edge)];
    }

public:
    
    MappableFoldedContourTree(const ContourTree& contour_tree) :
            Mixin(_fold_tree), _contour_tree(contour_tree),
            _ct_to_fold_node(_contour_tree),
            _fold_to_ct_node(_fold_tree), _fold_to_ct_edge(_fold_tree)
    {
        // we need to initialize the fold tree with the structure of the contour
        // tree. We also want to map the folds to their corresponding nodes and 
        // edges.

        for (NodeIterator<ContourTree> it(contour_tree); !it.done(); ++it)
        {
            // make the new node
            Node node = _fold_tree.addNode();

            // get the node fold
            NodeFold node_fold = _fold_tree.getNodeFold(node);

            // map the contour tree node to the fold node
            _ct_to_fold_node[it.node()] = node_fold;

            // map the node fold to the contour tree node
            _fold_to_ct_node[node_fold] = it.node();
        }

        for (EdgeIterator<ContourTree> it(_contour_tree); !it.done(); ++it)
        {
            // get the nodes in the contour tree
            typename ContourTree::Node ct_u = _contour_tree.u(it.edge());
            typename ContourTree::Node ct_v = _contour_tree.v(it.edge());

            // get the folds corresponding to the contour tree nodes
            NodeFold u_fold = _ct_to_fold_node[ct_u];
            NodeFold v_fold = _ct_to_fold_node[ct_v];

            // get the corresponding nodes in the fold tree
            Node u = _fold_tree.getNodeFromFold(u_fold);
            Node v = _fold_tree.getNodeFromFold(v_fold);

            // add the edge
            Edge edge = _fold_tree.addEdge(u,v);

            // get the edge fold
            EdgeFold edge_fold = _fold_tree.getEdgeFold(edge);

            // map the contour tree edge to this fold edge
            _fold_to_ct_edge[edge_fold] = it.edge();
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
        NodeFold node_fold = _ct_to_fold_node[_contour_tree.getNode(id)];
        return _fold_tree.getNodeFromFold(node_fold);
    }

    /// \brief Collapse an edge.
    void collapse(Edge edge) {
        return _fold_tree.collapse(edge);
    }

    /// \brief Reduced a node, connecting its neighbors.
    Edge reduce(Node v) {
        return _fold_tree.reduce(v);
    }

    int numberOfCollapsedEdgeFolds(NodeFold node_fold) const {
        return _fold_tree.numberOfCollapsedEdgeFolds(node_fold);
    }

    EdgeFold getCollapsedEdgeFold(NodeFold nf, size_t i) const
    {
        return _fold_tree.getCollapsedEdgeFold(nf, i);
    }

    EdgeFold uvFold(NodeFold nf) const {
        return _fold_tree.uvFold(nf);
    }

    EdgeFold vwFold(NodeFold nf) const {
        return _fold_tree.vwFold(nf);
    }

    Node getNodeFromFold(NodeFold nf) const {
        return _fold_tree.getNodeFromFold(nf);
    }

    NodeFold uFold(EdgeFold ef) const {
        return _fold_tree.uFold(ef);
    }

    NodeFold vFold(EdgeFold ef) const {
         return _fold_tree.vFold(ef);
    }

    NodeFold reducedFold(EdgeFold ef) const {
         return _fold_tree.reducedFold(ef);
    }

    bool hasReduced(EdgeFold ef) const {
         return _fold_tree.hasReduced(ef);
    }

    Edge getEdgeFromFold(EdgeFold ef) const {
         return _fold_tree.getEdgeFromFold(ef);
    }

    Edge uncollapse(Node u, int index=-1) {
        return _fold_tree.uncollapse(u, index);
    }

    Node unreduce(Edge uw) {
        return _fold_tree.unreduce(uw); 
    }

    const Members& getNodeMembers(Node node) const {
        return _members;
    }

    const Members& getEdgeMembers(Edge edge) const {
        return _members;
    }

    // HERE FOR COMPATIBILITY, REMOVE AFTER TESTS PASS: 

    bool hasReduced(Edge edge) const {
        return hasReduced(_fold_tree.getEdgeFold(edge));
    }

    bool hasCollapsed(Node node) const {
        return numberOfCollapsedEdgeFolds(_fold_tree.getNodeFold(node)) > 0;
    }





};


} // namespace denali


#endif
