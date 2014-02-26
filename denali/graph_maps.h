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

    void resize() {
        _values.resize(_graph.getMaxNodeIdentifier());
    }

    size_t size() const {
        return _values.size();
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

    void resize() {
        _values.resize(_graph.getMaxArcIdentifier());
    }

    size_t size() const {
        return _values.size();
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

    void resize() {
        _values.resize(_graph.getMaxEdgeIdentifier());
    }

    size_t size() const {
        return _values.size();
    }
};

}

#endif
