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

#ifndef DENALI_SIMPLIFY_H
#define DENALI_SIMPLIFY_H

#include <denali/graph_mixins.h>
#include <denali/graph_iterators.h>
#include <denali/folded.h>

#include <cmath>
#include <boost/shared_ptr.hpp>
#include <queue>
#include <stdexcept>
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

inline bool operator<(const Priority& lhs, const Priority& rhs) {
    return lhs.priority() < rhs.priority();
}

////////////////////////////////////////////////////////////////////////////////
//
// Persistence Simplifier
//
////////////////////////////////////////////////////////////////////////////////

template <typename Context>
struct NullProtector
{
    bool operator[](const typename Context::Node&) const { return false; }
};


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
    static typename Tree::Node getLeaf(const Tree& tree, typename Tree::Edge edge) {
        return tree.degree(tree.u(edge)) == 1 ? tree.u(edge) : tree.v(edge);
    }

    /// \brief Simplifies the contour tree in the context.
    template <typename Context, typename ProtectedNodes>
    void simplifyCore(Context& context, const ProtectedNodes& protected_nodes)
    {
        typedef PersistencePriority<typename Context::Node> Priority;
        typedef typename Context::Node Node;
        typedef typename Context::Edge Edge;

        // first, we reduce all degree-2 nodes
        std::vector<Node> reduce_vector;
        for (NodeIterator<Context> it(context); !it.done(); ++it)
        {
            if (isRegular(context, it.node()))
            {
                reduce_vector.push_back(it.node());
            }
        }

        for (typename std::vector<Node>::iterator it = reduce_vector.begin(); 
                it != reduce_vector.end(); ++it)
        {
            context.reduce(*it);
        }

        // make a priority queue of PersistencePriorities
        std::priority_queue<Priority> simplify_queue;

        // add every leaf edge to the queue
        for (EdgeIterator<Context> it(context); !it.done(); ++it)
        {
            Node u = context.u(it.edge());
            Node v = context.v(it.edge());

            if (protected_nodes[u] || protected_nodes[v]) continue;

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

            // make sure the leaf is valid
            if (!context.isNodeValid(leaf)) {
                continue;
            }

            // get the leaf edge and the parent
            UndirectedNeighborIterator<Context> neighbor_it(context, leaf);
            Edge edge = neighbor_it.edge();
            Node parent = context.opposite(leaf, edge);

            if (protected_nodes[parent] || protected_nodes[leaf]) {
                continue;
            }

            if (preserveForReduction(context, edge)) {
                continue;
            }

            if (persistence > _threshold) {
                break;
            }

            // collapse the edge
            context.collapse(edge);

            // if the parent is reducible, reduce it now
            if (isRegular(context, parent)) 
            {
                Edge edge = context.reduce(parent);

                // add the leaf nodes of the parent to the queue, as they
                // may no longer need to be preserved
                Node u = context.u(edge);
                Node v = context.v(edge);

                if (context.degree(u) == 1) {
                    double persistence = computePersistence(context, edge);
                    simplify_queue.push(Priority(u, persistence));
                } else {
                    // add u's leaf neighbors
                    for (UndirectedNeighborIterator<Context> neighbor_it(context, u);
                            !neighbor_it.done(); ++neighbor_it)
                    {
                        if (context.degree(neighbor_it.neighbor()) == 1)
                        {
                            double persistence = computePersistence(context, neighbor_it.edge());
                            simplify_queue.push(Priority(neighbor_it.neighbor(), persistence));
                        }
                    }
                }

                if (context.degree(v) == 1) {
                    double persistence = computePersistence(context, edge);
                    simplify_queue.push(Priority(v, persistence));
                } else {
                    // add v's leaf neighbors
                    for (UndirectedNeighborIterator<Context> neighbor_it(context, v);
                            !neighbor_it.done(); ++neighbor_it)
                    {
                        if (context.degree(neighbor_it.neighbor()) == 1)
                        {
                            double persistence = computePersistence(context, neighbor_it.edge());
                            simplify_queue.push(Priority(neighbor_it.neighbor(), persistence));
                        }
                    }
                }
            }
            else if (context.degree(parent) == 1)
            {
                // get the neighbor of the parent
                UndirectedNeighborIterator<Context> neighbor_it(context, parent);

                // compute the persistence
                double persistence = computePersistence(context, neighbor_it.edge());

                // add to the queue
                simplify_queue.push(Priority(parent, persistence));
            }

        }
    }


public:
    PersistenceSimplifier(double threshold) {
        setThreshold(threshold);
    }

    double getThreshold() const {
        return _threshold;
    }

    void setThreshold(double threshold) {
        if (threshold < 0) {
            throw std::runtime_error("Threshold must be nonnegative.");
        }
        _threshold = threshold;
    }

    template <typename Tree>
    static double computePersistence(
            const Tree& tree, 
            typename Tree::Edge edge)
    {
        typename Tree::Node u = tree.u(edge);
        typename Tree::Node v = tree.v(edge);

        return std::abs(tree.getValue(u) - tree.getValue(v));
    }

    /// \brief Returns true if u > v, breaking ties by ID.
    template <typename Tree>
    static bool nodeLess(const Tree& tree,
                  typename Tree::Node u,
                  typename Tree::Node v)
    {
        double u_value = tree.getValue(u);
        double v_value = tree.getValue(v);

        if (u_value < v_value)
        {
            return true;
        } 
        else if (u_value > v_value)
        {
            return false;
        } 
        else 
        {
            return tree.getID(u) < tree.getID(v);
        }
    }

    template <typename Tree>
    static unsigned int upDegree(const Tree& tree, typename Tree::Node node)
    {
        unsigned int n = 0;

        for (UndirectedNeighborIterator<Tree> it(tree, node); !it.done(); ++it)
        {
            if (nodeLess(tree, node, it.neighbor())) {
                n++;
            }
        }
        return n;
    }

    template <typename Tree>
    static unsigned int downDegree(const Tree& tree, typename Tree::Node node)
    {
        unsigned int n = 0;

        for (UndirectedNeighborIterator<Tree> it(tree, node); !it.done(); ++it)
        {
            if (nodeLess(tree, it.neighbor(), node)) {
                n++;
            }
        }
        return n;
    }

    template <typename Tree>
    static bool preserveForReduction(
            const Tree& tree, 
            typename Tree::Edge edge)
    {
        typename Tree::Node child = getLeaf(tree, edge);
        typename Tree::Node parent = tree.opposite(child, edge);

        if (nodeLess(tree, child, parent) && (downDegree(tree, parent) == 1)) 
        {
            return true;
        } 
        else if (nodeLess(tree, parent, child) && (upDegree(tree, parent) == 1))
        {
            return true;
        } 
        else 
        {
            return false;
        }
    }

    template <typename Tree>
    static bool isRegular(const Tree& tree, typename Tree::Node node) {
        return (upDegree(tree, node) == 1) && (downDegree(tree, node) == 1);
    }

    /// \brief Simplifies the contour tree in the context.
    template <typename Context>
    void simplify(Context& context)
    {
        // no nodes are going to be protected
        NullProtector<Context> null_protected;
        simplifyCore(context, null_protected);
    }

    /// \brief Simplifies a subtree.
    /*!
     *  A subtree is defined by giving a parent node, and a pivot node inside
     *  the subtree. A BFS is done from the pivot, but stops at the parent. Every
     *  node reached by the BFS is considered to be inside the subtree.
     */
    template <typename Context>
    void simplifySubtree(Context& context, 
                         typename Context::Node parent,
                         typename Context::Node pivot)
    {
        // we create a new node map that defaults to false
        StaticNodeMap<Context, bool> protected_nodes(context);

        // we set each node to protected
        for (NodeIterator<Context> it(context); !it.done(); ++it) {
            protected_nodes[it.node()] = true;
        }

        // now set each of the nodes in the subtree so that they arent
        // protected
        // protected_nodes[parent] = false;
        // protected_nodes[child] = false;
        for (UndirectedBFSIterator<Context> it(context, parent, pivot);
                !it.done(); ++it)
        {
            protected_nodes[it.child()] = false; 
        }
        protected_nodes[parent] = true;
        protected_nodes[pivot] = false;

        // perform the simplification
        simplifyCore(context, protected_nodes);

    }

};


template <typename Tree>
double computeMaxPersistence(const Tree& tree)
{
    double max_persistence = 0;
    for (EdgeIterator<Tree> it(tree); !it.done(); ++it)
    {
        double p = PersistenceSimplifier::computePersistence(tree, it.edge()); 

        if (p > max_persistence)
        {
            max_persistence = p;
        }
    }

    return max_persistence;
}


} // namespace denali

#endif
