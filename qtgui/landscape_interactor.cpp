#include "landscape_interface.h"

void LandscapeInteractorStyle::OnRightButtonDown()
{
    // Get the location of the click (in window coordinates)
    int* pos = this->GetInteractor()->GetEventPosition();

    vtkSmartPointer<vtkCellPicker> picker =
            vtkSmartPointer<vtkCellPicker>::New();
    picker->SetTolerance(0.0005);

    // Pick from this location.
    picker->Pick(pos[0], pos[1], 0, _renderer);

    int cell = picker->GetCellId();

    // send the event to the interface's event manager
    if (cell >= 0) {
        _event_manager->notifyCellSelection(cell);
    }

    std::cout << cell << std::endl;

}

void LandscapeInteractorStyle::OnLeftButtonDown()
{
    // forward events
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

vtkStandardNewMacro(LandscapeInteractorStyle);

