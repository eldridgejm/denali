#ifndef DENALI_VISUALIZE_H
#define DENALI_VISUALIZE_H

#include "visualize.h"

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

namespace denali {

    namespace visualize {

        template <typename Landscape> 
        vtkSmartPointer<vtkPolyData> buildMesh(const Landscape&, double);

    }

    class Visualizer;

}


template <typename Landscape>
vtkSmartPointer<vtkPolyData>
denali::visualize::buildMesh(
        const Landscape& landscape,
        double squash_factor)
{
    // store some points
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New(); 

    // insert the vertices from the landscape into the vtk container
    double max_z = landscape.getMaxPoint().z();
    double min_z = landscape.getMinPoint().z();

    for (size_t i=0; i<landscape.numberOfPoints(); ++i) {
        // retrieve the point from the landscape
        typename Landscape::Point point = landscape.getPoint(i);

        // normalize the z coordinate
        double normed_z = (point.z() - min_z) / (max_z - min_z);

        // insert into the point array
        points->InsertNextPoint(point.x(), point.y(), normed_z * squash_factor);
    }

    // now we insert the triangles
    vtkSmartPointer<vtkCellArray> triangles = vtkSmartPointer<vtkCellArray>::New();

    for (size_t i=0; i<landscape.numberOfTriangles(); ++i) {
        // retrieve the triangle
        typename Landscape::Triangle triangle = landscape.getTriangle(i);

        vtkSmartPointer<vtkTriangle> triangle_cell = vtkSmartPointer<vtkTriangle>::New();
        triangle_cell->GetPointIds()->SetId(1, triangle.i());
        triangle_cell->GetPointIds()->SetId(2, triangle.j());
        triangle_cell->GetPointIds()->SetId(0, triangle.k());
        triangles->InsertNextCell(triangle_cell);
    }

    // Create a polydata object
    vtkSmartPointer<vtkPolyData> trianglePolyData =
        vtkSmartPointer<vtkPolyData>::New();

    // Add the geometry and topology to the polydata
    trianglePolyData->SetPoints(points);
    trianglePolyData->SetPolys(triangles);

    return trianglePolyData;
}


class denali::Visualizer
{

public:
    
    template <typename Landscape>
    void visualize(
            const Landscape& landscape)
    {

        vtkSmartPointer<vtkPolyData> trianglePolyData = 
                visualize::buildMesh(landscape, 1.);

        // visualize::colorizeLandscape(trianglePolyData);
      

        // Create mapper and actor
        vtkSmartPointer<vtkPolyDataMapper> mapper =
                vtkSmartPointer<vtkPolyDataMapper>::New();

        #if VTK_MAJOR_VERSION <= 5
            mapper->SetInput(trianglePolyData);
        #else
            mapper->SetInputData(trianglePolyData);
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
        /*
        vtkSmartPointer<visualize::LandscapeInteractorStyle> style =
                vtkSmartPointer<visualize::LandscapeInteractorStyle>::New();
        renderWindowInteractor->SetInteractorStyle( style );
        style->SetDefaultRenderer(renderer);
        style->SetLandscape(&landscape);
        */

        // Add the actors to the scene
        renderer->AddActor(actor);
        // renderer->SetBackground(bg_intensity_, bg_intensity_, bg_intensity_); 


        // Render and interact
        renderWindow->Render();
        renderWindowInteractor->Start();

    }
};



#endif
