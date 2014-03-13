#ifndef DENALI_QTGUI_LANDSCAPE_INTERFACE_H
#define DENALI_QTGUI_LANDSCAPE_INTERFACE_H

#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCellPicker.h>
#include <vtkCubeSource.h>
#include <vtkDataObjectToTable.h>
#include <vtkDataSetMapper.h>
#include <vtkElevationFilter.h>
#include <vtkInteractorStyleTerrain.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkObjectFactory.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPointData.h>
#include <vtkPointPicker.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProgrammableSource.h>
#include <vtkQtTableView.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTriangle.h>

#include <denali/fileio.h>
#include "landscape_context.h"

#include <map>

typedef std::map<unsigned int, double> ColorMap;

class ColorMapFormatParser
{
    ColorMap& _color_map;
    int lineno;

public:

    ColorMapFormatParser(ColorMap& color_map)
        : _color_map(color_map), lineno(0) { }


    void insert(std::vector<std::string>& line)
    {
        size_t u = lineno;

        if (line.size() != 1) {
            std::stringstream msg;
            msg << "More or less than 1 entry on line " << lineno;
            throw std::runtime_error(msg.str());
        }

        char * err_color;
        double color = strtod(line[0].c_str(), &err_color);

        if (*err_color != 0) {
            std::stringstream msg;
            msg << "Problem interpreting line " << lineno << " as an edge.";
            throw std::runtime_error(msg.str());
        }
        lineno++;

        _color_map[u] = color;
    }
};


inline void readColorMapFromStream(
    std::istream& ctstream,
    ColorMap& color_map)
{
    // clear the color map
    color_map.clear();

    // make a new contour tree format parser
    ColorMapFormatParser format_parser(color_map);

    // and a tabular file parser
    denali::TabularFileParser parser;

    // now parse
    parser.parse(ctstream, format_parser);
}


inline void readColorMapFile(
    const char * filename,
    ColorMap& color_map)
{
    // create a file stream
    std::ifstream fh;
    denali::safeOpenFile(filename, fh);

    readColorMapFromStream(fh, color_map);
}


class ValueMapper
{
public:
    virtual ~ValueMapper() {}
    virtual double operator()(unsigned int) const = 0;
    virtual double getMaxValue() const = 0;
    virtual double getMinValue() const = 0;
};


class HeightValueMapper : public ValueMapper
{
    LandscapeContext& _context;

public:
    HeightValueMapper(LandscapeContext& context) :
        _context(context) {}

    double operator()(unsigned int i) const {
        return _context.getPoint(i).z();
    }

    double getMaxValue() const {
        return _context.getMaxPoint().z();
    }

    double getMinValue() const {
        return _context.getMinPoint().z();
    }
};


class ReductionValueMapper : public ValueMapper
{
    LandscapeContext& _context;
    ColorMap& _color_map;
    Reduction& _reduction;

public:

    ReductionValueMapper(
        LandscapeContext& context, 
        ColorMap& color_map, 
        Reduction& reduction) :
        _context(context), _color_map(color_map), _reduction(reduction) 
    {}

    double operator()(unsigned int i) const {
        return 42;
    }

    double getMaxValue() const {
        return 42;
    }

    double getMinValue() const {
        return 42;
    }

};


inline void 
pointColorizer(
        vtkSmartPointer<vtkPolyData> trianglePolyData,
        ValueMapper& valueMapper)
{
    vtkSmartPointer<vtkLookupTable> color_table =
            vtkSmartPointer<vtkLookupTable>::New();

    color_table->SetTableRange(valueMapper.getMinValue(), valueMapper.getMaxValue());
    color_table->Build();

    // Generate the colors for each point based on the color map
    vtkSmartPointer<vtkUnsignedCharArray> colors = 
        vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetNumberOfComponents(3);
    colors->SetName("Colors");

    for(int i = 0; i < trianglePolyData->GetNumberOfPoints(); i++)
    {
        // get the value of the point
        double value = valueMapper(i);

        // lookup the color
        double dcolor[3];
        color_table->GetColor(value, dcolor);

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

    trianglePolyData->GetPointData()->SetScalars(colors);
}


inline void
cellColorizer(
        vtkSmartPointer<vtkPolyData> trianglePolyData,
        ValueMapper& value_mapper)
{
    vtkSmartPointer<vtkUnsignedCharArray> cell_data =
            vtkSmartPointer<vtkUnsignedCharArray>::New();

    size_t n_cells = trianglePolyData->GetNumberOfCells();

    cell_data->SetNumberOfComponents(3);
    cell_data->SetNumberOfTuples(n_cells);

    for (size_t i=0; i<n_cells; ++i)
    {
        float colors[3];
        colors[0] = 255;
        colors[1] = 10;
        colors[2] = 10;

        cell_data->InsertTuple(i, colors);
    }

    trianglePolyData->GetCellData()->SetScalars(cell_data);
}


template <typename _LandscapeContext>
struct LandscapeMeshBuilderArguments
{
    typedef _LandscapeContext LandscapeContext;

    // the source that will be modified
    vtkProgrammableSource* source;

    // the landscape that will be read from
    LandscapeContext* landscape_context;
};


template <typename Args>
void LandscapeMeshBuilder(void* args)
{
    typedef typename Args::LandscapeContext LandscapeContext;

    std::cout << "Computing mesh..." << std::endl;

    // cast the input
    Args* input = static_cast<Args*>(args);

    // get the landscape context
    LandscapeContext& context = *(input->landscape_context);

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> triangles = vtkSmartPointer<vtkCellArray>::New();

    // get the range of heights for the purpose of normalization
    double max_z = context.getMaxPoint().z();
    double min_z = context.getMinPoint().z();
    double z_range = max_z - min_z;

    for (size_t i=0; i<context.numberOfPoints(); ++i)
    {
        Point point = context.getPoint(i);

        // normalize the height
        double normalized_z = (point.z() - min_z) / z_range;

        points->InsertNextPoint(point.x(), point.y(), normalized_z);
    }

    for (size_t i=0; i<context.numberOfTriangles(); ++i)
    {
        // get the triangle from the landscape
        Triangle tri = context.getTriangle(i);

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


struct LandscapeColorUpdaterArguments
{
    // the source that will be modified
    vtkProgrammableSource* source;
};


template <typename Args>
void LandscapeColorUpdater(void* args)
{
    // cast the input
    Args* input = static_cast<Args*>(args);

    std::cout << "Color updater called..." << std::endl;
    vtkPoints* points = input->source->GetPolyDataOutput()->GetPoints();
    vtkCellArray* triangles = input->source->GetPolyDataOutput()->GetPolys();

    input->source->GetPolyDataOutput()->SetPoints(points);
    input->source->GetPolyDataOutput()->SetPolys(triangles);

    std::cout << input->source->GetPolyDataOutput()->GetNumberOfCells() << std::endl;
};



class LandscapeEventObserver
{
public:
    virtual void receiveCellSelection(unsigned int cell) = 0;
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


class LandscapeInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static LandscapeInteractorStyle* New();

    virtual void OnRightButtonDown();
    virtual void OnLeftButtonDown();

    void SetEventManager(LandscapeEventManager* manager) {
        _event_manager = manager;
    }

    void SetRenderer(vtkRenderer* renderer) {
        _renderer = renderer;
    }

private:
    LandscapeEventManager* _event_manager;
    vtkRenderer* _renderer;
};



class LandscapeInterface
{
    LandscapeEventManager _event_manager;

    vtkSmartPointer<vtkProgrammableSource> _landscape_source;
    vtkSmartPointer<vtkPolyDataMapper> _mapper;
    vtkSmartPointer<vtkActor> _landscape_actor;
    vtkSmartPointer<vtkRenderer> _renderer;
    vtkSmartPointer<LandscapeInteractorStyle> _interactor_style;
    vtkRenderWindow* _render_window;

    boost::shared_ptr<ColorMap> _color_map;
    boost::shared_ptr<Reduction> _reduction;

public:
    LandscapeInterface(vtkRenderWindow* render_window) :
            _landscape_source(vtkSmartPointer<vtkProgrammableSource>::New()),
            _mapper(vtkSmartPointer<vtkPolyDataMapper>::New()),
            _landscape_actor(vtkSmartPointer<vtkActor>::New()),
            _renderer(vtkSmartPointer<vtkRenderer>::New()),
            _interactor_style(vtkSmartPointer<LandscapeInteractorStyle>::New()),
            _render_window(render_window)
    {
        _mapper->SetInputConnection(_landscape_source->GetOutputPort());

        _landscape_actor->SetMapper(_mapper);

        //Add the actor to the scene
        _renderer->AddActor(_landscape_actor);
        _renderer->SetBackground(.4, .5, .6); // Background color white

        _render_window->AddRenderer(_renderer);

        _interactor_style->SetEventManager(&_event_manager);
        _interactor_style->SetRenderer(_renderer);
        render_window->GetInteractor()->SetInteractorStyle(_interactor_style);
    }

    void registerEventObserver(LandscapeEventObserver* observer) {
        _event_manager.registerObserver(observer);
    }

    void renderLandscape(LandscapeContext& landscape_context)
    {
        typedef LandscapeMeshBuilderArguments<LandscapeContext> Args;;
        Args args;
        args.source = _landscape_source;
        args.landscape_context = &landscape_context;

        _landscape_source->SetExecuteMethod(LandscapeMeshBuilder<Args>, &args);

        _landscape_source->Modified();
        _landscape_source->Update();

        colorizeLandscape(landscape_context);


        _render_window->Render();

    }

    void setColorMap(ColorMap* color_map) {
        _color_map = boost::shared_ptr<ColorMap>(color_map);
    }

    void setColorReduction(Reduction* reduction) {
        _reduction = boost::shared_ptr<Reduction>(reduction);
    }

    void colorizeLandscape(LandscapeContext& landscape_context)
    {
        std::cout << "Colorizing..." << std::endl;

        if (_color_map && _reduction) {
            std::cout << "This is working!." << std::endl;
            ReductionValueMapper value_mapper(landscape_context, *_color_map, *_reduction);
            cellColorizer(_landscape_source->GetPolyDataOutput(), value_mapper);
        } else {
            HeightValueMapper value_mapper(landscape_context);
            pointColorizer(_landscape_source->GetPolyDataOutput(), value_mapper);
        }

        typedef LandscapeColorUpdaterArguments Args;
        Args args;
        args.source = _landscape_source;

        _landscape_source->SetExecuteMethod(LandscapeColorUpdater<Args>, &args);

        _landscape_source->Modified();
        _landscape_source->Update();

        _render_window->Render();
    }

    

};

#endif
