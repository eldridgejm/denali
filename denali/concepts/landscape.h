#ifndef DENALI_CONCEPTS_LANDSCAPE_H
#define DENALI_CONCEPTS_LANDSCAPE_H


namespace denali {
    namespace concepts {

        /// \brief A directed tree representing the tree structure of a landscape.
        /// \ingroup concepts_landscape
        template <typename ContourTree>
        class LandscapeTree : public ReadableDirectedGraph
        {
            typename ContourTree::Members members;
        public:

            typedef typename ContourTree::Members Members;

            /// \brief Get the corresponding node in the contour tree.
            typename ContourTree::Node getContourTreeNode(Node node) const
                    { return ContourTree::Node(); }

            /// \brief Get the scalar value of the node.
            double getValue(Node node) const { return 0.; }

            /// \brief Get the ID of the node.
            unsigned int getID(Node node) const { return 0; }

            /// \brief Get a node from an ID.
            Node getNode(unsigned int index) const { return Node(); }

            /// \brief Retrieve the members of the node.
            const Members& getNodeMembers(Node node) const { return members; }

            /// \brief Retrieve the members of the arc.
            const Members& getArcMembers(Arc arc) const { return members; }

            /// \brief Get the root node of the tree.
            Node getRoot() const { return Node(); }


            template <typename _LandscapeTree>
            struct Constraints
            {
                void constraints()
                {
                    double x = _tree.getValue(_Node()); 
                    unsigned int u = _tree.getID(_Node());
                    _Node node = _tree.getNode(0);
                    _Members members = _tree.getNodeMembers(_Node());
                    members = _tree.getArcMembers(_Arc());
                    _Node root = _tree.getRoot();
                    
                }

                _LandscapeTree& _tree;
                typedef typename _LandscapeTree::Node _Node;
                typedef typename _LandscapeTree::Arc _Arc;
                typedef typename _LandscapeTree::Members _Members;
            };

        };

    }
}


#endif
