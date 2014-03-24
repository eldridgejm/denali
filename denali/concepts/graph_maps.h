#ifndef DENALI_CONCEPTS_MAPS_H
#define DENALI_CONCEPTS_MAPS_H

namespace denali {
namespace concepts {

/// \brief A node map that observes a graph for changes.
/// \ingroup concepts_graph_maps
/*!
 *  The graph must conform to concepts::NodeObservable and
 *  to concepts::NodeMappable.
 */
template <typename GraphType, typename ValueType>
class ObservingNodeMap : public GraphType::Observer
{
    ValueType _value;

public:
    ObservingNodeMap(GraphType& graph) { }

    void notify() { }

    ValueType& operator[](typename GraphType::Node node) { return _value; }
    const ValueType& operator[](typename GraphType::Node node) const { return _value; }

    template <typename _ObservingNodeMap>
    struct Constraints
    {
        void constraints()
        {
            _map.notify();
            _ObservingNodeMap new_map(_graph);
            ValueType value = _map[_Node()];
            const ValueType const_value = _map[_Node()];

            ignore_unused_variable_warning(value);
            ignore_unused_variable_warning(const_value);
        }
        _ObservingNodeMap& _map;
        GraphType& _graph;
        typedef typename GraphType::Node _Node;

    };
};


/// \brief An arc map that observes a graph for changes.
/// \ingroup concepts_graph_maps
/*!
 *  The graph must conform to concepts::ArcObservable and
 *  to concepts::ArcMappable.
 */
template <typename GraphType, typename ValueType>
class ObservingArcMap : public GraphType::Observer
{
    ValueType _value;

public:
    ObservingArcMap(GraphType& graph) { }

    void notify() { }

    ValueType& operator[](typename GraphType::Arc arc) { return _value; }
    const ValueType& operator[](typename GraphType::Arc arc) const { return _value; }

    template <typename _ObservingArcMap>
    struct Constraints
    {
        void constraints()
        {
            _map.notify();
            _ObservingArcMap new_map(_graph);
            ValueType value = _map[_Arc()];
            const ValueType const_value = _map[_Arc()];

            ignore_unused_variable_warning(value);
            ignore_unused_variable_warning(const_value);
        }
        _ObservingArcMap& _map;
        GraphType& _graph;
        typedef typename GraphType::Arc _Arc;
    };
};


/// \brief An edge map that observes a graph for changes.
/// \ingroup concepts_graph_maps
/*!
 *  The graph must conform to concepts::EdgeObservable and
 *  to concepts::EdgeMappable.
 */
template <typename GraphType, typename ValueType>
class ObservingEdgeMap : public GraphType::Observer
{
    ValueType _value;

public:
    ObservingEdgeMap(GraphType& graph) { }

    void notify() { }

    ValueType& operator[](typename GraphType::Edge edge) { return _value; }
    const ValueType& operator[](typename GraphType::Edge edge) const { return _value; }

    template <typename _ObservingEdgeMap>
    struct Constraints
    {
        void constraints()
        {
            _map.notify();
            _ObservingEdgeMap new_map(_graph);
            ValueType value = _map[_Edge()];
            const ValueType const_value = _map[_Edge()];

            ignore_unused_variable_warning(value);
            ignore_unused_variable_warning(const_value);
        }
        _ObservingEdgeMap& _map;
        GraphType& _graph;
        typedef typename GraphType::Edge _Edge;
    };
};


/// \brief A node map that does not grow.
/// \ingroup concepts_graph_maps
template <typename GraphType, typename ValueType>
class StaticNodeMap
{
    ValueType _value;

public:
    StaticNodeMap(const GraphType& graph) { }

    ValueType& operator[](typename GraphType::Node node) { return _value; }
    const ValueType& operator[](typename GraphType::Node node) const { return _value; }

    template <typename _StaticNodeMap>
    struct Constraints
    {
        void constraints()
        {
            _StaticNodeMap new_map(_graph);
            ValueType value = _map[_Node()];
            const ValueType const_value = _map[_Node()];

            ignore_unused_variable_warning(value);
            ignore_unused_variable_warning(const_value);
        }
        _StaticNodeMap& _map;
        const GraphType& _graph;
        typedef typename GraphType::Node _Node;
    };
};


/// \brief An arc map that does not grow.
/// \ingroup concepts_graph_maps
template <typename GraphType, typename ValueType>
class StaticArcMap
{
    ValueType _value;

public:
    StaticArcMap(const GraphType& graph) { }

    ValueType& operator[](typename GraphType::Arc arc) { return _value; }
    const ValueType& operator[](typename GraphType::Arc arc) const { return _value; }

    template <typename _StaticArcMap>
    struct Constraints
    {
        void constraints()
        {
            _StaticArcMap new_map(_graph);
            ValueType value = _map[_Arc()];
            const ValueType const_value = _map[_Arc()];

            ignore_unused_variable_warning(value);
            ignore_unused_variable_warning(const_value);
        }
        _StaticArcMap& _map;
        const GraphType& _graph;
        typedef typename GraphType::Arc _Arc;
    };
};


/// \brief An edge map that does not grow.
/// \ingroup concepts_graph_maps
template <typename GraphType, typename ValueType>
class StaticEdgeMap
{
    ValueType _value;

public:
    StaticEdgeMap(const GraphType& graph) { }

    ValueType& operator[](typename GraphType::Edge edge) { return _value; }
    const ValueType& operator[](typename GraphType::Edge edge) const { _value; }

    template <typename _StaticEdgeMap>
    struct Constraints
    {
        void constraints()
        {
            _StaticEdgeMap new_map(_graph);
            ValueType value = _map[_Edge()];
            const ValueType const_value = _map[_Edge()];

            ignore_unused_variable_warning(value);
            ignore_unused_variable_warning(const_value);
        }
        _StaticEdgeMap& _map;
        const GraphType& _graph;
        typedef typename GraphType::Edge _Edge;
    };
};
}
}


#endif
