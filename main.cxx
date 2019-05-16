#include <QApplication>
#include "VentTrackS.h"
#include "VMainWindow.h"
#include "Dynamics.h"
VMainWindow* MWP;
extern VolumeData vd;

int main( int argc, char** argv )
{
    QApplication app( argc, argv );

    vd.readPhilipsDicomFile();
    VMainWindow mainWindow;
	MWP = &mainWindow;
    mainWindow.show();
	doSomething();
	app.exec();
	return 0;
}
