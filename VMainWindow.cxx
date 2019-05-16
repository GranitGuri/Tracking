// C++ includes
#include <fstream>
#include <iostream>
#include <cstdio>
#include <string.h>

// VTK includes
#include "vtkUnsignedCharArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
//#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkImageActor.h"
#include "vtkImageMapper3D.h"

#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"

#include "vtkImageResliceMapper.h"
#include "vtkImageProperty.h"
#include "vtkImageSlice.h"


#define CreateInstance(object, class) vtkSmartPointer<class> object = vtkSmartPointer<class>::New()

#include "VentTrackS.h"
#include "VMainWindow.h"
#include "SliceInteractorStyle.h"

extern VolumeData vd;

VMainWindow::VMainWindow()
{
    this->setupUi(this);

    /* Prepare data */
    
    importImages();
    xPos = vd.width/2, yPos = vd.height/2, zPos = vd.depth/2;
    tPos = 0;
    
    /* Set up volume rendering */
    
    // Transfer functions
    vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
    opacityTransferFunction->AddPoint(0, 0);
    opacityTransferFunction->AddPoint(255, 0.1);
    vtkPiecewiseFunction *opacityTransferFunction2 = vtkPiecewiseFunction::New();
    opacityTransferFunction2->AddPoint(127, 0.0);
    opacityTransferFunction2->AddPoint(128, 0.0);
    opacityTransferFunction2->AddPoint(255, 1.0);
    vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();
    vtkColorTransferFunction *colorTransferFunction2 = vtkColorTransferFunction::New();
    colorTransferFunction->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(255.0, 1.0, 1.0, 0.0);
    colorTransferFunction2->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
    colorTransferFunction2->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
    colorTransferFunction2->AddRGBPoint(127.0, 0.0, 0.0, 0.0);
//    colorTransferFunction2->AddRGBPoint(191.0, 0.8, 0.8, 0.0);
//    colorTransferFunction2->AddRGBPoint(223.0, 0.8, 0.8, 0.0);
//    colorTransferFunction2->AddRGBPoint(237.0, 0.9, 0.9, 0.0);
    colorTransferFunction2->AddRGBPoint(255.0, 1.0, 1.0, 0.0);
    
    // Volume property
    vtkVolumeProperty *volumeProperty = vtkVolumeProperty::New();
    volumeProperty->SetColor(colorTransferFunction);
    volumeProperty->SetScalarOpacity(opacityTransferFunction);
    //volumeProperty->ShadeOn();
    volumeProperty->SetInterpolationTypeToLinear();

    vtkVolumeProperty *volumeProperty2 = vtkVolumeProperty::New();
    volumeProperty2->SetColor(colorTransferFunction);
    volumeProperty2->SetScalarOpacity(opacityTransferFunction);
    //volumeProperty->ShadeOn();
    volumeProperty2->SetInterpolationTypeToLinear();

    
    // Mapper
   // vtkVolumeRayCastCompositeFunction *compositeFunction = vtkVolumeRayCastCompositeFunction::New();
    //vtkVolumeRayCastCompositeFunction *compositeFunction2 = vtkVolumeRayCastCompositeFunction::New();

    volumeMapper = vtkFixedPointVolumeRayCastMapper::New();
   // volumeMapper->SetVolumeRayCastFunction(compositeFunction);
    volumeMapper->SetInputConnection(image[tPos]->GetOutputPort());
    vtkVolume *volume = vtkVolume::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);

    volumeMapper2 = vtkFixedPointVolumeRayCastMapper::New();
   // volumeMapper2->SetVolumeRayCastFunction(compositeFunction2);
    volumeMapper2->SetInputConnection(image2[tPos]->GetOutputPort());
    vtkVolume *volume2 = vtkVolume::New();
    volume2->SetMapper(volumeMapper2);
    volume2->SetProperty(volumeProperty2);
    
    // Renderer
    vtkRenderer *ren = vtkRenderer::New();
    ren->AddVolume(volume);
    ren->SetBackground(0, 0, 0);

    vtkRenderer *ren2 = vtkRenderer::New();
    ren2->AddVolume(volume2);
    ren2->SetBackground(0, 0, 0);
    
    /* Set up slice planes */
    
    // X Plane
    xSlice = vtkSmartPointer<vtkImageReslice>::New();
    xSlice->SetInputConnection(image[tPos]->GetOutputPort());
    xSlice->SetOutputDimensionality(2);
    xSlice->SetResliceAxesDirectionCosines(0, 1, 0, 0, 0, -1, 1, 0, 0); // (y, -z, x)
    xSlice->SetResliceAxesOrigin(xPos, yPos, zPos);
    xSlice->SetInterpolationModeToLinear();
    
    //xSlice2 = vtkSmartPointer<vtkImageReslice>::New();
    
    CreateInstance(xPlaneActor, vtkImageActor);
    
    // Create a greyscale lookup table
    vtkSmartPointer<vtkLookupTable> table = vtkSmartPointer<vtkLookupTable>::New();
    table->SetRange(0, 255); // image intensity range
    table->SetValueRange(0.0, 1.0); // from black to white
    table->SetSaturationRange(0.0, 0.0); // no color saturation
    table->SetRampToLinear();
    table->Build();
    
    // Map the image through the lookup table
    vtkSmartPointer<vtkImageMapToColors> color = vtkSmartPointer<vtkImageMapToColors>::New();
    color->SetLookupTable(table);
    color->SetInputConnection(xSlice->GetOutputPort());
    
    xPlaneActor->GetMapper()->SetInputConnection(color->GetOutputPort());
    xPlaneRenderer = vtkSmartPointer<vtkRenderer>::New();
    xPlaneRenderer->AddActor(xPlaneActor);
    
    // alternative X Slicing
    //xSlice = vtkSmartPointer<vtkImageResliceMapper>::New();
    //xSlice->SetInputConnection(image[tPos]->GetOutputPort());
    
    // Y Plane
    ySlice = vtkSmartPointer<vtkImageReslice>::New();
    ySlice->SetInputConnection(image[tPos]->GetOutputPort());
    ySlice->SetOutputDimensionality(2);
    ySlice->SetResliceAxesDirectionCosines(1, 0, 0, 0, 0, -1, 0, 1, 0); // (x, -z, y)
    ySlice->SetResliceAxesOrigin(xPos, yPos, zPos);
    ySlice->SetInterpolationModeToLinear();
    CreateInstance(yPlaneActor, vtkImageActor);
    yPlaneActor->GetMapper()->SetInputConnection(ySlice->GetOutputPort());
    yPlaneRenderer = vtkSmartPointer<vtkRenderer>::New();
    yPlaneRenderer->AddActor(yPlaneActor);
    
    // Z Plane
    zSlice = vtkSmartPointer<vtkImageReslice>::New();
    zSlice->SetInputConnection(image[tPos]->GetOutputPort());
    zSlice->SetOutputDimensionality(2);
    zSlice->SetResliceAxesDirectionCosines(0, 1, 0, -1, 0, 0, 0, 0, 1); // (y, -x, z)
    zSlice->SetResliceAxesOrigin(xPos, yPos, zPos);
    zSlice->SetInterpolationModeToLinear();

//    createZImg();
//    zSlice3 = vtkSmartPointer<vtkImageViewer2>::New();
////    zSlice3->SetInputConnection(xImg->GetOutputPort());
//    zSlice3->SetInputConnection(image[tPos]->GetOutputPort());
//    
//    zSlice3->SetSliceOrientationToXY();
//    zSlice3->SetSlice(zPos);
//    zSlice3->SetRenderWindow(qvtkWidgetPlaneZ->GetRenderWindow());
//    zSlice3->Render();

    CreateInstance(zPlaneActor, vtkImageActor);
//    CreateInstance(zPlaneActor, vtkActor2D);
    zPlaneActor->GetMapper()->SetInputConnection(zSlice->GetOutputPort());
    zPlaneRenderer = vtkSmartPointer<vtkRenderer>::New();
    zPlaneRenderer->AddActor(zPlaneActor);
    
    // X Plane 2
    xSlice2 = vtkSmartPointer<vtkImageReslice>::New();
    xSlice2->SetInputConnection(image2[tPos]->GetOutputPort());
    xSlice2->SetOutputDimensionality(2);
    xSlice2->SetResliceAxesDirectionCosines(0, 1, 0, 0, 0, -1, 1, 0, 0); // (y, -z, x)
    xSlice2->SetResliceAxesOrigin(xPos, yPos, zPos);
    xSlice2->SetInterpolationModeToLinear();
    
//    createXImg();
//    xSlice3 = vtkSmartPointer<vtkImageViewer2>::New();
//    xSlice3->SetInputConnection(xImg->GetOutputPort());
//    xSlice3->Render();
    
    
    CreateInstance(xPlaneActor2, vtkImageActor);
    xPlaneActor2->GetMapper()->SetInputConnection(xSlice2->GetOutputPort());
    
    xPlaneRenderer2 = vtkSmartPointer<vtkRenderer>::New();
    xPlaneRenderer2->AddActor(xPlaneActor2);
    
    
    
    // Y Plane 2
    ySlice2 = vtkSmartPointer<vtkImageReslice>::New();
    ySlice2->SetInputConnection(image2[tPos]->GetOutputPort());
    ySlice2->SetOutputDimensionality(2);
    ySlice2->SetResliceAxesDirectionCosines(1, 0, 0, 0, 0, -1, 0, 1, 0); // (x, -z, y)
    ySlice2->SetResliceAxesOrigin(xPos, yPos, zPos);
    ySlice2->SetInterpolationModeToLinear();
    CreateInstance(yPlaneActor2, vtkImageActor);
    yPlaneActor2->GetMapper()->SetInputConnection(ySlice2->GetOutputPort());
    yPlaneRenderer2 = vtkSmartPointer<vtkRenderer>::New();
    yPlaneRenderer2->AddActor(yPlaneActor2);
    
    // Z Plane 2
    zSlice2 = vtkSmartPointer<vtkImageReslice>::New();
    zSlice2->SetInputConnection(image2[tPos]->GetOutputPort());
    zSlice2->SetOutputDimensionality(2);
    zSlice2->SetResliceAxesDirectionCosines(0, 1, 0, -1, 0, 0, 0, 0, 1); // (y, -x, z)
    zSlice2->SetResliceAxesOrigin(xPos, yPos, zPos);
    zSlice2->SetInterpolationModeToLinear();
    CreateInstance(zPlaneActor2, vtkImageActor);
    zPlaneActor2->GetMapper()->SetInputConnection(zSlice2->GetOutputPort());
    zPlaneRenderer2 = vtkSmartPointer<vtkRenderer>::New();
    zPlaneRenderer2->AddActor(zPlaneActor2);

    
    /* Set up Qt window */
    
    //this->setupUi(this);
    
    // Assign widgets
    this->qvtkWidgetVolume->GetRenderWindow()->AddRenderer(ren);
    this->qvtkWidgetPlaneX->GetRenderWindow()->AddRenderer(xPlaneRenderer);
    this->qvtkWidgetPlaneY->GetRenderWindow()->AddRenderer(yPlaneRenderer);
    this->qvtkWidgetPlaneZ->GetRenderWindow()->AddRenderer(zPlaneRenderer);
//    this->qvtkWidgetPlaneZ->GetRenderWindow()->AddRenderer(zSlice3->GetRenderer());
    this->qvtkWidgetVolume2->GetRenderWindow()->AddRenderer(ren2);
    this->qvtkWidgetPlaneX2->GetRenderWindow()->AddRenderer(xPlaneRenderer2);
//    this->qvtkWidgetPlaneX2->GetRenderWindow()->AddRenderer(xSlice3->GetRenderer());
    this->qvtkWidgetPlaneY2->GetRenderWindow()->AddRenderer(yPlaneRenderer2);
    this->qvtkWidgetPlaneZ2->GetRenderWindow()->AddRenderer(zPlaneRenderer2);
    
    // Set up mouse interaction
    CreateInstance(xStyle, SliceInteractorStyle);
    xStyle->SetMainWindow(this);
    xStyle->SetSliceType(SliceType::X);
    qvtkWidgetPlaneX->GetRenderWindow()->GetInteractor()->SetInteractorStyle(xStyle);
    qvtkWidgetPlaneX2->GetRenderWindow()->GetInteractor()->SetInteractorStyle(xStyle);
    CreateInstance(yStyle, SliceInteractorStyle);
    yStyle->SetMainWindow(this);
    yStyle->SetSliceType(SliceType::Y);
    qvtkWidgetPlaneY->GetRenderWindow()->GetInteractor()->SetInteractorStyle(yStyle);
    qvtkWidgetPlaneY2->GetRenderWindow()->GetInteractor()->SetInteractorStyle(yStyle);
    CreateInstance(zStyle, SliceInteractorStyle);
    zStyle->SetMainWindow(this);
    zStyle->SetSliceType(SliceType::Z);
    qvtkWidgetPlaneZ->GetRenderWindow()->GetInteractor()->SetInteractorStyle(zStyle);
    //qvtkWidgetPlaneZ2->GetRenderWindow()->GetInteractor()->SetInteractorStyle(zStyle);
    
    
    // Set up sliders
    xSlider->setRange(0, vd.width-1);
    ySlider->setRange(0, vd.height-1);
    zSlider->setRange(0, vd.depth-1);
    tSlider->setRange(0, vd.numVolumes-1);

    // Set up action signals and slots
    connect(this->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
    connect(xSlider, SIGNAL(valueChanged(int)), this, SLOT(changeX(int)));
    connect(ySlider, SIGNAL(valueChanged(int)), this, SLOT(changeY(int)));
    connect(zSlider, SIGNAL(valueChanged(int)), this, SLOT(changeZ(int)));
    connect(tSlider, SIGNAL(valueChanged(int)), this, SLOT(changeT(int)));
    
    connect(this, SIGNAL(xChanged(int)), xSlider, SLOT(setValue(int)));
    connect(this, SIGNAL(yChanged(int)), ySlider, SLOT(setValue(int)));
    connect(this, SIGNAL(zChanged(int)), zSlider, SLOT(setValue(int)));
    connect(this, SIGNAL(tChanged(int)), tSlider, SLOT(setValue(int)));
    
    emit xChanged(xPos);
    emit yChanged(yPos);
    emit zChanged(zPos);
    emit tChanged(tPos);
}

void VMainWindow::changeX(int x)
{
    cout << "New x position: " << x << "\n";
    xPos = x;
    xSlice->SetResliceAxesOrigin(xPos, vd.height/2, vd.depth/2);
    xSlice2->SetResliceAxesOrigin(xPos, vd.height/2, vd.depth/2);
    qvtkWidgetPlaneX->update();
    qvtkWidgetPlaneX2->update();
    emit xChanged(xPos);
}

void VMainWindow::changeY(int y)
{
    cout << "New y position: " << y << "\n";
    yPos = y;
    ySlice->SetResliceAxesOrigin(vd.width/2, yPos, vd.depth/2);
    ySlice2->SetResliceAxesOrigin(vd.width/2, yPos, vd.depth/2);
    qvtkWidgetPlaneY->update();
    qvtkWidgetPlaneY2->update();
    emit yChanged(yPos);
}

void VMainWindow::changeZ(int z)
{
    cout << "New z position: " << z << "\n";
    zPos = z;
    zSlice->SetResliceAxesOrigin(vd.width/2, vd.height/2, zPos);
    zSlice2->SetResliceAxesOrigin(vd.width/2, vd.height/2, zPos);
    qvtkWidgetPlaneZ->update();
    qvtkWidgetPlaneZ2->update();
    emit zChanged(zPos);
}

void VMainWindow::changeT(int t)
{
    cout << "New t position: " << t << "\n";
    tPos = t;
    xSlice->SetInputConnection(image[tPos]->GetOutputPort());
    xSlice->Update();
    qvtkWidgetPlaneX->update();
    ySlice->SetInputConnection(image[tPos]->GetOutputPort());
    ySlice->Update();
    qvtkWidgetPlaneY->update();
    zSlice->SetInputConnection(image[tPos]->GetOutputPort());
    zSlice->Update();
    qvtkWidgetPlaneZ->update();
    volumeMapper->SetInputConnection(image[tPos]->GetOutputPort());
    volumeMapper->Update();
    qvtkWidgetVolume->update();

    xSlice2->SetInputConnection(image2[tPos]->GetOutputPort());
    xSlice2->Update();
    qvtkWidgetPlaneX2->update();
    ySlice2->SetInputConnection(image2[tPos]->GetOutputPort());
    ySlice2->Update();
    qvtkWidgetPlaneY2->update();
    zSlice2->SetInputConnection(image2[tPos]->GetOutputPort());
    zSlice2->Update();
    qvtkWidgetPlaneZ2->update();
    volumeMapper2->SetInputConnection(image2[tPos]->GetOutputPort());
    volumeMapper2->Update();
    qvtkWidgetVolume2->update();
}

void VMainWindow::slotExit()
{
    qApp->exit();
}

void VMainWindow::importImages()
{
    // Create VTK image objects
    image = new vtkSmartPointer<vtkImageImport>[vd.numVolumes];
    for (int i=0; i<vd.numVolumes; i++) {
        image[i] = vtkSmartPointer<vtkImageImport>::New();
        image[i]->SetDataSpacing(1,1,1);
        image[i]->SetDataOrigin(0, 0, 0);
        image[i]->SetWholeExtent(0, vd.width-1, 0, vd.height-1, 0, vd.depth-1);
        image[i]->SetDataExtentToWholeExtent();
        image[i]->SetDataScalarTypeToUnsignedChar();
        image[i]->SetNumberOfScalarComponents(1);
        image[i]->SetImportVoidPointer(vd.fro[i]);
        //vd.drawGrid();
        image[i]->Update();
    }
    image2 = new vtkSmartPointer<vtkImageImport>[vd.numVolumes];
    for (int i=0; i<vd.numVolumes; i++) {
        image2[i] = vtkSmartPointer<vtkImageImport>::New();
        image2[i]->SetDataSpacing(1,1,1);
        image2[i]->SetDataOrigin(0, 0, 0);
        image2[i]->SetWholeExtent(0, vd.width-1, 0, vd.height-1, 0, vd.depth-1);
        image2[i]->SetDataExtentToWholeExtent();
        image2[i]->SetDataScalarTypeToUnsignedChar();
        image2[i]->SetNumberOfScalarComponents(1);
        image2[i]->SetImportVoidPointer(vd.frame[i]);
        //image2[i]->SetImportVoidPointer(vd.dm[i]);
        //vd.drawGrid();
        image2[i]->Update();
    }
}

void VMainWindow::createXImg() {
    xImg = vtkSmartPointer<vtkImageImport>::New();
    int x, y, z;
    x = vd.width-1;
    y = vd.height-1;
    xImg->SetWholeExtent(0, x  , 0, y, 0, 0);
    xImg->SetDataExtentToWholeExtent();
    xImg->SetDataScalarTypeToUnsignedChar();
    xImg->SetNumberOfScalarComponents(1);
    xImg->SetImportVoidPointer(vd.fro[0]+x*y*zPos);
    xImg->Update();
}

void VMainWindow::createZImg() {
    xImg = vtkSmartPointer<vtkImageImport>::New();
    int x, y, z;
    x = vd.width-1;
    y = vd.height-1;
    xImg->SetWholeExtent(0, x  , 0, y, 0, 0);
    xImg->SetDataExtentToWholeExtent();
    xImg->SetDataScalarTypeToUnsignedChar();
    xImg->SetNumberOfScalarComponents(1);
    xImg->SetImportVoidPointer(vd.fro[0] + zPos * vd.width * vd.height);
    xImg->Update();
}
