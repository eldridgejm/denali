// Copyright (c) 2014, Justin Eldridge, Mikhail Belkin, and Yusu Wang
// at The Ohio State University. All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
        "             [--join <filename>] [--split <filename>]\n"
        "\n"
        "Given the 1-skeleton of a simplicial complex in the form of a list of\n"
        "vertex values and a list of edges, prints the edges of the contour\n"
        "tree to the tree file.\n"
        "\n"
        "Required arguments:\n"
        "<vertex value file>\n"
        "\tA file containing the scalar values defined at the vertices of the\n"
        "\tsimplicial complex, one per line.\n"
        "\n"
        "<edge file>\n"
        "\tA tab-delimited file containing the edges in the 1-skeleton of\n"
        "\tthe simplicial complex. An edge is represented by the indices of\n"
        "\tthe vertices it connects, where indexing starts at zero. For more\n"
        "\tinformation, see the documentation, or an example edge file.\n"
        "\n"
        "<tree file>\n"
        "\tThe file in which to place the output. The file will be overwritten\n"
        "\twithout warning.\n"
        "\n"
        "Optional arguments:\n"
        "--join <filename>\n"
        "\tAlso output the join tree to the specified file.\n"
        "\n"
        "--split <filename>\n"
        "\tAlso output the split tree to the specified file.\n";

    if (cmdOptionExists(argv, argv + argc, "-h") ||
            cmdOptionExists(argv, argv + argc, "--help")) {
        std::cout << usage << std::endl;
        return 0;
    }

    if (argc < 4) {
        std::cerr << "Insufficient number of arguments provided." << std::endl;
        std::cerr << usage << std::endl;
        return 1;
    }

    char* join_file = getCmdOption(argv, argv + argc, "--join");
    char* split_file = getCmdOption(argv, argv + argc, "--split");

    try {
        // create a simplicial complex
        denali::ScalarSimplicialComplex plex;

        // read the vertices and edges into it
        denali::readSimplicialVertexFile(argv[1], plex);
        denali::readSimplicialEdgeFile(argv[2], plex);

        // check that the input graph is connected
        if (!denali::isConnected(plex))
        {
            std::cerr << "Error: The input graph is not connected." << std::endl;
            return 1;
        }

        // compute the contour tree
        denali::CarrsAlgorithm carrs_algorithm;

        if (join_file || split_file)
        {
            carrs_algorithm.setCopyJoinSplitTrees(true);
        }

        denali::ContourTree contour_tree =
            denali::ContourTree::compute(plex, carrs_algorithm);

        typedef denali::CarrsAlgorithm::JoinSplitTree JoinSplitTree;

        if (join_file)
        {
            const JoinSplitTree& join_tree = carrs_algorithm.getJoinTree();
            denali::writeJoinSplitTreeFile(join_file, join_tree, plex);
        }

        if (split_file)
        {
            const JoinSplitTree& split_tree = carrs_algorithm.getSplitTree();
            denali::writeJoinSplitTreeFile(split_file, split_tree, plex);
        }

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
