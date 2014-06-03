// Copyright (c) 2014, Justin Eldridge, Mikhail Belkin, and Yusu Wang
// at The Ohio State University. All rights reserved.
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

#ifndef DENALI_GRAPH_ATTRIBUTES_H
#define DENALI_GRAPH_ATTRIBUTES_H

#include <denali/concepts/graph_objects.h>

namespace denali {
namespace concepts {

/// \brief A type which has Nodes and Arcs
class BaseDirectedGraph
{
public:
    typedef concepts::Node Node;
    typedef concepts::Arc Arc;
};


/// \brief A type which has Nodes and Edges
class BaseUndirectedGraph
{
public:
    typedef concepts::Node Node;
    typedef concepts::Edge Edge;
};


/// \brief A type which supports iteration over its nodes.
/// \ingroup concepts_graph_attributes
/*! An object conforming to the NodeIterable concept allows iteration
    over its nodes. If provides a NodeIterator convenience class which
    conforms to the NodeIterator concept, and provides methods for manual
    iteration over the nodes.
    \sa ArcIterable, EdgeIterable
*/
template <typename BaseConcept>
class NodeIterable : public BaseConcept
{
public:

    /// Checks if the node is valid.
    bool isNodeValid(Node) const {
        return true;
    }

    /// Gets the first node from the graph.
    Node getFirstNode() const {
        return Node();
    }

    /// Gets the next node from the graph.
    Node getNextNode(Node) const {
        return Node();
    }

    template <typename _NodeIterable>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<concepts::Node, _Node> ();

            _Node node;
            iterable.isNodeValid(node);
            node = iterable.getFirstNode();
            node = iterable.getNextNode(node);
        }
        typedef typename _NodeIterable::Node _Node;
        _NodeIterable& iterable;
    };
};


/// \brief A type which supports iteration over its arcs.
/// \ingroup concepts_graph_attributes
/*!
 *  \sa NodeIterable, EdgeIterable
 */
template <typename BaseConcept>
class ArcIterable : public BaseConcept
{
public:

    bool isArcValid(Arc) const {
        return true;
    }
    Arc getFirstArc() const {
        return Arc();
    }
    Arc getNextArc(Arc) const {
        return Arc();
    }

    template <typename _ArcIterable>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<concepts::Arc, _Arc> ();

            _Arc arc;
            iterable.isArcValid(arc);
            arc = iterable.getFirstArc();
            arc = iterable.getNextArc(arc);
        }
        typedef typename _ArcIterable::Arc _Arc;
        _ArcIterable& iterable;
    };
};


/// \brief A type which supports iteration over its edges.
/// \ingroup concepts_graph_attributes
/*!
 *  \sa NodeIterable, ArcIterable
 */
template <typename BaseConcept>
class EdgeIterable : public BaseConcept
{
public:

    /// Checks that the edge is valid.
    bool isEdgeValid(Edge) const {
        return true;
    }

    /// Gets the first edge from the graph.
    Edge getFirstEdge() const {
        return Edge();
    }

    /// Gets the next edge from the graph.
    Edge getNextEdge(Edge) const {
        return Edge();
    }

    template <typename _EdgeIterable>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<concepts::Edge, _Edge>();

            _Edge edge;
            iterable.isEdgeValid(edge);
            edge = iterable.getFirstEdge();
            edge = iterable.getNextEdge(edge);

        }
        typedef typename _EdgeIterable::Edge _Edge;
        _EdgeIterable& iterable;
    };
};


/// \brief A type which allows iteration over the parents of a node.
/// \ingroup concepts_graph_attributes
template <typename BaseConcept>
class ParentIterable : public BaseConcept
{
public:

    /// Returns true if the arc is valid.
    bool isArcValid(Arc) const {
        return true;
    }

    /// Gets the first out arc of a node.
    Arc getFirstInArc(Node) const {
        return Arc();
    }

    /// \brief Gets the next out arc of the node which is the source of the
    /// passed arc.
    Arc getNextInArc(Arc) const {
        return Arc();
    }

    /// \brief Get the source of the arc.
    /// \pre The arc must be valid.
    Node source(Arc) const {
        return Node();
    }

    template <typename _ParentIterable>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<concepts::Node, _Node>();
            checkConcept<concepts::Arc, _Arc>();

            _Node node;
            _Arc arc = iterable.getFirstInArc(node);
            iterable.isArcValid(arc);
            iterable.getNextInArc(arc);
            node = iterable.source(arc);
        }
        typedef typename _ParentIterable::Node _Node;
        typedef typename _ParentIterable::Arc _Arc;
        _ParentIterable& iterable;
    };
};


/// \brief A type which allows iteration over the children of a node.
/// \ingroup concepts_graph_attributes
template <typename BaseConcept>
class ChildIterable : public BaseConcept
{
public:

    /// Returns true if the arc is valid.
    bool isArcValid(Arc) const {
        return true;
    }

    /// Gets the first out arc of a node.
    Arc getFirstOutArc(Node) const {
        return Arc();
    }

    /// \brief Gets the next out arc of the node which is the source of the
    /// passed arc.
    Arc getNextOutArc(Arc) const {
        return Arc();
    }

    /// \brief Get the target of the arc.
    /// \pre The arc must be valid.
    Node target(Arc) const {
        return Node();
    }

    template <typename _ChildIterable>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<concepts::Node, _Node>();
            checkConcept<concepts::Arc, _Arc>();

            _Node node;
            _Arc arc = iterable.getFirstOutArc(node);
            iterable.isArcValid(arc);
            iterable.getNextOutArc(arc);
            node = iterable.target(arc);
        }
        typedef typename _ChildIterable::Node _Node;
        typedef typename _ChildIterable::Arc _Arc;
        _ChildIterable& iterable;
    };
};


/// \brief A type which allows iteration over the neighbors of a node in
/// a directed graph.
/// \ingroup concepts_graph_attributes
template <typename BaseConcept>
class DirectedNeighborIterable : public BaseConcept
{
public:

    /// Returns true if the arc is valid.
    bool isArcValid(Arc) const {
        return true;
    }

    /// Gets the first neighbor arc attached to the node.
    Arc getFirstNeighborArc(Node) const {
        return Arc();
    }

    /// Gets the next neighbor arc attached to the node.
    Arc getNextNeighborArc(Node, Arc) const {
        return Arc();
    }

    /// \brief Get the opposite node of the arc.
    /// \pre The node must be in the arc.
    Node opposite(Node, Arc) const {
        return Node();
    }

    template <typename _DirectedNeighborIterable>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<concepts::Node, _Node>();
            checkConcept<concepts::Arc, _Arc>();

            _Node node;
            _Arc arc = iterable.getFirstNeighborArc(node);
            iterable.isArcValid(arc);
            iterable.getNextNeighborArc(node, arc);
            node = iterable.opposite(node, arc);
        }
        typedef typename _DirectedNeighborIterable::Node _Node;
        typedef typename _DirectedNeighborIterable::Arc _Arc;
        _DirectedNeighborIterable& iterable;
    };
};


/// \brief A type which allows iteration over the neighbors of a node in
/// an undirected graph.
/// \ingroup concepts_graph_attributes
template <typename BaseConcept>
class UndirectedNeighborIterable : public BaseConcept
{
public:

    /// Returns true if the edge is valid.
    bool isEdgeValid(Edge) const {
        return true;
    }

    /// Gets the first neighbor edge attached to the node.
    Edge getFirstNeighborEdge(Node) const {
        return Edge();
    }

    /// Gets the next neighbor edge attached to the node.
    Edge getNextNeighborEdge(Node, Edge) const {
        return Edge();
    }

    /// \brief Get the opposite node of the edge.
    /// \pre The node must be in the edge.
    Node opposite(Node, Edge) const {
        return Node();
    }

    template <typename _UndirectedNeighborIterable>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<concepts::Node, _Node>();
            checkConcept<concepts::Edge, _Edge>();

            _Node node;
            _Edge edge = iterable.getFirstNeighborEdge(node);
            iterable.isEdgeValid(edge);
            iterable.getNextNeighborEdge(node, edge);
            node = iterable.opposite(node, edge);
        }
        _UndirectedNeighborIterable& iterable;
        typedef typename _UndirectedNeighborIterable::Node _Node;
        typedef typename _UndirectedNeighborIterable::Edge _Edge;
    };
};


/// \brief A type which provides the ability to efficiently map its nodes
/// to values.
/// \ingroup concepts_graph_attributes
/*!
 *  A NodeMappable type allows for the efficient mapping of nodes to values
 *  by assigning a unique node identifier to each node object. In general,
 *  standard map containers can hash their keys to support O(log n) lookup.
 *  Implementations of NodeMap concepts can support O(1) lookup by using
 *  the node identifier to index into an array.
 *
 *  If the graph type allows itself to be mutated, a node map also needs to
 *  be an Observer.
 */
template <typename BaseConcept>
class NodeMappable : public BaseConcept
{
public:

    typedef concepts::Node Node;

    /// \brief Gets the maximum node identifier in the graph.
    unsigned int getMaxNodeIdentifier() const {
        return 0;
    }

    /// \brief Get the identifier of the node.
    unsigned int getNodeIdentifier(Node) const {
        return 0;
    }

    Node getNodeFromIdentifier(unsigned int) const {
        return Node();
    }

    template <typename _NodeMappable>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<concepts::Node, _Node>();

            unsigned int n = graph.getMaxNodeIdentifier();
            n = graph.getNodeIdentifier(_Node());

            _Node node = graph.getNodeFromIdentifier(0);

            ignore_unused_variable_warning(n);
            ignore_unused_variable_warning(node);
        }
        _NodeMappable& graph;
        typedef typename _NodeMappable::Node _Node;
    };

};


/// \brief A type which supports efficient arc mapping.
/// \ingroup concepts_graph_attributes
/*!
 *  \sa NodeMappable, EdgeMappable
 */
template <typename BaseConcept>
class ArcMappable : public BaseConcept
{
public:
    
    typedef concepts::Arc Arc;

    /// \brief Gets the maximum arc identifier in the graph.
    unsigned int getMaxArcIdentifier() const {
        return 0;
    }

    /// \brief Get the identifier of the arc.
    unsigned int getArcIdentifier(Arc) const {
        return 0;
    }

    /// \brief Get an arc from an identifier.
    Arc getArcFromIdentifier(unsigned int identifier) const {
        return Arc();
    }

    template <typename _ArcMappable>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<concepts::Arc, _Arc>();

            unsigned int n = graph.getMaxArcIdentifier();
            n = graph.getArcIdentifier(_Arc());
            _Arc arc = graph.getArcFromIdentifier(0);

            ignore_unused_variable_warning(n);
            ignore_unused_variable_warning(arc);
        }
        _ArcMappable& graph;
        typedef typename _ArcMappable::Arc _Arc;
    };

};


/// \brief A type which supports efficient edge mapping.
/// \ingroup concepts_graph_attributes
/*!
 *  \sa NodeMappable, ArcMappable
 */
template <typename BaseConcept>
class EdgeMappable : public BaseConcept
{
public:

    typedef concepts::Edge Edge;

    /// \brief Gets the maximum edge identifier in the graph.
    unsigned int getMaxEdgeIdentifier() const {
        return 0;
    }

    /// \brief Get the identifier of the edge.
    unsigned int getEdgeIdentifier(Edge) const {
        return 0;
    }

    /// \brief Get an edge from an identifier.
    Edge getEdgeFromIdentifier(unsigned int identifier) const {
        return Edge();
    }

    template <typename _EdgeMappable>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<concepts::Edge, _Edge>();

            unsigned int n = graph.getMaxEdgeIdentifier();
            n = graph.getEdgeIdentifier(_Edge());
            _Edge edge = graph.getEdgeFromIdentifier(0);

            ignore_unused_variable_warning(n);
            ignore_unused_variable_warning(edge);
        }
        _EdgeMappable& graph;
        typedef typename _EdgeMappable::Edge _Edge;
    };

};


/// \brief A base class providing an Observer object.
template <typename BaseConcept>
class BaseObservable : public BaseConcept
{
public:
    class Observer
    {
    public:
        virtual void notify() = 0;
    };

};


/// \brief A type that can notifier observers on a change in the node set.
/// \ingroup concepts_graph_attributes
/*!
 *  BaseConcept must derive from BaseObservable
 */
template <typename BaseConcept>
class NodeObservable : public BaseConcept
{
public:

    typedef typename BaseConcept::Observer Observer;

    /// \brief Attach an Observer to the object.
    /*!
     *  The attached object must conform to the Observer concept.
     *  Whenever a change occurs in this object, the observer's
     *  `notify` method will be called without parameters.
     */
    void attachNodeObserver(Observer&) {}

    /// \brief Detach an Observer from the object.
    void detachNodeObserver(Observer&) {}

    template <typename _NodeObservable>
    struct Constraints
    {
        void constraints()
        {
            observable.attachNodeObserver(nvobserver);
            observable.detachNodeObserver(nvobserver);
            nvobserver.notify();
        }
        _NodeObservable& observable;
        typedef typename _NodeObservable::Observer _Observer;

        class NonVirtualObserver : public _Observer
        {
        public:
            virtual void notify() {}
        };

        NonVirtualObserver nvobserver;

    };

};


/// \brief A type that can notifier observers on a change in the arc set.
/// \ingroup concepts_graph_attributes
/*!
 *  BaseConcept must derive from BaseObservable
 */
template <typename BaseConcept>
class ArcObservable : public BaseConcept
{
public:

    typedef typename BaseConcept::Observer Observer;

    /// \brief Attach an Observer to the object.
    /*!
     *  The attached object must conform to the Observer concept.
     *  Whenever a change occurs in this object, the observer's
     *  `notify` method will be called without parameters.
     */
    void attachArcObserver(Observer&) {}

    /// \brief Detach an Observer from the object.
    void detachArcObserver(Observer&) {}

    template <typename _ArcObservable>
    struct Constraints
    {
        void constraints()
        {
            observable.attachArcObserver(nvobserver);
            observable.detachArcObserver(nvobserver);
            nvobserver.notify();
        }
        _ArcObservable& observable;
        typedef typename _ArcObservable::Observer _Observer;

        class NonVirtualObserver : public _Observer
        {
        public:
            virtual void notify() {}
        };

        NonVirtualObserver nvobserver;

    };

};


/// \brief A type that can notifier observers on a change in the edge set.
/// \ingroup concepts_graph_attributes
/*!
 *  BaseConcept must derive from BaseObservable
 */
template <typename BaseConcept>
class EdgeObservable : public BaseConcept
{
public:

    typedef typename BaseConcept::Observer Observer;

    /// \brief Attach an Observer to the object.
    /*!
     *  The attached object must conform to the Observer concept.
     *  Whenever a change occurs in this object, the observer's
     *  `notify` method will be called without parameters.
     */
    void attachEdgeObserver(Observer&) {}

    /// \brief Detach an Observer from the object.
    void detachEdgeObserver(Observer&) {}

    template <typename _EdgeObservable>
    struct Constraints
    {
        void constraints()
        {
            observable.attachEdgeObserver(nvobserver);
            observable.detachEdgeObserver(nvobserver);
            nvobserver.notify();
        }
        _EdgeObservable& observable;
        typedef typename _EdgeObservable::Observer _Observer;

        class NonVirtualObserver : public _Observer
        {
        public:
            virtual void notify() {}
        };

        NonVirtualObserver nvobserver;

    };

};


/// \brief A directed graph that can be read, iterated, and mapped.
/// \ingroup concepts_graph_attributes
/*!
    A ReadableDirectedGraph is a type which represents the most common
    functionality of a directed graph that can be read from, but not written
    to. This includes the ability to iterate over its nodes and arcs.

    A ReadableDirectedGraph conforms to these concepts:
        - NodeIterable
        - ArcIterable
        - ParentIterable
        - ChildIterable
        - DirectedNeighborIterable
        - NodeMappable
        - ArcMappable
*/
class ReadableDirectedGraph :
    public
    NodeIterable <
    ArcIterable <
    ParentIterable <
    ChildIterable <
    DirectedNeighborIterable <
    NodeMappable <
    ArcMappable <
    BaseDirectedGraph > > > > > > >
{
public:

    /// \brief Get the degree of the node.
    /// \pre The node must be valid.
    unsigned int degree(Node) const {
        return 0;
    }

    /// \brief Get the in degree of the node.
    /// \pre The node must be valid.
    unsigned int inDegree(Node) const {
        return 0;
    }

    /// \brief Get the out degree of the node.
    /// \pre The node must be valid.
    unsigned int outDegree(Node) const {
        return 0;
    }

    /// \brief The number of nodes in the graph
    unsigned int numberOfNodes() const {
        return 0;
    }

    /// \brief The number of arcs in the graph
    unsigned int numberOfArcs() const {
        return 0;
    }

    /// \brief Finds an arc in the graph with the given source and target.
    /*!
     *  If no such arc exists, an invalid arc is returned.
     */
    Arc findArc(Node source, Node target) const {
        return Arc();
    }

    /// \brief Get an invalid node
    Node getInvalidNode() const {
        return Node();
    }

    /// \brief Get and invalid arc
    Arc getInvalidArc() const {
        return Arc();
    }


    template <typename _ReadableDirectedGraph>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<
            concepts::NodeIterable<concepts::BaseDirectedGraph>,
                     _ReadableDirectedGraph
                     > ();

            checkConcept<
            concepts::ArcIterable<concepts::BaseDirectedGraph>,
                     _ReadableDirectedGraph
                     > ();

            checkConcept<
            concepts::ParentIterable<concepts::BaseDirectedGraph>,
                     _ReadableDirectedGraph
                     > ();

            checkConcept<
            concepts::ChildIterable<concepts::BaseDirectedGraph>,
                     _ReadableDirectedGraph
                     > ();

            checkConcept<
            concepts::DirectedNeighborIterable<concepts::BaseDirectedGraph>,
                     _ReadableDirectedGraph
                     > ();

            checkConcept<
            concepts::NodeMappable<concepts::BaseDirectedGraph>,
                     _ReadableDirectedGraph
                     > ();

            checkConcept<
            concepts::ArcMappable<concepts::BaseDirectedGraph>,
                     _ReadableDirectedGraph
                     > ();

            unsigned int n = _graph.degree(_Node());
            n = _graph.inDegree(_Node());
            n = _graph.outDegree(_Node());
            n = _graph.numberOfNodes();
            n = _graph.numberOfArcs();
            _Arc arc = _graph.findArc(_Node(), _Node());
            _Node node = _graph.getInvalidNode();
            arc = _graph.getInvalidArc();
            
            ignore_unused_variable_warning(n);
            ignore_unused_variable_warning(node);

        }

        _ReadableDirectedGraph& _graph;
        typedef typename _ReadableDirectedGraph::Node _Node;
        typedef typename _ReadableDirectedGraph::Arc _Arc;
    };

};


/// \brief An undirected graph that can be read and iterated over.
/// \ingroup concepts_graph_attributes
/*!
    A ReadableUndirectedGraph is a type which represents the most common
    functionality of an undirected graph that can be read from, but not written
    to. This includes the ability to iterate over its nodes and edges.

    A ReadableUndirectedGraph conforms to these concepts:
    - NodeIterable
    - EdgeIterable
    - UndirectedNeighborIterable
    - NodeMappable
    - EdgeMappable
*/
class ReadableUndirectedGraph :
    public
    NodeIterable <
    EdgeIterable <
    UndirectedNeighborIterable <
    NodeMappable <
    EdgeMappable <
    BaseUndirectedGraph > > > > >
{
public:

    /// \brief Gets the degree of the node.
    /// \pre The node must be valid.
    unsigned int degree(Node) const {
        return 0;
    }

    /// \brief Gets the "u" node at the end of an edge.
    /// \pre The edge must be valid.
    Node u(Edge) const {
        return Node();
    }

    /// \brief Gets the "v" node at the end of an edge.
    /// \pre The edge must be valid.
    Node v(Edge) const {
        return Node();
    }

    /// \brief The number of nodes in the graph
    unsigned int numberOfNodes() const {
        return 0;
    }

    /// \brief The number of arcs in the graph
    unsigned int numberOfEdges() const {
        return 0;
    }

    /// \brief Finds an edge in the graph with the given source and target.
    /*!
     *  If no such edge exists, an invalid edge is returned.
     */
    Edge findEdge(Node u, Node v) const {
        return Edge();
    }

    /// \brief Get an invalid node
    Node getInvalidNode() const {
        return Node();
    }

    /// \brief Get and invalid arc
    Edge getInvalidEdge() const {
        return Edge();
    }

    template <typename _ReadableUndirectedGraph>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<
            concepts::NodeIterable<concepts::BaseUndirectedGraph>,
                     _ReadableUndirectedGraph
                     > ();

            checkConcept<
            concepts::EdgeIterable<concepts::BaseUndirectedGraph>,
                     _ReadableUndirectedGraph
                     > ();

            checkConcept<
            concepts::UndirectedNeighborIterable<concepts::BaseUndirectedGraph>,
                     _ReadableUndirectedGraph
                     > ();

            checkConcept<
            concepts::NodeMappable<concepts::BaseUndirectedGraph>,
                     _ReadableUndirectedGraph
                     > ();

            checkConcept<
            concepts::EdgeMappable<concepts::BaseUndirectedGraph>,
                     _ReadableUndirectedGraph
                     > ();

            unsigned int n = _graph.degree(_Node());
            _Node node = _graph.u(_Edge());
            node = _graph.v(_Edge());
            n = _graph.numberOfNodes();
            n = _graph.numberOfEdges();
            _Edge edge = _graph.findEdge(node,node);
            node = _graph.getInvalidNode();
            edge = _graph.getInvalidEdge();

            ignore_unused_variable_warning(n);
            ignore_unused_variable_warning(edge);
        }

        _ReadableUndirectedGraph& _graph;
        typedef typename _ReadableUndirectedGraph::Node _Node;
        typedef typename _ReadableUndirectedGraph::Edge _Edge;
    };

};


/// \brief A writable and readable directed graph.
/// \ingroup concepts_graph_attributes
/*!
 *  A WritableReadableDirectedGraph conforms to these concepts:
 *      - ReadableDirectedGraph
 */
class WritableReadableDirectedGraph : public ReadableDirectedGraph
{
public:
    /// \brief Add a node to the graph.
    Node addNode() {
        return Node();
    }

    /// \brief Add an arc to the graph.
    /// \pre Requires that both nodes are in the graph.
    Arc addArc(Node, Node) {
        return Arc();
    }

    /// \brief Remove a node from the graph.
    /// \pre Requires that the node is in the graph.
    void removeNode(Node) { }

    /// \brief Remove an arc from the graph.
    /// \pre Requires that the arc is in the graph.
    void removeArc(Arc) { }

    /// \brief Clears the nodes of the graph.
    void clear() { }

    template <typename _WritableReadableDirectedGraph>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<ReadableDirectedGraph, _WritableReadableDirectedGraph> ();

            _Node node = graph.addNode();
            graph.removeNode(node);

            _Arc arc = graph.addArc(_Node(), _Node());
            graph.removeArc(arc);

            graph.clear();
        }
        _WritableReadableDirectedGraph& graph;
        typedef typename _WritableReadableDirectedGraph::Node _Node;
        typedef typename _WritableReadableDirectedGraph::Arc _Arc;

    };
};


/// \brief A writeable and readable undirected graph.
/// \ingroup concepts_graph_attributes
/*!
 *  A WritableReadableUndirectedGraph conforms to these concepts:
 *      - ReadableUndirectedGraph
 */
class WritableReadableUndirectedGraph : public ReadableUndirectedGraph
{
public:

    /// \brief Add a node to the graph
    Node addNode() {
        return Node();
    }

    /// \brief Add an edge to the graph
    /// \pre Both nodes must be in the graph.
    Edge addEdge(Node, Node) {
        return Edge();
    }

    /// \brief Remove a node.
    /// \pre The node must be in the graph.
    void removeNode(Node) { }

    /// \brief Remove an edge.
    /// \pre The edge must be in the graph.
    void removeEdge(Edge) { }

    /// \brief Clear the nodes of the graph
    void clear() { }


    template <typename _WritableReadableUndirectedGraph>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<ReadableUndirectedGraph,   _WritableReadableUndirectedGraph> ();

            _Node node = graph.addNode();
            graph.removeNode(node);

            _Edge edge = graph.addEdge(_Node(), _Node());
            graph.removeEdge(edge);

            graph.clear();
        }
        _WritableReadableUndirectedGraph& graph;
        typedef typename _WritableReadableUndirectedGraph::Node _Node;
        typedef typename _WritableReadableUndirectedGraph::Edge _Edge;

    };

};


}
}

#endif
