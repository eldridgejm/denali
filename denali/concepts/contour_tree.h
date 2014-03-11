#ifndef DENALI_CONCEPTS_CONTOUR_TREE_H
#define DENALI_CONCEPTS_CONTOUR_TREE_H

#include <denali/concepts/graph_attributes.h>

#include <set>
#include <vector>

namespace denali {
namespace concepts {

/// \brief A simplicial complex with a scalar value at each vertex.
/// \ingroup concepts_contour_tree
/*!
 *  A ScalarSimplicialComplex represents the 1-skeleton of a simplicial
 *  complex with a scalar value at each vertex. This concept allows the
 *  client to add vertices and edges to the complex.
 *
 *  Nodes are assigned indices according to the order they are placed
 *  into the simplicial complex. The first node entered has index 0.
 *
 *  To support vector-like operations on the simplicial complex, a
 *  size() and random-access operator[]() are provided.
 *  operator[](i) retrieves the scalar value of the ith node.
 */
class ScalarSimplicialComplex : public concepts::ReadableUndirectedGraph
{
public:

    typedef concepts::Node Node;
    typedef concepts::Edge Edge;

    /// \brief Add a node to the complex, with associated scalar value.
    Node addNode(double value) { return Node(); }

    /// \brief Add an edge to the complex.
    /// \pre The nodes must be in the complex.
    Edge addEdge(Node u, Node v) { return Edge(); }

    /// \brief Retrieve the scalar value of a node.
    double getValue(Node node) const {
        return 0.0;
    }

    /// \brief Retrieve the node associated with the index.
    Node getNode(unsigned int index) const {
        return Node();
    }

    /// \brief Get the ID of a node.
    unsigned int getID(Node node) const {
        return 0;
    }


    template <typename _ScalarSimplicialComplex>
    struct Constraints
    {
        typedef typename _ScalarSimplicialComplex::Node _Node;
        typedef typename _ScalarSimplicialComplex::Edge _Edge;

        void constraints()
        {
            checkConcept
            <
            ReadableUndirectedGraph,
            _ScalarSimplicialComplex
            > ();

            _Node node = _plex.addNode(42.42);
            _Edge edge = _plex.addEdge(_Node(), _Node());
            double value = _plex.getValue(_Node());
            node = _plex.getNode(0);
            unsigned int id = _plex.getID(_Node());

            ignore_unused_variable_warning(edge);
            ignore_unused_variable_warning(id);
            ignore_unused_variable_warning(value);
        }

        _ScalarSimplicialComplex& _plex;
    };
};


/// \brief A set of member node ids.
/// \ingroup concepts_contour_tree
class Members
{
public:
    typedef std::set<unsigned int>::iterator iterator;
    typedef std::set<unsigned int>::const_iterator const_iterator;

    const_iterator begin() {
        return const_iterator();
    }
    const_iterator end() {
        return const_iterator();
    }
    size_t size() const {
        return 0;
    }

    /// Insert a member into the set.
    void insert(unsigned int) { }

    template <typename _Members>
    struct Constraints
    {
        void constraints()
        {
            typename _Members::const_iterator it = _members.begin();
            it != _members.end();
            // make sure that the iterator can be dereferenced to
            // get an unsigned integer
            unsigned int id = *it;
            _members.insert(42);
            size_t sz = _members.size();

            ignore_unused_variable_warning(sz);
            ignore_unused_variable_warning(id);
        }

        _Members& _members;
    };

};

/// \brief Represents a contour tree.
/// \ingroup concepts_contour_tree
/*!
 *  A contour tree is an undirected tree where each node has a:
 *
 *   - Scalar value
 *   - ID
 *   - Member set (of IDs)
 *
 *  and each edge has:
 *
 *   - Member set
 *
 *  A contour tree should conform to concepts::ReadableUndirectedGraph
 */
class ContourTree : public concepts::ReadableUndirectedGraph
{
    concepts::Members members;

public:

    typedef concepts::Members Members;

    /// \brief Get a node's scalar value
    double getValue(Node node) {
        return 0.;
    }

    /// \brief Get a node's ID
    unsigned int getID(Node node) {
        return 0;
    }

    /// \brief Retrieve the members of the node
    const Members& getNodeMembers(Node node) {
        return members;
    }

    /// \brief Retrieve a node by its ID. If there is no node with such an ID,
    //      an invalid node is returned.
    Node getNode(unsigned int id) const {
        return Node();
    }

    /// \brief Retrieve the members of the edge.
    const Members& getEdgeMembers(Edge edge) {
        return members;
    }


    template <typename _ContourTree>
    struct Constraints
    {
        void constraints()
        {
            checkConcept
            <
            ReadableUndirectedGraph,
            _ContourTree
            > ();

            double value = _tree.getValue(_Node());
            unsigned int id = _tree.getID(_Node());
            const _Members& node_members = _tree.getNodeMembers(_Node());
            _Node node = _tree.getNode(id);
            const _Members& edge_members = _tree.getEdgeMembers(_Edge());

            ignore_unused_variable_warning(node);
            ignore_unused_variable_warning(value);
            ignore_unused_variable_warning(node_members);
            ignore_unused_variable_warning(edge_members);
        }

        _ContourTree& _tree;
        typedef typename _ContourTree::Node _Node;
        typedef typename _ContourTree::Edge _Edge;
        typedef typename _ContourTree::Members _Members;
    };

};


/// \brief A graph with scalar value and member attributes.
/// \ingroup concepts_contour_tree
/*!
 *  The graph has the following node attributes:
 *   - ID
 *   - Members
 *   - Value
 *
 *  and the edges have:
 *   - Members
 *
 *  Conforms to the concepts::ReadableUndirectedGraph concept.
 */
class UndirectedScalarMemberIDGraph :
    public
    concepts::EdgeObservable <
    concepts::NodeObservable <
    concepts::BaseObservable <
    concepts::ReadableUndirectedGraph > > >
{

    concepts::Members members;

public:

    typedef concepts::Members Members;

    /// \brief Add a node to the graph.
    /*!
     *  \param  id      The id of the node.
     *  \param  value   The scalar value of the node.
     */
    Node addNode(unsigned int id, double value) {
        return Node();
    }

    /// \brief Add an edge to the graph.
    Edge addEdge(Node u, Node v) {
        return Edge();
    }

    /// \brief Remove the node.
    void removeNode(Node node) { }

    /// \brief Remove the edge.
    void removeEdge(Edge edge) { }

    /// \brief Insert a member into the node's member set.
    void insertNodeMember(Node node, unsigned int member) { }

    /// \brief Insert a member into the edge's member set.
    void insertEdgeMember(Edge edge, unsigned int member) { }

    /// \brief Insert multiple members.
    void insertNodeMembers(Node node, const Members& members) { }

    /// \brief Insert multiple members.
    void insertEdgeMembers(Edge edge, const Members& members) { }

    /// \brief Get a node's scalar value
    double getValue(Node node) const {
        return 0.;
    }

    /// \brief Get a node's ID
    unsigned int getID(Node node) const {
        return 0;
    }

    /// \brief Retrieve the members of the node
    const Members& getNodeMembers(Node node) {
        return members;
    }

    /// \brief Retrieve a node by its ID
    Node getNode(unsigned int id) const {
        return Node();
    }

    /// \brief Retrieve the members of the edge.
    const Members& getEdgeMembers(Edge edge) {
        return members;
    }

    /// \brief Clear the nodes of the graph.
    void clearNodes() { }

    /// \brief Clear the edges of the graph.
    void clearEdges() { }

    template <typename _UndirectedScalarMemberIDGraph>
    struct Constraints
    {
        void constraints()
        {
            checkConcept
            <
            concepts::ReadableUndirectedGraph,
                     _UndirectedScalarMemberIDGraph
                     > ();

            _Node node = _graph.addNode(14, 42.);
            _Edge edge = _graph.addEdge(_Node(), _Node());
            _graph.insertNodeMember(_Node(), 42);
            _graph.insertEdgeMember(_Edge(), 42);
            _graph.insertNodeMembers(_Node(), members);
            _graph.insertEdgeMembers(_Edge(), members);

            double value = _graph.getValue(_Node());
            unsigned int id = _graph.getID(_Node());
            const _Members& node_members = _graph.getNodeMembers(_Node());
            node = _graph.getNode(id);
            const _Members& edge_members = _graph.getEdgeMembers(_Edge());
            _graph.clearNodes();
            _graph.clearEdges();
            _graph.removeNode(_Node());
            _graph.removeEdge(_Edge());

            ignore_unused_variable_warning(value);
            ignore_unused_variable_warning(edge);
            ignore_unused_variable_warning(node_members);
            ignore_unused_variable_warning(edge_members);
        }

        _UndirectedScalarMemberIDGraph& _graph;
        typedef typename _UndirectedScalarMemberIDGraph::Node _Node;
        typedef typename _UndirectedScalarMemberIDGraph::Edge _Edge;
        typedef typename _UndirectedScalarMemberIDGraph::Members _Members;

        _Members& members;
    };

};


/// \brief An algorithm that computes a contour tree.
/// \ingroup concepts_contour_tree
class ContourTreeAlgorithm
{
public:

    /// \brief Compute the contour tree.
    /*!
     *  \param  simplicial_complex  The simplicial complex from which the tree
     *                              will be computed. Must conform to
     *                              concepts::ScalarSimplicialComplex.
     *
     *  \param  graph               The graph in which to place the output.
     *                              Must conform to
     *                              concepts::UndirectedScalarMemberIDGraph
     */
    template
    <
        typename ScalarSimplicialComplex,
        typename UndirectedScalarMemberIDGraph
        >
    void compute(
        const ScalarSimplicialComplex& simplicial_complex,
        UndirectedScalarMemberIDGraph& graph) { }

    template <typename _ContourTreeAlgorithm>
    struct Constraints
    {
        void constraints()
        {
            _algorithm.compute(_simplicial_complex, _graph);
        }

        _ContourTreeAlgorithm& _algorithm;
        const concepts::ScalarSimplicialComplex _simplicial_complex;
        concepts::UndirectedScalarMemberIDGraph _graph;
    };
};

////////////////////////////////////////////////////////////////////////
//
// RandomAccessComparisonFunctor
//
////////////////////////////////////////////////////////////////////////

template <typename RandomAccessContainer>
/// \ingroup concepts_contour_tree
class RandomAccessComparisonFunctor
{
public:
    RandomAccessComparisonFunctor(const RandomAccessContainer& values) { }
    bool operator()(unsigned int u, unsigned int v) {
        return true;
    }


    template <typename _RandomAccessComparisonFunctor>
    struct Constraints
    {
        void constraints()
        {
            _RandomAccessComparisonFunctor ftor(_values);
            bool decision = ftor(1,2);

            ignore_unused_variable_warning(decision);
        }

        std::vector<double> _values;
    };
};

////////////////////////////////////////////////////////////////////////
//
// TotalOrder
//
////////////////////////////////////////////////////////////////////////

/// \brief A total ordering.
/// \ingroup concepts_contour_tree
class TotalOrder
{
public:

    /// \brief Retrieve the position of the element in the ordering.
    unsigned int positionToElement(unsigned int index) const {
        return 0;
    }

    /// \brief Retrieve the element at this position in the ordering.
    unsigned int elementToPosition(unsigned int index) const {
        return 0;
    }

    size_t size() const {
        return 0;
    }

    template <typename Values, typename RandomAccessComparisonFunctor>
    static TotalOrder compute(const Values&, RandomAccessComparisonFunctor&)
    {
        return TotalOrder();
    }

    template <typename _TotalOrder>
    struct Constraints
    {
        void constraints()
        {
            unsigned int x = _total_order.positionToElement(0);
            x = _total_order.elementToPosition(0);
            _TotalOrder tot = _TotalOrder::compute(_values, _functor);
            size_t sz = _total_order.size();

            ignore_unused_variable_warning(x);
            ignore_unused_variable_warning(sz);
            ignore_unused_variable_warning(tot);
        }

        _TotalOrder& _total_order;
        std::vector<double> _values;
        RandomAccessComparisonFunctor<std::vector<double> > _functor;

        Constraints() : _functor(_values) { }
    };

};

}
}

#endif
