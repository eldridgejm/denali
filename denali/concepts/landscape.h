#ifndef DENALI_CONCEPTS_LANDSCAPE_H
#define DENALI_CONCEPTS_LANDSCAPE_H


namespace denali {
    namespace concepts {

        /// \brief A point in a landscape.
        class Point
        {
        public:
            double x() const { return 0; }
            double y() const { return 0; }
            double z() const { return 0; }
            unsigned int id() const { return 0; }

            template <typename _Point>
            struct Constraints
            {
                void constraints()
                {
                    double s = point.x(); 
                    s = point.y();
                    s = point.z();
                    unsigned int id = point.id();
                }

                _Point& point;
            }

        };

        /// \brief A directed tree representing the tree structure of a landscape.
        /// \ingroup concepts_landscape
        template <typename ContourTree>
        class LandscapeTree : public ReadableDirectedGraph
        {
            typename ContourTree::Members members;
        public:

            typedef typename ContourTree::Members Members;

            /// \brief Get the root of the landscape tree.
            Node getRoot() const { return Node(); }
                    
            /// \brief Retrieve the corresponding node in the contour tree.
            typename ContourTree::Node getContourTreeNode(Node node)
            {
                return typename ContourTree::Node();
            }

            /// \brief Retrieve the corresponding edge in the contour tree.
            typename ContourTree::Edge getContourTreeEdge(Arc arc)
            {
                return typename ContourTree::Edge();
            }

            /// \brief Get the node corresponding to the contour tree node.
            Node getLandscapeTreeNode(typename ContourTree::Node node)
            {
                return Node();
            }

            /// \brief Get the arc corresponding to the contour tree edge.
            Arc getLandscapeTreeArc(typename ContourTree::Edge edge)
            {
                return Arc();
            }

            /// \brief Get the value at the node.
            double getValue(Node node) const 
            {
                return 0.;
            }

            /// \brief Get the ID of the node.
            unsigned int getID(Node node) const
            {
                return 0;
            }

            /// \brief Retrieve the members of the node.
            const Members& getNodeMembers(Node node) const
            {
                return members;
            }

            /// \brief Get the node from an ID.
            Node getNode(unsigned int id) const
            {
                return Node();
            }

            /// \brief Retrieve the members of the arc.
            const Members& getArcMembers(Arc arc) const 
            {
                return members;
            }

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
                    typename ContourTree::Node ctnode = _tree.getContourTreeNode(Node());
                    typename ContourTree::Edge ctedge = _tree.getContourTreeEdge(Arc());
                    node = _tree.getLandscapeTreeNode(typename ContourTree::Node());
                    _Arc arc = _tree.getLandscapeTreeArc(typename ContourTree::Edge());
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
