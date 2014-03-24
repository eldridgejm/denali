#ifndef DENALI_VISUALIZE_H
#define DENALI_VISUALIZE_H

#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkTriangle.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkAxesActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPointPicker.h>
#include <vtkCallbackCommand.h>
#include <vtkRendererCollection.h>
#include <vtkObjectFactory.h>
#include <vtkInteractorStyleTerrain.h>
#include <vtkCellPicker.h>
#include <vtkDataSetMapper.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkProgrammableSource.h>

#include <sstream>


namespace denali {


class LandscapeEventObserver
{
public:
    virtual void receiveCellSelection(int cell) { }
};


class LandscapeEventManager
{
    typedef std::list<LandscapeEventObserver*> Observers;
    Observers _observers;

public:
    
    void registerObserver(LandscapeEventObserver* observer) {
        _observers.push_back(observer);
    }

    void notifyCellSelection(int cell)
    {
        for (Observers::const_iterator it = _observers.begin(); 
                it != _observers.end(); ++it)
        {
            (*it)->receiveCellSelection(cell);
        }
    }
    
};


template <typename _ContourTree, typename _LandscapeBuilder>
class LandscapeContext
{
public:
    typedef _ContourTree        ContourTree;
    typedef _LandscapeBuilder   LandscapeBuilder;
    typedef typename _LandscapeBuilder::LandscapeType Landscape;

    LandscapeContext(
            ContourTree* contour_tree, 
            typename ContourTree::Node root,
            LandscapeBuilder* landscape_builder) :
            _contour_tree(contour_tree),
            _root(root),
            _landscape_builder(landscape_builder),
            _landscape(new Landscape(_landscape_builder->build(*_contour_tree, _root)))
    { }

    void setContourTree(ContourTree* contour_tree) { 
        _contour_tree = _contour_tree; 
    }

    void updateLandscape() {
        delete _landscape;
        _landscape = new Landscape(_landscape_builder->build(*_contour_tree, _root));
    }

    ContourTree* getContourTree() const { 
        return _contour_tree; 
    }

    Landscape* getLandscape() const {
        return _landscape;
    }

    LandscapeBuilder* getLandscapeBuilder() const {
        return _landscape_builder;
    }

private:
    ContourTree* _contour_tree;
    typename ContourTree::Node _root;
    LandscapeBuilder* _landscape_builder;
    Landscape* _landscape;

};


template <typename _LandscapeContext>
struct LandscapeMeshBuilderArguments
{
    typedef _LandscapeContext LandscapeContext;

    // the source that will be modified
    vtkProgrammableSource* source;

    // the context that will be read from
    LandscapeContext* context;
};


class LandscapeInteractorStyle : public vtkInteractorStyleTrackballCamera
{
    LandscapeEventManager* _manager;

public:
    static LandscapeInteractorStyle* New();

    void SetLandscapeEventManager(LandscapeEventManager* manager) {
        _manager = manager;
    }

    virtual void OnRightButtonDown()
    {
        // forward events
        vtkInteractorStyleTrackballCamera::OnRightButtonDown();

        // Get the location of the click (in window coordinates)
        int* pos = this->GetInteractor()->GetEventPosition();

        vtkSmartPointer<vtkCellPicker> picker =
                vtkSmartPointer<vtkCellPicker>::New();
        picker->SetTolerance(0.0005);

        // Pick from this location.
        picker->Pick(pos[0], pos[1], 0, this->GetCurrentRenderer());

        double* worldPosition = picker->GetPickPosition();

        int cell = picker->GetCellId();

        // send the event to the interface's event manager
        if (cell >= 0) {
            _manager->notifyCellSelection(cell);
        }
    }

    virtual void OnLeftButtonDown()
    {
        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }
};
// vtkStandardNewMacro(LandscapeInteractorStyle);



template <typename Args>
void LandscapeMeshBuilder(void* args)
{
    std::cout << "Building mesh..." << std::endl;

    // make some typedefs to save some typing
    typedef typename Args::LandscapeContext::Landscape Landscape;
    typedef typename Landscape::Point       Point;
    typedef typename Landscape::Triangle    Triangle;
    
    // cast the input
    Args* input = static_cast<Args*>(args);

    // get the landscape
    Landscape& landscape = *(input->context->getLandscape());

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> triangles = vtkSmartPointer<vtkCellArray>::New();

    // get the range of heights for the purpose of normalization
    double max_z = landscape.getMaxPoint().z();
    double min_z = landscape.getMinPoint().z();
    double z_range = max_z - min_z;

    // insert the points from the landscape
    for (size_t i=0; i<landscape.numberOfPoints(); ++i)
    {
        Point point = landscape.getPoint(i);

        // normalize the height
        double normalized_z = (point.z() - min_z) / z_range;

        points->InsertNextPoint(point.x(), point.y(), normalized_z);
    }

    for (size_t i=0; i<landscape.numberOfTriangles(); ++i)
    {
        // get the triangle from the landscape
        Triangle tri = landscape.getTriangle(i);

        // and add it to the cell array
        vtkSmartPointer<vtkTriangle> vtk_tri = vtkSmartPointer<vtkTriangle>::New();
        vtk_tri->GetPointIds()->SetId(1, tri.i());
        vtk_tri->GetPointIds()->SetId(2, tri.j());
        vtk_tri->GetPointIds()->SetId(0, tri.k());
        triangles->InsertNextCell(vtk_tri);
    }

    // set the output
    input->source->GetPolyDataOutput()->SetPoints(points);
    input->source->GetPolyDataOutput()->SetPolys(triangles);

}


class LandscapeInterface
{
    LandscapeEventManager& _event_manager;

    vtkSmartPointer<vtkProgrammableSource> _landscape_source;
    vtkSmartPointer<vtkPolyDataMapper> _mapper;
    vtkSmartPointer<vtkActor> _landscape_actor;
    vtkSmartPointer<vtkRenderer> _renderer;
    vtkSmartPointer<vtkRenderWindow> _render_window;
    vtkSmartPointer<vtkRenderWindowInteractor> _interactor;
    vtkSmartPointer<LandscapeInteractorStyle> _interactor_style;

public:
    LandscapeInterface(
            LandscapeEventManager& event_manager) :
            _event_manager(event_manager),
            _landscape_source(vtkSmartPointer<vtkProgrammableSource>::New()),
            _mapper(vtkSmartPointer<vtkPolyDataMapper>::New()),
            _landscape_actor(vtkSmartPointer<vtkActor>::New()),
            _renderer(vtkSmartPointer<vtkRenderer>::New()),
            _render_window(vtkSmartPointer<vtkRenderWindow>::New()),
            _interactor(vtkSmartPointer<vtkRenderWindowInteractor>::New()),
            _interactor_style(vtkSmartPointer<LandscapeInteractorStyle>::New()) 
    { 
        _mapper->SetInputConnection(_landscape_source->GetOutputPort());

        _landscape_actor->SetMapper(_mapper);

        //Create a renderer, render window, and interactor
        _render_window->AddRenderer(_renderer);
        _interactor->SetRenderWindow(_render_window);

        //Add the actor to the scene
        _renderer->AddActor(_landscape_actor);
        _renderer->SetBackground(.4, .5, .6); // Background color white

        _interactor->SetInteractorStyle(_interactor_style);
        _interactor_style->SetDefaultRenderer(_renderer);
        _interactor_style->SetLandscapeEventManager(&_event_manager);

    }

    template <typename LandscapeContext>
    void buildLandscapeMesh(LandscapeContext& context)
    {
        typedef LandscapeMeshBuilderArguments<LandscapeContext> Args;;
        Args args;
        args.source = _landscape_source;
        args.context = &context;

        _landscape_source->SetExecuteMethod(LandscapeMeshBuilder<Args>, &args);
        _landscape_source->Modified();
        _landscape_source->Update();
    }

    void render()
    {
        _render_window->Render();
    }

    void start()
    {
        render();
        _interactor->Start();
    }

};


template <typename LandscapeContext>
class TestObserver : public LandscapeEventObserver
{
    LandscapeContext& _context;
    LandscapeInterface& _interface;

    typedef typename LandscapeContext::Landscape Landscape;
    typedef typename LandscapeContext::ContourTree ContourTree;

public:
    TestObserver(
            LandscapeContext& context, 
            LandscapeInterface& interface) :
            _context(context),
            _interface(interface)
    {}

    void receiveCellSelection(int cell) 
    {
        std::cout << "Cell selected: " << cell << std::endl;

        // we need the parent and child nodes in the contour tree
        Landscape& landscape = *_context.getLandscape();

        typename Landscape::Triangle tri = landscape.getTriangle(cell);

        typename Landscape::Arc arc = landscape.getComponent(tri);

        typename ContourTree::Node parent = landscape.getContourTreeNode(
                                            landscape.source(arc));

        typename ContourTree::Node child = landscape.getContourTreeNode(
                                           landscape.target(arc));

        // expand the contour tree
        expandSubtree(*_context.getContourTree(), parent, child);

        // update the landscape
        _context.updateLandscape();

        _interface.buildLandscapeMesh(_context);
    }
};


template <typename ContourTree>
class Visualizer
{

public:

    template <typename LandscapeBuilder>
    void visualize(
            ContourTree& contour_tree, 
            typename ContourTree::Node root, 
            LandscapeBuilder& landscape_builder)
    {
        typedef typename LandscapeBuilder::LandscapeType Landscape;
        typedef LandscapeContext<ContourTree, LandscapeBuilder> LandscapeContext;

        // construct a context for visualizing the landscape
        LandscapeContext context(&contour_tree, root, &landscape_builder);

        LandscapeEventManager event_manager;

        LandscapeInterface interface(event_manager);

        TestObserver<LandscapeContext> observer(context, interface);
        event_manager.registerObserver(&observer);

        interface.buildLandscapeMesh(context);
        interface.start();
    }

};


}

#endif
