#ifndef DENALI_FOLDED_H
#define DENALI_FOLDED_H

#include <denali/contour_tree.h>
#include <denali/graph_maps.h>
#include <denali/graph_mixins.h>
#include <denali/graph_structures.h>
#include <denali/mappable_list.h>

#include <stdexcept>

namespace denali {

////////////////////////////////////////////////////////////////////////////////
//
// Helpers
//
////////////////////////////////////////////////////////////////////////////////


/// \brief Expands the subtree
/// \ingroup fold_tree
/*!
 *  Given an edge, specified by the parent and child nodes, fully expands every
 *  edge and node that is in the induced subtree. 
 */
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

////////////////////////////////////////////////////////////////////////////////
//
// FoldTree
//
////////////////////////////////////////////////////////////////////////////////

class FoldTree : public
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
        friend class FoldTree;
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
        friend class FoldTree;
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

    FoldTree() :
            Mixin(_graph), _node_to_fold(_graph), _edge_to_fold(_graph),
            _master_node_fold_observer(_node_fold_observers), 
            _master_edge_fold_observer(_edge_fold_observers)
    {
        _node_folds.attachObserver(_master_node_fold_observer); 
        _edge_folds.attachObserver(_master_edge_fold_observer); 
    }

    ~FoldTree()
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
        u_fold_rep.collapsed.erase(u_fold_rep.collapsed.begin() + index);

        // get the collapsed node fold at the other end of the edge
        NodeFold v_fold = oppositeNodeFold(u_fold, uv_fold);

        // restore the node
        restoreNode(v_fold);

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
        restoreEdge(uv_fold);
        restoreEdge(vw_fold);

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
        return _edge_folds.getMaxIdentifier();
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

////////////////////////////////////////////////////////////////////////////////
//
// ObservingNodeFoldMap
//
////////////////////////////////////////////////////////////////////////////////

/// \brief Map which observes the node folds of a NodeFoldObservable tree.
/// \ingroup fold_tree
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

////////////////////////////////////////////////////////////////////////////////
//
// ObservingEdgeFoldMap
//
////////////////////////////////////////////////////////////////////////////////

/// \brief Map which observes the edge folds of a EdgeFoldObservable tree.
/// \ingroup fold_tree
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

////////////////////////////////////////////////////////////////////////////////
//
// FoldedContourTree
//
////////////////////////////////////////////////////////////////////////////////

/// \brief A contour tree whose nodes and edges can be folded out of view.
/// \ingroup fold_tree
template <typename ContourTree>
class FoldedContourTree :
        public
        NodeObservableMixin <FoldTree,
        EdgeObservableMixin <FoldTree,
        ReadableUndirectedGraphMixin <FoldTree,
        BaseGraphMixin <FoldTree> > > >
{
public:
    typedef FoldTree::Node Node;
    typedef FoldTree::Edge Edge;

    /// \brief Represents a folded node.
    /*!
     *  The folded tree consists of nodes, both hidden and visible. All nodes have
     *  node folds, which describes the node and its status in the fully expanded tree.
     */
    typedef FoldTree::NodeFold NodeFold;

    /// \brief Represents a folded edge.
    /*!
     *  A folded edge represents the history of an edge at some point in the folding
     *  of the tree.
     */
    typedef FoldTree::EdgeFold EdgeFold;

    /// \brief A set of node members.
    class Members
    {
        template <typename T> friend class FoldedContourTree;

        typedef boost::shared_ptr<Members> MembersPtr;

        size_t _size;
        const typename ContourTree::Members* _ct_members;
        std::list<MembersPtr> _nested_members;

        Members(const typename ContourTree::Members* ctm) :
                _size(ctm->size()), _ct_members(ctm) {}

    public:
        class const_iterator
        {
            friend class Members;

            typedef typename ContourTree::Member Member;

            const Members* _members;
            typename ContourTree::Members::iterator _ct_member_it;
            typename std::list<MembersPtr>::const_iterator _folded_list_it;
            boost::shared_ptr<const_iterator> _folded_member_it;

            const_iterator(const Members* members) : _members(members) {}

            /// \brief Advance the list and make the _folded_member_it valid, if possible.
            void advanceList()
            {
                while (_folded_list_it != _members->_nested_members.end() &&
                       (*_folded_list_it)->size() == 0) {
                    _folded_list_it++;
                }

                if (_folded_list_it != _members->_nested_members.end()) 
                {
                    _folded_member_it = boost::shared_ptr<const_iterator>(new 
                            const_iterator((*_folded_list_it)->begin()));
                }
            }


        public:
            bool operator==(const const_iterator& rhs) const 
            {
                // check that the two iterators iterate over the same set
                if (_members != rhs._members) {
                    return false;
                }

                if (_members->_ct_members) {
                    if (_ct_member_it != rhs._ct_member_it) {
                        return false;
                    }
                }

                if (_folded_list_it != rhs._folded_list_it) {
                    return false;
                }

                if (_folded_list_it != _members->_nested_members.end()) {
                    if ((*_folded_member_it) != (*rhs._folded_member_it)) {
                        return false;
                    }
                }

                return true;
            }

            bool operator!=(const const_iterator rhs) const {
                return !((*this) == (rhs));
            }

            /// \brief Advances the iterator until the end or a valid entry is met.
            void operator++() 
            {
                if (_members->_ct_members)
                {
                    if (_ct_member_it != _members->_ct_members->end())
                    {
                        _ct_member_it++;
                        return;
                    }
                }

                // either there are no ct members, or we have reached the end of them
                ++(*_folded_member_it);

                if ((*_folded_member_it) == (*_folded_list_it)->end()) {
                    _folded_list_it++;
                    advanceList();
                }
            }

            const Member& operator*() const 
            {
                if (_members->_ct_members)
                {
                    if (_ct_member_it != _members->_ct_members->end())
                    {
                        return *_ct_member_it;
                    }
                }

                // either there are no ct members, of we have reached the end of them
                return **_folded_member_it;
            }

        };
        friend class const_iterator;

        Members() : 
                _size(0), _ct_members(0) {}

        size_t size() const {
            return _size;
        }

        const_iterator begin() const {
            const_iterator it(this);

            if (_ct_members)
            {
                it._ct_member_it = (_ct_members->begin());
            }

            it._folded_list_it = _nested_members.begin();

            if (_nested_members.begin() != _nested_members.end()) {
                it.advanceList();
            }


            return it;
        }

        const_iterator end() const
        {
            const_iterator it(this);

            if (_ct_members)
            {
                it._ct_member_it = (_ct_members->end());
            }

            it._folded_list_it = _nested_members.end();

            return it;
        }

    };


private:
    typedef
    NodeObservableMixin <FoldTree,
    EdgeObservableMixin <FoldTree,
    ReadableUndirectedGraphMixin <FoldTree,
    BaseGraphMixin <FoldTree> > > >
    Mixin;

    typedef typename ContourTree::Members ContourTreeMembers;

    const ContourTree& _contour_tree;
    FoldTree _fold_tree;

    StaticNodeMap<ContourTree, NodeFold> _ct_to_fold_node;

    ObservingNodeFoldMap<FoldTree, typename ContourTree::Node>
            _fold_to_ct_node;

    ObservingEdgeFoldMap<FoldTree, typename ContourTree::Edge>
            _fold_to_ct_edge;

    typedef boost::shared_ptr<Members> MembersPtr;

    ObservingNodeFoldMap<FoldTree, boost::shared_ptr<Members> > _node_members;
    ObservingEdgeFoldMap<FoldTree, boost::shared_ptr<Members> > _edge_members;

    typename ContourTree::Node getContourTreeNode(Node node) const {
        return _fold_to_ct_node[_fold_tree.getNodeFold(node)];
    }

    typename ContourTree::Edge getContourTreeEdge(Edge edge) const {
        return _fold_to_ct_edge[_fold_tree.getEdgeFold(edge)];
    }

public:

    typedef typename ContourTree::Member Member;
    
    FoldedContourTree(const ContourTree& contour_tree) :
            Mixin(_fold_tree), _contour_tree(contour_tree),
            _ct_to_fold_node(_contour_tree),
            _fold_to_ct_node(_fold_tree), _fold_to_ct_edge(_fold_tree),
            _node_members(_fold_tree), _edge_members(_fold_tree)
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

            // set the node's members to be the CT node's members
            const ContourTreeMembers* ctm = &_contour_tree.getNodeMembers(it.node());
            _node_members[node_fold] = MembersPtr(new Members(ctm));
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

            // set the edge's members to be the CT edge's members
            const ContourTreeMembers* ctm = &_contour_tree.getEdgeMembers(it.edge());
            _edge_members[edge_fold] = MembersPtr(new Members(ctm));
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
    Node getNode(unsigned int id) const
    {
        typename ContourTree::Node node = _contour_tree.getNode(id);

        // if there isn't a node with the ID, return an invalid node
        if (_contour_tree.isNodeValid(node)) 
        {
            NodeFold node_fold = _ct_to_fold_node[_contour_tree.getNode(id)];
            return _fold_tree.getNodeFromFold(node_fold);
        }
        else
        {
            return _fold_tree.getInvalidNode();
        }
    }

    /// \brief Collapse an edge.
    void collapse(Edge edge) 
    {
        // get the base of the edge (the non-leaf parent)
        Node u = _fold_tree.u(edge);
        Node v = _fold_tree.v(edge);
        Node base = _fold_tree.degree(u) == 1 ? v : u;
        Node leaf = _fold_tree.opposite(base, edge);

        // get the node fold of the base
        NodeFold base_fold = _fold_tree.getNodeFold(base);

        // get the base's members
        MembersPtr base_members = _node_members[base_fold];

        // add the leaf members and collapsed edge members to the base node's members
        NodeFold leaf_fold = _fold_tree.getNodeFold(leaf);
        MembersPtr leaf_members = _node_members[leaf_fold];
        base_members->_nested_members.push_back(leaf_members);

        EdgeFold edge_fold = _fold_tree.getEdgeFold(edge);
        MembersPtr edge_members = _edge_members[edge_fold];
        base_members->_nested_members.push_back(edge_members);

        // update the size of the node's members
        base_members->_size += leaf_members->_size + edge_members->_size;

        // collapse the edge.
        _fold_tree.collapse(edge);
    }

    /// \brief Reduced a node, connecting its neighbors.
    Edge reduce(Node v) 
    {
        // get the edges incident on this node
        UndirectedNeighborIterator<FoldTree> it(_fold_tree, v);
        Edge uv = it.edge(); ++it;
        Edge vw = it.edge();

        MembersPtr v_members = _node_members[_fold_tree.getNodeFold(v)];
        MembersPtr uv_members = _edge_members[_fold_tree.getEdgeFold(uv)];
        MembersPtr vw_members = _edge_members[_fold_tree.getEdgeFold(vw)];

        // make the new edge
        Edge uw = _fold_tree.reduce(v);

        // we have a new set of members for the new edge
        MembersPtr uw_members = MembersPtr(new Members());
        _edge_members[_fold_tree.getEdgeFold(uw)] = uw_members;

        uw_members->_nested_members.push_back(v_members);
        uw_members->_nested_members.push_back(uv_members);
        uw_members->_nested_members.push_back(vw_members);

        uw_members->_size += v_members->_size + uv_members->_size + vw_members->_size;

        return uw;
    }

    /// \brief Returns the number of edges contained within the node.
    int numberOfCollapsedEdgeFolds(NodeFold node_fold) const {
        return _fold_tree.numberOfCollapsedEdgeFolds(node_fold);
    }

    /// \brief Returns the ith edge fold contained within the node.
    EdgeFold getCollapsedEdgeFold(NodeFold nf, size_t i) const
    {
        return _fold_tree.getCollapsedEdgeFold(nf, i);
    }

    /// \brief Returns the fold of the edge that connected the node to its
    /// first neighbor / before it was reduced.
    EdgeFold uvFold(NodeFold nf) const {
        return _fold_tree.uvFold(nf);
    }

    /// \brief Returns the fold of the edge that connected the node to its
    /// second neighbor / before it was reduced.
    EdgeFold vwFold(NodeFold nf) const {
        return _fold_tree.vwFold(nf);
    }

    /// \brief Given a node fold, returns the associated node.
    Node getNodeFromFold(NodeFold nf) const {
        return _fold_tree.getNodeFromFold(nf);
    }

    /// \brief Returns the fold of the u node of the edge.
    NodeFold uFold(EdgeFold ef) const {
        return _fold_tree.uFold(ef);
    }

    /// \brief Returns the fold of the v node of the edge.
    NodeFold vFold(EdgeFold ef) const {
         return _fold_tree.vFold(ef);
    }

    /// \brief Returns the node that was reduced to form this edge, if any.
    NodeFold reducedFold(EdgeFold ef) const {
         return _fold_tree.reducedFold(ef);
    }

    /// \brief Returns true if a reduced node is contained within the edge, 
    /// false otherwise.
    bool hasReduced(EdgeFold ef) const {
         return _fold_tree.hasReduced(ef);
    }

    /// \brief Returns the tree edge associated with the edge fold. Only
    /// valid if the edge fold corresponds to an edge that is currently visible.
    Edge getEdgeFromFold(EdgeFold ef) const {
         return _fold_tree.getEdgeFromFold(ef);
    }

    /// \brief Uncollapses the collapsed edge.
    Edge uncollapse(Node u, int index=-1) 
    {
        // recover the edge
        Edge edge = _fold_tree.uncollapse(u, index);

        // recover the node at the other end of the edge
        Node v = _fold_tree.opposite(u, edge);

        // get their member sets
        MembersPtr edge_members = _edge_members[_fold_tree.getEdgeFold(edge)];
        MembersPtr v_members = _node_members[_fold_tree.getNodeFold(v)];

        // now remove these from u's nested members
        MembersPtr u_members = _node_members[_fold_tree.getNodeFold(u)];
        u_members->_nested_members.remove(v_members);
        u_members->_nested_members.remove(edge_members);

        // and decrease the size by the appropriate amount
        u_members->_size -= v_members->_size + edge_members->_size;

        return edge;
    }

    /// \brief Unreduces the edge.
    Node unreduce(Edge uw)
    {
        return _fold_tree.unreduce(uw); 
    }

    /// \brief Retrieves the members contained within the node.
    const Members& getNodeMembers(Node node) const {
        return *_node_members[_fold_tree.getNodeFold(node)];
    }

    /// \brief Retrieves the members contained within the edge.
    const Members& getEdgeMembers(Edge edge) const {
        return *_edge_members[_fold_tree.getEdgeFold(edge)];
    }

    /// \brief Returns true if the edge has a reduced node within.
    bool hasReduced(Edge edge) const {
        return hasReduced(_fold_tree.getEdgeFold(edge));
    }

    /// \brief Returns true if the node has collapsed edges within.
    bool hasCollapsed(Node node) const {
        return numberOfCollapsedEdgeFolds(_fold_tree.getNodeFold(node)) > 0;
    }

};


} // namespace denali


#endif
