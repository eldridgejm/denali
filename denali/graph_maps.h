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

#ifndef DENALI_GRAPH_MAPS_H
#define DENALI_GRAPH_MAPS_H

#include <vector>

namespace denali {

/// \brief An observing node map.
/// \ingroup graph_implementations_maps
/*!
 *  The observed graph must conform to both concepts::NodeObservable
 *  and concepts::NodeMappable.
 *
 *  The map will resize itself whenever the watched graph grows or shrinks.
 */
template <typename NodeObservable, typename ValueType>
class ObservingNodeMap : public NodeObservable::Observer
{
    NodeObservable& _graph;
    std::vector<ValueType> _values;

public:
    ObservingNodeMap(NodeObservable& graph)
        : _graph(graph), _values(_graph.getMaxNodeIdentifier())
    {
        _graph.attachNodeObserver(*this);
    }

    ~ObservingNodeMap()
    {
        _graph.detachNodeObserver(*this);
    }

    void notify()
    {
        _values.resize(_graph.getMaxNodeIdentifier());
    }

    typename std::vector<ValueType>::reference
    operator[](typename NodeObservable::Node node)
    {
        return _values[_graph.getNodeIdentifier(node)];
    }

    typename std::vector<ValueType>::const_reference
    operator[](typename NodeObservable::Node node) const
    {
        return _values[_graph.getNodeIdentifier(node)];
    }

};


/// \brief An observing arc map.
/// \ingroup graph_implementations_maps
/*!
 *  The observed graph must conform to both concepts::ArcObservable
 *  and concepts::ArcMappable.
 *
 *  The map will resize itself whenever the watched graph grows or shrinks.
 */
template <typename ArcObservable, typename ValueType>
class ObservingArcMap : public ArcObservable::Observer
{
    ArcObservable& _graph;
    std::vector<ValueType> _values;

public:
    ObservingArcMap(ArcObservable& graph)
        : _graph(graph), _values(_graph.getMaxArcIdentifier())
    {
        _graph.attachArcObserver(*this);
    }

    ~ObservingArcMap()
    {
        _graph.detachArcObserver(*this);
    }

    void notify()
    {
        _values.resize(_graph.getMaxArcIdentifier());
    }

    typename std::vector<ValueType>::reference
    operator[](typename ArcObservable::Arc arc)
    {
        return _values[_graph.getArcIdentifier(arc)];
    }

    typename std::vector<ValueType>::const_reference
    operator[](typename ArcObservable::Arc arc) const
    {
        return _values[_graph.getArcIdentifier(arc)];
    }

};


/// \brief An observing edge map.
/// \ingroup graph_implementations_maps
/*!
 *  The observed graph must conform to both concepts::EdgeObservable
 *  and concepts::EdgeMappable.
 *
 *  The map will resize itself whenever the watched graph grows or shrinks.
 */
template <typename EdgeObservable, typename ValueType>
class ObservingEdgeMap : public EdgeObservable::Observer
{
    EdgeObservable& _graph;
    std::vector<ValueType> _values;

public:
    ObservingEdgeMap(EdgeObservable& graph)
        : _graph(graph), _values(_graph.getMaxEdgeIdentifier())
    {
        _graph.attachEdgeObserver(*this);
    }

    ~ObservingEdgeMap()
    {
        _graph.detachEdgeObserver(*this);
    }

    void notify()
    {
        _values.resize(_graph.getMaxEdgeIdentifier());
    }

    typename std::vector<ValueType>::reference
    operator[](typename EdgeObservable::Edge edge)
    {
        return _values[_graph.getEdgeIdentifier(edge)];
    }

    typename std::vector<ValueType>::const_reference
    operator[](typename EdgeObservable::Edge edge) const
    {
        return _values[_graph.getEdgeIdentifier(edge)];
    }

};


/// \brief A static node map.
/// \ingroup graph_implementations_maps
/*!
 *  The observed graph must conform to concepts::NodeMappable
 */
template <typename NodeMappable, typename ValueType>
class StaticNodeMap
{
    const NodeMappable& _graph;
    std::vector<ValueType> _values;

public:
    StaticNodeMap(const NodeMappable& graph)
        : _graph(graph), _values(_graph.getMaxNodeIdentifier()) {}

    typename std::vector<ValueType>::reference
    operator[](typename NodeMappable::Node node)
    {
        return _values[_graph.getNodeIdentifier(node)];
    }

    typename std::vector<ValueType>::const_reference
    operator[](typename NodeMappable::Node node) const
    {
        return _values[_graph.getNodeIdentifier(node)];
    }
};


/// \brief A static arc map.
/// \ingroup graph_implementations_maps
/*!
 *  The observed graph must conform to concepts::ArcMappable
 */
template <typename ArcMappable, typename ValueType>
class StaticArcMap
{
    const ArcMappable& _graph;
    std::vector<ValueType> _values;

public:
    StaticArcMap(const ArcMappable& graph)
        : _graph(graph), _values(_graph.getMaxArcIdentifier()) {}

    typename std::vector<ValueType>::reference
    operator[](typename ArcMappable::Arc arc)
    {
        return _values[_graph.getArcIdentifier(arc)];
    }

    typename std::vector<ValueType>::const_reference
    operator[](typename ArcMappable::Arc arc) const
    {
        return _values[_graph.getArcIdentifier(arc)];
    }
};


/// \brief A static edge map.
/// \ingroup graph_implementations_maps
/*!
 *  The observed graph must conform to concepts::EdgeMappable
 */
template <typename EdgeMappable, typename ValueType>
class StaticEdgeMap
{
    const EdgeMappable& _graph;
    std::vector<ValueType> _values;

public:
    StaticEdgeMap(const EdgeMappable& graph)
        : _graph(graph), _values(_graph.getMaxEdgeIdentifier()) {}

    typename std::vector<ValueType>::reference
    operator[](typename EdgeMappable::Edge edge)
    {
        return _values[_graph.getEdgeIdentifier(edge)];
    }

    typename std::vector<ValueType>::const_reference
    operator[](typename EdgeMappable::Edge edge) const
    {
        return _values[_graph.getEdgeIdentifier(edge)];
    }
};

}

#endif
