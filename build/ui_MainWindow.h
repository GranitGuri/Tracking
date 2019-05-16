/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
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

    void setupUi(QMainWindow *VMainWindow)
    {
        if (VMainWindow->objectName().isEmpty())
            VMainWindow->setObjectName(QStringLiteral("VMainWindow"));
        VMainWindow->resize(800, 822);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Icons/help.png"), QSize(), QIcon::Normal, QIcon::Off);
        VMainWindow->setWindowIcon(icon);
        VMainWindow->setIconSize(QSize(22, 22));
        actionOpenFile = new QAction(VMainWindow);
        actionOpenFile->setObjectName(QStringLiteral("actionOpenFile"));
        actionOpenFile->setEnabled(true);
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/Icons/fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpenFile->setIcon(icon1);
        actionExit = new QAction(VMainWindow);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        QIcon icon2;
        icon2.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
        actionExit->setIcon(icon2);
        actionPrint = new QAction(VMainWindow);
        actionPrint->setObjectName(QStringLiteral("actionPrint"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/Icons/print.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPrint->setIcon(icon3);
        actionHelp = new QAction(VMainWindow);
        actionHelp->setObjectName(QStringLiteral("actionHelp"));
        actionHelp->setIcon(icon);
        actionSave = new QAction(VMainWindow);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/Icons/filesave.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon4);
        centralwidget = new QWidget(VMainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        qvtkWidgetPlaneX = new QVTKWidget(centralwidget);
        qvtkWidgetPlaneX->setObjectName(QStringLiteral("qvtkWidgetPlaneX"));
        qvtkWidgetPlaneX->setGeometry(QRect(0, 0, 400, 400));
        qvtkWidgetPlaneY = new QVTKWidget(centralwidget);
        qvtkWidgetPlaneY->setObjectName(QStringLiteral("qvtkWidgetPlaneY"));
        qvtkWidgetPlaneY->setGeometry(QRect(400, 0, 400, 400));
        qvtkWidgetPlaneZ = new QVTKWidget(centralwidget);
        qvtkWidgetPlaneZ->setObjectName(QStringLiteral("qvtkWidgetPlaneZ"));
        qvtkWidgetPlaneZ->setGeometry(QRect(0, 400, 400, 400));
        qvtkWidgetVolume = new QVTKWidget(centralwidget);
        qvtkWidgetVolume->setObjectName(QStringLiteral("qvtkWidgetVolume"));
        qvtkWidgetVolume->setGeometry(QRect(400, 400, 400, 400));
        xSlider = new QSlider(centralwidget);
        xSlider->setObjectName(QStringLiteral("xSlider"));
        xSlider->setGeometry(QRect(0, 800, 200, 22));
        xSlider->setOrientation(Qt::Horizontal);
        ySlider = new QSlider(centralwidget);
        ySlider->setObjectName(QStringLiteral("ySlider"));
        ySlider->setGeometry(QRect(200, 800, 200, 22));
        ySlider->setOrientation(Qt::Horizontal);
        zSlider = new QSlider(centralwidget);
        zSlider->setObjectName(QStringLiteral("zSlider"));
        zSlider->setGeometry(QRect(400, 800, 200, 22));
        zSlider->setOrientation(Qt::Horizontal);
        tSlider = new QSlider(centralwidget);
        tSlider->setObjectName(QStringLiteral("tSlider"));
        tSlider->setGeometry(QRect(600, 800, 200, 22));
        tSlider->setOrientation(Qt::Horizontal);
        VMainWindow->setCentralWidget(centralwidget);

        retranslateUi(VMainWindow);

        QMetaObject::connectSlotsByName(VMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *VMainWindow)
    {
        VMainWindow->setWindowTitle(QApplication::translate("VMainWindow", "VentTrackS", Q_NULLPTR));
        actionOpenFile->setText(QApplication::translate("VMainWindow", "Open File...", Q_NULLPTR));
        actionExit->setText(QApplication::translate("VMainWindow", "Exit", Q_NULLPTR));
        actionPrint->setText(QApplication::translate("VMainWindow", "Print", Q_NULLPTR));
        actionHelp->setText(QApplication::translate("VMainWindow", "Help", Q_NULLPTR));
        actionSave->setText(QApplication::translate("VMainWindow", "Save", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class VMainWindow: public Ui_VMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H