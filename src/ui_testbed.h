/********************************************************************************
** Form generated from reading UI file 'testbed.ui'
**
** Created: Wed Nov 7 15:37:24 2012
**      by: Qt User Interface Compiler version 4.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TESTBED_H
#define UI_TESTBED_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDockWidget>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>
#include "QVTKWidget.h"

QT_BEGIN_NAMESPACE

class Ui_TestbedWindow
{
public:
    QAction *actionLoadVolume;
    QAction *actionLoadGeometry;
    QAction *actionVolumeRender;
    QAction *actionTF_customize;
    QAction *actionGeometryRender;
    QAction *actionCompositeRender;
    QAction *actionExit;
    QAction *actionLoadData;
    QAction *actionMultipleVolumesRender;
    QAction *actionSettings;
    QAction *actionClose_current;
    QWidget *centralwidget;
    QVTKWidget *renderView;
    QMenuBar *menubar;
    QMenu *menuData;
    QMenu *menuControl;
    QStatusBar *statusbar;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QGroupBox *groupBoxVolRender;
    QLabel *label_presets;
    QLabel *label_methods;
    QComboBox *comboBoxVolRenderPresets;
    QComboBox *comboBoxVolRenderMethods;
    QGroupBox *groupBoxGeoRender;
    QCheckBox *checkBoxTubeHalos;
    QLabel *label_tubesize;
    QDoubleSpinBox *doubleSpinBoxTubeSize;
    QCheckBox *checkBoxDepthSize;
    QLabel *label_tubesizescale;
    QCheckBox *checkBoxDepthTrans;
    QDoubleSpinBox *doubleSpinBoxAlphaStart;
    QDoubleSpinBox *doubleSpinBoxAlphaEnd;
    QCheckBox *checkBoxDepthColor;
    QDoubleSpinBox *doubleSpinBoxHueStart;
    QDoubleSpinBox *doubleSpinBoxHueEnd;
    QLabel *label_hue;
    QDoubleSpinBox *doubleSpinBoxSatuStart;
    QDoubleSpinBox *doubleSpinBoxSatuEnd;
    QLabel *label_saturation;
    QLabel *label_value;
    QDoubleSpinBox *doubleSpinBoxValueEnd;
    QDoubleSpinBox *doubleSpinBoxValueStart;
    QDoubleSpinBox *doubleSpinBoxTubeSizeScale;
    QSlider *sliderHaloWidth;
    QLabel *label_halowidth;
    QCheckBox *checkBoxHatching;
    QCheckBox *checkBoxDepthColorLAB;
    QCheckBox *checkBoxDepthValue;
    QDoubleSpinBox *doubleSpinBoxDValueEnd;
    QDoubleSpinBox *doubleSpinBoxDValueStart;
    QPushButton *pushButtonApply;

    void setupUi(QMainWindow *TestbedWindow)
    {
        if (TestbedWindow->objectName().isEmpty())
            TestbedWindow->setObjectName(QString::fromUtf8("TestbedWindow"));
        TestbedWindow->resize(1089, 791);
        actionLoadVolume = new QAction(TestbedWindow);
        actionLoadVolume->setObjectName(QString::fromUtf8("actionLoadVolume"));
        actionLoadVolume->setCheckable(false);
        actionLoadGeometry = new QAction(TestbedWindow);
        actionLoadGeometry->setObjectName(QString::fromUtf8("actionLoadGeometry"));
        actionVolumeRender = new QAction(TestbedWindow);
        actionVolumeRender->setObjectName(QString::fromUtf8("actionVolumeRender"));
        actionTF_customize = new QAction(TestbedWindow);
        actionTF_customize->setObjectName(QString::fromUtf8("actionTF_customize"));
        actionGeometryRender = new QAction(TestbedWindow);
        actionGeometryRender->setObjectName(QString::fromUtf8("actionGeometryRender"));
        actionCompositeRender = new QAction(TestbedWindow);
        actionCompositeRender->setObjectName(QString::fromUtf8("actionCompositeRender"));
        actionExit = new QAction(TestbedWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionLoadData = new QAction(TestbedWindow);
        actionLoadData->setObjectName(QString::fromUtf8("actionLoadData"));
        actionMultipleVolumesRender = new QAction(TestbedWindow);
        actionMultipleVolumesRender->setObjectName(QString::fromUtf8("actionMultipleVolumesRender"));
        actionSettings = new QAction(TestbedWindow);
        actionSettings->setObjectName(QString::fromUtf8("actionSettings"));
        actionClose_current = new QAction(TestbedWindow);
        actionClose_current->setObjectName(QString::fromUtf8("actionClose_current"));
        centralwidget = new QWidget(TestbedWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        renderView = new QVTKWidget(centralwidget);
        renderView->setObjectName(QString::fromUtf8("renderView"));
        renderView->setGeometry(QRect(0, 0, 891, 761));
        TestbedWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(TestbedWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1089, 27));
        menuData = new QMenu(menubar);
        menuData->setObjectName(QString::fromUtf8("menuData"));
        menuControl = new QMenu(menubar);
        menuControl->setObjectName(QString::fromUtf8("menuControl"));
        TestbedWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(TestbedWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        TestbedWindow->setStatusBar(statusbar);
        dockWidget = new QDockWidget(TestbedWindow);
        dockWidget->setObjectName(QString::fromUtf8("dockWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(dockWidget->sizePolicy().hasHeightForWidth());
        dockWidget->setSizePolicy(sizePolicy);
        dockWidget->setMinimumSize(QSize(186, 541));
        dockWidget->setAcceptDrops(false);
        dockWidget->setAutoFillBackground(true);
        dockWidget->setFloating(false);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        groupBoxVolRender = new QGroupBox(dockWidgetContents);
        groupBoxVolRender->setObjectName(QString::fromUtf8("groupBoxVolRender"));
        groupBoxVolRender->setGeometry(QRect(10, 0, 171, 121));
        label_presets = new QLabel(groupBoxVolRender);
        label_presets->setObjectName(QString::fromUtf8("label_presets"));
        label_presets->setGeometry(QRect(10, 20, 67, 17));
        label_methods = new QLabel(groupBoxVolRender);
        label_methods->setObjectName(QString::fromUtf8("label_methods"));
        label_methods->setGeometry(QRect(10, 70, 67, 17));
        comboBoxVolRenderPresets = new QComboBox(groupBoxVolRender);
        comboBoxVolRenderPresets->setObjectName(QString::fromUtf8("comboBoxVolRenderPresets"));
        comboBoxVolRenderPresets->setGeometry(QRect(10, 40, 151, 23));
        comboBoxVolRenderMethods = new QComboBox(groupBoxVolRender);
        comboBoxVolRenderMethods->setObjectName(QString::fromUtf8("comboBoxVolRenderMethods"));
        comboBoxVolRenderMethods->setGeometry(QRect(10, 90, 151, 23));
        groupBoxGeoRender = new QGroupBox(dockWidgetContents);
        groupBoxGeoRender->setObjectName(QString::fromUtf8("groupBoxGeoRender"));
        groupBoxGeoRender->setGeometry(QRect(10, 120, 171, 601));
        checkBoxTubeHalos = new QCheckBox(groupBoxGeoRender);
        checkBoxTubeHalos->setObjectName(QString::fromUtf8("checkBoxTubeHalos"));
        checkBoxTubeHalos->setGeometry(QRect(10, 20, 93, 22));
        label_tubesize = new QLabel(groupBoxGeoRender);
        label_tubesize->setObjectName(QString::fromUtf8("label_tubesize"));
        label_tubesize->setGeometry(QRect(30, 100, 31, 21));
        doubleSpinBoxTubeSize = new QDoubleSpinBox(groupBoxGeoRender);
        doubleSpinBoxTubeSize->setObjectName(QString::fromUtf8("doubleSpinBoxTubeSize"));
        doubleSpinBoxTubeSize->setGeometry(QRect(80, 100, 61, 23));
        doubleSpinBoxTubeSize->setMaximum(5);
        doubleSpinBoxTubeSize->setSingleStep(0.01);
        doubleSpinBoxTubeSize->setValue(0.03);
        checkBoxDepthSize = new QCheckBox(groupBoxGeoRender);
        checkBoxDepthSize->setObjectName(QString::fromUtf8("checkBoxDepthSize"));
        checkBoxDepthSize->setGeometry(QRect(10, 80, 111, 22));
        label_tubesizescale = new QLabel(groupBoxGeoRender);
        label_tubesizescale->setObjectName(QString::fromUtf8("label_tubesizescale"));
        label_tubesizescale->setGeometry(QRect(30, 130, 41, 17));
        checkBoxDepthTrans = new QCheckBox(groupBoxGeoRender);
        checkBoxDepthTrans->setObjectName(QString::fromUtf8("checkBoxDepthTrans"));
        checkBoxDepthTrans->setGeometry(QRect(10, 160, 161, 22));
        doubleSpinBoxAlphaStart = new QDoubleSpinBox(groupBoxGeoRender);
        doubleSpinBoxAlphaStart->setObjectName(QString::fromUtf8("doubleSpinBoxAlphaStart"));
        doubleSpinBoxAlphaStart->setGeometry(QRect(10, 190, 62, 23));
        doubleSpinBoxAlphaStart->setMaximum(1);
        doubleSpinBoxAlphaStart->setSingleStep(0.1);
        doubleSpinBoxAlphaEnd = new QDoubleSpinBox(groupBoxGeoRender);
        doubleSpinBoxAlphaEnd->setObjectName(QString::fromUtf8("doubleSpinBoxAlphaEnd"));
        doubleSpinBoxAlphaEnd->setGeometry(QRect(100, 190, 62, 23));
        doubleSpinBoxAlphaEnd->setMaximum(1);
        doubleSpinBoxAlphaEnd->setSingleStep(0.1);
        doubleSpinBoxAlphaEnd->setValue(1);
        checkBoxDepthColor = new QCheckBox(groupBoxGeoRender);
        checkBoxDepthColor->setObjectName(QString::fromUtf8("checkBoxDepthColor"));
        checkBoxDepthColor->setGeometry(QRect(10, 400, 151, 22));
        doubleSpinBoxHueStart = new QDoubleSpinBox(groupBoxGeoRender);
        doubleSpinBoxHueStart->setObjectName(QString::fromUtf8("doubleSpinBoxHueStart"));
        doubleSpinBoxHueStart->setGeometry(QRect(10, 440, 62, 23));
        doubleSpinBoxHueStart->setMaximum(1);
        doubleSpinBoxHueStart->setSingleStep(0.1);
        doubleSpinBoxHueEnd = new QDoubleSpinBox(groupBoxGeoRender);
        doubleSpinBoxHueEnd->setObjectName(QString::fromUtf8("doubleSpinBoxHueEnd"));
        doubleSpinBoxHueEnd->setGeometry(QRect(100, 440, 62, 23));
        doubleSpinBoxHueEnd->setMaximum(1);
        doubleSpinBoxHueEnd->setSingleStep(0.1);
        doubleSpinBoxHueEnd->setValue(0);
        label_hue = new QLabel(groupBoxGeoRender);
        label_hue->setObjectName(QString::fromUtf8("label_hue"));
        label_hue->setGeometry(QRect(50, 420, 71, 17));
        doubleSpinBoxSatuStart = new QDoubleSpinBox(groupBoxGeoRender);
        doubleSpinBoxSatuStart->setObjectName(QString::fromUtf8("doubleSpinBoxSatuStart"));
        doubleSpinBoxSatuStart->setGeometry(QRect(10, 490, 62, 23));
        doubleSpinBoxSatuStart->setMaximum(1);
        doubleSpinBoxSatuStart->setSingleStep(0.1);
        doubleSpinBoxSatuEnd = new QDoubleSpinBox(groupBoxGeoRender);
        doubleSpinBoxSatuEnd->setObjectName(QString::fromUtf8("doubleSpinBoxSatuEnd"));
        doubleSpinBoxSatuEnd->setGeometry(QRect(100, 490, 62, 23));
        doubleSpinBoxSatuEnd->setMaximum(1);
        doubleSpinBoxSatuEnd->setSingleStep(0.1);
        label_saturation = new QLabel(groupBoxGeoRender);
        label_saturation->setObjectName(QString::fromUtf8("label_saturation"));
        label_saturation->setGeometry(QRect(40, 470, 111, 20));
        label_value = new QLabel(groupBoxGeoRender);
        label_value->setObjectName(QString::fromUtf8("label_value"));
        label_value->setGeometry(QRect(40, 520, 91, 20));
        doubleSpinBoxValueEnd = new QDoubleSpinBox(groupBoxGeoRender);
        doubleSpinBoxValueEnd->setObjectName(QString::fromUtf8("doubleSpinBoxValueEnd"));
        doubleSpinBoxValueEnd->setGeometry(QRect(100, 540, 62, 23));
        doubleSpinBoxValueEnd->setMaximum(1);
        doubleSpinBoxValueEnd->setSingleStep(0.1);
        doubleSpinBoxValueEnd->setValue(1);
        doubleSpinBoxValueStart = new QDoubleSpinBox(groupBoxGeoRender);
        doubleSpinBoxValueStart->setObjectName(QString::fromUtf8("doubleSpinBoxValueStart"));
        doubleSpinBoxValueStart->setGeometry(QRect(10, 540, 62, 23));
        doubleSpinBoxValueStart->setMaximum(1);
        doubleSpinBoxValueStart->setSingleStep(0.1);
        doubleSpinBoxTubeSizeScale = new QDoubleSpinBox(groupBoxGeoRender);
        doubleSpinBoxTubeSizeScale->setObjectName(QString::fromUtf8("doubleSpinBoxTubeSizeScale"));
        doubleSpinBoxTubeSizeScale->setGeometry(QRect(80, 130, 62, 23));
        doubleSpinBoxTubeSizeScale->setMinimum(1);
        doubleSpinBoxTubeSizeScale->setMaximum(50);
        doubleSpinBoxTubeSizeScale->setValue(20);
        sliderHaloWidth = new QSlider(groupBoxGeoRender);
        sliderHaloWidth->setObjectName(QString::fromUtf8("sliderHaloWidth"));
        sliderHaloWidth->setGeometry(QRect(10, 60, 151, 16));
        sliderHaloWidth->setMouseTracking(true);
        sliderHaloWidth->setMinimum(0);
        sliderHaloWidth->setMaximum(10);
        sliderHaloWidth->setPageStep(2);
        sliderHaloWidth->setValue(1);
        sliderHaloWidth->setOrientation(Qt::Horizontal);
        label_halowidth = new QLabel(groupBoxGeoRender);
        label_halowidth->setObjectName(QString::fromUtf8("label_halowidth"));
        label_halowidth->setGeometry(QRect(70, 40, 41, 21));
        checkBoxHatching = new QCheckBox(groupBoxGeoRender);
        checkBoxHatching->setObjectName(QString::fromUtf8("checkBoxHatching"));
        checkBoxHatching->setGeometry(QRect(10, 570, 93, 22));
        checkBoxDepthColorLAB = new QCheckBox(groupBoxGeoRender);
        checkBoxDepthColorLAB->setObjectName(QString::fromUtf8("checkBoxDepthColorLAB"));
        checkBoxDepthColorLAB->setGeometry(QRect(10, 230, 151, 22));
        checkBoxDepthValue = new QCheckBox(groupBoxGeoRender);
        checkBoxDepthValue->setObjectName(QString::fromUtf8("checkBoxDepthValue"));
        checkBoxDepthValue->setGeometry(QRect(10, 270, 151, 22));
        doubleSpinBoxDValueEnd = new QDoubleSpinBox(groupBoxGeoRender);
        doubleSpinBoxDValueEnd->setObjectName(QString::fromUtf8("doubleSpinBoxDValueEnd"));
        doubleSpinBoxDValueEnd->setGeometry(QRect(100, 310, 62, 23));
        doubleSpinBoxDValueEnd->setMaximum(1);
        doubleSpinBoxDValueEnd->setSingleStep(0.1);
        doubleSpinBoxDValueEnd->setValue(1);
        doubleSpinBoxDValueStart = new QDoubleSpinBox(groupBoxGeoRender);
        doubleSpinBoxDValueStart->setObjectName(QString::fromUtf8("doubleSpinBoxDValueStart"));
        doubleSpinBoxDValueStart->setGeometry(QRect(10, 310, 62, 23));
        doubleSpinBoxDValueStart->setMaximum(1);
        doubleSpinBoxDValueStart->setSingleStep(0.1);
        pushButtonApply = new QPushButton(groupBoxGeoRender);
        pushButtonApply->setObjectName(QString::fromUtf8("pushButtonApply"));
        pushButtonApply->setGeometry(QRect(22, 350, 121, 41));
        dockWidget->setWidget(dockWidgetContents);
        TestbedWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(1), dockWidget);

        menubar->addAction(menuData->menuAction());
        menubar->addAction(menuControl->menuAction());
        menuData->addAction(actionLoadVolume);
        menuData->addAction(actionLoadGeometry);
        menuData->addAction(actionLoadData);
        menuData->addSeparator();
        menuData->addAction(actionClose_current);
        menuControl->addAction(actionVolumeRender);
        menuControl->addAction(actionGeometryRender);
        menuControl->addAction(actionMultipleVolumesRender);
        menuControl->addAction(actionCompositeRender);
        menuControl->addSeparator();
        menuControl->addAction(actionTF_customize);
        menuControl->addAction(actionSettings);
        menuControl->addSeparator();
        menuControl->addAction(actionExit);

        retranslateUi(TestbedWindow);
        QObject::connect(actionExit, SIGNAL(triggered()), TestbedWindow, SLOT(close()));

        QMetaObject::connectSlotsByName(TestbedWindow);
    } // setupUi

    void retranslateUi(QMainWindow *TestbedWindow)
    {
        TestbedWindow->setWindowTitle(QApplication::translate("TestbedWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionLoadVolume->setText(QApplication::translate("TestbedWindow", "Load volume...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionLoadVolume->setToolTip(QApplication::translate("TestbedWindow", "Load single volume or multiple volumes", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionLoadVolume->setShortcut(QApplication::translate("TestbedWindow", "F2", 0, QApplication::UnicodeUTF8));
        actionLoadGeometry->setText(QApplication::translate("TestbedWindow", "Load geometry...", 0, QApplication::UnicodeUTF8));
        actionLoadGeometry->setShortcut(QApplication::translate("TestbedWindow", "F3", 0, QApplication::UnicodeUTF8));
        actionVolumeRender->setText(QApplication::translate("TestbedWindow", "Single Volume Render", 0, QApplication::UnicodeUTF8));
        actionVolumeRender->setShortcut(QApplication::translate("TestbedWindow", "F5", 0, QApplication::UnicodeUTF8));
        actionTF_customize->setText(QApplication::translate("TestbedWindow", "TF customize...", 0, QApplication::UnicodeUTF8));
        actionTF_customize->setShortcut(QApplication::translate("TestbedWindow", "F10", 0, QApplication::UnicodeUTF8));
        actionGeometryRender->setText(QApplication::translate("TestbedWindow", "Geometry Render", 0, QApplication::UnicodeUTF8));
        actionGeometryRender->setShortcut(QApplication::translate("TestbedWindow", "F6", 0, QApplication::UnicodeUTF8));
        actionCompositeRender->setText(QApplication::translate("TestbedWindow", "Composite Render", 0, QApplication::UnicodeUTF8));
        actionCompositeRender->setShortcut(QApplication::translate("TestbedWindow", "F8", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("TestbedWindow", "Exit", 0, QApplication::UnicodeUTF8));
        actionExit->setShortcut(QApplication::translate("TestbedWindow", "F12", 0, QApplication::UnicodeUTF8));
        actionLoadData->setText(QApplication::translate("TestbedWindow", "Load data...", 0, QApplication::UnicodeUTF8));
        actionLoadData->setShortcut(QApplication::translate("TestbedWindow", "F4", 0, QApplication::UnicodeUTF8));
        actionMultipleVolumesRender->setText(QApplication::translate("TestbedWindow", "Multiple Volumes Render", 0, QApplication::UnicodeUTF8));
        actionMultipleVolumesRender->setShortcut(QApplication::translate("TestbedWindow", "F7", 0, QApplication::UnicodeUTF8));
        actionSettings->setText(QApplication::translate("TestbedWindow", "Settings...", 0, QApplication::UnicodeUTF8));
        actionClose_current->setText(QApplication::translate("TestbedWindow", "Close current", 0, QApplication::UnicodeUTF8));
        menuData->setTitle(QApplication::translate("TestbedWindow", "Data", 0, QApplication::UnicodeUTF8));
        menuControl->setTitle(QApplication::translate("TestbedWindow", "Control", 0, QApplication::UnicodeUTF8));
        dockWidget->setWindowTitle(QApplication::translate("TestbedWindow", "Settings...", 0, QApplication::UnicodeUTF8));
        groupBoxVolRender->setTitle(QApplication::translate("TestbedWindow", "Volume Rendering", 0, QApplication::UnicodeUTF8));
        label_presets->setText(QApplication::translate("TestbedWindow", "Presets", 0, QApplication::UnicodeUTF8));
        label_methods->setText(QApplication::translate("TestbedWindow", "Methods", 0, QApplication::UnicodeUTF8));
        groupBoxGeoRender->setTitle(QApplication::translate("TestbedWindow", "Geometry Rendering", 0, QApplication::UnicodeUTF8));
        checkBoxTubeHalos->setText(QApplication::translate("TestbedWindow", "Tube halos", 0, QApplication::UnicodeUTF8));
        label_tubesize->setText(QApplication::translate("TestbedWindow", "size", 0, QApplication::UnicodeUTF8));
        checkBoxDepthSize->setText(QApplication::translate("TestbedWindow", "Depth->Size", 0, QApplication::UnicodeUTF8));
        label_tubesizescale->setText(QApplication::translate("TestbedWindow", "scale", 0, QApplication::UnicodeUTF8));
        checkBoxDepthTrans->setText(QApplication::translate("TestbedWindow", "Depth->Transparency", 0, QApplication::UnicodeUTF8));
        checkBoxDepthColor->setText(QApplication::translate("TestbedWindow", "Depth->Color (HSV)", 0, QApplication::UnicodeUTF8));
        label_hue->setText(QApplication::translate("TestbedWindow", "Hue Range", 0, QApplication::UnicodeUTF8));
        label_saturation->setText(QApplication::translate("TestbedWindow", "Saturation Range", 0, QApplication::UnicodeUTF8));
        label_value->setText(QApplication::translate("TestbedWindow", "Value Range", 0, QApplication::UnicodeUTF8));
        label_halowidth->setText(QApplication::translate("TestbedWindow", "width", 0, QApplication::UnicodeUTF8));
        checkBoxHatching->setText(QApplication::translate("TestbedWindow", "Hatching", 0, QApplication::UnicodeUTF8));
        checkBoxDepthColorLAB->setText(QApplication::translate("TestbedWindow", "Depth->Color (LAB)", 0, QApplication::UnicodeUTF8));
        checkBoxDepthValue->setText(QApplication::translate("TestbedWindow", "Depth->Value", 0, QApplication::UnicodeUTF8));
        pushButtonApply->setText(QApplication::translate("TestbedWindow", "Apply", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TestbedWindow: public Ui_TestbedWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TESTBED_H
