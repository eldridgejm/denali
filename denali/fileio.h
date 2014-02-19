#ifndef DENALI_FILEIO_H
#define DENALI_FILEIO_H

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <sstream>

#include <denali/graph_iterators.h>

namespace denali {

    /// \brief Parses a tabular file, calling FormatParser to handle each line.
    class TabularFileParser 
    {
        public:
        template <typename FormatParser>
        void parse(const char * filename, FormatParser& parser)
        {
            // create and open a file handle
            std::ifstream fh;
            fh.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            try {
                fh.open(filename);
            }
            catch (std::exception& e) {
                std::stringstream message;
                message << "Couldn't open file '" << filename << "'";
                throw std::runtime_error(message.str());
            }

            std::string line;
            fh.exceptions(std::ifstream::goodbit);
            while (std::getline(fh, line)) {

                fh.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                std::vector<std::string> tokenized_line;

                // add one to the size to account for null
                char * cline = new char[line.size() + 1];  

                // copy the line
                strcpy(cline, line.c_str());

                // tokenize it
                char * pch = strtok(cline, "\t ");
                while (pch != NULL) {
                    tokenized_line.push_back(pch);
                    pch = strtok(NULL, "\t ");
                }

                delete [] cline;
                parser.insert(tokenized_line);

                fh.exceptions(std::ifstream::goodbit);
            }
        }
    };

    ////////////////////////////////////////////////////////////////////////////
    //
    // ScalarSimplicialVertex
    //
    ////////////////////////////////////////////////////////////////////////////

    template <typename ScalarSimplicialComplex>
    class VertexValueFormatParser
    {
        ScalarSimplicialComplex& plex;
        int lineno;

        public:

        VertexValueFormatParser(ScalarSimplicialComplex& plex)
            : plex(plex), lineno(0) { }
        
        void insert(std::vector<std::string>& line) 
        {
            if (line.size() == 0) {
                std::stringstream msg;
                msg << "Not enough values to read on line " << lineno;
                throw std::runtime_error(msg.str());
            }

            if (line.size() > 1) {
                std::stringstream msg;
                msg << "Too many values on line " << lineno;
                throw std::runtime_error(msg.str());
            }

            // convert the first element to a double
            char * err;
            double value = strtod(line.front().c_str(), &err);

            if (*err != 0) {
                std::stringstream msg;
                msg << "Could not convert line number " << lineno << " to a vertex value.";
                throw std::runtime_error(msg.str());
            }

            lineno++;
            plex.addNode(value);
        }
    };


    /// \brief Read vertex values into a scalar simplicial complex.
    /// \ingroup fileio
    template <typename ScalarSimplicialComplex>
    void readSimplicialVertexFile(
            const char * filename,
            ScalarSimplicialComplex& plex)
    {
        VertexValueFormatParser<ScalarSimplicialComplex> format_parser(plex);
        TabularFileParser parser;
        parser.parse(filename, format_parser);
    }

    ////////////////////////////////////////////////////////////////////////////
    //
    // ScalarSimplicialEdge
    //
    ////////////////////////////////////////////////////////////////////////////

    template <typename ScalarSimplicialComplex>
    class EdgeFormatParser
    {
        ScalarSimplicialComplex& plex;
        int lineno;

        public:

        EdgeFormatParser(ScalarSimplicialComplex& plex)
            : plex(plex), lineno(0) { }


        void insert(std::vector<std::string>& line)
        {
            if (line.size() != 2) {
                std::stringstream msg;
                msg << "More or less than 2 node ids on line " << lineno;
                throw std::runtime_error(msg.str());
            }

            char * err_u;
            long int u = strtol(line[0].c_str(), &err_u, 10);
            char * err_v;
            long int v = strtol(line[1].c_str(), &err_v, 10);

            if (*err_u != 0 || *err_v != 0) {
                std::stringstream msg;
                msg << "Problem interpreting line " << lineno << " as an edge.";
                throw std::runtime_error(msg.str());
            }
            lineno++;
            plex.addEdge(plex.getNode(u),plex.getNode(v));
        }
    };


    /// \brief Read edges into a scalar simplicial complex.
    /// \ingroup fileio
    template <typename ScalarSimplicialComplex>
    void readSimplicialEdgeFile(
            const char * filename,
            ScalarSimplicialComplex& plex)
    {
        EdgeFormatParser<ScalarSimplicialComplex> format_parser(plex);
        TabularFileParser parser;
        parser.parse(filename, format_parser);
    }

    ////////////////////////////////////////////////////////////////////////////
    //
    // WriteContourTree
    //
    ////////////////////////////////////////////////////////////////////////////
    
    template <typename ContourTree>
    void writeContourTreeFile(
            const char * filename,
            const ContourTree& tree)
    {
        typedef typename ContourTree::Members Members;

        std::ofstream fh;
        fh.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            fh.open(filename);
        }
        catch (std::exception& e) {
            std::stringstream message;
            message << "Couldn't open file '" << filename << "'";
            throw std::runtime_error(message.str());
        }

        // write the number of vertices
        fh << tree.numberOfNodes() << std::endl;

        // write each vertex to the file
        for (NodeIterator<ContourTree> it(tree);
                !it.done(); ++it) {
            fh << tree.getID(it.node()) << "\t" << tree.getValue(it.node());

            // get the node's members
            const Members& members = tree.getNodeMembers(it.node());

            for (typename Members::const_iterator m_it = members.begin();
                    m_it != members.end(); ++m_it) {
                fh << "\t" << *m_it;
            }

            fh << std::endl;
        }

        // write each edge to the file
        for (EdgeIterator<ContourTree> it(tree);
                !it.done(); ++it) {
            fh << tree.getID(tree.u(it.edge())) 
               << "\t" 
               << tree.getID(tree.v(it.edge()));

            const Members& members = tree.getEdgeMembers(it.edge());

            for (typename Members::const_iterator m_it = members.begin();
                    m_it != members.end(); ++m_it) {
                fh << "\t" << *m_it;
            }

            fh << std::endl;
        }

        fh.close();
    }

    ////////////////////////////////////////////////////////////////////////////
    //
    // ReadContourTree
    //
    ////////////////////////////////////////////////////////////////////////////

    template <typename ContourTreeBuilder>
    class ContourTreeFormatParser
    {
        unsigned int lineno;
        ContourTreeBuilder& _builder;

    public:
        ContourTreeFormatParser(ContourTreeBuilder& builder)
                : _builder(builder), lineno(0) { }

        void insert(std::vector<std::string>& line)
        {
            std::cout << "Inserting: " << line[0] << std::endl;
        }


    };

    
            
}

#endif
