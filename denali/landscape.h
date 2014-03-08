#ifndef DENALI_LANDSCAPE_H
#define DENALI_LANDSCAPE_H

#include <denali/graph_structures.h>
#include <denali/graph_mixins.h>
#include <denali/graph_iterators.h>
#include <stack>

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

    GraphType _graph;
    const ContourTree& _contour_tree;
    typename GraphType::Node _root;

    StaticNodeMap<ContourTree, typename GraphType::Node> _ct_node_to_lscape_node;
    StaticEdgeMap<ContourTree, typename GraphType::Arc> _ct_edge_to_lscape_arc;

    ObservingNodeMap<GraphType, typename ContourTree::Node> _lscape_node_to_ct_node;
    ObservingArcMap<GraphType, typename ContourTree::Edge> _lscape_arc_to_ct_edge;

public:

    typedef typename GraphType::Node Node;
    typedef typename GraphType::Arc Arc;
    typedef typename ContourTree::Members Members;

    LandscapeTreeBase(
        const ContourTree& contour_tree,
        typename ContourTree::Node root)
        : Mixin(_graph), _contour_tree(contour_tree), _ct_node_to_lscape_node(contour_tree),
          _ct_edge_to_lscape_arc(contour_tree), _lscape_node_to_ct_node(_graph),
          _lscape_arc_to_ct_edge(_graph)
    {
        _root = addNode(root);
    }

    Node addNode(typename ContourTree::Node ct_node)
    {
        Node node = _graph.addNode();
        _ct_node_to_lscape_node[ct_node] = node;
        _lscape_node_to_ct_node[node] = ct_node;

        return node;
    }

    Arc addArc(Node parent, Node child, typename ContourTree::Edge ct_edge)
    {
        Arc arc = _graph.addArc(parent, child);

        _ct_edge_to_lscape_arc[ct_edge] = arc;
        _lscape_arc_to_ct_edge[arc] = ct_edge;
        return arc;
    }

    typename ContourTree::Node getContourTreeNode(Node node) const
    {
        return _lscape_node_to_ct_node[node];
    }

    typename ContourTree::Edge getContourTreeEdge(Arc arc) const
    {
        return _lscape_arc_to_ct_edge[arc];
    }

    Node getLandscapeTreeNode(typename ContourTree::Node node) const
    {
        return _ct_node_to_lscape_node[node];
    }

    Arc getLandscapeTreeArc(typename ContourTree::Edge edge) const
    {
        return _ct_edge_to_lscape_arc[edge];
    }

    Node getRoot() const {
        return _root;
    }

    double getValue(Node node) const
    {
        return _contour_tree.getValue(_lscape_node_to_ct_node[node]);
    }

    unsigned int getID(Node node) const
    {
        return _contour_tree.getID(_lscape_node_to_ct_node[node]);
    }

    const Members& getNodeMembers(Node node) const
    {
        return _contour_tree.getNodeMembers(_lscape_node_to_ct_node[node]);
    }

    Node getNode(unsigned int id) const
    {
        return _ct_node_to_lscape_node[_contour_tree.getNode(id)];
    }

    const Members& getArcMembers(Arc arc) const
    {
        return _contour_tree.getEdgeMembers(_lscape_arc_to_ct_edge[arc]);
    }
};


/// \brief A rooted contour tree.
/// \ingroup landscape
/*!
 *  Builds a landscape tree, which is a rooted contour tree. There is a 1-1
 *  relationship between the nodes of the contour tree and the landscape
 *  tree, as well as between the edges of the contour tree and the arcs of
 *  the landscape tree.
 */
template <typename ContourTree>
class LandscapeTree :
    public
    ArcObservableMixin < LandscapeTreeBase<ContourTree, DirectedGraph>,
    NodeObservableMixin < LandscapeTreeBase<ContourTree, DirectedGraph>,
    ReadableDirectedGraphMixin <LandscapeTreeBase<ContourTree, DirectedGraph>,
    BaseGraphMixin <LandscapeTreeBase<ContourTree, DirectedGraph> > > > >

{
    typedef
    ArcObservableMixin < LandscapeTreeBase<ContourTree, DirectedGraph>,
                       NodeObservableMixin < LandscapeTreeBase<ContourTree, DirectedGraph>,
                       ReadableDirectedGraphMixin <LandscapeTreeBase<ContourTree, DirectedGraph>,
                       BaseGraphMixin <LandscapeTreeBase<ContourTree, DirectedGraph> > > > >
                       Mixin;

    LandscapeTreeBase<ContourTree, DirectedGraph> _tree;

public:

    typedef typename Mixin::Node Node;
    typedef typename Mixin::Arc Arc;
    typedef typename LandscapeTreeBase<ContourTree, DirectedGraph>::Members Members;

    /// \brief Build a landscape tree from the contour tree.
    /*!
     *  This computes a landscape tree by performing a BFS from the root
     *  node, resulting in a directed acyclic graph.
     */
    LandscapeTree(
        const ContourTree& contour_tree,
        typename ContourTree::Node root)
        : Mixin(_tree), _tree(contour_tree, root)
    {
        // the root has already been added to the tree
        // do a search from the root
        for (UndirectedBFSIterator<ContourTree> it(contour_tree, root);
                !it.done(); ++it) {
            // add the new node to the landscape
            _tree.addNode(it.child());

            // add the new arc to the landscape
            _tree.addArc(
                _tree.getLandscapeTreeNode(it.parent()),
                _tree.getLandscapeTreeNode(it.child()),
                it.edge());
        }
    }

    /// \brief Get the root of the landscape tree.
    Node getRoot() const {
        return _tree.getRoot();
    }

    /// \brief Retrieve the corresponding node in the contour tree.
    typename ContourTree::Node getContourTreeNode(Node node) const
    {
        return _tree.getContourTreeNode(node);
    }

    /// \brief Retrieve the corresponding edge in the contour tree.
    typename ContourTree::Edge getContourTreeEdge(Arc arc) const
    {
        return _tree.getContourTreeEdge(arc);
    }

    /// \brief Get the node corresponding to the contour tree node.
    Node getLandscapeTreeNode(typename ContourTree::Node node) const
    {
        return _tree.getLandscapeTreeNode(node);
    }

    /// \brief Get the arc corresponding to the contour tree edge.
    Arc getLandscapeTreeArc(typename ContourTree::Edge edge) const
    {
        return _tree.getLandscapeTreeArc(edge);
    }

    /// \brief Get the value at the node.
    double getValue(Node node) const
    {
        return _tree.getValue(node);
    }

    /// \brief Get the ID of the node.
    unsigned int getID(Node node) const
    {
        return _tree.getID(node);
    }

    /// \brief Retrieve the members of the node.
    const Members& getNodeMembers(Node node) const
    {
        return _tree.getNodeMembers(node);
    }

    /// \brief Get the node from an ID.
    Node getNode(unsigned int id) const
    {
        return _tree.getNode(id);
    }

    /// \brief Retrieve the members of the arc.
    const Members& getArcMembers(Arc arc) const
    {
        return _tree.getArcMembers(arc);
    }
};


/// \brief Represents weights of nodes and arcs in the landscape tree.
/// \ingroup landscape
/*!
 *  Weights are computed as follows:
 *   - The weight of a node is the number of members it contains, plus one (for itself).
 *   - The weight of an arc is the number of members it contains.
 *   - The total weight of a node it its node weight, plus the weight of all of its
 *     out arcs, plus the total weight of all of its children.
 */
template <typename LandscapeTree>
class LandscapeWeights
{
    typedef typename LandscapeTree::Node Node;
    typedef typename LandscapeTree::Arc Arc;

    const LandscapeTree& _tree;

    StaticNodeMap<LandscapeTree, double> _node_to_weight;
    StaticNodeMap<LandscapeTree, double> _node_to_total_weight;
    StaticArcMap<LandscapeTree, double> _arc_to_weight;

private:
    double computeNodeWeight(Node node)
    {
        return _tree.getNodeMembers(node).size() + 1;
    }

    double computeArcWeight(Arc arc)
    {
        return _tree.getArcMembers(arc).size();
    }

public:
    LandscapeWeights(const LandscapeTree& tree)
        : _tree(tree), _node_to_weight(_tree), _node_to_total_weight(_tree),
          _arc_to_weight(_tree)
    {
        // first, build a stack of the nodes in order of BFS visit, while recording
        // their weights
        std::stack<Node> _bfs_nodes;

        // handle the root node
        _node_to_weight[_tree.getRoot()] = computeNodeWeight(_tree.getRoot());
        _bfs_nodes.push(tree.getRoot());

        // handle the rest of the nodes
        for (DirectedBFSIterator<LandscapeTree> it(tree, tree.getRoot());
                !it.done(); ++it) {

            _bfs_nodes.push(it.child());

            // get the weight of the arc
            _arc_to_weight[it.arc()] = computeArcWeight(it.arc());

            // get the weight of the child node
            _node_to_weight[it.child()] = computeNodeWeight(it.child());
        }

        // now recurse to compute the total weight of each node
        while (_bfs_nodes.size() > 0) {
            Node node = _bfs_nodes.top();
            _bfs_nodes.pop();

            double total_weight = _node_to_weight[node];
            for (ChildIterator<LandscapeTree> it(tree, node); !it.done(); ++it) {
                total_weight += _arc_to_weight[it.arc()];
                total_weight += _node_to_total_weight[it.child()];
            }
            _node_to_total_weight[node] = total_weight;
        }
    }

    /// \brief Get the total weight of the node.
    double getTotalNodeWeight(Node node) const {
        return _node_to_total_weight[node];
    }

    /// \brief Get the weight of the node.
    double getNodeWeight(Node node) const {
        return _node_to_weight[node];
    }

    /// \brief Get the weight of the arc.
    double getArcWeight(Arc arc) const {
        return _arc_to_weight[arc];
    }

};


}


#endif
