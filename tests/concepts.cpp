#include <UnitTest++.h>
#include <iostream>

#include <denali/concepts/check.h>
#include <denali/concepts/common.h>
#include <denali/concepts/graph_attributes.h>
#include <denali/concepts/graph_iterators.h>
#include <denali/concepts/graph_maps.h>
#include <denali/concepts/contour_tree.h>
#include <denali/concepts/landscape.h>


TEST(GraphObjects)
{
    denali::concepts::checkSelfConsistent <
            denali::concepts::GraphObject> ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::Node> ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::Arc> ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::Edge> ();
}


TEST(GraphAttributes)
{
    denali::concepts::checkSelfConsistent <
            denali::concepts::NodeIterable <
            denali::concepts::BaseDirectedGraph> > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::ArcIterable <
            denali::concepts::BaseDirectedGraph> > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::EdgeIterable <
            denali::concepts::BaseUndirectedGraph> > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::ParentIterable <
            denali::concepts::BaseDirectedGraph> > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::ChildIterable <
            denali::concepts::BaseDirectedGraph> > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::DirectedNeighborIterable <
            denali::concepts::BaseDirectedGraph> > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::UndirectedNeighborIterable <
            denali::concepts::BaseUndirectedGraph> > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::NodeMappable <
            denali::concepts::BaseDirectedGraph> > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::ArcMappable <
            denali::concepts::BaseDirectedGraph> > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::EdgeMappable <
            denali::concepts::BaseUndirectedGraph> > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::NodeObservable <
            denali::concepts::BaseObservable <
            denali::concepts::BaseDirectedGraph> > > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::ArcObservable <
            denali::concepts::BaseObservable <
            denali::concepts::BaseDirectedGraph> > > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::EdgeObservable <
            denali::concepts::BaseObservable <
            denali::concepts::BaseUndirectedGraph> > > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::ReadableDirectedGraph> ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::ReadableUndirectedGraph> ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::WritableReadableDirectedGraph> ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::WritableReadableUndirectedGraph> ();
}


TEST(GraphIterators)
{
    denali::concepts::checkSelfConsistent <
            denali::concepts::NodeIterator<
            denali::concepts::NodeIterable<
            denali::concepts::BaseDirectedGraph
            > > > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::ArcIterator<
            denali::concepts::ArcIterable<
            denali::concepts::BaseDirectedGraph
            > > > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::EdgeIterator<
            denali::concepts::EdgeIterable<
            denali::concepts::BaseUndirectedGraph
            > > > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::ParentIterator<
            denali::concepts::ParentIterable<
            denali::concepts::BaseDirectedGraph
            > > > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::ChildIterator<
            denali::concepts::ChildIterable<
            denali::concepts::BaseDirectedGraph
            > > > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::DirectedNeighborIterator<
            denali::concepts::DirectedNeighborIterable<
            denali::concepts::BaseDirectedGraph
            > > > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::UndirectedNeighborIterator<
            denali::concepts::UndirectedNeighborIterable<
            denali::concepts::BaseUndirectedGraph
            > > > ();
}


TEST(GraphMaps)
{
    denali::concepts::checkSelfConsistent <
            denali::concepts::ObservingNodeMap<
                denali::concepts::NodeObservable<
                    denali::concepts::BaseObservable<
                        denali::concepts::NodeMappable<
                            denali::concepts::BaseDirectedGraph
                        > 
                    >
                >,
                int> 
            > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::ObservingArcMap<
                denali::concepts::ArcObservable<
                    denali::concepts::BaseObservable<
                        denali::concepts::ArcMappable<
                            denali::concepts::BaseDirectedGraph
                        > 
                    >
                >,
                int> 
            > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::ObservingEdgeMap<
                denali::concepts::EdgeObservable<
                    denali::concepts::BaseObservable<
                        denali::concepts::EdgeMappable<
                            denali::concepts::BaseUndirectedGraph
                        > 
                    >
                >,
                int> 
            > ();
}


TEST(ContourTree)
{
    denali::concepts::checkSelfConsistent <
            denali::concepts::ScalarSimplicialComplex
            > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::Members
            > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::ContourTree
            > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::UndirectedScalarMemberIDGraph
            > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::ContourTreeAlgorithm
            > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::RandomAccessComparisonFunctor<std::vector<double> >
            > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::TotalOrder
            > ();
}


TEST(Landscape)
{

}


int main()
{
    return UnitTest::RunAllTests();
}
