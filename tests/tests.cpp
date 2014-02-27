#include <UnitTest++.h>
#include <iostream>

#include <string>
#include <set>
#include <vector>

#include <denali/concepts/check.h>
#include <denali/concepts/graph_attributes.h>
#include <denali/concepts/contour_tree.h>
#include <denali/concepts/landscape.h>
#include <denali/fileio.h>
#include <denali/graph_mixins.h>
#include <denali/graph_maps.h>
#include <denali/graph_iterators.h>
#include <denali/graph_structures.h>
#include <denali/contour_tree.h>
#include <denali/landscape.h>
#include <denali/rectangular_landscape.h>
#include <denali/simplify.h>
#include <denali/folded.h>

double wenger_vertex_values[] =
// 0   1   2   3   4   5   6   7   8   9  10  11
{ 25, 62, 45, 66, 16, 32, 64, 39, 58, 51, 53, 30 };

const size_t n_wenger_vertices = sizeof(wenger_vertex_values)/sizeof(double);

unsigned int wenger_edges[][2] =
{   {0, 1}, {1, 2}, {3, 4}, {4, 5}, {6, 7}, {7, 8}, {9, 10}, {10, 11}, {0, 3},
    {1, 4}, {2, 5}, {3, 6}, {4, 7}, {5, 8}, {6, 9}, {7, 10}, {8, 11}, {0, 4},
    {1, 5}, {3, 7}, {4, 8}, {6, 10}, {7, 11}
};

const size_t n_wenger_edges = sizeof(wenger_edges)/sizeof(unsigned int[2]);


template <typename UndirectedGraph, typename IDMap>
void printUndirectedGraph(const UndirectedGraph& graph, IDMap& idmap)
{
    for (denali::EdgeIterator<UndirectedGraph> it(graph); !it.done(); ++it) {
        typename UndirectedGraph::Node u = graph.u(it.edge());
        typename UndirectedGraph::Node v = graph.v(it.edge());
        std::cout << idmap[u] << " <----> " << idmap[v] << std::endl;
    }
}


template <typename ContourTree>
void printContourTree(const ContourTree& tree)
{
    for (denali::EdgeIterator<ContourTree> it(tree); !it.done(); ++it) {
        typename ContourTree::Node u = tree.u(it.edge());
        typename ContourTree::Node v = tree.v(it.edge());
        std::cout << tree.getID(u) << " <----> " << tree.getID(v) << std::endl;
    }
}


TEST(Mixins)
{
    denali::concepts::checkConcept
    <
    denali::concepts::ReadableDirectedGraph,
           denali::ReadableDirectedGraphMixin
           <
           denali::concepts::ReadableDirectedGraph,
           denali::BaseGraphMixin<denali::concepts::ReadableDirectedGraph>
           >
           > ();

    denali::concepts::checkConcept
    <
    denali::concepts::WritableReadableDirectedGraph,
           denali::WritableReadableDirectedGraphMixin
           <
           denali::concepts::WritableReadableDirectedGraph,
           denali::BaseGraphMixin<denali::concepts::WritableReadableDirectedGraph>
           >
           > ();

    typedef denali::concepts::NodeObservable <
    denali::concepts::BaseObservable<
    denali::concepts::BaseUndirectedGraph
    >
    >
    MockNodeObservable;

    typedef denali::concepts::ArcObservable <
    denali::concepts::BaseObservable<
    denali::concepts::BaseDirectedGraph
    >
    >
    MockArcObservable;

    typedef denali::concepts::EdgeObservable <
    denali::concepts::BaseObservable<
    denali::concepts::BaseUndirectedGraph
    >
    >
    MockEdgeObservable;

    denali::concepts::checkConcept
    <
    MockNodeObservable,
    denali::NodeObservableMixin<MockNodeObservable, MockNodeObservable>
    > ();

    denali::concepts::checkConcept
    <
    MockArcObservable,
    denali::ArcObservableMixin<MockArcObservable, MockArcObservable>
    > ();

    denali::concepts::checkConcept
    <
    MockEdgeObservable,
    denali::EdgeObservableMixin<MockEdgeObservable, MockEdgeObservable>
    > ();
}


SUITE(GraphStructures)
{
    TEST(DirectedGraphBase)
    {
        // Check for coverage:
        /*
         *  Implementation must conform to:
         *   - NodeObservable
         *   - ArcObservable
         *   - WritableReadableDirectedGraph
         */
        typedef denali::concepts::NodeObservable <
        denali::concepts::ArcObservable <
        denali::concepts::BaseObservable <
        denali::concepts::WritableReadableDirectedGraph > > >
        Implementation;

        denali::concepts::checkConcept<
        denali::concepts::NodeObservable<
        denali::concepts::BaseObservable<
        denali::concepts::BaseDirectedGraph> >,
               denali::DirectedGraphBase<Implementation>
               > ();

        denali::concepts::checkConcept<
        denali::concepts::ArcObservable<
        denali::concepts::BaseObservable<
        denali::concepts::BaseDirectedGraph> >,
               denali::DirectedGraphBase<Implementation>
               > ();

    }

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

        graph.clearNodes();

        denali::ObservingNodeMap<Graph, int> ids(graph);

        n1 = graph.addNode();
        ids[n1] = 1;
        n2 = graph.addNode();
        ids[n2] = 2;
        n3 = graph.addNode();
        ids[n3] = 3;
        n4 = graph.addNode();
        ids[n4] = 4;
        n5 = graph.addNode();
        ids[n5] = 5;

        graph.addArc(n3,n1);
        graph.addArc(n3,n2);
        graph.addArc(n4,n3);
        graph.addArc(n5,n3);

    }



    TEST(UndirectedGraph)
    {

        typedef denali::UndirectedGraph Graph;

        denali::concepts::checkConcept
        <
        denali::concepts::WritableReadableUndirectedGraph,
               Graph
               > ();

        denali::concepts::checkConcept
        <
        denali::concepts::NodeObservable<
        denali::concepts::BaseObservable<
        denali::concepts::BaseUndirectedGraph> >,
               Graph
               > ();

        denali::concepts::checkConcept
        <
        denali::concepts::EdgeObservable<
        denali::concepts::BaseObservable<
        denali::concepts::BaseUndirectedGraph> >,
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

        Graph::Edge e12 = graph.addEdge(n1,n2);

        CHECK(graph.isEdgeValid(graph.findEdge(n1,n2)));
        CHECK(graph.isEdgeValid(graph.findEdge(n2,n1)));
        CHECK(!graph.isEdgeValid(graph.findEdge(n3,n1)));
    }


    TEST(UndirectedBFSIterator)
    {
        typedef denali::UndirectedGraph Graph;

        Graph graph;
        denali::ObservingNodeMap<Graph, int> node_ids(graph);
        std::vector<Graph::Node> nodes;

        for (int i=0; i<13; ++i) {
            Graph::Node node = graph.addNode();
            node_ids[node] = i;
            nodes.push_back(node);
        }

        unsigned int edges[][2] =
        {   {0,1}, {1,4}, {1,5}, {1,3}, {9,3}, {0,3}, {2,0},
            {2,6}, {7,2}, {2,8}, {8,11}, {12,8}
        };

        size_t n_edges = 12;

        for (int i=0; i<n_edges; ++i) {
            graph.addEdge(nodes[edges[i][0]], nodes[edges[i][1]]);
        }

        /*
        for (denali::UndirectedBFSIterator<Graph> it(graph, nodes[0], nodes[2]);
                !it.done(); ++it) {
            std::cout << node_ids[it.parent()] << " --> "
                    << node_ids[it.child()] << std::endl;
        }
        */

    }
}


SUITE(ContourTree)
{

    TEST(ScalarSimplicialComplex)
    {
        denali::concepts::checkConcept
        <
        denali::concepts::ScalarSimplicialComplex,
               denali::ScalarSimplicialComplex
               > ();

        denali::ScalarSimplicialComplex plex;

        for (int i=0; i<n_wenger_vertices; ++i) {
            plex.addNode(wenger_vertex_values[i]);
        }

        for (int i=0; i<n_wenger_edges; ++i) {
            plex.addEdge(
                plex.getNode(wenger_edges[i][0]),
                plex.getNode(wenger_edges[i][1]));
        }

        CHECK_EQUAL(n_wenger_vertices, plex.numberOfNodes());
        CHECK_EQUAL(n_wenger_edges, plex.numberOfEdges());
    }

    TEST(UndirectedScalarMemberIDGraph)
    {
        denali::concepts::checkConcept
        <
        denali::concepts::UndirectedScalarMemberIDGraph,
               denali::UndirectedScalarMemberIDGraph
               > ();

        typedef denali::UndirectedScalarMemberIDGraph Graph;
        Graph graph;
    }

    TEST(TotalOrder)
    {
        denali::concepts::checkConcept
        <
        denali::concepts::TotalOrder,
               denali::TotalOrder
               > ();
    }

    TEST(CarrsAlgorithm)
    {
        denali::concepts::checkConcept
        <
        denali::concepts::ContourTreeAlgorithm,
               denali::CarrsAlgorithm
               > ();

        denali::ScalarSimplicialComplex plex;

        for (int i=0; i<n_wenger_vertices; ++i) {
            plex.addNode(wenger_vertex_values[i]);
        }

        for (int i=0; i<n_wenger_edges; ++i) {
            plex.addEdge(
                plex.getNode(wenger_edges[i][0]),
                plex.getNode(wenger_edges[i][1]));
        }

        denali::CarrsAlgorithm alg;
        denali::UndirectedScalarMemberIDGraph graph;

        alg.compute(plex, graph);

    }

    TEST(ContourTree)
    {
        denali::concepts::checkConcept
        <
        denali::concepts::ContourTree,
               denali::ContourTree
               > ();


        denali::ScalarSimplicialComplex plex;

        for (int i=0; i<n_wenger_vertices; ++i) {
            plex.addNode(wenger_vertex_values[i]);
        }

        for (int i=0; i<n_wenger_edges; ++i) {
            plex.addEdge(
                plex.getNode(wenger_edges[i][0]),
                plex.getNode(wenger_edges[i][1]));
        }

        denali::CarrsAlgorithm alg;
        denali::ContourTree tree =
            denali::ContourTree::compute(plex, alg);

    }

}


SUITE(Landscape)
{

    TEST(LandscapeTree)
    {
        denali::ScalarSimplicialComplex plex;

        for (int i=0; i<n_wenger_vertices; ++i) {
            plex.addNode(wenger_vertex_values[i]);
        }

        for (int i=0; i<n_wenger_edges; ++i) {
            plex.addEdge(
                plex.getNode(wenger_edges[i][0]),
                plex.getNode(wenger_edges[i][1]));
        }

        denali::CarrsAlgorithm alg;
        denali::ContourTree tree =
            denali::ContourTree::compute(plex, alg);

        denali::LandscapeTree<denali::ContourTree> lscape(tree, tree.getNode(4));

        denali::LandscapeWeights<denali::LandscapeTree<denali::ContourTree> > weights(lscape);

        CHECK_EQUAL(12, weights.getTotalNodeWeight(lscape.getRoot()));

    }
}

SUITE(RectangularLandscape)
{
    TEST(RectangularLandscape)
    {
        denali::ScalarSimplicialComplex plex;

        for (int i=0; i<n_wenger_vertices; ++i) {
            plex.addNode(wenger_vertex_values[i]);
        }

        for (int i=0; i<n_wenger_edges; ++i) {
            plex.addEdge(
                plex.getNode(wenger_edges[i][0]),
                plex.getNode(wenger_edges[i][1]));
        }

        denali::CarrsAlgorithm alg;
        denali::ContourTree tree =
            denali::ContourTree::compute(plex, alg);

        typedef denali::RectangularLandscape<denali::ContourTree> RectangularLandscape;
        RectangularLandscape rlscape(tree, tree.getNode(4));

    }

}


SUITE(fileio)
{
    TEST(readContourTree)
    {

        denali::ContourTree ct = denali::readContourTreeFile("wenger_tree");

        CHECK_EQUAL(9, ct.numberOfNodes());
        CHECK_EQUAL(8, ct.numberOfEdges());
    }
}


SUITE(Simplify)
{

    TEST(PersistenceSimplifier)
    {

        denali::ScalarSimplicialComplex plex;

        for (int i=0; i<n_wenger_vertices; ++i) {
            plex.addNode(wenger_vertex_values[i]);
        }

        for (int i=0; i<n_wenger_edges; ++i) {
            plex.addEdge(
                plex.getNode(wenger_edges[i][0]),
                plex.getNode(wenger_edges[i][1]));
        }

        typedef denali::ContourTree ContourTree;
        typedef denali::FoldedContourTree<ContourTree> FoldedContourTree;

        denali::CarrsAlgorithm alg;

        ContourTree tree = ContourTree::compute(plex, alg);

        denali::PersistenceSimplifier simplifier(15);
        FoldedContourTree folded_tree(tree);
        simplifier.simplify(folded_tree);
    }
}


SUITE(Folded)
{
    TEST(FoldTree)
    {
        typedef denali::FoldTree FoldTree;

        FoldTree fold_tree;
        std::map<FoldTree::Node, int> idmap;

        FoldTree::Node n1 = fold_tree.addNode();
        idmap[n1] = 1;
        FoldTree::Node n2 = fold_tree.addNode();
        idmap[n2] = 2;
        FoldTree::Node n3 = fold_tree.addNode();
        idmap[n3] = 3;
        FoldTree::Node n4 = fold_tree.addNode();
        idmap[n4] = 4;
        FoldTree::Node n5 = fold_tree.addNode();
        idmap[n5] = 5;
        FoldTree::Node n6 = fold_tree.addNode();
        idmap[n6] = 6;

        FoldTree::Edge e12 = fold_tree.addEdge(n1,n2);
        FoldTree::Edge e23 = fold_tree.addEdge(n2,n3);
        FoldTree::Edge e24 = fold_tree.addEdge(n2,n4);
        FoldTree::Edge e45 = fold_tree.addEdge(n5,n4);
        FoldTree::Edge e46 = fold_tree.addEdge(n6,n4);

        fold_tree.collapse(e45);

        CHECK_EQUAL(5, fold_tree.numberOfNodes());
        CHECK_EQUAL(6, fold_tree.numberOfNodeFolds());
        CHECK_EQUAL(2, fold_tree.degree(n4));

        FoldTree::Edge e26 = fold_tree.reduce(n4);

        CHECK(!fold_tree.isNodeValid(n4));
        CHECK_EQUAL(4, fold_tree.numberOfNodes());
        CHECK_EQUAL(1, fold_tree.degree(n6));

        n4 = fold_tree.unreduce(e26);
        fold_tree.uncollapse(n4);

    }


    TEST(FoldedContourTree)
    {
        std::cout << "Folded Contour Tree" << std::endl;

        denali::ScalarSimplicialComplex plex;

        for (int i=0; i<n_wenger_vertices; ++i) {
            plex.addNode(wenger_vertex_values[i]);
        }

        for (int i=0; i<n_wenger_edges; ++i) {
            plex.addEdge(
                plex.getNode(wenger_edges[i][0]),
                plex.getNode(wenger_edges[i][1]));
        }

        typedef denali::ContourTree ContourTree;
        typedef denali::FoldedContourTree<ContourTree> FoldedContourTree; 

        denali::CarrsAlgorithm alg;
        ContourTree tree = ContourTree::compute(plex, alg);

        printContourTree(tree);
        std::cout << "=====" << std::endl;

        denali::PersistenceSimplifier simplifier(15);
        FoldedContourTree folded_tree(tree);
        simplifier.simplify(folded_tree);

        printContourTree(folded_tree);
        std::cout << "=====" << std::endl;

        FoldedContourTree::Edge edge = folded_tree.findEdge(
                folded_tree.getNode(5), folded_tree.getNode(8));

        CHECK(folded_tree.isEdgeValid(edge)); 

        printContourTree(folded_tree);
        std::cout << "=====" << std::endl;

        // expand the 5 --> 8 subtree
        expandSubtree(folded_tree, folded_tree.getNode(5), folded_tree.getNode(8));


        printContourTree(folded_tree);

    }


}



int main()
{
    return UnitTest::RunAllTests();
}
