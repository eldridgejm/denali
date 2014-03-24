#ifndef DENALI_GRAPH_OBJECTS_H
#define DENALI_GRAPH_OBJECTS_H

#include <denali/concepts/check.h>

namespace denali {
namespace concepts {

/// \brief An object in a directed or undirected graph.
/// \ingroup concepts_graph_objects
class GraphObject
{
public:
    GraphObject() {}
    bool operator==(GraphObject) const {
        return true;
    }
    bool operator!=(GraphObject) const {
        return true;
    }

    /// Enumerates constraints used for concept checking.
    template <typename _Node>
    struct Constraints
    {
        void constraints()
        {
            _Node n1;
            _Node n2;
            n1 == n2;
            n1 != n2;
        }

        Constraints() {}
    };
};


/// \brief A node in a graph.
/// \ingroup concepts_graph_objects
class Node : public GraphObject
{
public:
    template <typename _Node>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<Node, _Node>();
        }
    };
};


/// \brief An arc in a graph.
/// \ingroup concepts_graph_objects
class Arc : public GraphObject
{
public:
    template <typename _Arc>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<Arc, _Arc>();
        }
    };
};


/// \brief An edge in a graph.
/// \ingroup concepts_graph_objects
class Edge : public GraphObject
{
public:
    template <typename _Edge>
    struct Constraints
    {
        void constraints()
        {
            checkConcept<Edge, _Edge>();
        }
    };
};

}
}


#endif
