#ifndef DENALI_FOLDED_H
#define DENALI_FOLDED_H

#include <denali/graph_structures.h>
#include <denali/graph_maps.h>

#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>

namespace denali {

    ////////////////////////////////////////////////////////////////////////////
    //
    // UndirectedSpliceTree
    //
    ////////////////////////////////////////////////////////////////////////////

    template <typename AlphaTree, typename BetaTree>
    class UndirectedSpliceTree :
            public
            ReadableUndirectedGraphMixin <UndirectedGraph,
            BaseGraphMixin <UndirectedGraph> >
    {
        typedef
        ReadableUndirectedGraphMixin <UndirectedGraph,
        BaseGraphMixin <UndirectedGraph> >
        Mixin;

    public:
        typedef UndirectedGraph::Node Node;
        typedef UndirectedGraph::Edge Edge;

        typedef typename AlphaTree::Node AlphaNode;
        typedef typename AlphaTree::Edge AlphaEdge;

        typedef typename BetaTree::Node BetaNode;
        typedef typename BetaTree::Edge BetaEdge;

    private:
        UndirectedGraph _graph;
        const AlphaTree& _alpha;
        const BetaTree& _beta;

        typedef boost::variant<AlphaNode, BetaNode> NodeVariant;
        typedef boost::variant<AlphaEdge, BetaEdge> EdgeVariant;

        ObservingNodeMap<UndirectedGraph, bool> _is_node_alpha;
        ObservingEdgeMap<UndirectedGraph, bool> _is_edge_alpha;

        ObservingNodeMap<UndirectedGraph, NodeVariant> _spliced_to_donor_node;
        ObservingEdgeMap<UndirectedGraph, EdgeVariant> _spliced_to_donor_edge;

        StaticNodeMap<AlphaTree, Node> _alpha_to_spliced_node;
        StaticEdgeMap<AlphaTree, Edge> _alpha_to_spliced_edge;

        StaticNodeMap<BetaTree, Node> _beta_to_spliced_node;
        StaticEdgeMap<BetaTree, Edge> _beta_to_spliced_edge;

    public:

        
        UndirectedSpliceTree(const AlphaTree& alpha, const BetaTree& beta)
            : _alpha(alpha), _beta(beta), Mixin(_graph),
              _is_node_alpha(_graph), _is_edge_alpha(_graph),
              _spliced_to_donor_node(_graph), _spliced_to_donor_edge(_graph),
              _alpha_to_spliced_node(_alpha), _alpha_to_spliced_edge(_alpha),
              _beta_to_spliced_node(_beta), _beta_to_spliced_edge(_beta)
            {}

        Node addAlphaNode(AlphaNode alpha_node)
        {
            Node node = _graph.addNode();
            _is_node_alpha[node] = true;
            _spliced_to_donor_node[node] = NodeVariant(alpha_node);
            _alpha_to_spliced_node[alpha_node] = node;
            return node;
        }

        Node addBetaNode(BetaNode beta_node)
        {
            Node node = _graph.addNode();
            _is_node_alpha[node] = false;
            _spliced_to_donor_node[node] = NodeVariant(beta_node);
            _beta_to_spliced_node[beta_node] = node;
            return node;
        }

        void removeNode(Node node)
        {
            _graph.removeNode(node);
        }

        Edge addAlphaEdge(Node u, Node v, AlphaEdge alpha_edge)
        {
            Edge edge = _graph.addEdge(u,v);
            _is_edge_alpha[edge] = true;
            _spliced_to_donor_edge[edge] = alpha_edge;
            _alpha_to_spliced_edge[alpha_edge] = edge;
            return edge;
        }

        Edge addBetaEdge(Node u, Node v, BetaEdge beta_edge)
        {
            Edge edge = _graph.addEdge(u,v);
            _is_edge_alpha[edge] = false;
            _spliced_to_donor_edge[edge] = beta_edge;
            _beta_to_spliced_edge[beta_edge] = edge;
            return edge;
        }

        void removeEdge(Edge edge)
        {
            _graph.removeEdge(edge);
        }

        AlphaNode getAlphaNode(Node node)
        {
            return boost::get<AlphaNode>(_spliced_to_donor_node[node]);
        }

        BetaNode getBetaNode(Node node)
        {
            return boost::get<BetaNode>(_spliced_to_donor_node[node]);
        }

        AlphaEdge getAlphaEdge(Edge edge)
        {
            return boost::get<AlphaEdge>(_spliced_to_donor_edge[edge]);
        }

        BetaEdge getBetaEdge(Edge edge)
        {
            return boost::get<BetaEdge>(_spliced_to_donor_edge[edge]);
        }

        bool isNodeAlpha(Node node)
        {
            return _is_node_alpha[node];
        }

        bool isEdgeAlpha(Edge edge)
        {
            return _is_edge_alpha[edge];
        }

        Node getNodeFromAlpha(AlphaNode alpha_node)
        {
            return _alpha_to_spliced_node[alpha_node];
        }

        Node getNodeFromBeta(BetaNode beta_node)
        {
            return _beta_to_spliced_node[beta_node];
        }

        Edge getEdgeFromAlpha(AlphaEdge alpha_edge)
        {
            return _alpha_to_spliced_edge[alpha_edge];
        }

        Edge getEdgeFromBeta(BetaEdge beta_edge)
        {
            return _beta_to_spliced_edge[beta_edge];
        }

        void spliceFromAlpha(
                Node this_parent, 
                Edge this_edge,
                AlphaNode alpha_parent,
                AlphaEdge alpha_edge)
        {
            Node this_child = _graph.opposite(this_parent, this_edge);
            AlphaNode alpha_child = _alpha.opposite(alpha_parent, alpha_edge);

            // remove the subtree
            removeSubtree(this_parent, this_child);

            // now splice in the alpha tree
            Node child = addAlphaNode(alpha_child);
            addAlphaEdge(this_parent, child, alpha_edge);

            for (UndirectedBFSIterator<AlphaTree> it(_alpha, alpha_parent, alpha_child);
                    !it.done(); ++it) {
                // we have already added the parent, so just add the child
                Node child = addAlphaNode(it.child());

                // connect it to its parent
                addAlphaEdge(getNodeFromAlpha(it.parent()), child, it.edge());
            }
        }

        void spliceFromBeta(
                Node this_parent, 
                Edge this_edge,
                BetaNode beta_parent,
                BetaEdge beta_edge)
        {
            Node this_child = _graph.opposite(this_parent, this_edge);
            BetaNode beta_child = _beta.opposite(beta_parent, beta_edge);

            // remove the subtree
            removeSubtree(this_parent, this_child);

            // now splice in the beta tree
            Node child = addBetaNode(beta_child);
            addBetaEdge(this_parent, child, beta_edge);

            for (UndirectedBFSIterator<BetaTree> it(_beta, beta_parent, beta_child);
                    !it.done(); ++it) {
                // we have already added the parent, so just add the child
                Node child = addBetaNode(it.child());

                // connect it to its parent
                addBetaEdge(getNodeFromBeta(it.parent()), child, it.edge());
            }
        }


        static UndirectedSpliceTree<AlphaTree, BetaTree>
        fromAlphaTree(const AlphaTree& alpha, const BetaTree& beta)
        {
            UndirectedSpliceTree<AlphaTree, BetaTree> splice_tree(alpha, beta);

            // copy the structure of the alpha tree into this
            for (NodeIterator<AlphaTree> it(alpha); !it.done(); ++it) {
                splice_tree.addAlphaNode(it.node());
            }

            for (EdgeIterator<AlphaTree> it(alpha); !it.done(); ++it) {
                AlphaNode alpha_u = alpha.u(it.edge());
                AlphaNode alpha_v = alpha.v(it.edge());
                Node u = splice_tree.getNodeFromAlpha(alpha_u);
                Node v = splice_tree.getNodeFromAlpha(alpha_v);
                splice_tree.addAlphaEdge(u, v, it.edge());
            }

            return splice_tree;
        }

    private:
        
        void removeSubtree(
                Node this_parent,
                Node this_child)
        {
            // make a list of nodes to delete
            std::list<Node> marked_nodes;
            marked_nodes.push_back(this_child);

            for (UndirectedBFSIterator<UndirectedGraph> it(_graph, this_parent, this_child);
                    !it.done(); ++it) {
                // the parent has already been marked
                marked_nodes.push_back(it.child());
            }

            // actually remove the nodes
            for (std::list<Node>::iterator it = marked_nodes.begin();
                    it != marked_nodes.end(); ++it) {
                removeNode(*it);
            }
        }

    };


}


#endif
