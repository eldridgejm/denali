#ifndef DENALI_FOLDED_H
#define DENALI_FOLDED_H

#include <denali/graph_structures.h>
#include <denali/graph_maps.h>

#include <boost/shared_ptr.hpp>

namespace denali {


    template <typename GraphType>
    class UndirectedSpliceTree :
            public
            ReadableUndirectedGraphMixin <GraphType,
            BaseGraphMixin <GraphType> >
    {
        typedef
        ReadableUndirectedGraphMixin <GraphType,
        BaseGraphMixin <GraphType> >
        Mixin;

    public:
        typedef typename GraphType::Node Node;
        typedef typename GraphType::Edge Edge;

        GraphType _graph;

        ObservingNodeMap<GraphType, unsigned int> _node_owner;
        ObservingEdgeMap<GraphType, unsigned int> _edge_owner;

    public:

        UndirectedSpliceTree()
            : Mixin(_graph) {}

        Node addNode(unsigned int owner)
        {
            Node node = _graph.addNode();
            _node_owner[node] = owner;
            return node;
        }

        Edge addEdge(Node u, Node v, unsigned int owner)
        {
            Edge edge = _graph.addEdge(u,v);
            _edge_owner[edge] = owner;
            return edge; 
        }

        void removeNode(Node node) { _graph.removeNode(node); }
        void removeEdge(Edge edge) { _graph.removeEdge(edge); }

        void clearNodes() const { _graph.clearNodes(); }
        void clearEdges() const { _graph.clearEdges(); }

        unsigned int getNodeOwner(Node node) const { return _node_owner[node]; }
        unsigned int getEdgeOwner(Edge edge) const { return _edge_owner[edge]; }

        template <typename Tree>
        void splice(
                const Tree& tree, 
                Edge replaced_edge, 
                Node replaced_parent, 
                typename Tree::Edge donor_edge,
                typename Tree::Node donor_node)
        {

        }


    };
 



}


#endif
