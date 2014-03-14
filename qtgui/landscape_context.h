#ifndef LANDSCAPE_CONTEXT_H
#define LANDSCAPE_CONTEXT_H

#include <boost/shared_ptr.hpp>
#include <cmath>
#include <sstream>

#include <denali/contour_tree.h>
#include <denali/fileio.h>
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


class Reduction
{
public:
    virtual ~Reduction() {}
    virtual void insert(double, double) = 0;
    virtual double reduce() = 0;
    virtual void clear() = 0;
};


class MaxReduction : public Reduction
{
    bool _inserted;
    double _current;

public:
    MaxReduction() : 
        _inserted(false), _current(0)  {}

    void insert(double reference_value, double color_value) {
        if (!_inserted || (color_value > _current)) {
            _current = color_value;
            _inserted = true;
        }
    }

    virtual double reduce() {
        if (!_inserted)
            throw std::runtime_error("Reducing with nothing inserted.");

        return _current;
    }

    virtual void clear() {
        _inserted = false;
        _current = 0;
    }
};


class MinReduction : public Reduction
{
    bool _inserted;
    double _current;

public:
    MinReduction() : 
        _inserted(false), _current(0)  {}

    virtual void insert(double reference_value, double color_value) {
        if (!_inserted || (color_value < _current)) {
            _current = color_value;
            _inserted = true;
        }
    }

    virtual double reduce() {
        if (!_inserted)
            throw std::runtime_error("Reducing with nothing inserted.");

        return _current;
    }

    virtual void clear() {
        _inserted = false;
        _current = 0;
    }
};


class MeanReduction : public Reduction
{
    unsigned int _n;
    double _sum;

public:
    MeanReduction() : _n(0), _sum(0) {}

    virtual void insert(double reference_value, double color_value)
    {
        _n++;
        _sum += color_value;
    }

    virtual double reduce() {
        return _sum / _n;
    }

    virtual void clear() {
        _n = 0;
        _sum = 0;
    }
};


class CountReduction : public Reduction
{
    unsigned int _n;

public:
    CountReduction() : _n(0) {}

    virtual void insert(double reference_value, double color_value) {
        _n++;
    }

    virtual double reduce() {
        return _n;
    }

    virtual void clear() {
        _n = 0;
    }
};


/// \brief An online variance reduction, using Knuth's algorithm.
class VarianceReduction : public Reduction
{
    unsigned int _n;
    double _mean;
    double _m2;

public:
    VarianceReduction() : _n(0), _mean(0), _m2(0) {}

    virtual void insert(double reference_value, double color_value)
    {
        _n++;
        double delta = color_value - _mean;
        _mean += delta / _n;
        _m2 += delta * (color_value - _mean);
    }

    virtual double reduce() {
        return _m2 / _n;
    }

    virtual void clear() {
        _n = _mean = _m2 = 0;
    }

};


class CovarianceReduction : public Reduction
{
    double _sum_xy;
    double _sum_x;
    double _sum_y;
    unsigned int _n;

public:
    CovarianceReduction() : _sum_xy(0), _sum_x(0), _sum_y(0), _n(0) {}

    virtual void insert(double x, double y)
    {
        _sum_x += x;
        _sum_y += y;
        _sum_xy += (x*y);
        _n++;
    }

    virtual double reduce() 
    {
        // NOTE: investigate this for catastrophic cancellation
        double e_xy = _sum_xy / _n;
        double e_x = _sum_x / _n;
        double e_y = _sum_y / _n;
        return e_xy - (e_x * e_y);
    }

    virtual void clear() {
        _sum_xy = _sum_x = _sum_y = _n = 0;
    }
};


class CorrelationReduction : public Reduction
{
    VarianceReduction _var_x;
    VarianceReduction _var_y;
    CovarianceReduction _cov;

public:
    
    virtual void insert(double x, double y)
    {
        std::cout << "Inserting " << x << " " << y << std::endl;
        _var_x.insert(y, x);
        _var_y.insert(x, y);
        _cov.insert(x, y);
    }

    virtual double reduce()
    {
        double sigma_x = std::sqrt(_var_x.reduce());
        double sigma_y = std::sqrt(_var_y.reduce());
        std::cout << "Cov: " << _cov.reduce() << std::endl;
        std::cout << "Sigma x: " << sigma_x << std::endl;
        std::cout << "Sigma y: " << sigma_y << std::endl;
        return _cov.reduce() / (sigma_x * sigma_y);
    }

    virtual void clear()
    {
        _var_x.clear();
        _var_y.clear();
        _cov.clear();
    }

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

    /// \brief Get the component ID of the ith cell
    virtual size_t getComponentIdentifierFromTriangle(size_t i) const = 0;

    /// \brief Get the maximum possible component ID
    virtual size_t getMaxComponentIdentifier() const = 0;

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

    virtual void setWeightMap(denali::WeightMap*) = 0;

    virtual void setColorMap(denali::ColorMap*) = 0;
    virtual void setColorReduction(Reduction*) = 0;
    virtual double getComponentReductionValue(unsigned int) = 0;
    virtual double getMaxReductionValue() = 0;
    virtual double getMinReductionValue() = 0;
    virtual void setMaxReductionValue(double) = 0;
    virtual void setMinReductionValue(double) = 0;
    virtual bool hasReduction() const = 0;

};


template <typename ContourTree, template <class T> class LandscapeBuilderTemplate>
class ConcreteLandscapeContext : public LandscapeContext
{
    typedef denali::FoldedContourTree<ContourTree> FoldedContourTree;
    typedef LandscapeBuilderTemplate<FoldedContourTree> LandscapeBuilder;
    typedef typename LandscapeBuilder::LandscapeType Landscape;
    typedef denali::ColorMap ColorMap;
    typedef denali::WeightMap WeightMap;
    typedef denali::ObservingEdgeMap<FoldedContourTree, double> ReductionMap;

    boost::shared_ptr<ContourTree> _contour_tree;
    boost::shared_ptr<LandscapeBuilder> _landscape_builder;
    boost::shared_ptr<Landscape> _landscape;
    boost::shared_ptr<WeightMap> _weight_map;
    boost::shared_ptr<ColorMap> _color_map;
    boost::shared_ptr<Reduction> _reduction;

    FoldedContourTree _folded_tree;

    boost::shared_ptr<ReductionMap> _reduction_map;
    double _max_reduction;
    double _min_reduction;

    double _max_persistence;

    virtual double getColorMapValue(unsigned int id) const
    {
        ColorMap::const_iterator it = (*_color_map).find(id);
        if (it == (*_color_map).end()) {
            std::stringstream message;
            message << "The member '" << id << "' is not in the color map." 
                    << std::endl;
            throw std::runtime_error(message.str());
        }

        return it->second;
    }

    virtual double computeEdgeReduction(typename FoldedContourTree::Edge edge) const
    {
        typedef typename FoldedContourTree::Members Members;

        if (!_folded_tree.isEdgeValid(edge))
            throw std::runtime_error("Invalid edge for reduction.");

        // get the edge's members
        const Members& edge_members = _folded_tree.getEdgeMembers(edge);

        // insert each member into reduction
        _reduction->clear();
        for (typename Members::const_iterator it = edge_members.begin();
                it != edge_members.end(); ++it)
        {
            unsigned int member_id = (*it).getID();

            // the reference value is the value of the contour tree node
            double reference_value = (*it).getValue();

            // lookup the member in the color map
            double color_value = getColorMapValue(member_id);

            // insert the unsigned int id of the member
            _reduction->insert(reference_value, color_value);
        }

        // do the same for each of the edge's nodes
        typename FoldedContourTree::Node u = _folded_tree.u(edge);
        typename FoldedContourTree::Node v = _folded_tree.v(edge);

        unsigned int u_id = _folded_tree.getID(u);
        unsigned int v_id = _folded_tree.getID(v);

        double u_reference_value = _folded_tree.getValue(u);
        double v_reference_value = _folded_tree.getValue(v);

        double u_color_value = getColorMapValue(u_id);
        double v_color_value = getColorMapValue(v_id);

        _reduction->insert(u_reference_value, u_color_value);
        _reduction->insert(v_reference_value, v_color_value);
        
        return _reduction->reduce();
    }    
    
    void computeReductions()
    {
        assert(_color_map && _reduction);
        bool first_iteration = true;
        for (denali::EdgeIterator<FoldedContourTree> it(_folded_tree);
                !it.done(); ++it) 
        {
            double value = computeEdgeReduction(it.edge());            
            (*_reduction_map)[it.edge()] = value;

            if (value > _max_reduction || first_iteration)
                _max_reduction = value;

            if (value < _min_reduction || first_iteration)
                _min_reduction = value;

            first_iteration = false;

            std::cout << "Computed reduction: " << value << std::endl;
        }
    }

public:

    ConcreteLandscapeContext(
            ContourTree* contour_tree) :
            _contour_tree(contour_tree),
            _landscape_builder(boost::shared_ptr<LandscapeBuilder>(
                    new LandscapeBuilder)),
            _folded_tree(*_contour_tree),
            _reduction_map(new ReductionMap(_folded_tree))
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
        typename ContourTree::Node root = _folded_tree.getNode(root_id);
        Landscape* lscape;

        if (_weight_map)
        {
            lscape = _landscape_builder->build(_folded_tree, root, &*_weight_map);
        } else {
            lscape = _landscape_builder->build(_folded_tree, root);
        }

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
        typename Landscape::Arc arc = _landscape->getComponentFromTriangle(tri);

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
        typename Landscape::Arc arc = _landscape->getComponentFromTriangle(tri);

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
        return _max_persistence+1;
    }

    virtual void simplifySubtreeByPersistence(
            size_t parent_id,
            size_t child_id,
            double persistence)
    {
        typename FoldedContourTree::Node parent_node, child_node;
        parent_node = _folded_tree.getNode(parent_id);
        child_node  = _folded_tree.getNode(child_id);

        expandSubtree(_folded_tree, parent_node, child_node);
        denali::PersistenceSimplifier simplifier(persistence); 
        simplifier.simplifySubtree(_folded_tree, parent_node, child_node);

        if (_color_map && _reduction) computeReductions();
    }

    /// \brief Sets the weight map, assuming ownership of the memory.
    virtual void setWeightMap(WeightMap* weight_map)
    {
        _weight_map = boost::shared_ptr<WeightMap>(weight_map);
    }

    /// \brief Get the component ID of the ith triangle.
    virtual size_t getComponentIdentifierFromTriangle(size_t i) const
    {
        // get the i-th triangle
        typename Landscape::Triangle tri = _landscape->getTriangle(i);

        // map it to a landscape arc
        typename Landscape::Arc arc = _landscape->getComponentFromTriangle(tri);

        // return the ID
        return _landscape->getArcIdentifier(arc);
    }

    virtual size_t getMaxComponentIdentifier() const {
        return _landscape->getMaxArcIdentifier();
    }

    virtual void setColorMap(ColorMap* color_map) {
        _color_map = boost::shared_ptr<ColorMap>(color_map);
        if (_color_map && _reduction) computeReductions();
    }

    virtual void setColorReduction(Reduction* reduction) {
        _reduction = boost::shared_ptr<Reduction>(reduction);
        if (_color_map && _reduction) computeReductions();
    }

    virtual double getComponentReductionValue(unsigned int component_id)
    {
        assert(_color_map && _reduction);

        typename Landscape::Arc arc =
                _landscape->getComponentFromIdentifier(component_id);

        typename FoldedContourTree::Edge edge = 
                _landscape->getContourTreeEdge(arc);

        assert(_folded_tree.isEdgeValid(edge));

        return (*_reduction_map)[edge];
    }

    virtual double getMaxReductionValue() {
        assert(_color_map && _reduction);
        return _max_reduction;
    }

    virtual double getMinReductionValue() {
        assert(_color_map && _reduction);
        return _min_reduction;
    }

    virtual void setMaxReductionValue(double value) {
        _max_reduction = value;
    }

    virtual void setMinReductionValue(double value) {
        _min_reduction = value;
    }

    virtual bool hasReduction() const {
        return _color_map && _reduction;
    }


};

#endif
