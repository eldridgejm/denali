#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

#include <denali/contour_tree.h>
#include <denali/fileio.h>

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

int main(int argc, char ** argv) try
{
    std::string usage =
        "usage: ctree <vertex value file> <edge file> <tree file>\n"
        "\n"
        "Given the 1-skeleton of a simplicial complex in the form of a list of vertex\n"
        "values and a list of edges, prints the edges of the contour tree to the tree file.\n"
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

    if (argc != 4) {
        std::cerr << "Insufficient number of arguments provided." << std::endl;
        std::cerr << usage << std::endl;
        return 1;
    }

    try {
        // create a simplicial complex
        denali::ScalarSimplicialComplex plex;

        // read the vertices and edges into it
        denali::readSimplicialVertexFile(argv[1], plex);
        denali::readSimplicialEdgeFile(argv[2], plex);

        // compute the contour tree
        denali::CarrsAlgorithm carrs_algorithm;
        denali::ContourTree contour_tree =
            denali::ContourTree::compute(plex, carrs_algorithm);

        typedef denali::CarrsAlgorithm::JoinSplitTree JoinSplitTree;

        const JoinSplitTree& join_tree = carrs_algorithm.getJoinTree();
        const JoinSplitTree& split_tree = carrs_algorithm.getSplitTree();

        std::cout << "The join tree has " << join_tree.numberOfNodes() << " nodes." << std::endl;

        // write it to disk
        denali::writeContourTreeFile(argv[3], contour_tree);
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
catch (std::exception& e) {
    std::cerr << "Fatal error: an uncaught exception occurred:"
              << e.what();
}
