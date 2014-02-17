#ifndef DENALI_LANDSCAPE_H
#define DENALI_LANDSCAPE_H

#include <denali/graph_iterators.h>

namespace denali {

    template <typename ContourTree, typename GraphType>
    class LandscapeTreeBase :
            public
            ArcObservableMixin < GraphType,
            NodeObservableMixin < GraphType, 
            ReadableDirectedGraphMixin <GraphType,
            BaseGraphMixin <GraphType> > > >
    {
        typedef
        ArcObservableMixin < GraphType,
        NodeObservableMixin < GraphType, 
        ReadableDirectedGraphMixin <GraphType,
        BaseGraphMixin <GraphType> > > >
        Mixin;

        typedef typename GraphType::Node Node;
        typedef typename GraphType::Arc Arc;

        GraphType _graph;
        const ContourTree& _contour_tree;
        typename GraphType::Node _root;

        StaticNodeMap<ContourTree, Node> _ct_node_to_lscape_node;
        StaticEdgeMap<ContourTree, Arc> _ct_edge_to_lscape_arc;

        ObservingNodeMap<GraphType, typename ContourTree::Node> _lscape_node_to_ct_node;
        ObservingArcMap<GraphType, typename ContourTree::Edge> _lscape_arc_to_ct_edge;

    private:
        void addNode(typename ContourTree::Node ct_node)
        {
            Node node = _graph.addNode();
            _ct_node_to_lscape_node[ct_node] = node;
            _lscape_node_to_ct_node[node] = ct_node;
        }

        void addArc(typename ContourTree::Edge ct_edge)
        {
            typename ContourTree::Node ct_parent = _contour_tree.source(ct_edge);
            typename ContourTree::Node ct_child = _contour_tree.target(ct_edge);

            Node parent = _ct_node_to_lscape_node[ct_parent];
            Node child = _ct_node_to_lscape_node[ct_child];

            Arc arc = _graph.addEdge(parent, child);

            _ct_edge_to_lscape_arc[ct_edge] = arc;
            _lscape_arc_to_ct_edge[arc] = ct_edge;
        }


    protected:
        LandscapeTreeBase(
                const ContourTree& contour_tree, 
                typename ContourTree::Node root)
            : _contour_tree(contour_tree), _ct_node_to_lscape_node(contour_tree),
              _ct_edge_to_lscape_arc(contour_tree), _lscape_node_to_ct_node(_graph),
              _lscape_arc_to_ct_edge(_graph), Mixin(_graph)
        {
            // start a tree at the root
            addNode(root);

            // build the rest of the tree with a BFS
            for (UndirectedBFSIterator<ContourTree> it(contour_tree, root);
                    !it.done();
                    ++it) {

                // add the child node to the landscape
                addNode(it.child());

                // add the arc to the landscape
                addArc(it.edge());
            }
        }

    };


}


#endif
