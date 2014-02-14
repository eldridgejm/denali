#include <UnitTest++.h>
#include <iostream>

#include <string>
#include <set>

#include <denali/concepts/check.h>
#include <denali/concepts/graph_attributes.h>
#include <denali/graph_mixins.h>
#include <denali/graph_maps.h>
#include <denali/graph_iterators.h>
#include <denali/graph_structures.h>

double wenger_vertex_values[] = 
    // 0   1   2   3   4   5   6   7   8   9  10  11
    { 25, 62, 45, 66, 16, 32, 64, 39, 58, 51, 53, 30 };

const size_t n_wenger_vertices = sizeof(wenger_vertex_values)/sizeof(double);

unsigned int wenger_edges[][2] =
   {{0, 1}, {1, 2}, {3, 4}, {4, 5}, {6, 7}, {7, 8}, {9, 10}, {10, 11}, {0, 3},
    {1, 4}, {2, 5}, {3, 6}, {4, 7}, {5, 8}, {6, 9}, {7, 10}, {8, 11}, {0, 4},
    {1, 5}, {3, 7}, {4, 8}, {6, 10}, {7, 11}};

const size_t n_wenger_edges = sizeof(wenger_edges)/sizeof(unsigned int[2]);

SUITE(GraphMixins)
{

}


SUITE(GraphStructures)
{

    TEST(DirectedGraph)
    {
        typedef denali::DirectedGraph Graph;

        denali::concepts::checkConcept
            <
            denali::concepts::WritableReadableDirectedGraph,
            Graph
            > ();

        denali::concepts::checkConcept
            <
            denali::concepts::NodeObservable<
                    denali::concepts::BaseObservable<
                        denali::concepts::BaseDirectedGraph >
                >,
            Graph
            > ();

        denali::concepts::checkConcept
            <
            denali::concepts::ArcObservable<
                    denali::concepts::BaseObservable<
                        denali::concepts::BaseDirectedGraph >
                >,
            Graph
            > ();

        Graph graph;
        Graph::Node n1 = graph.addNode();
        Graph::Node n2 = graph.addNode();
        Graph::Node n3 = graph.addNode();
        Graph::Node n4 = graph.addNode();
        Graph::Node n5 = graph.addNode();

        CHECK(graph.isNodeValid(n1));
        CHECK(graph.isNodeValid(n5));

        Graph::Arc a12 = graph.addArc(n1, n2);
        Graph::Arc a13 = graph.addArc(n1, n3);

        CHECK(graph.isArcValid(a12));

        Graph::Node first_node = graph.getFirstNode();
        CHECK(graph.isNodeValid(first_node));

        Graph::Node next_node = graph.getNextNode(first_node);
        CHECK(graph.isNodeValid(next_node));

        Graph::Arc first_arc = graph.getFirstArc();
        CHECK(graph.isArcValid(first_arc));

        Graph::Arc next_arc = graph.getNextArc(first_arc);
        CHECK(graph.isArcValid(next_arc));

        CHECK_EQUAL(2, graph.degree(n1));
        CHECK_EQUAL(1, graph.degree(n2));
        CHECK_EQUAL(1, graph.degree(n3));

        CHECK_EQUAL(2, graph.outDegree(n1));
        CHECK_EQUAL(0, graph.inDegree(n1));

        CHECK_EQUAL(1, graph.inDegree(n2));
        CHECK_EQUAL(1, graph.inDegree(n3));

        CHECK_EQUAL(0, graph.outDegree(n2));
        CHECK_EQUAL(0, graph.outDegree(n3));

        graph.removeNode(n1);

        // The graph now has no arcs, and nodes n2,n3,n4,n5

        CHECK_EQUAL(0, graph.inDegree(n2));
        CHECK_EQUAL(0, graph.inDegree(n3));

        CHECK_EQUAL(0, graph.outDegree(n2));
        CHECK_EQUAL(0, graph.outDegree(n3));

        CHECK(!graph.isNodeValid(n1));

        denali::ObservingNodeMap<Graph, int> node_map(graph);
        for (denali::NodeIterator<Graph> it(graph);
                !it.done();
                ++it) {
            node_map[it.node()] = 42;
            CHECK_EQUAL(42, node_map[it.node()]);
        }

        Graph::Node n6 = graph.addNode();
        Graph::Arc a23 = graph.addArc(n2,n3);
        Graph::Arc a34 = graph.addArc(n3,n4);
        Graph::Arc a45 = graph.addArc(n4,n5);

        denali::ObservingArcMap<Graph, int> arcmap(graph);

        // The graph now has three arcs (those just defined) and nodes
        // n2 through n6

        CHECK(graph.isArcValid(a23));
        CHECK(graph.isArcValid(graph.findArc(n2,n3)));
        CHECK(!graph.isArcValid(graph.findArc(n3,n2)));

        graph.clearArcs();

        CHECK_EQUAL(5, graph.numberOfNodes());
        CHECK_EQUAL(0, graph.numberOfArcs());

        graph.clearNodes();

        CHECK_EQUAL(0, graph.numberOfNodes());

        Graph::Node n7 = graph.addNode();
        CHECK(!graph.isNodeValid(n6));

        CHECK_EQUAL(1, graph.numberOfNodes());
    }


    TEST(UndirectedGraph)
    {

        typedef denali::UndirectedGraph Graph;

        denali::concepts::checkConcept
            <
            denali::concepts::WritableReadableUndirectedGraph,
            Graph
            > ();

        /*
        denali::concepts::checkConcept
            <
            denali::concepts::NodeObservable<
                denali::concepts::BaseUndirectedGraph>,
            Graph
            > ();

        denali::concepts::checkConcept
            <
            denali::concepts::EdgeObservable<
                denali::concepts::BaseUndirectedGraph>,
            Graph
            > ();
        */

        Graph graph;
        Graph::Node n1 = graph.addNode();
        Graph::Node n2 = graph.addNode();
        Graph::Node n3 = graph.addNode();
        Graph::Node n4 = graph.addNode();
        Graph::Node n5 = graph.addNode();

        CHECK(graph.isNodeValid(n1));
        CHECK(graph.isNodeValid(n5));

        Graph::Edge e12 = graph.addEdge(n1,n2);

        CHECK(graph.isEdgeValid(graph.findEdge(n1,n2)));
        CHECK(graph.isEdgeValid(graph.findEdge(n2,n1)));
        CHECK(!graph.isEdgeValid(graph.findEdge(n3,n1)));

    }
}



int main()
{
    return UnitTest::RunAllTests();
}
