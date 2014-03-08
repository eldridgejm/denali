#ifndef DENALI_GRAPH_MIXINS_H
#define DENALI_GRAPH_MIXINS_H

namespace denali {

/// \brief An empty base for mixing in graph concepts.
/// \ingroup graph_implementations_mixins
template <typename Anything>
class BaseGraphMixin
{
public:
    BaseGraphMixin(const Anything&) {}
};


/// \brief Forwards the ReadableDirectedGraph interface.
/// \ingroup graph_implementations_mixins
/*!
 *  Given a graph conforming to concepts::ReadableDirectedGraph,
 *  forwards the concept's interface to the inheriting class.
 */
template <typename ReadableDirectedGraph, typename Super>
class ReadableDirectedGraphMixin : public Super
{
    const ReadableDirectedGraph& _graph;

public:
    /// \brief Constructor taking a graph conforming to concepts::ReadableDirectedGraph.
    ReadableDirectedGraphMixin(const ReadableDirectedGraph& graph)
        : Super(graph), _graph(graph) {}

public:
    /// A node in the graph
    typedef typename ReadableDirectedGraph::Node Node;

    /// An arc in the graph
    typedef typename ReadableDirectedGraph::Arc Arc;

    /// \brief Checks if the node is valid.
    bool isNodeValid(Node node) const {
        return _graph.isNodeValid(node);
    }

    /// \brief Checks if the arc is valid.
    bool isArcValid(Arc arc) const {
        return _graph.isArcValid(arc);
    }

    /// \brief Gets the first node from the graph.
    Node getFirstNode() const {
        return _graph.getFirstNode();
    }

    /// \brief Gets the next node from the graph.
    /// \pre Requires that the node is valid.
    Node getNextNode(Node node) const {
        return _graph.getNextNode(node);
    }

    /// \brief Gets the first arc from the graph.
    Arc getFirstArc() const {
        return _graph.getFirstArc();
    }

    /// \brief Gets the next arc from the graph.
    /// \pre Requires that the arc is valid.
    Arc getNextArc(Arc arc) const {
        return _graph.getNextArc(arc);
    }

    /// \brief Gets the first arc into the node.
    /// \pre The node must be valid.
    Arc getFirstInArc(Node node) const {
        return _graph.getFirstInArc(node);
    }

    /// \brief Gets the next arc into the node.
    /// \pre The arc must be valid.
    Arc getNextInArc(Arc arc) const {
        return _graph.getNextInArc(arc);
    }

    /// \brief Get the source of the arc.
    /// \pre The arc must be valid.
    Node source(Arc arc) const {
        return _graph.source(arc);
    }

    /// \brief Gets the first arc out of the node.
    /// \pre The node must be valid.
    Arc getFirstOutArc(Node node) const {
        return _graph.getFirstOutArc(node);
    }

    /// \brief Gets the next arc out of the node.
    /// \pre The arc must be valid.
    Arc getNextOutArc(Arc arc) const {
        return _graph.getNextOutArc(arc);
    }

    /// \brief Get the target of the arc.
    /// \pre The arc must be valid.
    Node target(Arc arc) const {
        return _graph.target(arc);
    }

    /// \brief Gets the first neighbor arc attached to the node.
    /// \pre The node must be valid.
    Arc getFirstNeighborArc(Node node) const {
        return _graph.getFirstNeighborArc(node);
    }

    /// \brief Gets the next neighbor arc attached to the node.
    /// \pre The node must be valid, and the arc must be attached to it.
    Arc getNextNeighborArc(Node node, Arc arc) const {
        return _graph.getNextNeighborArc(node, arc);
    }

    /// \brief Get the opposite node of the arc.
    /// \pre The node must be in the arc.
    Node opposite(Node node, Arc arc) const {
        return _graph.opposite(node, arc);
    }

    /// \brief Get the degree of the node.
    /// \pre The node must be valid.
    unsigned int degree(Node node) const {
        return _graph.degree(node);
    }

    /// \brief Get the in degree of the node.
    /// \pre The node must be valid.
    unsigned int inDegree(Node node) const {
        return _graph.inDegree(node);
    }

    /// \brief Get the out degree of the node.
    /// \pre The node must be valid.
    unsigned int outDegree(Node node) const {
        return _graph.outDegree(node);
    }

    /// \brief The number of nodes in the graph
    unsigned int numberOfNodes() const {
        return _graph.numberOfNodes();
    }

    /// \brief The number of arcs in the graph
    unsigned int numberOfArcs() const {
        return _graph.numberOfArcs();
    }

    /// \brief Gets the maximum node identifier in the graph.
    unsigned int getMaxNodeIdentifier() const {
        return _graph.getMaxNodeIdentifier();
    }

    /// \brief Get the identifier of the node.
    unsigned int getNodeIdentifier(Node node) const {
        return _graph.getNodeIdentifier(node);
    }

    /// \brief Gets the maximum arc identifier in the graph.
    unsigned int getMaxArcIdentifier() const {
        return _graph.getMaxArcIdentifier();
    }

    /// \brief Get the identifier of the arc.
    unsigned int getArcIdentifier(Arc arc) const {
        return _graph.getArcIdentifier(arc);
    }

    /// \brief Finds an arc in the graph with the given source and target.
    /*!
     *  If no such arc exists, an invalid arc is returned.
     */
    Arc findArc(Node source, Node target) const {
        return _graph.findArc(source, target);
    }

    /// \brief Get an invalid node
    Node getInvalidNode() const {
        return _graph.getInvalidNode();
    }

    /// \brief Get and invalid arc
    Arc getInvalidArc() const {
        return _graph.getInvalidArc();
    }

};


/// \brief Forwards the ReadableUndirectedGraph interface.
/// \ingroup graph_implementations_mixins
/*!
 *  Given a graph conforming to concepts::ReadableUndirectedGraph,
 *  forwards the concept's interface to the inheriting class.
 */
template <typename ReadableUndirectedGraph, typename Super>
class ReadableUndirectedGraphMixin : public Super
{
    const ReadableUndirectedGraph& _graph;

public:

    ReadableUndirectedGraphMixin(const ReadableUndirectedGraph& graph)
        : Super(graph), _graph(graph) {}

    /// A node in the graph
    typedef typename ReadableUndirectedGraph::Node Node;

    /// An edge in the graph
    typedef typename ReadableUndirectedGraph::Edge Edge;

    /// \brief Checks if the node is valid.
    bool isNodeValid(Node node) const {
        return _graph.isNodeValid(node);
    }

    /// \brief Checks if the edge is valid.
    bool isEdgeValid(Edge edge) const {
        return _graph.isEdgeValid(edge);
    }

    /// \brief Gets the first node from the graph.
    Node getFirstNode() const {
        return _graph.getFirstNode();
    }

    /// \brief Gets the next node from the graph.
    /// \pre Requires that the node is valid.
    Node getNextNode(Node node) const {
        return _graph.getNextNode(node);
    }

    /// Gets the first edge from the graph.
    Edge getFirstEdge() const {
        return _graph.getFirstEdge();
    }

    /// Gets the next edge from the graph.
    Edge getNextEdge(Edge edge) const {
        return _graph.getNextEdge(edge);
    }

    /// Gets the first neighbor edge attached to the node.
    Edge getFirstNeighborEdge(Node node) const
    {
        return _graph.getFirstNeighborEdge(node);
    }

    /// Gets the next neighbor edge attached to the node.
    Edge getNextNeighborEdge(Node node, Edge edge) const
    {
        return _graph.getNextNeighborEdge(node,edge);
    }

    /// \brief Return the node opposite in the edge.
    /// \pre The node must be a part of the edge.
    Node opposite(Node node, Edge edge) const
    {
        return _graph.opposite(node,edge);
    }

    /// \brief Gets the maximum node identifier in the graph.
    unsigned int getMaxNodeIdentifier() const
    {
        return _graph.getMaxNodeIdentifier();
    }

    /// \brief Get the identifier of the node.
    unsigned int getNodeIdentifier(Node node) const
    {
        return _graph.getNodeIdentifier(node);
    }

    /// \brief Gets the maximum edge identifier in the graph.
    unsigned int getMaxEdgeIdentifier() const
    {
        return _graph.getMaxEdgeIdentifier();
    }

    /// \brief Get the identifier of the edge.
    unsigned int getEdgeIdentifier(Edge edge) const
    {
        return _graph.getEdgeIdentifier(edge);
    }

    /// \brief Gets the degree of the node.
    /// \pre The node must be valid.
    unsigned int degree(Node node) const
    {
        return _graph.degree(node);
    }

    /// \brief Gets the "u" node at the end of an edge.
    /// \pre The edge must be valid.
    Node u(Edge edge) const {
        return _graph.u(edge);
    }

    /// \brief Gets the "v" node at the end of an edge.
    /// \pre The edge must be valid.
    Node v(Edge edge) const {
        return _graph.v(edge);
    }

    /// \brief The number of nodes in the graph
    unsigned int numberOfNodes() const {
        return _graph.numberOfNodes();
    }

    /// \brief The number of arcs in the graph
    unsigned int numberOfEdges() const {
        return _graph.numberOfEdges();
    }

    /// \brief Finds an edge in the graph with the given source and target.
    /*!
     *  If no such edge exists, an invalid edge is returned.
     */
    Edge findEdge(Node u, Node v) const {
        return _graph.findEdge(u,v);
    }

    /// \brief Get an invalid node
    Node getInvalidNode() const {
        return _graph.getInvalidNode();
    }

    /// \brief Get and invalid arc
    Edge getInvalidEdge() const {
        return _graph.getInvalidEdge();
    }
};


/// \brief A mixin to forward the concepts::WritableReadableDirectedGraph interface.
/// \ingroup graph_implementations_mixins
/*!
 *  Given a graph conforming to concepts::WritableReadableDirectedGraph,
 *  forwards the interface of that concept to the inherited class.
 */
template <typename WritableReadableDirectedGraph, typename Super>
class WritableReadableDirectedGraphMixin :
    public
    ReadableDirectedGraphMixin <WritableReadableDirectedGraph, Super>
{
    WritableReadableDirectedGraph& _graph;
    typedef ReadableDirectedGraphMixin <WritableReadableDirectedGraph, Super> RDGM;

public:
    WritableReadableDirectedGraphMixin(WritableReadableDirectedGraph& graph)
        : RDGM(graph), _graph(graph) {}

    typedef typename RDGM::Node Node;
    typedef typename RDGM::Arc Arc;

    /// \brief Add a node to the graph.
    Node addNode() {
        return _graph.addNode();
    }

    /// \brief Add an arc to the graph.
    /// \pre Requires that both nodes are in the graph.
    Arc addArc(Node u, Node v) {
        return _graph.addArc(u,v);
    }

    /// \brief Remove a node from the graph.
    /// \pre Requires that the node is in the graph.
    void removeNode(Node node) {
        _graph.removeNode(node);
    }

    /// \brief Remove an arc from the graph.
    /// \pre Requires that the arc is in the graph.
    void removeArc(Arc arc) {
        return _graph.removeArc(arc);
    }

    /// \brief Clear the nodes of the graph.
    void clearNodes() {
        return _graph.clearNodes();
    }

    /// \brief Clear the arcs of the graph.
    void clearArcs() {
        return _graph.clearArcs();
    }

};


template<typename WritableReadableUndirectedGraph, typename Super>
class WritableReadableUndirectedGraphMixin :
    public
    ReadableUndirectedGraphMixin <WritableReadableUndirectedGraph, Super>
{
    WritableReadableUndirectedGraph& _graph;
    typedef ReadableUndirectedGraphMixin <WritableReadableUndirectedGraph, Super> RUGM;

public:
    WritableReadableUndirectedGraphMixin(WritableReadableUndirectedGraph& graph)
        : RUGM(graph), _graph(graph) {}

    typedef typename RUGM::Node Node;
    typedef typename RUGM::Edge Edge;

    /// \brief Add a node to the graph.
    Node addNode() {
        return _graph.addNode();
    }

    /// \brief Add an edge to the graph.
    /// \pre The nodes must be in the graph.
    Edge addEdge(Node u, Node v) {
        return _graph.addEdge(u,v);
    }

    /// \brief Remove a node from the graph.
    /// \pre The node must be in the graph.
    void removeNode(Node node) {
        return _graph.removeNode(node);
    }

    /// \brief Remove an edge from the graph.
    /// \pre The edge must be in the graph.
    void removeEdge(Edge edge) {
        return _graph.removeEdge(edge);
    }

    /// \brief Clear the nodes of the graph.
    void clearNodes() {
        return _graph.clearNodes();
    }

    /// \brief Clear the edges of the graph.
    void clearEdges() {
        return _graph.clearEdges();
    }
};


/// \brief A mixin to forward node observing concepts.
/// \ingroup graph_implementations_mixins
/*!
 *  Given a graph conforming to concepts::NodeObservable, forwards the
 *  interface of that concept to the inherited class.
 */
template <typename NodeObservable, typename Super>
class NodeObservableMixin : public Super
{
    NodeObservable& _graph;

public:
    NodeObservableMixin(NodeObservable& graph)
        : Super(graph), _graph(graph) {}

    typedef typename NodeObservable::Observer Observer;

    /// \brief Attach an Observer to the object.
    /*!
     *  The attached object must conform to the Observer concept.
     *  Whenever a change occurs in this object, the observer's
     *  `notify` method will be called without parameters.
     */
    void attachNodeObserver(Observer& ob)
    {
        _graph.attachNodeObserver(ob);
    }

    /// \brief Detach an Observer from the object.
    void detachNodeObserver(Observer& ob)
    {
        _graph.detachNodeObserver(ob);
    }

};


/// \brief A mixin to forward arc observing concepts.
/// \ingroup graph_implementations_mixins
/*!
 *  Given a graph conforming to concepts::ArcObservable, forwards the
 *  interface of that concept to the inherited class.
 */
template <typename ArcObservable, typename Super>
class ArcObservableMixin : public Super
{
    ArcObservable& _graph;

public:
    ArcObservableMixin(ArcObservable& graph)
        : Super(graph), _graph(graph) {}

    typedef typename ArcObservable::Observer Observer;

    /// \brief Attach an Observer to the object.
    /*!
     *  The attached object must conform to the Observer concept.
     *  Whenever a change occurs in this object, the observer's
     *  `notify` method will be called without parameters.
     */
    void attachArcObserver(Observer& ob)
    {
        _graph.attachArcObserver(ob);
    }

    /// \brief Detach an Observer from the object.
    void detachArcObserver(Observer& ob)
    {
        _graph.detachArcObserver(ob);
    }

};


/// \brief A mixin to forward edge observing concepts.
/// \ingroup graph_implementations_mixins
/*!
 *  Given a graph conforming to concepts::EdgeObservable, forwards the
 *  interface of that concept to the inherited class.
 */
template <typename EdgeObservable, typename Super>
class EdgeObservableMixin : public Super
{
    EdgeObservable& _graph;

public:
    EdgeObservableMixin(EdgeObservable& graph)
        : Super(graph), _graph(graph) {}

    typedef typename EdgeObservable::Observer Observer;

    /// \brief Attach an Observer to the object.
    /*!
     *  The attached object must conform to the Observer concept.
     *  Whenever a change occurs in this object, the observer's
     *  `notify` method will be called without parameters.
     */
    void attachEdgeObserver(Observer& ob)
    {
        _graph.attachEdgeObserver(ob);
    }

    /// \brief Detach an Observer from the object.
    void detachEdgeObserver(Observer& ob)
    {
        _graph.detachEdgeObserver(ob);
    }

};



}

#endif
