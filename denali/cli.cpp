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
        /*
        // create a simplicial complex
        denali::SimplicialComplex plex;

        // read the vertices and edges into it
        denali::readSimplicialVertexFile(argv[2], plex);
        denali::readSimplicialEdgeFile(argv[3], plex);

        // compute the contour tree
        denali::ContourTree contour_tree = denali::ContourTree::compute(plex);

        // write it to the file
        denali::writeContourTreeFile(argv[4], contour_tree);
        */
        // create a simplicial complex
        denali::ScalarSimplicialComplex plex;

        // read the vertices and edges into it
        denali::readSimplicialVertexFile(argv[2], plex);
        denali::readSimplicialEdgeFile(argv[3], plex);

        // compute the contour tree
        denali::CarrsAlgorithm carrs_algorithm;
        denali::ComputedContourTree tree = 
                denali::ComputedContourTree::compute(plex, carrs_algorithm);



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
    }
    /*
    } else if (strcmp(argv[1], "visualize") == 0) {
        return cli_visualize(argc, argv);
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
