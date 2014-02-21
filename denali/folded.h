#ifndef DENALI_FOLDED_H
#define DENALI_FOLDED_H

#include <denali/graph_structures.h>
#include <denali/graph_maps.h>

#include <boost/shared_ptr.hpp>

namespace denali {

    ////////////////////////////////////////////////////////////////////////////
    //
    // UndirectedSpliceTree
    //
    ////////////////////////////////////////////////////////////////////////////

    template <typename GraphType>
    class UndirectedSpliceTreeBase :
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

        UndirectedSpliceTreeBase()
            : Mixin(_graph), _node_owner(_graph), _edge_owner(_graph) {}

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
                Edge this_edge, 
                Node this_parent, 
                const Tree& tree, 
                typename Tree::Edge donor_edge,
                typename Tree::Node donor_node)
        {
            Node this_child = _graph.opposite(this_parent, this_edge);

            // first, we build a list of nodes to remove
            std::list<Node> nodes_to_remove;
            for (UndirectedBFSIterator<GraphType> it(_graph, this_parent, this_child);
                    !it.done(); ++it) {
                nodes_to_remove.push_back(it.child());
            }

            // now actually remove the nodes
            for (typename std::list<Node>::iterator it = nodes_to_remove.begin();
                    it != nodes_to_remove.end(); ++it) {
                _graph.removeNode(*it);
            }
        }
    };


    /// \brief Represents the splicing of multiple trees.
    class UndirectedSpliceTree : public UndirectedSpliceTreeBase<UndirectedGraph>
    {
        typedef UndirectedSpliceTreeBase<UndirectedGraph> Base;
    public:
        typedef typename Base::Node Node;
        typedef typename Base::Edge Edge;

        template <typename DonorTree>
        static UndirectedSpliceTree
        fromDonor(DonorTree& donor, unsigned int owner=0)
        {
            // create a new splice tree
            UndirectedSpliceTree splice_tree;

            std::cout << "Donor has " << donor.numberOfNodes() << std::endl;

            // add the nodes from the donor to the splice tree
            for (NodeIterator<DonorTree> it(donor); !it.done(); ++it) {
                Node node = splice_tree.addNode(owner);
                donor.setSpliceTreeNode(it.node(), node);
            }

            for (EdgeIterator<DonorTree> it(donor); !it.done(); ++it) {
                Node u = donor.getSpliceTreeNode(donor.u(it.edge()));
                Node v = donor.getSpliceTreeNode(donor.v(it.edge()));
                Edge edge = splice_tree.addEdge(u,v,owner);
                donor.setSpliceTreeEdge(it.edge(), edge);
            }

            return splice_tree;
        }

    };


    /// \brief A mixin that allows a tree to become a donor in a splicing.
    template <typename TreeType>
    class UndirectedDonorTreeMixin : 
            public 
            ReadableUndirectedGraphMixin <TreeType,
            BaseGraphMixin <TreeType> >
    {
        typedef
        ReadableUndirectedGraphMixin <TreeType,
        BaseGraphMixin <TreeType> >
        Mixin;

        StaticNodeMap<TreeType, UndirectedSpliceTree::Node> _donor_to_spliced_node;
        StaticEdgeMap<TreeType, UndirectedSpliceTree::Edge> _donor_to_spliced_edge;

    public:
        typedef typename TreeType::Node Node;
        typedef typename TreeType::Edge Edge;

        UndirectedDonorTreeMixin(const TreeType& tree)
                : Mixin(tree), _donor_to_spliced_node(tree), 
                  _donor_to_spliced_edge(tree) {}

        UndirectedSpliceTree::Node getSpliceTreeNode(Node node)
        {
            return _donor_to_spliced_node[node];
        }

        UndirectedSpliceTree::Edge getSpliceTreeEdge(Edge edge)
        {
            return _donor_to_spliced_edge[edge];
        }

        void setSpliceTreeNode(Node node, UndirectedSpliceTree::Node splice_node)
        {
            _donor_to_spliced_node[node] = splice_node;
        }

        void setSpliceTreeEdge(Edge edge, UndirectedSpliceTree::Edge splice_edge)
        {
            _donor_to_spliced_edge[edge] = splice_edge;
        }
    };



}


#endif
