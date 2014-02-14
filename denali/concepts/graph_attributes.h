#ifndef DENALI_GRAPH_ATTRIBUTES_H
#define DENALI_GRAPH_ATTRIBUTES_H

#include <denali/concepts/graph_objects.h>

namespace denali {
    namespace concepts {

        /// \brief A type which supports iteration over its nodes.
        /// \ingroup concepts_graph_attributes
        /*! An object conforming to the NodeIterable concept allows iteration
            over its nodes. If provides a NodeIterator convenience class which
            conforms to the NodeIterator concept, and provides methods for manual
            iteration over the nodes.
            \sa ArcIterable, EdgeIterable
        */
        template <typename Base>
        class NodeIterable : public Base
        {
            public:

            typedef concepts::Node Node;

            /// Checks if the node is valid.
            bool isValid(Node) const { return true; }

            /// Gets the first node from the graph.
            Node getFirstNode() const { return Node(); } 

            /// Gets the next node from the graph.
            Node getNextNode(Node) const { return Node(); }

            template <typename _NodeIterable>
            struct Constraints
            {
                void constraints()
                {
                    checkConcept<concepts::Node, _Node> ();

                    _Node node;
                    iterable.isValid(node);
                    node = iterable.getFirstNode();
                    node = iterable.getNextNode(node);
                }
                typedef typename _NodeIterable::Node _Node;
                _NodeIterable& iterable;
            };
        };

        /// \brief A type which supports iteration over its arcs. 
        /// \ingroup concepts_graph_attributes
        //
        /// \sa NodeIterable, EdgeIterable
        class ArcIterable
        {
            public:

            typedef concepts::Arc Arc;

            bool isValid(Arc) const { return true; }
            Arc getFirstArc() const { return Arc(); } 
            Arc getNextArc(Arc) const { return Arc(); }

            template <typename _ArcIterable>
            struct Constraints
            {
                void constraints()
                {
                    checkConcept<concepts::Arc, _Arc> ();

                    _Arc arc;
                    iterable.isValid(arc);
                    arc = iterable.getFirstArc();
                    arc = iterable.getNextArc(arc);
                }
                typedef typename _ArcIterable::Arc _Arc;
                _ArcIterable& iterable;
            };
        };


        /// \brief A type which supports iteration over its edges. 
        /// \ingroup concepts_graph_attributes

        /// \sa NodeIterable, ArcIterable
        class EdgeIterable
        {
            public:

            typedef concepts::Edge Edge;

            /// Checks that the edge is valid.
            bool isValid(Edge) const { return true; }

            /// Gets the first edge from the graph.
            Edge getFirstEdge() const { return Edge(); } 

            /// Gets the next edge from the graph.
            Edge getNextEdge(Edge) const { return Edge(); }

            template <typename _EdgeIterable>
            struct Constraints
            {
                void constraints()
                {
                    checkConcept<concepts::Edge, _Edge>();

                    _Edge edge;
                    iterable.isValid(edge);
                    edge = iterable.getFirstEdge();
                    edge = iterable.getNextEdge(edge);

                }
                typedef typename _EdgeIterable::Edge _Edge;
                _EdgeIterable& iterable;
            };
        };




    }
}

#endif
