#ifndef SideBySideRenderWindowsQt_H
#define SideBySideRenderWindowsQt_H

#include "vtkSmartPointer.h"
#include "vtkImageImport.h"
#include "vtkGPUVolumeRayCastMapper.h"
#include "vtkImageReslice.h"
#include "vtkImageViewer2.h"
#include "vtkInteractorStyleTrackballCamera.h"

#include <QMainWindow>
#include "QKeyEvent"

#include "ui_VMainWindow.h"
#include "VentTrackS.h"

class VMainWindow : public QMainWindow, private Ui::VMainWindow
{
    Q_OBJECT
public:
    VMainWindow();
    public slots:
    virtual void slotExit();
    void changeX(int x);
    void changeY(int y);
    void changeZ(int z);
    void changeT(int t);

private:
    vtkSmartPointer<vtkImageImport> *image, *image2, xImg;
    vtkSmartPointer<vtkGPUVolumeRayCastMapper> volumeMapper, volumeMapper2;
    vtkSmartPointer<vtkImageReslice> xSlice, ySlice, zSlice;
    vtkSmartPointer<vtkImageReslice> xSlice2, ySlice2, zSlice2;
    //vtkSmartPointer<vtkImageImport> xSlice2, ySlice2, zSlice2;
    vtkSmartPointer<vtkImageViewer2> xSlice3, zSlice3;
    vtkSmartPointer<vtkRenderer> xPlaneRenderer, yPlaneRenderer, zPlaneRenderer;
    vtkSmartPointer<vtkRenderer> xPlaneRenderer2, yPlaneRenderer2, zPlaneRenderer2;
    int xPos, yPos, zPos, tPos;
	int fxPos, fyPos, fzPos;
    void importImages();
    void createXImg();
    void createYImg();
    void createZImg();

protected:

	void VMainWindow::keyPressEvent(QKeyEvent *event)
	{
		switch (event->key())
		{
		case (Qt::Key_Up):
			cout << "UP";
			break;
		}
		VMainWindow::update();
	}
    
signals:
    void xChanged(int x);
    void yChanged(int y);
    void zChanged(int z);
	void fxChanged(int x);
	void fyChanged(int y);
	void fzChanged(int z);
    void tChanged(int t);
};

#endif
