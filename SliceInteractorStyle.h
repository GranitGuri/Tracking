//
//  SliceInteractorStyle.h
//  VentTrackS
//
//  Created by Tobias Lang on 09.04.17.
//
//

#ifndef SliceInteractorStyle_h
#define SliceInteractorStyle_h

#include <stdio.h>

#include "vtkInteractorStyleTrackballCamera.h"
#include <vtkObjectFactory.h>

#include "VentTrackS.h"
#include "VMainWindow.h"


enum SliceType {X, Y, Z};

class SliceInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static SliceInteractorStyle* New();
    void SetMainWindow(VMainWindow* mainWindow);
    void SetSliceType(SliceType sliceType);
    virtual void OnLeftButtonDown();
private:
    VolumeData v;
    SliceType sliceType;
    VMainWindow *mainWindow;
    int calcX(double coord);
    int calcY(double coord);
    int calcZ(double coord);
};





#endif /* SliceInteractorStyle_h */
