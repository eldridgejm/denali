#ifndef DENALI_GRAPH_ITERATORS_H
#define DENALI_GRAPH_ITERATORS_H

/// \file 
/// \brief Implementations of generic graph iterators

namespace denali {

    /// \brief A generic node iterator.
    /// \ingroup graph_implementations_iterators
    /*!
     *  Given a GraphType that conforms to concepts::NodeIterable,
     *  produces an iterator on the nodes of the graph.
     */
    template <typename GraphType>
    class NodeIterator
    {
        typedef typename GraphType::Node Node;
        GraphType& _graph;
        Node _node;

        public:
        /// \brief Constructs an iterator to read nodes from the graph.
        /*!
         *  The graph must conform to concepts::NodeIterable.
         */
        NodeIterator(GraphType& graph) 
            : _graph(graph), _node(_graph.getFirstNode()) {}

        /// Returns true if the iterator is finished iterating.
        bool done() const { return !_graph.isNodeValid(_node); }

        /// Advances the iterator.
        void operator++() { _node = _graph.getNextNode(_node); }

        /// Returns the Node at which the iterator is currently at.
        Node node() const { return _node; }
    };


    /// \brief An arc iterator.
    /// \ingroup graph_implementations_iterators
    /*!
     *  An ArcIterator allows for easy iteration over the arcs of a directed 
     *  graph. Requires that the graph conforms to concepts::ArcIterable.
     */
    template <typename GraphType>
    class ArcIterator
    {
        typedef typename GraphType::Arc Arc;
        GraphType& _graph;
        Arc _arc;

        public:
        /// Constructs an iterator to read arcs from the graph.
        ArcIterator(GraphType& graph) 
            : _graph(graph), _arc(graph.getFirstArc()) {}

        /// Returns true if the iterator is finished iterating.
        bool done() const { !_graph.isArcValid(_arc); }

        /// Advances the iterator.
        void operator++() { _arc = _graph.getNextArc(_arc); }

        /// Returns the Arc at which the iterator is currently at.
        Arc arc() const { return _arc; }
    };


    /// \brief An edge iterator.
    /// \ingroup graph_implementations_iterators
    /*!
     *  An EdgeIterator allows for easy iteration over the edges of a directed 
     *  graph. Requires that the graph conforms to concepts::EdgeIterable.
     */
    template <typename GraphType>
    class EdgeIterator
    {
        typedef typename GraphType::Edge Edge;
        GraphType& _graph;
        Edge _edge;

        public:
        /// Constructs an iterator to read edges from the graph.
        EdgeIterator(GraphType& graph) 
            : _graph(graph), _edge(graph.getFirstEdge()) {}

        /// Returns true if the iterator is finished iterating.
        bool done() const { !_graph.isEdgeValid(_edge); }

        /// Advances the iterator.
        void operator++() { _edge = _graph.getNextEdge(_edge); }

        /// Returns the Edge at which the iterator is currently at.
        Edge edge() const { return _edge; }
    };


    /// \brief An iterator for iterating over the parents of a node in a 
    /// directed graph.
    /// \ingroup graph_implementations_iterators
    /*!
     *  Requires that the input graph conform to concepts::ParentIterable.
     */
    template <typename GraphType>
    class ParentIterator
    {
        typedef typename GraphType::Node Node;
        typedef typename GraphType::Arc Arc;
        GraphType& _graph;
        Arc _arc;

        public:
        /// \brief
        /// Constructs an iterator to read the parents of the node from the
        /// graph.
        ParentIterator(GraphType& graph, Node node)
            : _graph(graph), _arc(graph.getFirstInArc(node)) {}

        /// Returns true if the iterator is finished iterating.
        bool done() const { return !_graph.isArcValid(_arc); }

        /// Advances the iterator.
        void operator++() { _arc = _graph.getNextInArc(_arc); }

        /// Returns the parent Node at which the iterator is currently at.
        Node parent() const { return _graph.source(_arc); }

        /// Returns the arc at which the iterator is currently at.
        Arc arc() const { return _arc; }
    };       


    /// \brief An iterator for iterating over the children of a node in a 
    /// directed graph.
    /// \ingroup graph_implementations_iterators
    /*!
     *  Requires that the input graph conform to concepts::ChildIterable.
     */
    template <typename GraphType>
    class ChildIterator
    {
        typedef typename GraphType::Node Node;
        typedef typename GraphType::Arc Arc;
        GraphType& _graph;
        Arc _arc;

        public:
        /// \brief
        /// Constructs an iterator to read the children of the node from
        /// the graph.
        ChildIterator(GraphType& graph, Node node)
            : _graph(graph), _arc(graph.getFirstOutArc(node)) {}

        /// Returns true if the iterator is finished iterating.
        bool done() const { !_graph.isArcValid(_arc); }

        /// Advances the iterator.
        void operator++() { _arc = _graph.getNextOutArc(_arc); }

        /// Returns the child Node at which the iterator is currently at.
        Node child() const { return _graph.target(_arc); }

        /// Returns the arc at which the iterator is currently at.
        Arc arc() const { return _arc; }
    };     


    /// \brief An iterator for iterating over the neighbors of a node in a 
    /// directed graph.
    /// \ingroup graph_implementations_iterators
    /*!
     *  Requires that the input graph conform to concepts::DirectedNeighborIterable.
     */
    template <typename GraphType>
    class DirectedNeighborIterator
    {
        typedef typename GraphType::Node Node;
        typedef typename GraphType::Arc Arc;
        GraphType& _graph;
        Arc _arc;
        Node _node;

        public:
        /// \brief
        /// Constructs an iterator to read the neighboring nodes from the
        /// directed graph.
        DirectedNeighborIterator(GraphType& graph, Node node)
            : _graph(graph), _node(node), _arc(graph.getFirstNeighborArc(node)) {}

        /// Returns true if the iterator is finished iterating.
        bool done() const { return !_graph.isArcValid(_arc); }

        /// Advances the iterator.
        void operator++() { _arc = _graph.getNextNeighborArc(_node, _arc); }

        /// Returns the child Node at which the iterator is currently at.
        Node neighbor() const { _graph.opposite(_node, _arc); }

        /// Returns the arc at which the iterator is currently at.
        Arc arc() const { return _arc; }
    };     


    /// \brief An iterator for iterating over the neighbors of a node in a 
    /// directed graph.
    /// \ingroup graph_implementations_iterators
    /*!
     *  Requires that the input graph conform to concepts::UndirectedNeighborIterable.
     */
    template <typename GraphType>
    class UndirectedNeighborIterator
    {
        typedef typename GraphType::Node Node;
        typedef typename GraphType::Edge Edge;
        GraphType& _graph;
        Edge _edge;
        Node _node;

        public:
        /// \brief
        /// Constructs an iterator to read the neighboring nodes from the
        /// directed graph.
        UndirectedNeighborIterator(GraphType& graph, Node node)
            : _graph(graph), _node(node), _edge(graph.getFirstNeighborEdge(node)) {}

        /// Returns true if the iterator is finished iterating.
        bool done() const { return !_graph.isEdgeValid(_edge); }

        /// Advances the iterator.
        void operator++() { _edge = _graph.getNextNeighborEdge(_node, _edge); }

        /// Returns the child Node at which the iterator is currently at.
        Node neighbor() const { _graph.opposite(_node, _edge); }

        /// Returns the edge at which the iterator is currently at.
        Edge edge() const { return _edge; }
    };     

}

#endif
