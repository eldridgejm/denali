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
