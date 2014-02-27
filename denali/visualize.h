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

////////////////////////////////////////////////////////////////////////////////
//
// Visualization Events
//
////////////////////////////////////////////////////////////////////////////////

class VisualizationEventObserver
{
public:
    virtual void notifyCellSelected(int) { }
};

class VisualizationEventManager
{
    typedef std::list<VisualizationEventObserver*> Observers;
    Observers _observers; 

public:

    void registerObserver(VisualizationEventObserver* observer) {
        _observers.push_back(observer);
    }

    void sendCellSelected(int cell) const
    {
        for (Observers::const_iterator it=_observers.begin(); it != _observers.end(); ++it) {
            (*it)->notifyCellSelected(cell);
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
//
// Interface
//
////////////////////////////////////////////////////////////////////////////////

class LandscapeInteractorStyle : public vtkInteractorStyleTrackballCamera
{
    VisualizationEventManager* _event_manager; 

public:
    static LandscapeInteractorStyle * New();

    void SetEventManager(VisualizationEventManager* manager) {
        _event_manager = manager;
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

        // send the event to the interface's event manager
        _event_manager->sendCellSelected(picker->GetCellId()); 
    }

    virtual void OnLeftButtonDown()
    {
        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }
};
vtkStandardNewMacro(LandscapeInteractorStyle);


struct MeshBuilderArgs
{
    vtkProgrammableSource* source;
};


void buildMesh(void* args)
{
    std::cout << "Computing source..." << std::endl;

    MeshBuilderArgs* input = static_cast<MeshBuilderArgs*>(args);

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> triangles = vtkSmartPointer<vtkCellArray>::New();

    points->InsertNextPoint(0,0,0);
    points->InsertNextPoint(1,0,0);
    points->InsertNextPoint(1,1,0);
    points->InsertNextPoint(0,1,0);

    vtkSmartPointer<vtkTriangle> tri1 = vtkSmartPointer<vtkTriangle>::New();
    tri1->GetPointIds()->SetId(1, 0);
    tri1->GetPointIds()->SetId(2, 1);
    tri1->GetPointIds()->SetId(0, 2);
    triangles->InsertNextCell(tri1);

    vtkSmartPointer<vtkTriangle> tri2 = vtkSmartPointer<vtkTriangle>::New();
    tri2->GetPointIds()->SetId(1, 3);
    tri2->GetPointIds()->SetId(2, 0);
    tri2->GetPointIds()->SetId(0, 2);
    triangles->InsertNextCell(tri2);

    std::cout << "Setting points..." << std::endl;
    input->source->GetPolyDataOutput()->SetPoints(points);

    std::cout << "Setting polys..." << std::endl;
    input->source->GetPolyDataOutput()->SetPolys(triangles);

}


template <typename LandscapeContext>
class LandscapeInterface
{
    LandscapeContext& _context;

    VisualizationEventManager& _event_manager;

    vtkSmartPointer<LandscapeInteractorStyle> _interactor_style;
    vtkSmartPointer<vtkRenderWindow> _render_window;
    vtkSmartPointer<vtkPolyDataMapper> _mapper;
    vtkSmartPointer<vtkActor> _mesh_actor;
    vtkSmartPointer<vtkRenderer> _renderer;
    vtkSmartPointer<vtkRenderWindowInteractor> _interactor;

    vtkProgrammableSource* _source;
    MeshBuilderArgs _args; 

public:

    LandscapeInterface(
            LandscapeContext& context, 
            VisualizationEventManager& event_manager) :
            _context(context),
            _event_manager(event_manager),
            _source(vtkProgrammableSource::New()),
            _interactor_style(vtkSmartPointer<LandscapeInteractorStyle>::New()),
            _render_window(vtkSmartPointer<vtkRenderWindow>::New()),
            _mapper(vtkSmartPointer<vtkPolyDataMapper>::New()),
            _mesh_actor(vtkSmartPointer<vtkActor>::New()),
            _renderer(vtkSmartPointer<vtkRenderer>::New()),
            _interactor(vtkSmartPointer<vtkRenderWindowInteractor>::New())
    {
        _args.source = _source;
        _source->SetExecuteMethod(buildMesh, &_args);

        _interactor_style->SetEventManager(&_event_manager);

        _mapper->SetInputConnection(_source->GetOutputPort());

        _mesh_actor->SetMapper(_mapper);

        //Create a renderer, render window, and interactor
        _render_window->AddRenderer(_renderer);

        _interactor->SetRenderWindow(_render_window);

        _interactor->SetInteractorStyle(_interactor_style);
        _interactor_style->SetDefaultRenderer(_renderer);

        //Add the actor to the scene
        _renderer->AddActor(_mesh_actor);
        _renderer->SetBackground(.4, .5, .6); // Background color white
    }

    void buildLandscapeMesh()
    {
        _source->Modified();
        _source->Update();
        /*
        typedef typename LandscapeContext::Landscape Landscape;
        typedef typename Landscape::Point Point;
        typedef typename Landscape::Triangle Triangle;

        Landscape& landscape = _context.getLandscape();

        // make point and triangle data
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkCellArray> triangles = vtkSmartPointer<vtkCellArray>::New();

        // record the max and min heights for normalization purposes
        double max_z = landscape.getMaxPoint().z();
        double min_z = landscape.getMinPoint().z();
        double range = max_z - min_z;

        // build the point set
        for (size_t i=0; i<landscape.numberOfPoints(); ++i)
        {
            // get the point from the landscape
            Point point = landscape.getPoint(i);

            // normalize the height of the point
            double normalized_z = (point.z() - min_z) / range;

            // and insert it into the polydata
            points->InsertNextPoint(point.x(), point.y(), normalized_z);
        }

        // now build the topology
        for (size_t i=0; i<landscape.numberOfTriangles(); ++i)
        {
            // get the triangle from the landscape
            Triangle tri = landscape.getTriangle(i);

            // make a new vtk triangle
            vtkSmartPointer<vtkTriangle> vtk_tri = vtkSmartPointer<vtkTriangle>::New();
            vtk_tri->GetPointIds()->SetId(1, tri.i());
            vtk_tri->GetPointIds()->SetId(2, tri.j());
            vtk_tri->GetPointIds()->SetId(0, tri.k());
            triangles->InsertNextCell(vtk_tri);
        }

        // now update the mesh
        _landscape_mesh->SetPoints(points);
        _landscape_mesh->SetPolys(triangles);

        _landscape_mesh->Modified();
        _mapper->SetInput(_landscape_mesh);
        _landscape_mesh->Modified();
        _mapper->Update();
        _mapper->Modified();

        _mesh_actor->Modified();
        _interactor_style->Modified();
        _interactor->Modified();
        */
        
    }
    
    void render()
    {
        _render_window->Render();
    }

    void start()
    {
        //Render and interact
        render();
        _interactor->Start();
    }

};

////////////////////////////////////////////////////////////////////////////////
//
// LandscapeContext
//
////////////////////////////////////////////////////////////////////////////////

template <typename _ContourTree, typename _LandscapeBuilder>
class LandscapeContext
{
public:
    typedef _ContourTree        ContourTree;
    typedef _LandscapeBuilder   LandscapeBuilder;
    typedef typename _LandscapeBuilder::LandscapeType   Landscape;

    LandscapeContext(
            ContourTree& contour_tree, 
            typename ContourTree::Node root,
            LandscapeBuilder& landscape_builder) :
            _contour_tree(contour_tree),
            _root(root),
            _landscape_builder(landscape_builder),
            _landscape(new Landscape(landscape_builder.build(contour_tree, root)))
    {}

    ~LandscapeContext()
    {
        delete _landscape;
    }

    ContourTree& getContourTree() { return _contour_tree; }
    Landscape& getLandscape() { return *_landscape; }

    void updateLandscape()
    {
        delete _landscape;
        _landscape = new Landscape(_landscape_builder.build(_contour_tree, _root));
    }

private:

    ContourTree& _contour_tree;
    typename ContourTree::Node _root;
    LandscapeBuilder& _landscape_builder;
    Landscape* _landscape;
};

////////////////////////////////////////////////////////////////////////////////
//
// Visualizer
//
////////////////////////////////////////////////////////////////////////////////

template <typename LandscapeContext>
class TestObserver : public VisualizationEventObserver
{

    LandscapeContext& _landscape_context;
    LandscapeInterface<LandscapeContext>& _interface;

public:

    TestObserver(
            LandscapeContext& context,
            LandscapeInterface<LandscapeContext>& interface) :
            _landscape_context(context),
            _interface(interface)
    {}

    void notifyCellSelected(int id)
    {
        std::cout << "Cell " << id << " has been selected." << std::endl;

        // get the component from the landscape
        typedef typename LandscapeContext::Landscape Landscape;
        Landscape& landscape = _landscape_context.getLandscape();
        typename Landscape::Triangle triangle = landscape.getTriangle(id);
        typename Landscape::Arc arc = landscape.getComponent(triangle);

        // get the parent and child nodes from the contour tree
        typedef typename LandscapeContext::ContourTree ContourTree;
        ContourTree& contour_tree = _landscape_context.getContourTree();
        typename ContourTree::Node parent = landscape.getContourTreeNode(landscape.source(arc));
        typename ContourTree::Node child = landscape.getContourTreeNode(landscape.target(arc));

        std::cout << contour_tree.getID(parent) << " ----> " << contour_tree.getID(child) << std::endl;

        expandSubtree(contour_tree, parent, child);
        _landscape_context.updateLandscape();
        _interface.buildLandscapeMesh();
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

        // make a visualization context
        typedef LandscapeContext<ContourTree, LandscapeBuilder> Context;
        Context context(contour_tree, root, landscape_builder);

        // make a event manager
        VisualizationEventManager event_manager;

        // make an interface
        LandscapeInterface<Context> interface(context, event_manager);
        
        TestObserver<Context> observer(context, interface);
        event_manager.registerObserver(&observer);

        // start the visualization
        interface.buildLandscapeMesh();
        interface.start();
    }

};


}

#endif
