#ifndef DENALI_SIMPLIFY_H
#define DENALI_SIMPLIFY_H

#include <denali/graph_mixins.h>

namespace denali {

    ////////////////////////////////////////////////////////////////////////////
    //
    // UndirectedScalarMemberIDGraph
    //
    ////////////////////////////////////////////////////////////////////////////


    template <typename GraphType, typename Tab>
    class UndirectedTabbedScalarMemberIDGraphBase :
            public
            ReadableUndirectedGraphMixin <GraphType,
            BaseGraphMixin <GraphType> >
    {
        typedef
        ReadableUndirectedGraphMixin <GraphType,
        BaseGraphMixin <GraphType> >
        Mixin;

    public:
        typedef typename Mixin::Node Node;
        typedef typename Mixin::Edge Edge;
        typedef typename GraphType::Members Members;
        typedef Tab TabType;

    private:

        GraphType _graph;

        struct TabContainer
        {
            Node u;
            Tab u_tab;
            Node v;
            Tab v_tab;

            bool is_valid;

            TabContainer() : is_valid(false) {}

            Tab getTab(Node node)
            {
                return (node == u ? u_tab : v_tab);
            }

        };

        ObservingEdgeMap<GraphType, TabContainer> _edge_to_tabs;

    public:

        /// \brief Set an edge's tabs.
        void setTab(Edge edge, Node u, Tab u_tab, Node v, Tab v_tab)
        {
            TabContainer& tab = _edge_to_tabs[edge];
            tab.u = u;      tab.u_tab = u_tab;
            tab.v = v;      tab.v_tab = v_tab;
            tab.is_valid = true;
        }

        /// \brief Retrieve a tab.
        Tab getTab(Edge edge, Node node)
        {
            return _edge_to_tabs[edge].getTab(node);
        }

        /// \brief Checks if the edge has tabs.
        bool hasTabs(Edge edge)
        {
            return _edge_to_tabs[edge].is_valid;
        }

        /// \brief Add a node to the graph.
        /*!
         *  \param  id      The id of the node.
         *  \param  value   The scalar value of the node.
         */
        Node addNode(unsigned int id, double value)
        {
            return _graph.addNode(id, value);
        }

        /// \brief Add an edge to the graph.
        Edge addEdge(Node u, Node v)
        {
            return _graph.addEdge(u,v);
        }

        /// \brief Remove the node.
        void removeNode(Node node)
        {
            return _graph.removeNode(node);
        }

        /// \brief Remove the edge.
        void removeEdge(Edge edge)
        {
            return _graph.removeEdge(edge);
        }

        /// \brief Insert a member into the node's member set.
        void insertNodeMember(Node node, unsigned int member)
        {
            return _graph.insertNodeMember(node, member);
        }

        /// \brief Insert a member into the edge's member set.
        void insertEdgeMember(Edge edge, unsigned int member)
        {
            return _graph.insertEdgeMember(edge, member);
        }

        /// \brief Insert members into node member set.
        void insertNodeMembers(Node node, const Members& members)
        {
            return _graph.insertNodeMembers(node, members);
        }

        /// \brief Insert members into edge member set.
        void insertEdgeMembers(Edge edge, const Members& members)
        {
            return _graph.insertEdgeMembers(edge, members);
        }

        /// \brief Get a node's scalar value
        double getValue(Node node) const
        {
            return _graph.getValue(node);
        }

        /// \brief Get a node's ID
        unsigned int getID(Node node) const
        {
            return _graph.getID(node);
        }

        /// \brief Retrieve the members of the node
        const Members& getNodeMembers(Node node) const
        {
            return _graph.getNodeMembers(node);
        }

        /// \brief Retrieve a node by its ID.
        Node getNode(unsigned int id)
        {
            return _graph.getNode(id);
        }

        Node getNodeChecked(unsigned int id)
        {
            return _graph.getNodeChecked(id);
        }

        /// \brief Retrieve the members of the edge.
        const Members& getEdgeMembers(Edge edge) const
        {
            return _graph.getEdgeMembers(edge);
        }

        /// \brief Clear the nodes of the graph
        void clearNodes() { return _graph.clearNodes(); }

        /// \brief Clear the edges of the graph
        void clearEdges() { return _graph.clearEdges(); }

    };


    /// \brief An undirected graph with tabs in each edge.
    /// \ingroup simplified_contour_tree
    template <typename Tab>
    class UndirectedTabbedScalarMemberIDGraph : 
            public UndirectedTabbedScalarMemberIDGraphBase<
                   UndirectedScalarMemberIDGraph, Tab>
    {
        typedef 
        UndirectedTabbedScalarMemberIDGraphBase<
                UndirectedScalarMemberIDGraph, Tab>
        Base;

        public:
        typedef typename Base::Node Node;
        typedef typename Base::Edge Edge;
        typedef typename Base::Members Members;


    };


    ////////////////////////////////////////////////////////////////////////////
    //
    // PersistenceSimplifier
    //
    ////////////////////////////////////////////////////////////////////////////

    class PersistenceSimplifier
    {
        double _threshold;

    public:
        PersistenceSimplifier(double threshold) : _threshold(threshold) {};

        /// \brief Simplify the contour tree, placing results in `graph`.
        /*!
         *  GraphType must support edge tabs. Specifically, the edge tab
         *  type must be ContourTree::Edge.
         */
        template <typename ContourTree, typename GraphType>
        void simplify(
                const ContourTree& contour_tree, 
                GraphType& graph)
        {

        }

    };


}


#endif
