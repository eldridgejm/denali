#include <UnitTest++.h>
#include <iostream>

#include <denali/concepts/check.h>
#include <denali/concepts/common.h>
#include <denali/concepts/graph_attributes.h>
#include <denali/concepts/graph_iterators.h>


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
            denali::concepts::BaseDirectedGraph> > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::ArcObservable <
            denali::concepts::BaseDirectedGraph> > ();

    denali::concepts::checkSelfConsistent <
            denali::concepts::EdgeObservable <
            denali::concepts::BaseUndirectedGraph> > ();

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


int main()
{
    return UnitTest::RunAllTests();
}
