//
//  SliceInteractorStyle.cpp
//  VentTrackS
//
//  Created by Tobias Lang on 09.04.17.
//
//

#include "vtkRenderwindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkCoordinate.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"

#include "VentTrackS.h"
#include "SliceInteractorStyle.h"

extern VolumeData vd;

void SliceInteractorStyle::SetMainWindow(VMainWindow* _mainWindow)
{
    mainWindow = _mainWindow;
}

void SliceInteractorStyle::SetSliceType(SliceType _sliceType)
{
    sliceType = _sliceType;
}

void SliceInteractorStyle::OnLeftButtonDown()
{
    std::cout << "Pressed left mouse button." << std::endl;
    int x = this->Interactor->GetEventPosition()[0];
    int y = this->Interactor->GetEventPosition()[1];
    std::cout << "(x,y) = (" << x << "," << y << ")" << std::endl;
    vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
    coordinate->SetCoordinateSystemToDisplay();
    coordinate->SetValue(x,y,0);
    
    // This doesn't produce the right value if the sphere is zoomed in???
    double* world = coordinate->GetComputedWorldValue(this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
    std::cout << "World coordinate: " << world[0] << ", " << world[1] << ", " << world[2] << std::endl;
    
    switch(sliceType) {
        case SliceType::X:
            //mainWindow->changeY((world[0] + vd.height/2));
            mainWindow->changeY(calcY(world[0])*10/9);
            //mainWindow->changeZ(-world[1]*1.13 + vd.width/2);
            mainWindow->changeZ(calcZ(world[1])/1.06);
            break;
        case SliceType::Y:
            //mainWindow->changeX(world[0] + vd.width/2);
            mainWindow->changeX(calcX(world[0])*10/9);
            mainWindow->changeZ(calcZ(world[1])/1.06);
            break;
        case SliceType::Z:
            mainWindow->changeX((world[0] + vd.height/2));
            mainWindow->changeY((world[1] + vd.width/2)*10/9);
    }
    // Forward events
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

void SliceInteractorStyle::OnKeyPress()
{
	// Get the keypress
	vtkRenderWindowInteractor *rwi = this->Interactor;
	std::string key = rwi->GetKeySym();

	// Output the key that was pressed
	std::cout << "Pressed " << key << std::endl;

	// Handle an arrow key
	if (key == "Up")
	{
		std::cout << "The up arrow was pressed." << std::endl;
	}

	// Handle a "normal" key
	if (key == "a")
	{
		std::cout << "The a key was pressed." << std::endl;
	}

	// Forward events
	vtkInteractorStyleTrackballCamera::OnKeyPress();
}

int SliceInteractorStyle::calcX(double coord) {
    double xLeft = 98;
    double xRight = -100;
    return vd.width / (xRight - xLeft) * (coord - xLeft);
}

int SliceInteractorStyle::calcY(double coord) {
    double yBack = 105;
    double yFront = -102;
    return vd.depth / (yBack - yFront) * (coord - yFront);
}

int SliceInteractorStyle::calcZ(double coord) {
    double zTop = 98;
    double zBottom = -100;
    return vd.height / (zBottom - zTop) * (coord - zTop);
}

vtkStandardNewMacro(SliceInteractorStyle);
