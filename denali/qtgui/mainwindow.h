#ifndef DENALI_QTGUI_MAINWINDOW_H
#define DENALI_QTGUI_MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include "denali/qtgui/ui_MainWindow.h"

#include <vtkDataObjectToTable.h>
#include <vtkElevationFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkQtTableView.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSphereSource.h>
#include <vtkCubeSource.h>
#include <vtkSmartPointer.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    
    MainWindow()
    {
        _mainwindow.setupUi(this);

        // Sphere
        vtkSmartPointer<vtkSphereSource> sphereSource = 
            vtkSmartPointer<vtkSphereSource>::New();
        sphereSource->Update();
        vtkSmartPointer<vtkPolyDataMapper> sphereMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
        sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
        vtkSmartPointer<vtkActor> sphereActor = 
            vtkSmartPointer<vtkActor>::New();
        sphereActor->SetMapper(sphereMapper);

        // VTK Renderer
        vtkSmartPointer<vtkRenderer> renderer = 
            vtkSmartPointer<vtkRenderer>::New();
        renderer->AddActor(sphereActor);
        renderer->SetBackground(.4,.5,.6);

        // VTK/Qt wedded
        _mainwindow.qvtkWidget->GetRenderWindow()->AddRenderer(renderer);

    }

    ~MainWindow() {}

private:

    Ui::MainWindow _mainwindow;
};

#endif
