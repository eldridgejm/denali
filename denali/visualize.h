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
// Visualization Interface
//
////////////////////////////////////////////////////////////////////////////////

class LandscapeInteractorStyle : public vtkInteractorStyleTrackballCamera
{
    /* A class for interacting with the landscape in a trackball style. */
    private:
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
        picker->Pick(pos[0], pos[1], 0, this->GetDefaultRenderer());

        double* worldPosition = picker->GetPickPosition();

        std::cout << picker->GetCellId() << std::endl;
        _event_manager->sendCellSelected(picker->GetCellId());
    }

};
vtkStandardNewMacro(LandscapeInteractorStyle);


class VisualizationInterface
{
    vtkSmartPointer<vtkPoints> _mesh_points;
    vtkSmartPointer<vtkCellArray> _mesh_triangles;
    vtkSmartPointer<vtkPolyData> _mesh;
    vtkSmartPointer<LandscapeInteractorStyle> _interactor_style;

    VisualizationEventManager& _event_manager;

    vtkSmartPointer<vtkTextActor> _info_text; 

public:

    VisualizationInterface(VisualizationEventManager& manager)
        : _event_manager(manager), 
          _mesh_points(vtkSmartPointer<vtkPoints>::New()),
          _mesh_triangles(vtkSmartPointer<vtkCellArray>::New()),
          _mesh(vtkSmartPointer<vtkPolyData>::New()),
          _interactor_style(vtkSmartPointer<LandscapeInteractorStyle>::New()),
          _info_text(vtkSmartPointer<vtkTextActor>::New())
    {
        // Add the geometry and topology to the mesh
        _mesh->SetPoints(_mesh_points);
        _mesh->SetPolys(_mesh_triangles);

        // attach the interactor to the event manager
        _interactor_style->SetEventManager(&_event_manager);
    }

    void 
    colorizeMesh()
    {
        double bounds[6];
        _mesh->GetBounds(bounds);

        // Find min and max z
        double minz = bounds[4];
        double maxz = bounds[5];

        // Create the color map
        vtkSmartPointer<vtkLookupTable> colorLookupTable = 
            vtkSmartPointer<vtkLookupTable>::New();
        colorLookupTable->SetTableRange(minz, maxz);
        colorLookupTable->Build();

        // Generate the colors for each point based on the color map
        vtkSmartPointer<vtkUnsignedCharArray> colors = 
            vtkSmartPointer<vtkUnsignedCharArray>::New();
        colors->SetNumberOfComponents(3);
        colors->SetName("Colors");

        for(int i = 0; i < _mesh->GetNumberOfPoints(); i++)
        {
            // put the point's coordinates into p
            double p[3];
            _mesh->GetPoint(i,p);

            // lookup the color
            double dcolor[3];
            colorLookupTable->GetColor(p[2], dcolor);

            unsigned char color[3];
            for(unsigned int j = 0; j < 3; j++)
            {
                color[j] = static_cast<unsigned char>(255.0 * dcolor[j]);
            }

            // Notice this hack! The colors were backwards: blue was high,
            // red was low. This flips the two.
            unsigned char better_colors[3];
            better_colors[0] = color[2];
            better_colors[1] = color[1];
            better_colors[2] = color[0];

            colors->InsertNextTupleValue(better_colors);
        }

        _mesh->GetPointData()->SetScalars(colors);
    }


    template <typename Landscape>
    void buildMesh(Landscape& landscape)
    {
        typedef typename Landscape::Point Point;
        typedef typename Landscape::Triangle Triangle;

        // store some points
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New(); 

        // first, we determine the range of heights we will encounter
        double max_z = landscape.getMaxPoint().z();
        double min_z = landscape.getMinPoint().z();

        // insert the vertices from the landscape into the mesh
        for (size_t i=0; i<landscape.numberOfPoints(); ++i)
        {
            Point point = landscape.getPoint(i);
            double normalized_z = (point.z() - min_z) / (max_z - min_z);
            _mesh_points->InsertNextPoint(point.x(), point.y(), normalized_z);
        }

        // insert the triangles
        for (size_t i=0; i<landscape.numberOfTriangles(); ++i)
        {
            Triangle tri = landscape.getTriangle(i);

            vtkSmartPointer<vtkTriangle> triangle = vtkSmartPointer<vtkTriangle>::New();
            triangle->GetPointIds()->SetId(1, tri.i());
            triangle->GetPointIds()->SetId(2, tri.j());
            triangle->GetPointIds()->SetId(0, tri.k());
            _mesh_triangles->InsertNextCell(triangle);
        }

        colorizeMesh();
    }

    void clearMesh()
    {
          std::cout << "Clearing mesh!" << std::endl;
          _mesh_points = vtkSmartPointer<vtkPoints>::New();
          _mesh_triangles = vtkSmartPointer<vtkCellArray>::New();
          _mesh->SetPoints(_mesh_points);
          _mesh->SetPolys(_mesh_triangles);
    }

    void setInfo(std::string s) {
        _info_text->SetInput (s.c_str());
    }

    void render()
    {
        // Create mapper and actor
        vtkSmartPointer<vtkPolyDataMapper> mapper =
                vtkSmartPointer<vtkPolyDataMapper>::New();

        #if VTK_MAJOR_VERSION <= 5
            mapper->SetInput(_mesh);
        #else
            mapper->SetInputData(_mesh);
        #endif

        vtkSmartPointer<vtkActor> actor =
                vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);

        // Create a renderer, render window, and an interactor
        vtkSmartPointer<vtkRenderer> renderer =
                vtkSmartPointer<vtkRenderer>::New();
        vtkSmartPointer<vtkRenderWindow> renderWindow =
                vtkSmartPointer<vtkRenderWindow>::New();
        renderWindow->AddRenderer(renderer);
        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
                vtkSmartPointer<vtkRenderWindowInteractor>::New();
        renderWindowInteractor->SetRenderWindow(renderWindow);

        // Mouse interactor style
        renderWindowInteractor->SetInteractorStyle(_interactor_style);
        _interactor_style->SetDefaultRenderer(renderer);

        // Add the actors to the scene
        renderer->AddActor(actor);
        renderer->SetBackground(.7,.7,.7); 

        // Setup the text and add it to the window
        _info_text->GetTextProperty()->SetFontSize ( 14 );
        renderer->AddActor2D ( _info_text );
        _info_text->GetTextProperty()->SetColor (0.0,0.0,0.0);

        // Render and interact
        renderWindow->Render();
        renderWindowInteractor->Start();
    }

};

////////////////////////////////////////////////////////////////////////////////
//
// Visualizer
//
////////////////////////////////////////////////////////////////////////////////

template <typename ContourTree, typename Landscape>
class TestObserver : public VisualizationEventObserver
{
    VisualizationInterface& _interface;
    ContourTree& _contour_tree;
    Landscape& _landscape;

public:
    TestObserver(VisualizationInterface& interface, ContourTree& contour_tree, Landscape& landscape)
        : _interface(interface), _contour_tree(contour_tree), _landscape(landscape) {}

    virtual void notifyCellSelected(int id) {
        std::cout << "The cell " << id << " has been selected!." << std::endl;
        typename Landscape::Arc arc = _landscape.getComponent(_landscape.getTriangle(id));
        typename Landscape::Node landscape_parent = _landscape.source(arc);
        typename Landscape::Node landscape_child = _landscape.target(arc);
        typename ContourTree::Node ct_parent = _landscape.getContourTreeNode(landscape_parent);
        typename ContourTree::Node ct_child = _landscape.getContourTreeNode(landscape_child);

        std::stringstream message;
        message << _contour_tree.getID(ct_parent) << " ---> " <<
                _contour_tree.getID(ct_child) << std::endl;

        _interface.setInfo(message.str());
    }
};


template <typename ContourTree, typename Landscape, typename LandscapeBuilder>
class Expander : public VisualizationEventObserver
{
    VisualizationInterface& _interface;
    ContourTree& _contour_tree;
    Landscape& _landscape;
    LandscapeBuilder& _builder;

public:

    Expander(
            VisualizationInterface& interface,
            ContourTree& contour_tree,
            Landscape& landscape,
            LandscapeBuilder& builder)
        : _interface(interface), _contour_tree(contour_tree), _landscape(landscape),
          _builder(builder)
    {

    }

    virtual void notifyCellSelected(int id)
    {
        std::cout << "Expanding component." << std::endl;
        typename Landscape::Arc arc = _landscape.getComponent(_landscape.getTriangle(id));
        typename ContourTree::Edge edge = _landscape.getContourTreeEdge(arc);
        std::cout << _contour_tree.getID(_contour_tree.u(edge)) << " <----> " << 
                  _contour_tree.getID(_contour_tree.v(edge)) << std::endl;
        _contour_tree.unfold(edge);

        // recompute the landscape
        Landscape new_landscape = 
                _builder.build(_contour_tree, _contour_tree.getNode(4));

        // rebuild the mesh
        _interface.clearMesh();
        _interface.buildMesh(new_landscape);

    }
};


template <typename ContourTree>
class Visualizer
{

public:

    template <typename LandscapeBuilder>
    void visualize(ContourTree& contour_tree, LandscapeBuilder& landscape_builder)
    {
        std::cout << "Visualizing..." << std::endl;

        typedef typename LandscapeBuilder::LandscapeType Landscape;

        Landscape landscape = 
                landscape_builder.build(contour_tree, contour_tree.getNode(4));

        VisualizationEventManager event_manager;

        VisualizationInterface interface(event_manager);
        interface.buildMesh(landscape);

        TestObserver<ContourTree, Landscape> test_observer(interface, contour_tree, landscape);
        event_manager.registerObserver(&test_observer);

        Expander<ContourTree, Landscape, LandscapeBuilder> 
                expander(interface, contour_tree, landscape, landscape_builder);
        event_manager.registerObserver(&expander);

        interface.render();
    }

};


}

#endif
