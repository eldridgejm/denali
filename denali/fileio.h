#ifndef DENALI_FILEIO_H
#define DENALI_FILEIO_H

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <vector>

#include <denali/contour_tree.h>
#include <denali/graph_iterators.h>

namespace denali {


/// \brief Open a file, throwing exceptions if it can't be opened.
inline void safeOpenFile(const char* filename, std::ifstream& fh)
{
    fh.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        fh.open(filename);
    }
    catch (std::exception& e) {
        std::stringstream message;
        message << "Couldn't open file '" << filename << "'";
        throw std::runtime_error(message.str());
    }
}


/// \brief Parses a tabular file, calling FormatParser to handle each line.
class TabularFileParser
{
public:
    template <typename FormatParser>
    void parse(std::istream& tabstream, FormatParser& parser)
    {
        std::string line;
        tabstream.exceptions(std::ifstream::goodbit);
        while (std::getline(tabstream, line)) {

            tabstream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
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

            tabstream.exceptions(std::ifstream::goodbit);
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

    std::ifstream fh;
    safeOpenFile(filename, fh);
    parser.parse(fh, format_parser);
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

    std::ifstream fh;
    safeOpenFile(filename, fh);
    parser.parse(fh, format_parser);
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

template <typename GraphType>
class ContourTreeFormatParser
{
    typedef std::vector<std::string> Line;
    typedef typename GraphType::Node Node;
    typedef typename GraphType::Edge Edge;

    GraphType& _graph;
    unsigned int _lineno;
    unsigned int _n_vertices;

public:

    ContourTreeFormatParser(GraphType& graph)
        : _graph(graph), _lineno(0) { }

    void insert(const Line& line)
    {
        if (_lineno == 0) {
            readNumberOfVertices(line);
        } else if (_lineno <= _n_vertices) {
            readVertexLine(line);
        } else {
            readEdgeLine(line);
        }
        _lineno++;
    }

    void readNumberOfVertices(const Line& line)
    {
        if (line.size() != 1) {
            throw std::runtime_error(
                "The contour tree file's first line is malformed. It "
                "should be a single integer describing the number "
                "of vertices in the file.");
        }

        char * err;
        _n_vertices = strtol(line[0].c_str(), &err, 10);

        if (*err != 0) {
            throw std::runtime_error(
                "Could not interpret first line of contour tree file.");
        }

    }

    void readVertexLine(const Line& line)
    {
        std::stringstream msg;
        msg << "The contour tree file has a malformed vertex definition on line "
            << _lineno + 1 << ".";

        if (line.size() != 2) {
            throw std::runtime_error(msg.str());
        }

        char * id_err;
        unsigned int id = strtol(line[0].c_str(), &id_err, 10);

        char * value_err;
        double value = strtod(line[1].c_str(), &value_err);

        if (*id_err != 0 || *value_err != 0) {
            throw std::runtime_error(msg.str());
        }

        _graph.addNode(id, value);
    }

    void readEdgeLine(const Line& line)
    {
        std::stringstream msg;
        msg << "The contour tree file has a malformed edge definition on line "
            << _lineno+1 << ".";

        if (line.size() < 2)
            throw std::runtime_error(msg.str());

        char * u_err;
        char * v_err;
        unsigned int u = strtol(line[0].c_str(), &u_err, 10);
        unsigned int v = strtol(line[1].c_str(), &v_err, 10);

        if (*u_err != 0 || *v_err != 0)
            throw std::runtime_error(msg.str());

        Edge edge = _graph.addEdge(_graph.getNode(u), _graph.getNode(v));

        for (size_t i=2; i<line.size(); ++i) {
            char * err;
            unsigned int member = strtol(line[i].c_str(), &err, 10);
            if (*err != 0)
                throw std::runtime_error(msg.str());
            _graph.insertEdgeMember(edge, member);
        }

    }

};


inline ContourTree readContourTreeFromStream(
    std::istream& ctstream)
{
    // make a new graph object
    boost::shared_ptr<ContourTree::Graph> graph(new ContourTree::Graph);

    // make a new contour tree format parser
    ContourTreeFormatParser<ContourTree::Graph> format_parser(*graph);

    // and a tabular file parser
    TabularFileParser parser;

    // now parse
    parser.parse(ctstream, format_parser);

    // return the contour tree
    return denali::ContourTree::fromPrecomputed(graph);
}


inline ContourTree readContourTreeFile(
    const char * filename)
{
    // create a file stream
    std::ifstream fh;
    safeOpenFile(filename, fh);

    // read the contour tree from the stream
    return readContourTreeFromStream(fh);
}

////////////////////////////////////////////////////////////////////////////////
//
// WeightMap
//
////////////////////////////////////////////////////////////////////////////////

class WeightMapFormatParser
{
    WeightMap& _weight_map;
    int lineno;

public:

    WeightMapFormatParser(WeightMap& weight_map)
        : _weight_map(weight_map), lineno(0) { }


    void insert(std::vector<std::string>& line)
    {
        if (line.size() != 2) {
            std::stringstream msg;
            msg << "More or less than 2 entries on line " << lineno;
            throw std::runtime_error(msg.str());
        }

        char * err_u;
        long int u = strtol(line[0].c_str(), &err_u, 10);
        char * err_weight;
        double weight = strtod(line[1].c_str(), &err_weight);

        if (*err_u != 0 || *err_weight != 0 || u < 0) {
            std::stringstream msg;
            msg << "Problem interpreting line " << lineno << " as an edge.";
            throw std::runtime_error(msg.str());
        }
        lineno++;

        _weight_map[u] = weight;
    }
};


inline void readWeightMapFromStream(
    std::istream& ctstream,
    WeightMap& weight_map)
{
    // clear the weight map
    weight_map.clear();

    // make a new contour tree format parser
    WeightMapFormatParser format_parser(weight_map);

    // and a tabular file parser
    TabularFileParser parser;

    // now parse
    parser.parse(ctstream, format_parser);
}


inline void readWeightMapFile(
    const char * filename,
    WeightMap& weight_map)
{
    // create a file stream
    std::ifstream fh;
    safeOpenFile(filename, fh);

    readWeightMapFromStream(fh, weight_map);
}

} // namespace denali

#endif
