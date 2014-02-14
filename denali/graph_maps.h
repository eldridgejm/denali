#ifndef DENALI_GRAPH_MAPS_H
#define DENALI_GRAPH_MAPS_H

#include <vector>

namespace denali {

    /// \brief An observing node map.
    /// \ingroup graph_implementations_maps
    /*!
     *  The observed map must conform to both concepts::NodeObservable
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
            : _graph(graph), _values(_graph.getMaxNodeIdentifier()+1)
        {
            _graph.attachNodeObserver(*this);
        }

        ~ObservingNodeMap()
        {
            _graph.detachNodeObserver(*this);
        }

        void notify() 
        {
            _values.resize(_graph.getMaxNodeIdentifier() + 1);
        }

        ValueType& operator[](typename NodeObservable::Node node)
        {
            return _values[_graph.getNodeIdentifier(node)];
        }

        const ValueType& operator[](typename NodeObservable::Node node) const
        {
            return _values[_graph.getNodeIdentifier(node)];
        }

    };


    /// \brief An observing arc map.
    /// \ingroup graph_implementations_maps
    /*!
     *  The observed map must conform to both concepts::ArcObservable
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
            : _graph(graph), _values(_graph.getMaxArcIdentifier()+1)
        {
            _graph.attachArcObserver(*this);
        }

        ~ObservingArcMap()
        {
            _graph.detachArcObserver(*this);
        }

        void notify() 
        {
            _values.resize(_graph.getMaxArcIdentifier() + 1);
        }

        ValueType& operator[](typename ArcObservable::Arc arc)
        {
            return _values[_graph.getArcIdentifier(arc)];
        }

        const ValueType& operator[](typename ArcObservable::Arc arc) const
        {
            return _values[_graph.getArcIdentifier(arc)];
        }

    };


    /// \brief An observing edge map.
    /// \ingroup graph_implementations_maps
    /*!
     *  The observed map must conform to both concepts::EdgeObservable
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
            : _graph(graph), _values(_graph.getMaxEdgeIdentifier()+1)
        {
            _graph.attachEdgeObserver(*this);
        }

        ~ObservingEdgeMap()
        {
            _graph.detachEdgeObserver(*this);
        }

        void notify() 
        {
            _values.resize(_graph.getMaxEdgeIdentifier() + 1);
        }

        ValueType& operator[](typename EdgeObservable::Edge edge)
        {
            return _values[_graph.getEdgeIdentifier(edge)];
        }

        const ValueType& operator[](typename EdgeObservable::Edge edge) const
        {
            return _values[_graph.getEdgeIdentifier(edge)];
        }

    };
}

#endif
