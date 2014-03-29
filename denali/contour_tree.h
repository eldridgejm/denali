#ifndef DENALI_CONTOUR_TREE_H
#define DENALI_CONTOUR_TREE_H

#include <boost/shared_ptr.hpp>
#include <map>
#include <queue>
#include <set>

#include <denali/graph_iterators.h>
#include <denali/graph_maps.h>
#include <denali/graph_mixins.h>
#include <denali/graph_structures.h>

namespace denali {

////////////////////////////////////////////////////////////////////////////
//
// ScalarSimplicialComplex
//
////////////////////////////////////////////////////////////////////////////

/// \brief An implementation of ScalarSimplicialComplex
/*!
 *  GraphType must conform to:
 *      - concepts::WritableReadableUndirectedGraph
 *      - concepts::NodeObservable
 */
template <typename GraphType>
class ScalarSimplicialComplexBase :
    public
    ReadableUndirectedGraphMixin <GraphType,
    BaseGraphMixin <GraphType> >
{
    typedef
    ReadableUndirectedGraphMixin <GraphType,
                                 BaseGraphMixin <GraphType> >
                                 Mixin;

public:
    typedef typename Mixin::Node Node;
    typedef typename Mixin::Edge Edge;

private:
    GraphType _graph;
    ObservingNodeMap<GraphType, double> _node_to_value;
    ObservingNodeMap<GraphType, unsigned int> _node_to_id;

    // keep a copy of the nodes locally
    std::vector<Node> _nodes;


public:
    ScalarSimplicialComplexBase()
        : Mixin(_graph), _node_to_value(_graph), _node_to_id(_graph)  {};

    /// \brief Add a node to the complex, with associated scalar value.
    Node addNode(double value)
    {
        // add the node to the graph
        Node node = _graph.addNode();

        // add the node to the map
        _node_to_value[node] = value;
        _node_to_id[node] = _nodes.size();

        _nodes.push_back(node);

        return node;
    }

    /// \brief Add an edge to the complex.
    /// \pre The nodes must be in the complex.
    Edge addEdge(Node u, Node v)
    {
        return _graph.addEdge(u,v);
    }

    /// \brief Retrieve the scalar value of a node.
    double getValue(Node node) const
    {
        return _node_to_value[node];
    }

    Node getNode(unsigned int index) const
    {
        return _nodes[index];
    }

    unsigned int getID(Node node) const
    {
        return _node_to_id[node];
    }

};


/// \brief A scalar simplicial complex.
/// \ingroup contour_tree
/*!
 *  Provides an implementation of concepts::ScalarSimplicialComplex
 */
class ScalarSimplicialComplex :
    public
    ScalarSimplicialComplexBase<UndirectedGraph>
{
    typedef ScalarSimplicialComplexBase<UndirectedGraph> Base;

public:
    typedef Base::Node Node;
    typedef Base::Edge Edge;
};


/// \brief Adapts a scalar simplicial complex so that it
///      may be used by a concepts::RandomAccessComparisonFunctor
/// \ingroup contour_tree
template <typename ScalarSimplicialComplex>
class ScalarSimplicialComplexRandomAccessAdapter
{
    const ScalarSimplicialComplex& _plex;

public:
    ScalarSimplicialComplexRandomAccessAdapter(const ScalarSimplicialComplex& plex)
        : _plex(plex) { }

    const double operator[](unsigned int index) const
    {
        return _plex.getValue(_plex.getNode(index));
    }

    size_t size() const {
        return _plex.numberOfNodes();
    }
};

////////////////////////////////////////////////////////////////////////////
//
// UndirectedScalarMemberIDGraph
//
////////////////////////////////////////////////////////////////////////////

/// \brief An implementation of concepts::UndirectedScalarMemberIDGraph
/*!
 *  Requires that GraphType meets
 *   - concepts::WritableReadableUndirectedGraph
 *   - concepts::NodeObservable
 *   - concepts::EdgeObservable
 *
 *  Conforms to
 *   - concepts::UndirectedScalarMemberIDGraph
 *   - concepts::NodeObservable
 *   - concepts::EdgeObservable
 */
template <typename GraphType>
class UndirectedScalarMemberIDGraphBase :
    public
    EdgeObservableMixin < GraphType,
    NodeObservableMixin < GraphType,
    ReadableUndirectedGraphMixin <GraphType,
    BaseGraphMixin <GraphType> > > >
{
public:

    class Member
    {
        unsigned int _id;
        double _value;
    public:
        Member(unsigned int id, double value) :
            _id(id), _value(value) {}

        bool operator==(const Member& rhs) const {
            return _id == rhs._id;
        }

        bool operator<(const Member& rhs) const {
            return _id < rhs._id;
        }

        unsigned int getID() const {
            return _id;
        }

        double getValue() const {
            return _value;
        }
    };

    typedef std::set<Member> Members;

private:
    typedef
    EdgeObservableMixin < GraphType,
                        NodeObservableMixin < GraphType,
                        ReadableUndirectedGraphMixin <GraphType,
                        BaseGraphMixin <GraphType> > > >
                        Mixin;

    GraphType _graph;

    ObservingNodeMap<GraphType, unsigned int> _node_to_id;
    ObservingNodeMap<GraphType, double> _node_to_value;
    typedef std::map<unsigned int, typename GraphType::Node> IDToNode;
    IDToNode _id_to_node;

    ObservingNodeMap<GraphType, Members> _node_to_members;
    ObservingEdgeMap<GraphType, Members> _edge_to_members;

    // the total number of nodes + number of members
    size_t _nodes_plus_members;

public:

    typedef typename GraphType::Node Node;
    typedef typename GraphType::Edge Edge;

    UndirectedScalarMemberIDGraphBase()
        : Mixin(_graph), _node_to_id(_graph), _node_to_value(_graph), _node_to_members(_graph),
          _edge_to_members(_graph), _nodes_plus_members(0)
    {
    }

    /// \brief Add a node to the graph.
    /*!
     *  \param  id      The id of the node.
     *  \param  value   The scalar value of the node.
     */
    Node addNode(unsigned int id, double value)
    {
        Node node = _graph.addNode();
        _node_to_id[node] = id;
        _node_to_value[node] = value;
        _id_to_node[id] = node;

        _nodes_plus_members++;

        // the convention: a node is in its own member set
        _node_to_members[node].clear();
        _node_to_members[node].insert(Member(id, value));
        return node;
    }

    /// \brief Add an edge to the graph.
    Edge addEdge(Node u, Node v)
    {
        Edge edge = _graph.addEdge(u,v);
        _edge_to_members[edge].clear();
        return edge;
    }

    /// \brief Remove the node.
    void removeNode(Node node)
    {
        _nodes_plus_members -= _node_to_members[node].size();

        _id_to_node.erase(_node_to_id[node]);
        _graph.removeNode(node);
    }

    /// \brief Remove the edge.
    void removeEdge(Edge edge)
    {
        _nodes_plus_members -= _edge_to_members[edge].size();
        _graph.removeEdge(edge);
    }

    /// \brief Insert a member into the node's member set.
    void insertNodeMember(Node node, Member member)
    {
        _node_to_members[node].insert(member);
        _nodes_plus_members++;
    }

    /// \brief Insert a member into the edge's member set.
    void insertEdgeMember(Edge edge, Member member)
    {
        _edge_to_members[edge].insert(member);
        _nodes_plus_members++;
    }

    /// \brief Insert members into node member set.
    void insertNodeMembers(Node node, const Members& members)
    {
        for (typename Members::const_iterator it = members.begin();
                it != members.end();
                ++it) {
            _node_to_members[node].insert(*it);
            _nodes_plus_members++;
        }
    }

    /// \brief Insert members into edge member set.
    void insertEdgeMembers(Edge edge, const Members& members)
    {
        for (typename Members::const_iterator it = members.begin();
                it != members.end();
                ++it) {
            _edge_to_members[edge].insert(*it);
            _nodes_plus_members++;
        }
    }

    /// \brief Get a node's scalar value
    double getValue(Node node) const
    {
        return _node_to_value[node];
    }

    /// \brief Get a node's ID
    unsigned int getID(Node node) const
    {
        return _node_to_id[node];
    }

    /// \brief Retrieve the members of the node
    const Members& getNodeMembers(Node node) const
    {
        return _node_to_members[node];
    }

    /// \brief Retrieve a node by its ID.
    Node getNode(unsigned int id)
    {
        typename IDToNode::const_iterator it = _id_to_node.find(id);

        if (it == _id_to_node.end()) {
            return _graph.getInvalidNode();
        } else {
            return it->second;
        }
    }

    /// \brief Retrieve the members of the edge.
    const Members& getEdgeMembers(Edge edge) const
    {
        return _edge_to_members[edge];
    }

    /// \brief Clear the nodes of the graph
    void clear() {
        _nodes_plus_members = 0;
        return _graph.clear();
    }

    size_t numberNodesPlusMembers() const {
        return _nodes_plus_members;
    }

};


/// \brief A graph with scalar values, members, and IDs.
/// \ingroup contour_tree
/*!
 *  Conforms to concepts::UndirectedScalarMemberIDGraph
 */
class UndirectedScalarMemberIDGraph :
    public
    UndirectedScalarMemberIDGraphBase <UndirectedGraph>
{
    typedef UndirectedScalarMemberIDGraphBase <UndirectedGraph> Base;

public:
    typedef Base::Node Node;
    typedef Base::Edge Edge;
    typedef Base::Members Members;

};

////////////////////////////////////////////////////////////////////////////
//
// ContourTree
//
////////////////////////////////////////////////////////////////////////////

/// \brief Forwards the contour tree interface.
template <typename GraphType, typename Super>
class ContourTreeMixin :
    public
    ReadableUndirectedGraphMixin<GraphType, Super>
{
    typedef ReadableUndirectedGraphMixin<GraphType, Super> Base;

    GraphType& _graph;

public:

    typedef typename Base::Node Node;
    typedef typename Base::Edge Edge;
    typedef typename GraphType::Members Members;
    typedef typename GraphType::Member Member;

    ContourTreeMixin(GraphType& graph)
        : Base(graph), _graph(graph) {}

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

    /// \brief Retrieve a node by its ID
    Node getNode(unsigned int id) const
    {
        return _graph.getNode(id);
    }

    /// \brief Retrieve the members of the edge.
    const Members& getEdgeMembers(Edge edge) const
    {
        return _graph.getEdgeMembers(edge);
    }

    size_t numberNodesPlusMembers() const {
        return _graph.numberNodesPlusMembers();
    }

};

template <typename UndirectedScalarMemberIDGraph>
class ContourTreeBase :
    public
    ContourTreeMixin <UndirectedScalarMemberIDGraph,
    BaseGraphMixin <UndirectedScalarMemberIDGraph> >
{
    typedef UndirectedScalarMemberIDGraph Graph;

    typedef
    ContourTreeMixin <UndirectedScalarMemberIDGraph,
                     BaseGraphMixin <UndirectedScalarMemberIDGraph> >
                     Mixin;

    boost::shared_ptr<Graph> _graph;

protected:
    ContourTreeBase(boost::shared_ptr<Graph> graph)
        : Mixin(*graph), _graph(graph) { }

public:

    typedef typename Graph::Node Node;
    typedef typename Graph::Edge Edge;
    typedef typename Graph::Members Members;

};


/// \brief A contour tree computed from data.
/// \ingroup contour_tree
class ContourTree : public ContourTreeBase<UndirectedScalarMemberIDGraph>
{

    ContourTree(boost::shared_ptr<UndirectedScalarMemberIDGraph> graph)
        : ContourTreeBase<UndirectedScalarMemberIDGraph>(graph) {}

public:

    typedef UndirectedScalarMemberIDGraph Graph;

    template <typename ScalarSimplicialComplex, typename ContourTreeAlgorithm>
    static ContourTree
    compute(
        const ScalarSimplicialComplex& simplicial_complex,
        ContourTreeAlgorithm& algorithm)
    {
        // allocate a new graph representation
        boost::shared_ptr<Graph> graph = boost::shared_ptr<Graph>(new Graph);

        // compute the contour tree
        algorithm.compute(simplicial_complex, *graph);

        // return the result as a contour tree
        return ContourTree(graph);
    }

    /// \brief Load a contour tree from a UndirectedScalarMemberIDGraph.
    /*!
     *  The utility of having this function as opposed to exposing the constructor
     *  publically is that we may choose to perform checks in this function that
     *  the constructor can avoid for reasons of efficiency.
     */
    static ContourTree
    fromPrecomputed(boost::shared_ptr<Graph>& graph)
    {
        boost::shared_ptr<Graph> old_graph = graph;
        graph = boost::shared_ptr<Graph>(new Graph);
        return ContourTree(old_graph);
    }

};

////////////////////////////////////////////////////////////////////////////////
//
// ContourTree helpers
//
////////////////////////////////////////////////////////////////////////////////

namespace {

template <typename T>
struct LesserComparator
{
    bool operator()(const T& x, const T& y) const
    {
        return x < y;
    }
};

template <typename T>
struct GreaterComparator
{
    bool operator()(const T& x, const T& y) const
    {
        return x > y;
    }
};

template <typename GraphType, typename Comparator>
typename GraphType::Node findLeafByComparator(
    const GraphType& tree,
    const Comparator& comparator)
{
    typedef typename GraphType::Node Node;

    bool found = false;

    Node best_node = tree.getFirstNode();
    double best_value = tree.getValue(best_node);

    for (NodeIterator<GraphType> it(tree); !it.done(); ++it) {
        // get the value of this node
        double value = tree.getValue(it.node());

        // check to see if it is a leaf
        bool is_leaf = tree.degree(it.node()) == 1;

        if (is_leaf && (!found || comparator(value, best_value))) {
            best_node = it.node();
            best_value = tree.getValue(it.node());
            found = true;
        }
    }

    return best_node;
}
}


/// \brief Find the leaf with the minimum scalar value.
/// \ingroup contour_tree
template <typename GraphType>
typename GraphType::Node findMinLeaf(
    const GraphType& tree)
{
    LesserComparator<double> comparator;
    return findLeafByComparator(tree, comparator);
}


/// \brief Find the leaf with the maximum scalar value.
/// \ingroup contour_tree
template <typename GraphType>
typename GraphType::Node findMaxLeaf(
    const GraphType& tree)
{
    GreaterComparator<double> comparator;
    return findLeafByComparator(tree, comparator);
}


/// \brief Find the node with the minimum scalar value.
template <typename GraphType>
typename GraphType::Node findMinNode(
    const GraphType& tree)
{
    typename GraphType::Node min_node;
    double min_value = 0.;
    bool found = false;

    for (NodeIterator<GraphType> it(tree); !it.done(); ++it)
    {
        double value = tree.getValue(it.node());

        if (!found || value < min_value)
        {
            min_value = value;
            min_node = it.node();
            found = true;
        }
    }

    return min_node;
}


/// \brief Find the node with the minimum scalar value.
template <typename GraphType>
typename GraphType::Node findMaxNode(
    const GraphType& tree)
{
    typename GraphType::Node max_node;
    double max_value = 0.;
    bool found = false;

    for (NodeIterator<GraphType> it(tree); !it.done(); ++it)
    {
        double value = tree.getValue(it.node());

        if (!found || value > max_value)
        {
            max_value = value;
            max_node = it.node();
            found = true;
        }
    }

    return max_node;
}

////////////////////////////////////////////////////////////////////////////////
//
// DisjointSetForest
//
////////////////////////////////////////////////////////////////////////////////

/// \brief A disjoint set forest class.
/// \ingroup contour_tree
template <typename TotalOrder>
class DisjointSetForest
{
    const TotalOrder& order;
    std::vector<unsigned int> parent;
    std::vector<unsigned int> rank;
    std::vector<unsigned int> max_element;
    std::vector<unsigned int> min_element;

public:

    DisjointSetForest(const TotalOrder& order)
        : order(order),
          parent(order.size()),
          rank(order.size()),
          max_element(order.size()),
          min_element(order.size())
    {
        for (size_t i=0; i<order.size(); ++i) {
            parent[i] = i;
            max_element[i] = i;
            min_element[i] = i;
            rank[i] = 0;
        }
    }


    unsigned int
    findSet(unsigned int x)
    {
        if (this->parent[x] != x) {
            this->parent[x] = findSet(this->parent[x]);
        }

        return this->parent[x];
    }

    unsigned int
    findMax(unsigned int x)
    {
        return this->max_element[this->findSet(x)];
    }


    unsigned int
    findMin(unsigned int x)
    {
        return this->min_element[this->findSet(x)];
    }


    void
    setUnion(unsigned int x, unsigned int y)
    {
        this->link(this->findSet(x), this->findSet(y));
    }


private:
    void
    link(unsigned int x, unsigned int y)
    {

        unsigned int max_x_id = this->order.elementToPosition(this->max_element[x]);
        unsigned int min_x_id = this->order.elementToPosition(this->min_element[x]);
        unsigned int max_y_id = this->order.elementToPosition(this->max_element[y]);
        unsigned int min_y_id = this->order.elementToPosition(this->min_element[y]);

        if (this->rank[x] > this->rank[y]) {
            this->parent[y] = x;

            if (max_x_id < max_y_id) {
                this->max_element[x] = this->max_element[y];
            }

            if (min_x_id > min_y_id) {
                this->min_element[x] = this->min_element[y];
            }

        } else {
            this->parent[x] = y;

            if (this->rank[x] == this->rank[y]) {
                this->rank[y] += 1;
            }

            if (max_y_id < max_x_id) {
                this->max_element[y] = this->max_element[x];
            }

            if (min_y_id > min_x_id) {
                this->min_element[y] = this->min_element[x];
            }
        }
    }
};

////////////////////////////////////////////////////////////////////////////
//
// TotalOrder
//
////////////////////////////////////////////////////////////////////////////

/// \brief A total ordering of values.
/// \ingroup contour_tree
class TotalOrder
{
public:
    typedef std::vector<unsigned int> ElementToPosition;
    typedef std::vector<unsigned int> PositionToElement;

private:
    ElementToPosition _element_to_position;
    PositionToElement _position_to_element;

    TotalOrder(size_t n)
        : _element_to_position(n), _position_to_element(n) { }

public:

    unsigned int elementToPosition(unsigned int element) const
    {
        return _element_to_position[element];
    }

    unsigned int positionToElement(unsigned int position) const
    {
        return _position_to_element[position];
    }

    size_t size() const {
        return _element_to_position.size();
    }

    /// Compute a total ordering.
    /*!
     *  Values must support random access by operator[] and have a size method.
     *
     *  ComparisonFunctor must provide `bool operator()(unsigned int x,
     *  unsigned int y)`.
     */
    template <typename Values, typename ComparisonFunctor>
    static TotalOrder compute(
        const Values& values,
        ComparisonFunctor& cmp)
    {
        // create a new total order object
        TotalOrder ordering(values.size());

        ElementToPosition& index_to_order = ordering._element_to_position;
        PositionToElement& order_to_index = ordering._position_to_element;

        // intialize _position_to_element to contain the range of indices
        unsigned int index = 0;
        for (PositionToElement::iterator it = order_to_index.begin();
                it != order_to_index.end();
                ++it) {
            *it = index++;
        }

        // now sort the ordering
        std::sort(order_to_index.begin(), order_to_index.end(), cmp);

        unsigned int order = 0;
        for (PositionToElement::iterator it = order_to_index.begin();
                it != order_to_index.end();
                ++it) {
            index_to_order[*it] = order++;
        }

        return ordering;
    }
};

////////////////////////////////////////////////////////////////////////////
//
// ValueSorter
//
////////////////////////////////////////////////////////////////////////////

/// \brief A comparison functor based on a list of values.
/// \ingroup contour_tree
/*!
 *  Calling the functor with u and v returns true if values[u] < values[v].
 */
template <typename Values>
class RandomAccessValueSorter
{
    const Values& values;

public:
    RandomAccessValueSorter(const Values& values) : values(values) { }
    bool operator()(unsigned int u, unsigned int v)
    {
        return values[u] < values[v];
    }
};

////////////////////////////////////////////////////////////////////////////
//
// CarrsAlgorithm
//
////////////////////////////////////////////////////////////////////////////

/// \brief A directed graph where each node has an ID.
template <typename GraphType>
class DirectedIDGraph :
    public
    ReadableDirectedGraphMixin <GraphType,
    BaseGraphMixin <GraphType> >
{
    typedef
    ReadableDirectedGraphMixin <GraphType,
                               BaseGraphMixin <GraphType> >
                               Mixin;

    GraphType _graph;

    ObservingNodeMap<GraphType, unsigned int> _node_to_id;
    std::vector<typename GraphType::Node> _id_to_node;

public:

    typedef typename GraphType::Node Node;
    typedef typename GraphType::Arc Arc;

    DirectedIDGraph() : Mixin(_graph), _node_to_id(_graph) { }
    DirectedIDGraph(size_t n) : Mixin(_graph), _node_to_id(_graph)
    {
        for (size_t i=0; i<n; ++i) {
            addNode();
        }
    }

    Node addNode()
    {
        Node node = _graph.addNode();
        _node_to_id[node] = _id_to_node.size();
        _id_to_node.push_back(node);
        return node;
    }

    Arc addArc(Node u, Node v)
    {
        return _graph.addArc(u,v);
    }

    void removeNode(Node node)
    {
        _graph.removeNode(node);
    }

    void removeArc(Arc arc)
    {
        _graph.removeArc(arc);
    }

    Node getNode(unsigned int id)
    {
        return _id_to_node[id];
    }

    unsigned int getID(Node node)
    {
        return _node_to_id[node];
    }

};


/// \brief An implementation of Carr's algorithm for computing contour trees.
/// \ingroup contour_tree
/*!
 *  Conforms to concepts::ContourTreeAlgorithm
 */
class CarrsAlgorithm
{
public:

    typedef DirectedIDGraph<DirectedGraph> JoinSplitTree;

    /// \brief Compute a contour tree from a simplicial complex.
    template <typename ScalarSimplicialComplex, typename UndirectedScalarMemberIDGraph>
    static void compute(
        const ScalarSimplicialComplex& simplicial_complex,
        UndirectedScalarMemberIDGraph& graph)
    {

        // clear the output
        graph.clear();

        // we need to establish a total order on the nodes of the simplicial
        // complex. First, we'll adapt the complex so that we can treat it
        // like a vector:
        typedef ScalarSimplicialComplexRandomAccessAdapter
        <ScalarSimplicialComplex>
        SortAdapter;

        SortAdapter adapter(simplicial_complex);

        // now create a node sorter
        RandomAccessValueSorter<SortAdapter> sorter(adapter);

        // and compute the total order
        TotalOrder order = TotalOrder::compute(adapter, sorter);

        JoinSplitTree join_tree = computeJoinTree(simplicial_complex, order);
        JoinSplitTree split_tree = computeSplitTree(simplicial_complex, order);

        computeMergeTree(simplicial_complex, join_tree, split_tree, graph);
        removeRegularNodes(graph, order);
    }


    /// \brief Compute the directed join tree.
    template <typename ScalarSimplicialComplex, typename TotalOrder>
    static JoinSplitTree computeJoinTree(
        const ScalarSimplicialComplex& plex,
        const TotalOrder& total_order)
    {
        // create a join tree
        JoinSplitTree join_tree(total_order.size());

        // create a disjoint set forest
        DisjointSetForest<TotalOrder> forest(total_order);

        // take the nodes in order
        for (size_t i=0; i<total_order.size(); ++i) {
            // get the id of this node in the order
            unsigned int vi = total_order.positionToElement(i);

            // iterate through the neighbors of the node in the complex
            typedef UndirectedNeighborIterator<ScalarSimplicialComplex> NeighborIt;

            for (NeighborIt neighbor_it(plex, plex.getNode(vi));
                    !neighbor_it.done();
                    ++neighbor_it) {

                unsigned int vj = plex.getID(neighbor_it.neighbor());

                if (total_order.elementToPosition(vj) < total_order.elementToPosition(vi)) {
                    if (forest.findSet(vi) != forest.findSet(vj)) {

                        unsigned int vk = forest.findMax(vj);
                        join_tree.addArc(
                            join_tree.getNode(vi),
                            join_tree.getNode(vk));
                        forest.setUnion(vi,vj);
                    }
                }
            }
        }

        return join_tree;
    };


    /// \brief Compute the directed split tree.
    template <typename ScalarSimplicialComplex, typename TotalOrder>
    static JoinSplitTree computeSplitTree(
        const ScalarSimplicialComplex& plex,
        const TotalOrder& total_order)
    {
        // create a split tree
        JoinSplitTree split_tree(total_order.size());

        // create a disjoint set forest
        DisjointSetForest<TotalOrder> forest(total_order);

        // take the nodes in reverse order
        for (int i=total_order.size()-1; i>=0; --i) {
            // get the id of this node in the order
            unsigned int vi = total_order.positionToElement(i);

            // iterate through the neighbors of the node in the complex
            typedef UndirectedNeighborIterator<ScalarSimplicialComplex> NeighborIt;

            for (NeighborIt neighbor_it(plex, plex.getNode(vi));
                    !neighbor_it.done();
                    ++neighbor_it) {

                unsigned int vj = plex.getID(neighbor_it.neighbor());

                if (total_order.elementToPosition(vj) > total_order.elementToPosition(vi)) {
                    if (forest.findSet(vi) != forest.findSet(vj)) {

                        unsigned int vk = forest.findMin(vj);
                        split_tree.addArc(
                            split_tree.getNode(vi),
                            split_tree.getNode(vk));
                        forest.setUnion(vi,vj);
                    }
                }
            }
        }

        return split_tree;
    };


    /// \brief Reduce a node in the join or split tree.
    /*!
     *  Reducing a node entails removing it from the tree and connecting
     *  its parent to its children.
     */
    static void reduceNode(
        JoinSplitTree& tree,
        JoinSplitTree::Node node)
    {
        // if the node has no parents, we just remove it
        if (tree.inDegree(node) == 0) {
            tree.removeNode(node);
        } else {
            // otherwise, we connect its parent to its child
            // (THERE CAN BE ONLY ONE!)
            JoinSplitTree::Node parent = tree.source(tree.getFirstInArc(node));
            JoinSplitTree::Node child = tree.target(tree.getFirstOutArc(node));
            tree.addArc(parent, child);

            tree.removeNode(node);
        }
    }


    /// \brief Compute the merge tree.
    template <typename ScalarSimplicialComplex, typename UndirectedScalarMemberIDGraph>
    static void computeMergeTree(
        ScalarSimplicialComplex& plex,
        JoinSplitTree& join_tree,
        JoinSplitTree& split_tree,
        UndirectedScalarMemberIDGraph& merge_tree)
    {
        typedef UndirectedScalarMemberIDGraph MergeTree;

        // add all of the nodes from the plex to the merge tree
        // simultaneously, add nodes to the merge queue if they have a total
        // of one child in the join and split tree
        std::vector<typename MergeTree::Node> merge_tree_nodes;
        std::queue<unsigned int> merge_queue;

        for (size_t i=0; i<plex.numberOfNodes(); ++i) {
            // add to the merge tree
            typename MergeTree::Node node =
                merge_tree.addNode(i, plex.getValue(plex.getNode(i)));
            merge_tree_nodes.push_back(node);

            // see if we should add this to the merge queue
            if (join_tree.outDegree(join_tree.getNode(i)) +
                    split_tree.outDegree(split_tree.getNode(i)) == 1) {
                merge_queue.push(i);
            }
        }

        // now merge the trees
        while (merge_queue.size() > 1) {
            unsigned int vi = merge_queue.front();
            merge_queue.pop();

            if (join_tree.outDegree(join_tree.getNode(vi)) == 0) {
                // get the parent in the join tree
                unsigned int vk =
                    join_tree.getID(
                        join_tree.source(
                            join_tree.getFirstInArc(
                                join_tree.getNode(vi))));

                // add the edge to the merge tree
                merge_tree.addEdge(merge_tree_nodes[vi], merge_tree_nodes[vk]);

                // remove the node from the join tree
                join_tree.removeNode(join_tree.getNode(vi));

                // reduce the node in the split tree
                reduceNode(split_tree, split_tree.getNode(vi));

                // check to see if we have a new merge candidate
                if (join_tree.outDegree(join_tree.getNode(vk)) +
                        split_tree.outDegree(split_tree.getNode(vk)) == 1) {
                    merge_queue.push(vk);
                }

            } else {
                // get the parent in the join tree
                unsigned int vk =
                    split_tree.getID(
                        split_tree.source(
                            split_tree.getFirstInArc(
                                split_tree.getNode(vi))));

                // add the edge to the merge tree
                merge_tree.addEdge(merge_tree_nodes[vi], merge_tree_nodes[vk]);

                // remove the node from the split tree
                split_tree.removeNode(split_tree.getNode(vi));

                // reduce the node in the join tree
                reduceNode(join_tree, join_tree.getNode(vi));

                // check to see if we have a new merge candidate
                if (split_tree.outDegree(split_tree.getNode(vk)) +
                        join_tree.outDegree(join_tree.getNode(vk)) == 1) {
                    merge_queue.push(vk);
                }
            }
        }
    }


    /// \brief Determines if a node is regular.
    /*!
     *  A node is regular if it has exactly two neighbors: one which
     *  comes after it in the order, and one that comes before it.
     */
    template <typename UndirectedScalarMemberIDGraph, typename TotalOrder>
    static bool isRegularNode(
        const UndirectedScalarMemberIDGraph& tree,
        const typename UndirectedScalarMemberIDGraph::Node node,
        const TotalOrder& order)
    {
        typedef UndirectedScalarMemberIDGraph Tree;
        typedef typename Tree::Node Node;
        typedef typename Tree::Edge Edge;

        if (tree.degree(node) != 2) {
            return false;
        }

        // the node has exactly two neighbors. Get them.
        Edge first_edge = tree.getFirstNeighborEdge(node);
        Edge second_edge = tree.getNextNeighborEdge(node, first_edge);

        Node first_node = tree.opposite(node, first_edge);
        Node second_node = tree.opposite(node, second_edge);

        unsigned int vi = tree.getID(first_node);
        unsigned int vj = tree.getID(node);
        unsigned int vk = tree.getID(second_node);

        unsigned int pi = order.elementToPosition(vi);
        unsigned int pj = order.elementToPosition(vj);
        unsigned int pk = order.elementToPosition(vk);

        if ((pi < pj) && (pj < pk)) {
            return true;
        } else if ((pi > pj) && (pj > pk)) {
            return true;
        } else {
            return false;
        }
    }


    /// \brief Remove the regular nodes from the merge tree.
    template <typename UndirectedScalarMemberIDGraph, typename TotalOrder>
    static void removeRegularNodes(
        UndirectedScalarMemberIDGraph& tree,
        const TotalOrder& order)
    {
        typedef UndirectedScalarMemberIDGraph Tree;
        typedef typename Tree::Node Node;
        typedef typename Tree::Edge Edge;
        typedef typename UndirectedScalarMemberIDGraph::Member Member;

        // make a list of regular nodes
        std::queue<Node> regular_nodes;
        for (NodeIterator<Tree> it(tree); !it.done(); ++it) {
            if (isRegularNode(tree, it.node(), order)) {
                regular_nodes.push(it.node());
            }
        }

        // now, remove them
        while (regular_nodes.size() > 0) 
        {
            Node v = regular_nodes.front();
            regular_nodes.pop();

            // get the two edges incident on this node
            Edge edge_uv = tree.getFirstNeighborEdge(v);
            Edge edge_vw = tree.getNextNeighborEdge(v, edge_uv);

            // get the neighboring nodes
            Node u = tree.opposite(v, edge_uv);
            Node w = tree.opposite(v, edge_vw);

            // connect the neighbors
            Edge edge_uw = tree.addEdge(u,w);

            // add the to-be-deleted node to the new edge's members
            Member member(tree.getID(v), tree.getValue(v));
            tree.insertEdgeMember(edge_uw, member);

            // union the members from the old edges
            tree.insertEdgeMembers(edge_uw, tree.getEdgeMembers(edge_uv));
            tree.insertEdgeMembers(edge_uw, tree.getEdgeMembers(edge_vw));

            // remove the middle node
            tree.removeNode(v);
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
//
// WeightMap
//
////////////////////////////////////////////////////////////////////////////////

typedef std::map<unsigned int, double> WeightMap;


}

#endif
