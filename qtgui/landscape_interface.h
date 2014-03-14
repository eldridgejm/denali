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
        if (line.size() != 2) {
            std::stringstream msg;
            msg << "More or less than 2 entries on line " << lineno;
            throw std::runtime_error(msg.str());
        }

        char* err_id;
        long int id = strtol(line[0].c_str(), &err_id, 10);

        char* err_color;
        double color = strtod(line[1].c_str(), &err_color);

        if (*err_color != 0 || *err_id != 0) {
            std::stringstream msg;
            msg << "Problem interpreting line " << lineno << " as an edge.";
            throw std::runtime_error(msg.str());
        }
        lineno++;

        _color_map[id] = color;
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
    
    std::vector<double> _component_values;
    std::vector<bool> _component_computed;

    double _max;
    double _min;

public:

    ReductionValueMapper(
        LandscapeContext& context, 
        ColorMap& color_map, 
        Reduction& reduction) :
        _context(context), _color_map(color_map), _reduction(reduction),
        _component_values(context.getMaxComponentIdentifier()),
        _component_computed(context.getMaxComponentIdentifier(), false)
    {
        // precompute the values for every cell in the landscape 
        for (size_t i=0; i<context.numberOfTriangles(); ++i)
        {
            size_t component_id = context.getComponentIdentifierFromTriangle(i);

            if (!_component_computed[component_id])
            {
                std::cout << "Computing for component: " << component_id << std::endl;
                double res = context.reduceComponent(component_id, &_reduction);
                _component_values[component_id] = res;
                _component_computed[component_id] = true;

                if (res > _max || i == 0)
                    _max = res;

                if (res < _min || i == 0)
                    _min = res;
            }
        }
    }

    double operator()(unsigned int i) const {
        // map the triangle to a component
        unsigned int component = _context.getComponentIdentifierFromTriangle(i);

        assert(component <= _component_values.size());
        assert(_component_computed[component]);

        std::cout << "Component " << component << " has value " << _component_values[component] << std::endl;
        return _component_values[component];
    }

    double getMaxValue() const {
        return _max;;
    }

    double getMinValue() const {
        return _min;
    }

};


namespace {

vtkSmartPointer<vtkLookupTable>
makeColorTable(ValueMapper& valueMapper)
{
    vtkSmartPointer<vtkLookupTable> color_table =
            vtkSmartPointer<vtkLookupTable>::New();

    color_table->SetTableRange(valueMapper.getMinValue(), valueMapper.getMaxValue());
    color_table->SetHueRange(2./3, 0);
    color_table->Build();

    return color_table;
}

}

inline void 
pointColorizer(
        vtkSmartPointer<vtkPolyData> trianglePolyData,
        ValueMapper& valueMapper)
{
    vtkSmartPointer<vtkLookupTable> color_table = makeColorTable(valueMapper);

    // Generate the colors for each point based on the color map
    vtkSmartPointer<vtkUnsignedCharArray> point_data = 
            vtkSmartPointer<vtkUnsignedCharArray>::New();

    point_data->SetNumberOfComponents(3);
    point_data->SetName("Colors");

    for(int i = 0; i < trianglePolyData->GetNumberOfPoints(); i++)
    {
        // get the value of the point
        double value = valueMapper(i);

        // lookup the color
        double colors[3];
        color_table->GetColor(value, colors);

        colors[0] *= 255;
        colors[1] *= 255;
        colors[2] *= 255;

        point_data->InsertTuple(i, colors);
    }

    trianglePolyData->GetPointData()->SetScalars(point_data);
}


inline void
cellColorizer(
        vtkSmartPointer<vtkPolyData> trianglePolyData,
        ValueMapper& valueMapper)
{
    vtkSmartPointer<vtkLookupTable> color_table = makeColorTable(valueMapper);

    vtkSmartPointer<vtkUnsignedCharArray> cell_data =
            vtkSmartPointer<vtkUnsignedCharArray>::New();

    size_t n_cells = trianglePolyData->GetNumberOfCells();

    cell_data->SetNumberOfComponents(3);
    cell_data->SetNumberOfTuples(n_cells);

    double colors[3];
    color_table->GetColor(valueMapper.getMinValue(), colors);
    std::cout << "Min color is: " << colors[0] << " " << colors[1] << " " << colors[2] << std::endl;
    color_table->GetColor(valueMapper.getMaxValue(), colors);
    std::cout << "Max color is: " << colors[0] << " " << colors[1] << " " << colors[2] << std::endl;

    for (size_t i=0; i<n_cells; ++i)
    {
        double value = valueMapper(i);

        double colors[3];
        color_table->GetColor(value, colors);

        colors[0] *= 255;
        colors[1] *= 255;
        colors[2] *= 255;

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

        if (_color_map && _reduction) 
        {
            ReductionValueMapper value_mapper(landscape_context, 
                                              *_color_map, 
                                              *_reduction);

            cellColorizer(_landscape_source->GetPolyDataOutput(), value_mapper);
        } else {
            HeightValueMapper value_mapper(landscape_context);
            pointColorizer(_landscape_source->GetPolyDataOutput(), 
                           value_mapper);
        }

        _render_window->Render();

    }

    void setColorMap(ColorMap* color_map) {
        std::cout << "Setting color map..." << std::endl;
        _color_map = boost::shared_ptr<ColorMap>(color_map);
    }

    void setColorReduction(Reduction* reduction) {
        _reduction = boost::shared_ptr<Reduction>(reduction);
    }

};

#endif
