#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

#include <denali/contour_tree.h>
#include <denali/fileio.h>
#include <denali/rectangular_landscape.h>
#include <denali/visualize.h>
#include <denali/simplify.h>
#include <denali/folded.h>

// the following two functions are pasted from a stack overflow post
// see: http://stackoverflow.com/questions/865668/parse-command-line-arguments
char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}


bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}


int cli_compute(int argc, char ** argv)
{
    std::string usage =
        "usage: denali compute <vertex value file> <edge file> <tree file>\n"
        "\n"
        "Given the 1-skeleton of a simplicial complex in the form of a list of vertex\n"
        "values and a list of edges, prints the edges of the contour tree to stdout.\n"
        "\n"
        "<vertex value file>\n"
        "\tA file containing the scalar values defined at the vertices of the simplicial\n"
        "\tcomplex, one per line.\n"
        "\n"
        "<edge file>\n"
        "\tA tab-delimited file containing the edges in the 1-skeleton of the\n"
        "\tsimplicial complex. An edge is represented by the indices of the vertices\n"
        "\tit connects, where indexing starts at zero. For more information, see the\n"
        "\tdocumentation, or an example edge file.\n"
        "\n"
        "<tree file>\n"
        "\tThe file in which to place the output. The file will be overwritten without\n"
        "\twarning.";

    if (cmdOptionExists(argv, argv + argc, "-h") ||
            cmdOptionExists(argv, argv + argc, "--help")) {
        std::cout << usage << std::endl;
        return 0;
    }

    if (argc != 5) {
        std::cerr << "Insufficient number of arguments provided to compute." << std::endl;
        std::cerr << usage << std::endl;
        return 1;
    }

    try {
        // create a simplicial complex
        denali::ScalarSimplicialComplex plex;

        // read the vertices and edges into it
        denali::readSimplicialVertexFile(argv[2], plex);
        denali::readSimplicialEdgeFile(argv[3], plex);

        // compute the contour tree
        denali::CarrsAlgorithm carrs_algorithm;
        denali::ContourTree contour_tree =
            denali::ContourTree::compute(plex, carrs_algorithm);

        // write it to disk
        denali::writeContourTreeFile(argv[4], contour_tree);
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}


int cli_visualize(int argc, char ** argv)
{
    std::string usage =
        "usage: denali visualize <contour tree file> [--root (max | min | <node>)]\n"
        "\n"
        "<contour tree file>\n"
        "\tThe file resulting from using 'denali compute' to compute a contour tree.\n"
        "\n"
        "Options:\n"
        "--root\n"
        "\tThe node to use as the root of the tree. Must be a node of degree one.\n"
        "\tValid options are:\n"
        "\t\tmax:     Use the node with the maximum vertex value as the root.\n"
        "\t\tmin:     Use the node with the minimum vertex value as the root.\n"
        "\t\t<node>:  Use the specified node as the root.\n"
        "\tDefault: min\n";

    if (cmdOptionExists(argv, argv + argc, "-h") ||
            cmdOptionExists(argv, argv + argc, "--help")) {
        std::cout << usage << std::endl;
        return 0;
    }

    if (argc < 3) {
        std::cerr << "Insufficient number of arguments provided to visualize." << std::endl;
        std::cerr << usage << std::endl;
        return 1;
    }

    char * option_root = 0;
    if (argc > 4) {
        option_root = getCmdOption(argv + 3, argv + argc, "--root");
    }

    try {
        // read the contour tree
        denali::ContourTree contour_tree = denali::readContourTreeFile(argv[2]);

        // simplify the tree
        typedef denali::FoldedContourTree<denali::ContourTree> FoldedContourTree;
        denali::PersistenceSimplifier simplifier(.5);
        FoldedContourTree folded_tree(contour_tree);
        simplifier.simplify(folded_tree);

        // an error return used in the following parsers
        char * err;

        // we use the min leaf by default
        FoldedContourTree::Node root = denali::findMinLeaf(folded_tree);

        if (option_root) {
            if (strcmp(option_root, "min") == 0) {
                // do nothing, we already have the min
            } else if (strcmp(option_root, "max") == 0) {
                // get the maximum leaf
                root = denali::findMaxLeaf(folded_tree);
            } else {
                // we have to check to make sure that the root node is valid.
                // first, was it converted to an int?
                unsigned int root_id = strtol(option_root, &err, 10);
                if (*err != 0) {
                    throw std::runtime_error("Invalid root specified.");
                }

                // try to get the root
                root = folded_tree.getNode(root_id);
                if (!folded_tree.isNodeValid(root)) {
                    throw std::runtime_error("Invalid root specified. Node is not in the tree.");
                }

                // make sure it is a leaf node
                if (folded_tree.degree(root) != 1) {
                    throw std::runtime_error("Invalid root specified. It is not a leaf node.");
                }
            }
        }

        denali::RectangularLandscapeBuilder<FoldedContourTree> builder;
        denali::Visualizer<FoldedContourTree> visualizer;

        visualizer.visualize(folded_tree, root, builder);


    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}



int main(int argc, char ** argv) try
{
    std::string usage =
        "usage: denali <command> [<args>]\n"
        "\n"
        "Commands:\n"
        "    compute     Computes a contour tree from a simplicial complex.\n"
        "\n"
        "To see more information about any command, type 'denali <command> -h'. To read\n"
        "about typical workflow, see the README.";

    if (argc < 2) {
        std::cerr << "Insufficient number of arguments provided!" << std::endl;
        std::cerr << usage << std::endl;
        return 1;
    }

    // select the command to run
    if (strcmp(argv[1], "compute") == 0) {
        return cli_compute(argc, argv);
    } else if (strcmp(argv[1], "visualize") == 0) {
        return cli_visualize(argc, argv);
    }
    /*
    } else if (strcmp(argv[1], "trim") == 0) {
        return cli_trim(argc, argv);
    } else {
        std::cerr << argv[1] << " is not a valid option." << std::endl;
        std::cerr << usage << std::endl;
        return 1;
    }
    */
}
catch (std::exception& e) {
    std::cerr << "Fatal error: an uncaught exception occurred:"
              << e.what();
}
