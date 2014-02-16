#ifndef DENALI_LANDSCAPE_H
#define DENALI_LANDSCAPE_H


namespace denali {

    template <typename ContourTree, typename GraphType>
    class LandscapeTreeBase :
            public
            ArcObservableMixin < GraphType,
            NodeObservableMixin < GraphType, 
            ReadableDirectedGraphMixin <GraphType,
            BaseGraphMixin <GraphType> > > >
    {
        typedef
        ArcObservableMixin < GraphType,
        NodeObservableMixin < GraphType, 
        ReadableDirectedGraphMixin <GraphType,
        BaseGraphMixin <GraphType> > > >
        Mixin;

        GraphType _graph;
        const ContourTree& _contour_tree;
        typename GraphType::Node _root;

    public:
        LandscapeTreeBase(
                const ContourTree& contour_tree, 
                typename ContourTree::Node root)
            : _contour_tree(contour_tree), Mixin(_graph)
        {
        }
    };


}


#endif
