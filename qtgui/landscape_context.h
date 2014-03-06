#ifndef LANDSCAPE_CONTEXT_H
#define LANDSCAPE_CONTEXT_H

#include <boost/shared_ptr.hpp>

#include <denali/contour_tree.h>
#include <denali/folded.h>
#include <denali/graph_iterators.h>
#include <denali/simplify.h>

class Point
{
    double _x, _y, _z;
public:
    Point(double x, double y, double z) :
            _x(x), _y(y), _z(z)
    {}

    double x() const { return _x; }
    double y() const { return _y; }
    double z() const { return _z; }
};


class Triangle
{
    int _i, _j, _k, _id;

public:
    Triangle(int i, int j, int k, int id) :
            _i(i), _j(j), _k(k), _id(id) 
    {}

    int i() const { return _i; }
    int j() const { return _j; }
    int k() const { return _k; }
};


class LandscapeContext
{

public:
    virtual ~LandscapeContext() {}

    virtual bool isValid() const = 0;

    virtual size_t numberOfPoints() const = 0;
    virtual Point getPoint(size_t) const = 0;
    virtual Point getMaxPoint() const = 0;
    virtual Point getMinPoint() const = 0;
    virtual size_t numberOfTriangles() const = 0;
    virtual Triangle getTriangle(size_t i) const = 0;

    virtual size_t getRootID() const = 0;
    virtual size_t getMinLeafID() const = 0;
    virtual size_t getMaxLeafID() const = 0;

    virtual void buildLandscape(size_t) = 0;

    virtual void getComponentParentChild(size_t, size_t&, size_t&) const = 0;
    virtual double getComponentWeight(size_t) const = 0;
    virtual double getTotalNodeWeight(size_t) const = 0;
    virtual double getValue(size_t) const = 0;

    virtual double getMaxPersistence() const = 0;

    virtual void simplifySubtreeByPersistence(size_t, size_t, double) = 0;

};

template <typename ContourTree>
void printContourTree(const ContourTree& tree)
{
    for (denali::EdgeIterator<ContourTree> it(tree); !it.done(); ++it) {
        typename ContourTree::Node u = tree.u(it.edge());
        typename ContourTree::Node v = tree.v(it.edge());
        std::cout << tree.getID(u) << " <----> " << tree.getID(v) << std::endl;
    }
}


template <typename ContourTree, template <class T> class LandscapeBuilderTemplate>
class ConcreteLandscapeContext : public LandscapeContext
{
    typedef denali::MappableFoldedContourTree<ContourTree> FoldedContourTree;
    typedef LandscapeBuilderTemplate<FoldedContourTree> LandscapeBuilder;
    typedef typename LandscapeBuilder::LandscapeType Landscape;

    boost::shared_ptr<ContourTree> _contour_tree;
    boost::shared_ptr<LandscapeBuilder> _landscape_builder;
    boost::shared_ptr<Landscape> _landscape;

    FoldedContourTree _folded_tree;

    double _max_persistence;

public:

    ConcreteLandscapeContext(
            ContourTree* contour_tree) :
            _contour_tree(contour_tree),
            _landscape_builder(boost::shared_ptr<LandscapeBuilder>(new LandscapeBuilder)),
            _folded_tree(*_contour_tree)
    {
        _max_persistence = computeMaxPersistence(*_contour_tree);
        
        denali::PersistenceSimplifier simplifier(15); 
        simplifier.simplify(_folded_tree);
    }

    virtual bool isValid() const {
        return _landscape;
    }

    void buildLandscape(size_t root_id)
    {
        printContourTree(_folded_tree);
        typename ContourTree::Node root = _folded_tree.getNode(root_id);
        Landscape* lscape = _landscape_builder->build(_folded_tree, root);
        _landscape = boost::shared_ptr<Landscape>(lscape);
    }

    size_t getMinLeafID() const 
    {
        typename ContourTree::Node node = denali::findMinLeaf(_folded_tree);
        return _folded_tree.getID(node);
    }

    size_t getMaxLeafID() const
    {
        typename ContourTree::Node node = denali::findMaxLeaf(_folded_tree);
        return _folded_tree.getID(node);
    }

    size_t getRootID() const
    {
        typename ContourTree::Node root = 
                _landscape->getContourTreeNode(_landscape->getRoot());
        return _folded_tree.getID(root);
    }

    virtual size_t numberOfPoints() const {
        return _landscape->numberOfPoints();
    }

    virtual Point getPoint(size_t i) const
    {
        typename Landscape::Point pt = _landscape->getPoint(i);
        return Point(pt.x(), pt.y(), pt.z());
    }

    virtual Point getMaxPoint() const {
        typename Landscape::Point pt = _landscape->getMaxPoint();
        return Point(pt.x(), pt.y(), pt.z());
    }

    virtual Point getMinPoint() const {
        typename Landscape::Point pt = _landscape->getMinPoint();
        return Point(pt.x(), pt.y(), pt.z());
    }

    virtual size_t numberOfTriangles() const {
        return _landscape->numberOfTriangles();
    }

    virtual Triangle getTriangle(size_t i) const {
        typename Landscape::Triangle tri = _landscape->getTriangle(i);
        return Triangle(tri.i(), tri.j(), tri.k(), i);
    }

    virtual void getComponentParentChild(size_t i, size_t& parent, size_t& child) const
    {
        // get the triangle of this cell
        typename Landscape::Triangle tri = _landscape->getTriangle(i);

        // now map the triangle back to an arc of the landscape
        typename Landscape::Arc arc = _landscape->getComponent(tri);

        // get the endpoints of the arc
        typename Landscape::Node parent_node = _landscape->source(arc);
        typename Landscape::Node child_node  = _landscape->target(arc);

        // now map them to contour tree ids
        typename ContourTree::Node parent_ct_node = 
                _landscape->getContourTreeNode(parent_node);

        typename ContourTree::Node child_ct_node = 
                _landscape->getContourTreeNode(child_node);

        parent = _folded_tree.getID(parent_ct_node);
        child  = _folded_tree.getID(child_ct_node);
    }

    virtual double getComponentWeight(size_t i) const
    {
        // get the triangle of this cell
        typename Landscape::Triangle tri = _landscape->getTriangle(i);

        // now map the triangle back to an arc of the landscape
        typename Landscape::Arc arc = _landscape->getComponent(tri);

        return _landscape->getComponentWeight(arc);
    }

    virtual double getTotalNodeWeight(size_t i) const 
    {
        return _landscape->getTotalNodeWeight(
                            _landscape->getLandscapeTreeNode(
                            _folded_tree.getNode(i)));
    }

    virtual double getValue(size_t i) const {
        return _folded_tree.getValue(_folded_tree.getNode(i));
    }

    virtual double getMaxPersistence() const {
        return _max_persistence;
    }

    virtual void simplifySubtreeByPersistence(
            size_t parent_id,
            size_t child_id,
            double persistence)
    {
        std::cout << "simplifying " << parent_id << " " << child_id << " " << persistence << std::endl;
        typename FoldedContourTree::Node parent_node, child_node;
        parent_node = _folded_tree.getNode(parent_id);
        child_node  = _folded_tree.getNode(child_id);

        std::cout << "The tree had " << _folded_tree.numberOfNodes() << std::endl;
        expandSubtree(_folded_tree, parent_node, child_node);
        denali::PersistenceSimplifier simplifier(persistence); 
        simplifier.simplifySubtree(_folded_tree, parent_node, child_node);
        std::cout << "Now it has " << _folded_tree.numberOfNodes() << std::endl;
    }


};

#endif
