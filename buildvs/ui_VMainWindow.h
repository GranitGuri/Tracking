/********************************************************************************
** Form generated from reading UI file 'VMainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VMAINWINDOW_H
#define UI_VMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>
#include "QVTKWidget.h"

QT_BEGIN_NAMESPACE

class Ui_VMainWindow
{
public:
    QAction *actionOpenFile;
    QAction *actionExit;
    QAction *actionPrint;
    QAction *actionHelp;
    QAction *actionSave;
    QWidget *centralwidget;
    QVTKWidget *qvtkWidgetPlaneX;
    QVTKWidget *qvtkWidgetPlaneY;
    QVTKWidget *qvtkWidgetPlaneZ;
    QVTKWidget *qvtkWidgetVolume;
    QSlider *xSlider;
    QSlider *ySlider;
    QSlider *zSlider;
    QSlider *tSlider;
    QVTKWidget *qvtkWidgetPlaneX2;
    QVTKWidget *qvtkWidgetPlaneY2;
    QVTKWidget *qvtkWidgetPlaneZ2;
    QVTKWidget *qvtkWidgetVolume2;

    void setupUi(QMainWindow *VMainWindow)
    {
        if (VMainWindow->objectName().isEmpty())
            VMainWindow->setObjectName(QString::fromUtf8("VMainWindow"));
        VMainWindow->resize(1600, 822);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Icons/help.png"), QSize(), QIcon::Normal, QIcon::Off);
        VMainWindow->setWindowIcon(icon);
        VMainWindow->setIconSize(QSize(22, 22));
        actionOpenFile = new QAction(VMainWindow);
        actionOpenFile->setObjectName(QString::fromUtf8("actionOpenFile"));
        actionOpenFile->setEnabled(true);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Icons/fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpenFile->setIcon(icon1);
        actionExit = new QAction(VMainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8("."), QSize(), QIcon::Normal, QIcon::Off);
        actionExit->setIcon(icon2);
        actionPrint = new QAction(VMainWindow);
        actionPrint->setObjectName(QString::fromUtf8("actionPrint"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Icons/print.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPrint->setIcon(icon3);
        actionHelp = new QAction(VMainWindow);
        actionHelp->setObjectName(QString::fromUtf8("actionHelp"));
        actionHelp->setIcon(icon);
        actionSave = new QAction(VMainWindow);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/Icons/filesave.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon4);
        centralwidget = new QWidget(VMainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        qvtkWidgetPlaneX = new QVTKWidget(centralwidget);
        qvtkWidgetPlaneX->setObjectName(QString::fromUtf8("qvtkWidgetPlaneX"));
        qvtkWidgetPlaneX->setGeometry(QRect(0, 0, 400, 400));
        qvtkWidgetPlaneY = new QVTKWidget(centralwidget);
        qvtkWidgetPlaneY->setObjectName(QString::fromUtf8("qvtkWidgetPlaneY"));
        qvtkWidgetPlaneY->setGeometry(QRect(400, 0, 400, 400));
        qvtkWidgetPlaneZ = new QVTKWidget(centralwidget);
        qvtkWidgetPlaneZ->setObjectName(QString::fromUtf8("qvtkWidgetPlaneZ"));
        qvtkWidgetPlaneZ->setGeometry(QRect(800, 0, 400, 400));
        qvtkWidgetVolume = new QVTKWidget(centralwidget);
        qvtkWidgetVolume->setObjectName(QString::fromUtf8("qvtkWidgetVolume"));
        qvtkWidgetVolume->setGeometry(QRect(1200, 0, 400, 400));
        xSlider = new QSlider(centralwidget);
        xSlider->setObjectName(QString::fromUtf8("xSlider"));
        xSlider->setGeometry(QRect(0, 800, 400, 22));
        xSlider->setOrientation(Qt::Horizontal);
        ySlider = new QSlider(centralwidget);
        ySlider->setObjectName(QString::fromUtf8("ySlider"));
        ySlider->setGeometry(QRect(400, 800, 400, 22));
        ySlider->setOrientation(Qt::Horizontal);
        zSlider = new QSlider(centralwidget);
        zSlider->setObjectName(QString::fromUtf8("zSlider"));
        zSlider->setGeometry(QRect(800, 800, 400, 22));
        zSlider->setOrientation(Qt::Horizontal);
        tSlider = new QSlider(centralwidget);
        tSlider->setObjectName(QString::fromUtf8("tSlider"));
        tSlider->setGeometry(QRect(1200, 800, 400, 22));
        tSlider->setOrientation(Qt::Horizontal);
        qvtkWidgetPlaneX2 = new QVTKWidget(centralwidget);
        qvtkWidgetPlaneX2->setObjectName(QString::fromUtf8("qvtkWidgetPlaneX2"));
        qvtkWidgetPlaneX2->setGeometry(QRect(0, 400, 400, 400));
        qvtkWidgetPlaneY2 = new QVTKWidget(centralwidget);
        qvtkWidgetPlaneY2->setObjectName(QString::fromUtf8("qvtkWidgetPlaneY2"));
        qvtkWidgetPlaneY2->setGeometry(QRect(400, 400, 400, 400));
        qvtkWidgetPlaneZ2 = new QVTKWidget(centralwidget);
        qvtkWidgetPlaneZ2->setObjectName(QString::fromUtf8("qvtkWidgetPlaneZ2"));
        qvtkWidgetPlaneZ2->setGeometry(QRect(800, 400, 400, 400));
        qvtkWidgetVolume2 = new QVTKWidget(centralwidget);
        qvtkWidgetVolume2->setObjectName(QString::fromUtf8("qvtkWidgetVolume2"));
        qvtkWidgetVolume2->setGeometry(QRect(1200, 400, 400, 400));
        VMainWindow->setCentralWidget(centralwidget);

        retranslateUi(VMainWindow);

        QMetaObject::connectSlotsByName(VMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *VMainWindow)
    {
        VMainWindow->setWindowTitle(QApplication::translate("VMainWindow", "VentTrackS", nullptr));
        actionOpenFile->setText(QApplication::translate("VMainWindow", "Open File...", nullptr));
        actionExit->setText(QApplication::translate("VMainWindow", "Exit", nullptr));
        actionPrint->setText(QApplication::translate("VMainWindow", "Print", nullptr));
        actionHelp->setText(QApplication::translate("VMainWindow", "Help", nullptr));
        actionSave->setText(QApplication::translate("VMainWindow", "Save", nullptr));
    } // retranslateUi

};

namespace Ui {
    class VMainWindow: public Ui_VMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VMAINWINDOW_H
