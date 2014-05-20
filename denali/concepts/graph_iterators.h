// Copyright (c) 2014, Justin Eldridge, The Ohio State University
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef DENALI_GRAPH_ITERATORS_H
#define DENALI_GRAPH_ITERATORS_H

#include <queue>

namespace denali {
namespace concepts {

/// \brief A node iterator.
/// \ingroup concepts_graph_iterators
/*!
 * A NodeIterator allows for easy iteration of the nodes in a graph.
 */
template <typename NodeIterable>
class NodeIterator
{
    typedef typename NodeIterable::Node Node;
public:
    /// Constructs an iterator to read nodes from the graph.
    NodeIterator(NodeIterable& graph) {}

    /// Returns true if the iterator is finished iterating.
    bool done() const {
        return true;
    }

    /// Advances the iterator.
    void operator++() {}

    /// Returns the Node at which the iterator is currently at.
    Node node() const {
        return Node();
    }

    template <typename _NodeIterator>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<concepts::Node, typename NodeIterable::Node>();
            _NodeIterator it(graph);
            ++it;
            Node node = it.node();
            it.done();

            ignore_unused_variable_warning(node);
        }

        NodeIterable& graph;
        Constraints() {}
    };
};


/// \brief An arc iterator.
/// \ingroup concepts_graph_iterators
/*!
 *  An ArcIterator allows for easy iteration over the arcs of a directed
 *  graph.
 */
template <typename ArcIterable>
class ArcIterator
{
    typedef typename ArcIterable::Arc Arc;
public:
    /// Constructs an iterator to read arcs from the graph.
    ArcIterator(ArcIterable& graph) {}

    /// Returns true if the iterator is finished iterating.
    bool done() const {
        return true;
    }

    /// Advances the iterator.
    void operator++() {}

    /// Returns the Arc at which the iterator is currently at.
    Arc arc() const {
        return Arc();
    }

    template <typename _ArcIterator>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<concepts::Arc, typename ArcIterable::Arc>();
            _ArcIterator it(graph);
            ++it;
            Arc arc = it.arc();
            it.done();

            ignore_unused_variable_warning(arc);
        }

        ArcIterable& graph;
        Constraints() {}
    };
};


/// \brief An edge iterator.
/// \ingroup concepts_graph_iterators
/*!
 *  An EdgeIterator allows for easy iteration over the edges of an
 *  undirected graph.
 */
template <typename EdgeIterable>
class EdgeIterator
{
    typedef typename EdgeIterable::Edge Edge;
public:
    /// Constructs an iterator to read edges from the graph.
    EdgeIterator(EdgeIterable& graph) {}

    /// Returns true if the iterator is finished iterating.
    bool done() const {
        return true;
    }

    /// Advances the iterator.
    void operator++() {}

    /// Returns the Edge at which the iterator is currently at.
    Edge edge() const {
        return Edge();
    }

    template <typename _EdgeIterator>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<concepts::Edge, typename EdgeIterable::Edge>();
            _EdgeIterator it(graph);
            ++it;
            Edge edge = it.edge();
            it.done();

            ignore_unused_variable_warning(edge);
        }

        EdgeIterable & graph;
        Constraints() {}
    };
};


/// \brief An iterator for iterating over the parents of a node in a
/// directed graph.
/// \ingroup concepts_graph_iterators
/*!
 */
template <typename ParentIterable>
class ParentIterator
{
    typedef typename ParentIterable::Node Node;
    typedef typename ParentIterable::Arc Arc;
public:
    /// \brief
    /// Constructs an iterator to read the parents of the node from the
    /// graph.
    ParentIterator(ParentIterable& graph, Node node) {}

    /// Returns true if the iterator is finished iterating.
    bool done() const {
        return true;
    }

    /// Advances the iterator.
    void operator++() {}

    /// Returns the parent Node at which the iterator is currently at.
    Node parent() const {
        return Node();
    }

    /// Returns the arc at which the iterator is currently at.
    Arc arc() const {
        return Arc();
    }

    template <typename _ParentIterator>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<concepts::Node, typename ParentIterable::Node>();
            checkConcept<concepts::Arc, typename ParentIterable::Arc>();
            _ParentIterator it(graph, Node());
            ++it;
            Node parent = it.parent();
            Arc arc = it.arc();
            it.done();

            ignore_unused_variable_warning(arc);
            ignore_unused_variable_warning(parent);
        }

        ParentIterable& graph;
        Constraints() {}
    };
};


/// \brief An iterator for iterating over the children of a node in a
/// directed graph.
/// \ingroup concepts_graph_iterators
template <typename ChildIterable>
class ChildIterator
{
    typedef typename ChildIterable::Node Node;
    typedef typename ChildIterable::Arc Arc;
public:
    /// \brief
    /// Constructs an iterator to read the children of the node from
    /// the graph.
    ChildIterator(ChildIterable& graph, Node node) {}

    /// Returns true if the iterator is finished iterating.
    bool done() const {
        return true;
    }

    /// Advances the iterator.
    void operator++() {}

    /// Returns the child Node at which the iterator is currently at.
    Node child() const {
        return Node();
    }

    /// Returns the arc at which the iterator is currently at.
    Arc arc() const {
        return Arc();
    }

    template <typename _ChildIterator>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<concepts::Node, typename ChildIterable::Node>();
            checkConcept<concepts::Arc, typename ChildIterable::Arc>();
            _ChildIterator it(graph, Node());
            ++it;
            Node child = it.child();
            Arc arc = it.arc();
            it.done();

            ignore_unused_variable_warning(arc);
            ignore_unused_variable_warning(child);
        }

        ChildIterable& graph;
        Constraints() {}
    };
};


/// \brief An iterator for iterating over the neighbors of a node in a
/// directed graph.
/// \ingroup concepts_graph_iterators
template <typename DirectedNeighborIterable>
class DirectedNeighborIterator
{
    typedef typename DirectedNeighborIterable::Node Node;
    typedef typename DirectedNeighborIterable::Arc Arc;
public:
    /// \brief
    /// Constructs an iterator to read the neighboring nodes from the
    /// directed graph.
    DirectedNeighborIterator(DirectedNeighborIterable& graph, Node node) {}

    /// Returns true if the iterator is finished iterating.
    bool done() const {
        return true;
    }

    /// Advances the iterator.
    void operator++() {}

    /// Returns the child Node at which the iterator is currently at.
    Node neighbor() const {
        return Node();
    }

    /// Returns the arc at which the iterator is currently at.
    Arc arc() const {
        return Arc();
    }

    template <typename _DirectedNeighborIterator>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<concepts::Node, typename DirectedNeighborIterable::Node>();
            checkConcept<concepts::Arc, typename DirectedNeighborIterable::Arc>();
            _DirectedNeighborIterator it(graph, Node());
            ++it;
            Node neighbor = it.neighbor();
            Arc arc = it.arc();
            it.done();

            ignore_unused_variable_warning(arc);
            ignore_unused_variable_warning(neighbor);
        }

        DirectedNeighborIterable& graph;
        Constraints() {}
    };
};


/// \brief An iterator for iterating over the neighbors of a node in a
/// undirected graph.
/// \ingroup concepts_graph_iterators
template <typename UndirectedNeighborIterable>
class UndirectedNeighborIterator
{
    typedef typename UndirectedNeighborIterable::Node Node;
    typedef typename UndirectedNeighborIterable::Edge Edge;
public:
    /// \brief
    /// Constructs an iterator to read the neighboring nodes from the
    /// undirected graph.
    UndirectedNeighborIterator(UndirectedNeighborIterable& graph, Node node) {}

    /// Returns true if the iterator is finished iterating.
    bool done() const {
        return true;
    }

    /// Advances the iterator.
    void operator++() {}

    /// Returns the child Node at which the iterator is currently at.
    Node neighbor() const {
        return Node();
    }

    /// Returns the arc at which the iterator is currently at.
    Edge edge() const {
        return Edge();
    }

    template <typename _UndirectedNeighborIterator>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<concepts::Node, typename UndirectedNeighborIterable::Node>();
            checkConcept<concepts::Edge, typename UndirectedNeighborIterable::Edge>();
            _UndirectedNeighborIterator it(graph, Node());
            ++it;
            Node neighbor = it.neighbor();
            Edge edge = it.edge();
            it.done();

            ignore_unused_variable_warning(neighbor);
            ignore_unused_variable_warning(edge);
        }

        UndirectedNeighborIterable& graph;
        Constraints() {}
    };
};


/// \brief BFS edge iterator.
/// \ingroup concepts_graph_iterators
/*!
 *  Iterate over the edges of an undirected graph in BFS order.
 *
 *  GraphType must conform to concepts::UndirectedNeighborIterable
 *  and concepts::
 */
template <typename GraphType>
class UndirectedBFSIterator
{
    typedef typename GraphType::Node Node;
    typedef typename GraphType::Edge Edge;
public:
    /// \brief Start the BFS at the root node.
    UndirectedBFSIterator(const GraphType& graph, Node root)
    { }

    /// \brief Start the (truncated) BFS along the root edge.
    UndirectedBFSIterator(const GraphType&, Node parent, Node child)
    { }

    /// \brief Returns true when done iterating.
    bool done() const {
        return false;
    }

    /// \brief The parent of the current edge.
    Node parent() const {
        return Node();
    }

    /// \brief The child of the current edge.
    Node child() const {
        return Node();
    }

    /// \brief The current edge in the BFS.
    Edge edge() const {
        return Edge();
    }


    template <typename _UndirectedBFSIterator>
    struct Constraints
    {
        void constraints()
        {
            _UndirectedBFSIterator it1(_graph, Node());
            _UndirectedBFSIterator it2(_graph, Node(), Node());

            bool x = it1.done();

            Node node = it1.parent();
            node = it1.child();
            Edge edge = it1.edge();

            ignore_unused_variable_warning(node);
            ignore_unused_variable_warning(edge);
            ignore_unused_variable_warning(x);
        }

        const GraphType& _graph;
    };
};

}
}

#endif
