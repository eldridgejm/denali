#ifndef DENALI_SIMPLIFY_H
#define DENALI_SIMPLIFY_H

#include <denali/graph_mixins.h>

#include <boost/shared_ptr.hpp>

namespace denali {

////////////////////////////////////////////////////////////////////////////
//
// UndirectedScalarMemberIDGraph
//
////////////////////////////////////////////////////////////////////////////


template <typename GraphType, typename TabType>
class UndirectedTabbedScalarMemberIDGraphBase :
    public
    EdgeObservableMixin < GraphType,
    NodeObservableMixin < GraphType,
    ReadableUndirectedGraphMixin <GraphType,
    BaseGraphMixin <GraphType> > > >
{
    typedef
    EdgeObservableMixin < GraphType,
                        NodeObservableMixin < GraphType,
                        ReadableUndirectedGraphMixin <GraphType,
                        BaseGraphMixin <GraphType> > > >
                        Mixin;

public:
    typedef typename Mixin::Node Node;
    typedef typename Mixin::Edge Edge;
    typedef typename GraphType::Members Members;
    typedef TabType Tab;

private:

    GraphType _graph;

    struct TabContainer
    {
        Node u;
        Tab u_tab;
        Node v;
        Tab v_tab;

        bool is_valid;

        TabContainer() : is_valid(false) {}

        Tab getTab(Node node)
        {
            return (node == u ? u_tab : v_tab);
        }
    };

    ObservingEdgeMap<GraphType, TabContainer> _edge_to_tabs;

public:

    UndirectedTabbedScalarMemberIDGraphBase()
        : Mixin(_graph), _edge_to_tabs(_graph) {}

    /// \brief Set an edge's tabs.
    void setTab(Edge edge, Node u, Tab u_tab, Node v, Tab v_tab)
    {
        TabContainer& tab = _edge_to_tabs[edge];
        tab.u = u;
        tab.u_tab = u_tab;
        tab.v = v;
        tab.v_tab = v_tab;
        tab.is_valid = true;
    }

    /// \brief Retrieve a tab.
    Tab getTab(Edge edge, Node node)
    {
        return _edge_to_tabs[edge].getTab(node);
    }

    /// \brief Checks if the edge has tabs.
    bool hasTabs(Edge edge)
    {
        return _edge_to_tabs[edge].is_valid;
    }

    /// \brief Add a node to the graph.
    /*!
     *  \param  id      The id of the node.
     *  \param  value   The scalar value of the node.
     */
    Node addNode(unsigned int id, double value)
    {
        return _graph.addNode(id, value);
    }

    /// \brief Add an edge to the graph.
    Edge addEdge(Node u, Node v)
    {
        return _graph.addEdge(u,v);
    }

    /// \brief Remove the node.
    void removeNode(Node node)
    {
        return _graph.removeNode(node);
    }

    /// \brief Remove the edge.
    void removeEdge(Edge edge)
    {
        return _graph.removeEdge(edge);
    }

    /// \brief Insert a member into the node's member set.
    void insertNodeMember(Node node, unsigned int member)
    {
        return _graph.insertNodeMember(node, member);
    }

    /// \brief Insert a member into the edge's member set.
    void insertEdgeMember(Edge edge, unsigned int member)
    {
        return _graph.insertEdgeMember(edge, member);
    }

    /// \brief Insert members into node member set.
    void insertNodeMembers(Node node, const Members& members)
    {
        return _graph.insertNodeMembers(node, members);
    }

    /// \brief Insert members into edge member set.
    void insertEdgeMembers(Edge edge, const Members& members)
    {
        return _graph.insertEdgeMembers(edge, members);
    }

    /// \brief Get a node's scalar value
    double getValue(Node node) const
    {
        return _graph.getValue(node);
    }

    /// \brief Get a node's ID
    unsigned int getID(Node node) const
    {
        return _graph.getID(node);
    }

    /// \brief Retrieve the members of the node
    const Members& getNodeMembers(Node node) const
    {
        return _graph.getNodeMembers(node);
    }

    /// \brief Retrieve a node by its ID.
    Node getNode(unsigned int id)
    {
        return _graph.getNode(id);
    }

    Node getNodeChecked(unsigned int id)
    {
        return _graph.getNodeChecked(id);
    }

    /// \brief Retrieve the members of the edge.
    const Members& getEdgeMembers(Edge edge) const
    {
        return _graph.getEdgeMembers(edge);
    }

    /// \brief Clear the nodes of the graph
    void clearNodes() {
        return _graph.clearNodes();
    }

    /// \brief Clear the edges of the graph
    void clearEdges() {
        return _graph.clearEdges();
    }

};


/// \brief An undirected graph with tabs in each edge.
/// \ingroup simplified_contour_tree
template <typename Tab>
class UndirectedTabbedScalarMemberIDGraph :
    public UndirectedTabbedScalarMemberIDGraphBase<
    UndirectedScalarMemberIDGraph, Tab>
{
    typedef
    UndirectedTabbedScalarMemberIDGraphBase<
    UndirectedScalarMemberIDGraph, Tab>
    Base;

public:
    typedef typename Base::Node Node;
    typedef typename Base::Edge Edge;
    typedef typename Base::Members Members;
};

////////////////////////////////////////////////////////////////////////////
//
// Priority
//
////////////////////////////////////////////////////////////////////////////

/// \brief A base class for representing a priority.
class Priority
{
    double _priority;
public:

    Priority(double priority)
        : _priority(priority) {}

    double priority() const
    {
        return _priority;
    }
};


bool operator<(const Priority& lhs, const Priority& rhs)
{
    return lhs.priority() < rhs.priority();
}

////////////////////////////////////////////////////////////////////////////
//
// SimplificationContext
//
////////////////////////////////////////////////////////////////////////////

template <typename ContourTree, typename GraphType>
class SimplificationContext
{
    typedef typename GraphType::Node Node;
    typedef typename GraphType::Edge Edge;
    typedef typename ContourTree::Edge Tab;

    GraphType& _graph;

    // keep track of reduced nodes
    ObservingNodeMap<GraphType, bool> _is_reduced;
    std::vector<Node> _reduced_nodes;

    // keep track of the correspondence between edges in the simplified tree
    // and the contour tree
    ObservingEdgeMap<GraphType, Tab> _simplified_edge_to_ct_edge;

public:
    /// \brief Create and initialize a simplification context.
    /*!
     *  This copies the contour tree to the graph.
     */
    SimplificationContext(
        const ContourTree& contour_tree,
        GraphType& graph)
        : _graph(graph), _is_reduced(_graph),
          _simplified_edge_to_ct_edge(_graph)
    {
        typedef typename ContourTree::Node ContourTreeNode;
        typedef typename ContourTree::Edge ContourTreeEdge;

        // make a map from new nodes to old
        StaticNodeMap<ContourTree, Node> _ct_node_to_simplified_node(contour_tree);

        for (NodeIterator<ContourTree> it(contour_tree); !it.done(); ++it) {
            // now add each node and edge of the contour tree to the simplified tree
            Node node = graph.addNode(
                            contour_tree.getID(it.node()),
                            contour_tree.getValue(it.node()));

            _ct_node_to_simplified_node[it.node()] = node;
        }

        for (EdgeIterator<ContourTree> it(contour_tree); !it.done(); ++it) {

            Edge edge = graph.addEdge(
                            _ct_node_to_simplified_node[contour_tree.u(it.edge())],
                            _ct_node_to_simplified_node[contour_tree.v(it.edge())]);

            _simplified_edge_to_ct_edge[edge] = it.edge();
        }
    }

    /// \brief Checks if the node has been reduced.
    bool isReduced(Node node)
    {
        return _is_reduced[node];
    }

    /// \brief The number of valid neighbors with higher scalar value.
    unsigned int validUpDegree(Node node)
    {
        unsigned int degree = 0;
        double node_value = _graph.getValue(node);

        for (UndirectedNeighborIterator<GraphType> it(_graph, node);
                !it.done(); ++it) {

            double neighbor_value = _graph.getValue(it.neighbor());
            if (neighbor_value > node_value) degree++;
        }

        return degree;
    }

    /// \brief The number of valid neighbors with lesser or equal scalar value.
    unsigned int validDownDegree(Node node)
    {
        unsigned int degree = 0;
        double node_value = _graph.getValue(node);

        for (UndirectedNeighborIterator<GraphType> it(_graph, node);
                !it.done(); ++it) {

            double neighbor_value = _graph.getValue(it.neighbor());
            if (neighbor_value <= node_value) degree++;
        }

        return degree;
    }

    /// \brief The number of valid neighbors.
    unsigned int validDegree(Node node)
    {
        return validUpDegree(node) + validDownDegree(node);
    }

    /// \brief Whether to save the edge for further reduction.
    /*!
     *  An edge is saved for reduction if the child node is the last valid
     *  up or down neighbor.
     */
    bool preserveForReduction(Node parent, Node child)
    {
        double child_value = _graph.getValue(child);
        double parent_value = _graph.getValue(parent);

        if (child_value <= parent_value) {
            return validDownDegree(parent) == 1;
        } else {
            return validUpDegree(parent) == 1;
        }
    }

    /// \brief Collapse the child into the parent.
    void collapse(Node parent, Node child)
    {
        _graph.insertNodeMembers(parent, _graph.getNodeMembers(child));
        _graph.removeNode(child);
    }

    /// \brief Reduces a node in the tree.
    void reduceNode(Node w, Node& u, Node& v)
    {
        // u <---> w <---> v

        _reduced_nodes.push_back(w);
        _is_reduced[w] = true;

        // we can assume that there are only two neighbors, else the node
        // should not have been reduced...
        UndirectedNeighborIterator<GraphType> it(_graph, w);

        // advance to the first valid neighbor
        while (isReduced(it.neighbor())) ++it;
        u = it.neighbor();
        Edge uw = it.edge();

        // advance to the next valid neighbor
        ++it;
        while (isReduced(it.neighbor())) ++it;
        v = it.neighbor();
        Edge wv = it.edge();

        // add the edge from u to v
        Edge uv = _graph.addEdge(u,v);

        Tab u_tab;
        Tab v_tab;

        // update the tabs, if necessary
        if (_graph.hasTabs(uw)) {
            u_tab = _graph.getTab(uw, u);
        } else {
            u_tab = _simplified_edge_to_ct_edge[uw];
        }

        if (_graph.hasTabs(wv)) {
            v_tab = _graph.getTab(wv, v);
        } else {
            v_tab = _simplified_edge_to_ct_edge[wv];
        }

        _graph.setTab(uv, u, u_tab, v, v_tab);
    }

    /// \brief Remove the reduced nodes in the graph.
    void removeReduced()
    {
        for (typename std::vector<Node>::iterator it = _reduced_nodes.begin();
                it != _reduced_nodes.end(); ++it) {

            _graph.removeNode(*it);

        }
        _reduced_nodes.clear();
    }

};

////////////////////////////////////////////////////////////////////////////
//
// PersistenceSimplifier
//
////////////////////////////////////////////////////////////////////////////

template <typename Node>
class ParentChildPriority : public Priority
{
    Node _parent;
    Node _child;

public:
    ParentChildPriority(Node parent, Node child, double priority)
        : _parent(parent), _child(child), Priority(priority) {}

    Node parent() const {
        return _parent;
    }
    Node child() const {
        return _child;
    }
};


/// \brief A simplification algorithm based on persistence.
class PersistenceSimplifier
{
    double _threshold;

    template <typename GraphType>
    class PersistencePriority : public
        ParentChildPriority<typename GraphType::Node>
    {
        typedef typename GraphType::Node Node;

        double _persistence;

    public:
        PersistencePriority(Node parent, Node child, double persistence)
            : ParentChildPriority<Node>(parent, child, 1/(persistence+1)),
              _persistence(persistence) {}

        static PersistencePriority<GraphType>
        compute(const GraphType& graph, Node parent, Node child)
        {
            double persistence = computePersistence(graph, parent, child);
            return PersistencePriority(parent, child, persistence);
        }

        double persistence() const {
            return _persistence;
        }
    };


public:
    PersistenceSimplifier(double threshold) : _threshold(threshold) {};

    /// \brief Compute the persistence of an edge in the tree.
    template <typename Tree>
    static double computePersistence(
        const Tree& tree,
        typename Tree::Node u,
        typename Tree::Node v)
    {
        return abs(tree.getValue(u) - tree.getValue(v));
    }

    /// \brief Simplify the contour tree, placing results in `graph`.
    /*!
     *  GraphType must support edge tabs. Specifically, the edge tab
     *  type must be ContourTree::Edge.
     */
    template <typename ContourTree, typename GraphType>
    void simplify(
        const ContourTree& contour_tree,
        GraphType& graph)
    {
        typedef typename GraphType::Node Node;
        typedef typename GraphType::Edge Edge;
        typedef PersistencePriority<GraphType> PersistencePriority;

        // first, we clear the graph
        graph.clearNodes();

        // we make a context, which initializes the graph
        SimplificationContext<ContourTree, GraphType> context(contour_tree, graph);

        // make a priority queue and populate it with leaf nodes
        std::priority_queue<PersistencePriority> simplify_queue;

        for (NodeIterator<GraphType> it(graph); !it.done(); ++it) {
            // if this is a leaf node
            if (graph.degree(it.node()) == 1) {
                // get the parent
                UndirectedNeighborIterator<GraphType> n_it(graph, it.node());

                // add to the queue
                PersistencePriority priority =
                    PersistencePriority::compute(graph, n_it.neighbor(), it.node());

                simplify_queue.push(priority);
            }
        }

        while (simplify_queue.size() > 0) {
            // get the top of the queue
            Node parent         = simplify_queue.top().parent();
            Node child          = simplify_queue.top().child();
            double persistence  = simplify_queue.top().persistence();
            simplify_queue.pop();

            if (persistence > _threshold) break;

            if (context.isReduced(parent)) continue;

            if (context.preserveForReduction(parent, child)) continue;

            context.collapse(parent, child);

            // check to see if we should reduce the node
            if (context.validDegree(parent) == 2) {
                // make some nodes to store the neighbors of the parent
                Node u,v;

                // perform the reduction
                context.reduceNode(parent, u, v);

                if (context.validDegree(u) == 1) {
                    double persistence = computePersistence(graph, u, v);
                    simplify_queue.push(PersistencePriority(v, u, persistence));
                }

                if (context.validDegree(v) == 1) {
                    double persistence = computePersistence(graph, u, v);
                    simplify_queue.push(PersistencePriority(u, v, persistence));
                }
            }
        }

        // remove the reduced nodes, and we are done
        context.removeReduced();
    }
};


////////////////////////////////////////////////////////////////////////////
//
// SimplifiedContourTree
//
////////////////////////////////////////////////////////////////////////////

template <typename GraphType, typename ContourTree>
class SimplifiedContourTreeBase :
    public
    ContourTreeMixin <GraphType,
    BaseGraphMixin <GraphType > >
{
    typedef
    ContourTreeMixin <GraphType,
                     BaseGraphMixin <GraphType > >
                     Mixin;

    const ContourTree& _contour_tree;
    boost::shared_ptr<GraphType> _graph;

protected:
    SimplifiedContourTreeBase(
        const ContourTree& contour_tree,
        boost::shared_ptr<GraphType> graph)
        : _graph(graph), _contour_tree(contour_tree),
          Mixin(*graph) { }

public:
    typedef typename GraphType::Node Node;
    typedef typename GraphType::Edge Edge;
    typedef typename GraphType::Members Members;
    typedef typename GraphType::Tab Tab;

    /// \brief Retrieve a tab.
    Tab getTab(Edge edge, Node node)
    {
        return _graph->getTab(edge, node);
    }

    /// \brief Checks if the edge has tabs.
    bool hasTabs(Edge edge)
    {
        return _graph->hasTabs(edge);
    }
};


/// \brief The simplified version of a contour tree.
/// \ingroup simplified_contour_tree
template <typename ContourTree>
class SimplifiedContourTree :
    public
    SimplifiedContourTreeBase
    <
    UndirectedTabbedScalarMemberIDGraph<
    typename ContourTree::Edge>,
    ContourTree
    >
{
    typedef UndirectedTabbedScalarMemberIDGraph<typename ContourTree::Edge> GraphType;
    typedef SimplifiedContourTreeBase<GraphType, ContourTree> Base;

    SimplifiedContourTree(
        const ContourTree& contour_tree,
        boost::shared_ptr<GraphType> graph)
        : Base(contour_tree, graph) {}

public:

    typedef typename Base::Node Node;
    typedef typename Base::Edge Edge;
    typedef typename Base::Members Members;

    /// \brief Simplify a contour tree.
    template <typename SimplificationAlgorithm>
    static SimplifiedContourTree
    simplify(
        const ContourTree& contour_tree,
        SimplificationAlgorithm simplifier)
    {
        // make a graph to store the result in
        boost::shared_ptr<GraphType> graph =
            boost::shared_ptr<GraphType>(new GraphType());

        // now compute the simplification
        simplifier.simplify(contour_tree, *graph);

        // make a new simplified contour tree and return it
        return SimplifiedContourTree(contour_tree, graph);
    }

};

}


#endif
