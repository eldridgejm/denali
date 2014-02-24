#ifndef DENALI_SIMPLIFY_H
#define DENALI_SIMPLIFY_H

#include <denali/graph_mixins.h>
#include <denali/graph_iterators.h>
#include <denali/folded.h>

#include <boost/shared_ptr.hpp>
#include <stdexcept>
#include <queue>
#include <vector>

namespace denali {

////////////////////////////////////////////////////////////////////////////////
//
// Priority
//
////////////////////////////////////////////////////////////////////////////////

class Priority
{
    double _priority;

public:
    Priority(double priority) : _priority(priority) {}

    double priority() const {
        return _priority;
    }
};

bool operator<(const Priority& lhs, const Priority& rhs) {
    return lhs.priority() < rhs.priority();
}

////////////////////////////////////////////////////////////////////////////////
//
// Persistence Simplifier
//
////////////////////////////////////////////////////////////////////////////////

class PersistenceSimplifier
{
    double _threshold;

    template <typename Node>
    class PersistencePriority : public Priority
    {
        double _persistence;
        Node _leaf;

    public:
        PersistencePriority(Node leaf, double persistence)
            : Priority(1/(persistence+1)), _persistence(persistence),
              _leaf(leaf) {}

        double persistence() const {
            return _persistence;
        }

        Node leaf() const {
            return _leaf;
        }
    };

    template <typename Tree>
    static double computePersistence(
            const Tree& tree, 
            typename Tree::Edge edge)
    {
        typename Tree::Node u = tree.u(edge);
        typename Tree::Node v = tree.v(edge);

        return std::abs(tree.getValue(u) - tree.getValue(v));
    }

    template <typename Tree>
    static typename Tree::Node getLeaf(const Tree& tree, typename Tree::Edge edge) {
        return tree.degree(tree.u(edge)) == 1 ? tree.u(edge) : tree.v(edge);
    }

public:
    PersistenceSimplifier(double threshold) {
        setThreshold(threshold);
    }

    double getThreshold() const {
        return _threshold;
    }

    void setThreshold(double threshold) {
        if (threshold <= 0) {
            throw std::runtime_error("Threshold must be positive.");
        }
        _threshold = threshold;
    }

    template <typename Tree>
    static unsigned int upDegree(const Tree& tree, typename Tree::Node node)
    {
        unsigned int n = 0;
        double node_value = tree.getValue(node);

        for (UndirectedNeighborIterator<Tree> it(tree, node); !it.done(); ++it)
        {
            if (tree.getValue(it.neighbor()) > node_value) {
                node_value++;
            }
        }
    }

    template <typename Tree>
    static unsigned int downDegree(const Tree& tree, typename Tree::Node node)
    {
        unsigned int n = 0;
        double node_value = tree.getValue(node);

        for (UndirectedNeighborIterator<Tree> it(tree, node); !it.done(); ++it)
        {
            if (tree.getValue(it.neighbor()) <= node_value) {
                node_value++;
            }
        }
    }

    template <typename Tree>
    static bool preserveForReduction(
            const Tree& tree, 
            typename Tree::Edge edge)
    {
        typename Tree::Node child = getLeaf(tree, edge);
        typename Tree::Node parent = tree.opposite(child, edge);

        double parent_value = tree.getValue(parent);
        double child_value = tree.getValue(child);

        if ((child_value <= parent_value) && (downDegree(tree, parent) == 1)) {
            return true;
        } else if ((child_value > parent_value) && (upDegree(tree, parent) == 1)) {
            return true;
        } else {
            return false;
        }
    }

    /// \brief Simplifies the contour tree in the context.
    template <typename Context>
    void simplify(Context& context)
    {
        typedef PersistencePriority<typename Context::Node> Priority;
        typedef typename Context::Node Node;
        typedef typename Context::Edge Edge;

        // make a priority queue of PersistencePriorities
        std::priority_queue<Priority> simplify_queue;

        // add every leaf edge to the queue
        for (EdgeIterator<Context> it(context); !it.done(); ++it)
        {
            Node u = context.u(it.edge());
            Node v = context.v(it.edge());

            if ((context.degree(u) == 1) || (context.degree(v) == 1)) 
            {
                // determine which is the leaf
                Node leaf = getLeaf(context, it.edge());

                // compute the persistence
                double persistence = computePersistence(context, it.edge());

                // enqueue
                simplify_queue.push(Priority(leaf, persistence));
            }
        }

        while (simplify_queue.size() > 0)
        {
            // get the leaf off of the queue
            Node leaf           = simplify_queue.top().leaf();
            double persistence  = simplify_queue.top().persistence();
            simplify_queue.pop();

            // get the leaf edge and the parent
            UndirectedNeighborIterator<Context> neighbor_it(context, leaf);
            Edge edge = neighbor_it.edge();
            Node parent = context.opposite(leaf, edge);

            if (preserveForReduction(context, edge)) {
                continue;
            }

            if (persistence > _threshold) {
                break;
            }

            // collapse the edge
            context.collapse(edge);

            // if the parent is reducible, reduce it now
            if (context.degree(parent) == 2) {
                context.reduce(parent);
            }
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
//
// FoldedContourTreeSimplificationContext
//
////////////////////////////////////////////////////////////////////////////////

/// \brief A simplification context that results in a folded tree.
template <typename ContourTree>
class FoldedContourTreeSimplificationContext :
        public
        ReadableUndirectedGraphMixin <FoldTree,
        BaseGraphMixin <FoldTree> >
{
public:
    typedef FoldTree::Node Node;
    typedef FoldTree::Edge Edge;

private:
    typedef
    ReadableUndirectedGraphMixin <FoldTree,
    BaseGraphMixin <FoldTree> >
    Mixin;

    const ContourTree& _contour_tree;
    FoldTree _fold_tree;


    // map the contour tree nodes to the fold tree nodes so we can later make
    // the correct edges.
    StaticNodeMap<ContourTree, Node> _ct_to_fold_node;

    std::vector<typename ContourTree::Node> _fold_id_to_ct_node;
    std::vector<typename ContourTree::Edge> _fold_id_to_ct_edge;

    typename ContourTree::Node getContourTreeNode(Node node) const
    {
        size_t id = _fold_tree.getNodeFold(node).getIdentifier();
        return _fold_id_to_ct_node[id];
    }

    typename ContourTree::Node getContourTreeEdge(Node node) const
    {
        size_t id = _fold_tree.getNodeFold(node).getIdentifier();
        return _fold_id_to_ct_node[id];
    }


public:
    FoldedContourTreeSimplificationContext(const ContourTree& contour_tree)
            : Mixin(_fold_tree), _contour_tree(contour_tree), 
              _ct_to_fold_node(_contour_tree)
    {
        // we need to initialize the fold tree with the structure of the 
        // contour tree. We also want to map the folds to their corresponding
        // contour tree nodes and edges.

        // add each node in turn
        for (NodeIterator<ContourTree> it(_contour_tree); !it.done(); ++it) {
            // add the node
            Node node = _fold_tree.addNode();

            // map the node fold id to the contour tree node
            _fold_id_to_ct_node.push_back(it.node());

            // map the contour tree node to the fold tree node
            _ct_to_fold_node[it.node()] = node;
        }

        for (EdgeIterator<ContourTree> it(_contour_tree); !it.done(); ++it) {
            // get the nodes in the contour tree
            typename ContourTree::Node ct_u = _contour_tree.u(it.edge());
            typename ContourTree::Node ct_v = _contour_tree.v(it.edge());

            // get the corresponding nodes in the fold tree
            Node u = _ct_to_fold_node[ct_u];
            Node v = _ct_to_fold_node[ct_v];

            // add the edge
            Edge edge = _fold_tree.addEdge(u,v);

            // map the contour tree edge to this fold edge
            _fold_id_to_ct_edge.push_back(it.edge());
        }
    }

    /// \brief Retrieve the node's scalar value.
    double getValue(Node node) const {
        return _contour_tree.getValue(getContourTreeNode(node));
    }

    /// \brief Get the ID of a node.
    unsigned int getID(Node node) const {
        return _contour_tree.getID(getContourTreeNode(node));
    }

    /// \brief Retrieve a node by ID.
    Node getNode(unsigned int id) const {
        return _ct_to_fold_node[_contour_tree.getNode(id)];
    }

    /// \brief Collapse an edge.
    void collapse(Edge edge) {
        return _fold_tree.collapse(edge);
    }

    /// \brief Reduced a node, connecting its neighbors.
    Edge reduce(Node v) {
        return _fold_tree.reduce(v);
    }

    /// \brief Uncollapse a collapsed edge in the node.
    Edge uncollapse(Node u, int index=-1)
    {
        Edge edge = _fold_tree.uncollapse(u, index);

        // we have to be careful: we have a new node in the tree now,
        // and we have to map the corresponding contour tree node to it
        Node node = _fold_tree.opposite(u, edge);
        _ct_to_fold_node[getContourTreeNode(node)] = node;

        return edge;
    }

    /// \brief Unreduce the edge.
    Node unreduce(Edge uw)
    {
        // unreduce and get the newly created node
        Node node = _fold_tree.unreduce(uw);

        // map the contour tree node to the new node
        _ct_to_fold_node[getContourTreeNode(node)] = node;

        return node;
    }
};

}


#endif
