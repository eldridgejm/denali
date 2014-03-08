#ifndef DENALI_GRAPH_ITERATORS_H
#define DENALI_GRAPH_ITERATORS_H

#include <queue>
#include <denali/graph_maps.h>

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
    const GraphType& _graph;
    Node _node;

public:
    /// \brief Constructs an iterator to read nodes from the graph.
    /*!
     *  The graph must conform to concepts::NodeIterable.
     */
    NodeIterator(const GraphType& graph)
        : _graph(graph), _node(_graph.getFirstNode()) {}

    /// Returns true if the iterator is finished iterating.
    bool done() const {
        return !_graph.isNodeValid(_node);
    }

    /// Advances the iterator.
    void operator++() {
        _node = _graph.getNextNode(_node);
    }

    /// Returns the Node at which the iterator is currently at.
    Node node() const {
        return _node;
    }
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
    const GraphType& _graph;

public:
    Arc _arc;
    /// Constructs an iterator to read arcs from the graph.
    ArcIterator(const GraphType& graph)
        : _graph(graph), _arc(_graph.getFirstArc()) {}

    /// Returns true if the iterator is finished iterating.
    bool done() const {
        return !_graph.isArcValid(_arc);
    }

    /// Advances the iterator.
    void operator++() {
        _arc = _graph.getNextArc(_arc);
    }

    /// Returns the Arc at which the iterator is currently at.
    Arc arc() const {
        return _arc;
    }
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
    const GraphType& _graph;
    Edge _edge;

public:
    /// Constructs an iterator to read edges from the graph.
    EdgeIterator(const GraphType& graph)
        : _graph(graph), _edge(graph.getFirstEdge()) {}

    /// Returns true if the iterator is finished iterating.
    bool done() const {
        return !_graph.isEdgeValid(_edge);
    }

    /// Advances the iterator.
    void operator++() {
        _edge = _graph.getNextEdge(_edge);
    }

    /// Returns the Edge at which the iterator is currently at.
    Edge edge() const {
        return _edge;
    }
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
    const GraphType& _graph;
    Arc _arc;

public:
    /// \brief
    /// Constructs an iterator to read the parents of the node from the
    /// graph.
    ParentIterator(const GraphType& graph, Node node)
        : _graph(graph), _arc(graph.getFirstInArc(node)) {}

    /// Returns true if the iterator is finished iterating.
    bool done() const {
        return !_graph.isArcValid(_arc);
    }

    /// Advances the iterator.
    void operator++() {
        _arc = _graph.getNextInArc(_arc);
    }

    /// Returns the parent Node at which the iterator is currently at.
    Node parent() const {
        return _graph.source(_arc);
    }

    /// Returns the arc at which the iterator is currently at.
    Arc arc() const {
        return _arc;
    }
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
    const GraphType& _graph;
    Arc _arc;

public:
    /// \brief
    /// Constructs an iterator to read the children of the node from
    /// the graph.
    ChildIterator(const GraphType& graph, Node node)
        : _graph(graph), _arc(graph.getFirstOutArc(node)) {}

    /// Returns true if the iterator is finished iterating.
    bool done() const {
        return !_graph.isArcValid(_arc);
    }

    /// Advances the iterator.
    void operator++() {
        _arc = _graph.getNextOutArc(_arc);
    }

    /// Returns the child Node at which the iterator is currently at.
    Node child() const {
        return _graph.target(_arc);
    }

    /// Returns the arc at which the iterator is currently at.
    Arc arc() const {
        return _arc;
    }
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
    const GraphType& _graph;
    Arc _arc;
    Node _node;

public:
    /// \brief
    /// Constructs an iterator to read the neighboring nodes from the
    /// directed graph.
    DirectedNeighborIterator(const GraphType& graph, Node node)
        : _graph(graph), _node(node), _arc(graph.getFirstNeighborArc(node)) {}

    /// Returns true if the iterator is finished iterating.
    bool done() const {
        return !_graph.isArcValid(_arc);
    }

    /// Advances the iterator.
    void operator++() {
        _arc = _graph.getNextNeighborArc(_node, _arc);
    }

    /// Returns the child Node at which the iterator is currently at.
    Node neighbor() const {
        return _graph.opposite(_node, _arc);
    }

    /// Returns the arc at which the iterator is currently at.
    Arc arc() const {
        return _arc;
    }
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
    const GraphType& _graph;
    Node _node;
    Edge _edge;

public:
    /// \brief
    /// Constructs an iterator to read the neighboring nodes from the
    /// directed graph.
    UndirectedNeighborIterator(const GraphType& graph, Node node)
        : _graph(graph), _node(node), _edge(graph.getFirstNeighborEdge(node)) {}

    /// Returns true if the iterator is finished iterating.
    bool done() const {
        return !_graph.isEdgeValid(_edge);
    }

    /// Advances the iterator.
    void operator++() {
        _edge = _graph.getNextNeighborEdge(_node, _edge);
    }

    /// Returns the child Node at which the iterator is currently at.
    Node neighbor() const {
        return _graph.opposite(_node, _edge);
    }

    /// Returns the edge at which the iterator is currently at.
    Edge edge() const {
        return _edge;
    }
};


/// \brief Performs a BFS on an undirected graph.
/// \ingroup graph_implementations_iterators
template <typename GraphType>
class UndirectedBFSIterator
{
    typedef typename GraphType::Node Node;
    typedef typename GraphType::Edge Edge;
    typedef std::pair<Node,Node> Direction;
    typedef std::pair<Edge, Direction> Visit;

    const GraphType& _graph;
    std::queue<Visit> _bfs_queue;
    StaticNodeMap<GraphType, bool> _visited;

private:
    void visit(Node node)
    {
        // visit the node and add it's unvisited children to the queue
        for (UndirectedNeighborIterator<GraphType> it(_graph, node);
                !it.done(); ++it) {
            if (!_visited[it.neighbor()]) {
                _bfs_queue.push(Visit(it.edge(), Direction(node, it.neighbor())));
                _visited[it.neighbor()] = true;
            }
        }
    }


public:
    /// \brief Perform a full BFS, starting at the root.
    UndirectedBFSIterator(const GraphType& graph, Node root)
        : _graph(graph), _visited(graph)
    {
        // no nodes have been visited
        for (NodeIterator<GraphType> node_it(graph); !node_it.done(); ++node_it) {
            _visited[node_it.node()] = false;
        }

        // except for the root node
        _visited[root] = true;

        // now visit the children
        visit(root);
    }

    /// \brief Perform a partial BFS, starting at the parent and searching down the edge
    ///     containing the child.
    UndirectedBFSIterator(const GraphType& graph, Node parent, Node child)
        : _graph(graph), _visited(graph)
    {
        // no nodes have been visited
        for (NodeIterator<GraphType> node_it(graph); !node_it.done(); ++node_it) {
            _visited[node_it.node()] = false;
        }

        // except for the root node
        _visited[parent] = true;
        _visited[child] = true;

        // now visit the child on the edge
        visit(child);

    }

    bool done() const {
        return _bfs_queue.size() == 0;
    }

    void operator++()
    {
        Visit v = _bfs_queue.front();
        _bfs_queue.pop();
        Node child = v.second.second;
        visit(child);
    }

    Node parent() const {
        return _bfs_queue.front().second.first;
    }
    Node child() const {
        return _bfs_queue.front().second.second;
    }
    Edge edge() const {
        return _bfs_queue.front().first;
    }

};


/// \brief Iterate through the arcs of a directed graph in BFS order.
/// \ingroup graph_implementations_iterators
template <typename GraphType>
class DirectedBFSIterator
{
    typedef typename GraphType::Node Node;
    typedef typename GraphType::Arc Arc;

    const GraphType& _graph;
    std::queue<Arc> _bfs_queue;

public:

    DirectedBFSIterator(const GraphType& graph, Node root)
        : _graph(graph)
    {
        for (ChildIterator<GraphType> it(graph, root); !it.done(); ++it) {
            _bfs_queue.push(it.arc());
        }
    }

    bool done() const {
        return _bfs_queue.size() == 0;
    }

    void operator++()
    {
        Arc arc = _bfs_queue.front();
        _bfs_queue.pop();
        for (ChildIterator<GraphType> it(_graph, _graph.target(arc));
                !it.done(); ++it) {
            _bfs_queue.push(it.arc());
        }
    }

    Node parent() const {
        return _graph.source(_bfs_queue.front());
    }
    Node child() const {
        return _graph.target(_bfs_queue.front());
    }
    Arc arc() const {
        return _bfs_queue.front();
    }
};



}

#endif
