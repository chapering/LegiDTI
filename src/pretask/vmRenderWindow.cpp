// ----------------------------------------------------------------------------
// vmRenderWindow.cpp : volume rendering Window with Qt Mainwindow
//
// Creation : Nov. 12th 2011
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#include "vmRenderWindow.h"
#include "vtkLegiInteractorStyle.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkLegiProperty.h"
#include "vtkLegiPolyDataMapper.h"
#include "vtkCanvasWidget.h"

#include <vtkPolyLine.h>
#include <vtkPolygon.h>
#include <vtkLine.h>
#include <vtkMatrix4x4.h>
#include <vtkPlaneWidget.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkActor2D.h>
#include <vtkProperty2D.h>
#include <vtkCoordinate.h>
#include <vtkLight.h>

using namespace std;

//////////////////////////////////////////////////////////////////////
// implementation of class vtkBoxWidgetCallback 
//////////////////////////////////////////////////////////////////////
vtkBoxWidgetCallback::vtkBoxWidgetCallback()
{ 
	this->Mapper = 0; 
}

vtkBoxWidgetCallback* vtkBoxWidgetCallback::New()
{
	return new vtkBoxWidgetCallback; 
}

void vtkBoxWidgetCallback::Execute(vtkObject *caller, unsigned long, void*)
{
	vtkBoxWidget *widget = reinterpret_cast<vtkBoxWidget*>(caller);
	if (this->Mapper)
	{
		vtkPlanes *planes = vtkPlanes::New();
		widget->GetPlanes(planes);
		this->Mapper->SetClippingPlanes(planes);
		planes->Delete();
	}
}

void vtkBoxWidgetCallback::SetMapper(vtkVolumeMapper* m) 
{ 
	this->Mapper = m; 
}

//////////////////////////////////////////////////////////////////////
// implementation of class vtkViewKeyEventCallback 
//////////////////////////////////////////////////////////////////////
vtkViewKeyEventCallback::vtkViewKeyEventCallback()
{
}

vtkViewKeyEventCallback* vtkViewKeyEventCallback::New()
{
	return new vtkViewKeyEventCallback;
}

void vtkViewKeyEventCallback::Execute(vtkObject *caller, unsigned long eid, void* edata)
{
	//CLegiMainWindow *legiWin = reinterpret_cast<CLegiMainWindow*> (caller);

	cout << "eid = " << eid << "\n";
	cout << "edata = " << (*(int*)edata) << "\n";
}

//////////////////////////////////////////////////////////////////////
// implementation of class CLegiMainWindow
//////////////////////////////////////////////////////////////////////
CLegiMainWindow::CLegiMainWindow(int argc, char **argv, QWidget* parent, Qt::WindowFlags f)
	: QMainWindow(parent, f), CApplication(argc, argv),
	m_strfnhelp(""),
	m_strfntask(""),
	m_strfnskeleton(""),
	m_nselbox(1),
	m_lod(5),
	m_fRadius(0.25),

	m_pImgRender(NULL),
	m_pstlineModel(NULL),
	m_render( vtkSmartPointer<vtkRenderer>::New() ),
	m_boxWidget(NULL),
	m_oldsz(0, 0),
	m_fnData(""),
	m_fnVolume(""),
	m_fnGeometry(""),
	m_nHaloType(0),
	m_nHaloWidth(1),
	m_bDepthHalo(false),
	m_bDepthSize(false),
	m_bDepthTransparency(false),
	m_bDepthColor(false),
	m_bDepthColorLAB(false),
	m_bDepthValue(false),
	m_nCurMethodIdx(0),
	m_nCurPresetIdx(0),
	m_bLighting(true),
	m_bCapping(false),
	m_bHatching(false),
	m_curFocus(FOCUS_GEO),
	m_bInit(false),
	m_bFirstHalo(true),
	m_bCurveMapping(false),
	m_nKey(0)
{
	setWindowTitle( "GUItestbed for LegiDTI v1.0" );

	setupUi( this );
	statusbar->setStatusTip ( "ready to load" );

	__init_volrender_methods();
	__init_tf_presets();

	m_transparency.update(0.0, 1.0);
	m_value.update(0.0, 1.0);

	m_pImgRender = new imgVolRender(this);
	m_pstlineModel = new vtkTgDataReader;

	connect( actionLoadVolume, SIGNAL( triggered() ), this, SLOT (onactionLoadVolume()) );
	connect( actionLoadGeometry, SIGNAL( triggered() ), this, SLOT (onactionLoadGeometry()) );
	connect( actionLoadData, SIGNAL( triggered() ), this, SLOT (onactionLoadData()) );
	connect( actionClose_current, SIGNAL( triggered() ), this, SLOT (onactionClose_current()) );

	connect( actionVolumeRender, SIGNAL( triggered() ), this, SLOT (onactionVolumeRender()) );
	connect( actionGeometryRender, SIGNAL( triggered() ), this, SLOT (onactionGeometryRender()) );
	connect( actionMultipleVolumesRender, SIGNAL( triggered() ), this, SLOT (onactionMultipleVolumesRender()) );
	connect( actionCompositeRender, SIGNAL( triggered() ), this, SLOT (onactionCompositeRender()) );

	connect( actionTF_customize, SIGNAL( triggered() ), this, SLOT (onactionTF_customize()) );
	connect( actionSettings, SIGNAL( triggered() ), this, SLOT (onactionSettings()) );

	//////////////////////////////////////////////////////////////////
	connect( comboBoxVolRenderMethods, SIGNAL( currentIndexChanged(int) ), this, SLOT ( onVolRenderMethodChanged(int) ) );
	connect( comboBoxVolRenderPresets, SIGNAL( currentIndexChanged(int) ), this, SLOT ( onVolRenderPresetChanged(int) ) );

	connect( checkBoxTubeHalos, SIGNAL( stateChanged(int) ), this, SLOT (onHaloStateChanged(int)) );
	connect( checkBoxDepthSize, SIGNAL( stateChanged(int) ), this, SLOT (onTubeSizeStateChanged(int)) );
	connect( checkBoxDepthTrans, SIGNAL( stateChanged(int) ), this, SLOT (onTubeAlphaStateChanged(int)) );
	connect( checkBoxDepthColor, SIGNAL( stateChanged(int) ), this, SLOT (onTubeColorStateChanged(int)) );

	connect( doubleSpinBoxTubeSize, SIGNAL( valueChanged(double) ), this, SLOT (onTubeSizeChanged(double)) );
	connect( doubleSpinBoxTubeSizeScale, SIGNAL( valueChanged(double) ), this, SLOT (onTubeSizeScaleChanged(double)) );

	connect( doubleSpinBoxAlphaStart, SIGNAL( valueChanged(double) ), this, SLOT (onTubeAlphaStartChanged(double)) );
	connect( doubleSpinBoxAlphaEnd, SIGNAL( valueChanged(double) ), this, SLOT (onTubeAlphaEndChanged(double)) );

	connect( doubleSpinBoxHueStart, SIGNAL( valueChanged(double) ), this, SLOT (onTubeHueStartChanged(double)) );
	connect( doubleSpinBoxHueEnd, SIGNAL( valueChanged(double) ), this, SLOT (onTubeHueEndChanged(double)) );
	connect( doubleSpinBoxSatuStart, SIGNAL( valueChanged(double) ), this, SLOT (onTubeSatuStartChanged(double)) );
	connect( doubleSpinBoxSatuEnd, SIGNAL( valueChanged(double) ), this, SLOT (onTubeSatuEndChanged(double)) );
	connect( doubleSpinBoxValueStart, SIGNAL( valueChanged(double) ), this, SLOT (onTubeValueStartChanged(double)) );
	connect( doubleSpinBoxValueEnd, SIGNAL( valueChanged(double) ), this, SLOT (onTubeValueEndChanged(double)) );

	connect( checkBoxDepthColorLAB, SIGNAL ( stateChanged(int) ), this, SLOT (onTubeColorLABStateChanged(int)) );

	connect( checkBoxDepthValue, SIGNAL (stateChanged(int)), this, SLOT (onTubeDValueStateChanged(int)) );
	connect( doubleSpinBoxDValueStart, SIGNAL( valueChanged(double) ), this, SLOT (onTubeDValueStartChanged(double)) );
	connect( doubleSpinBoxDValueEnd, SIGNAL( valueChanged(double) ), this, SLOT (onTubeDValueEndChanged(double)) );

	connect( sliderHaloWidth, SIGNAL( valueChanged(int) ), this, SLOT (onHaloWidthChanged(int)) );

	connect( checkBoxHatching, SIGNAL( stateChanged(int) ), this, SLOT (onHatchingStateChanged(int)) );

	connect( pushButtonApply, SIGNAL( released() ), this, SLOT (onButtonApply()) );
	//////////////////////////////////////////////////////////////////
	
	m_render->SetBackground( 0.4392, 0.5020, 0.5647 );

	//vtkSmartPointer< vtkInteractorStyleTrackballCamera > style = vtkSmartPointer< vtkInteractorStyleTrackballCamera >::New();
	vtkSmartPointer< vtkLegiInteractorStyle > style = vtkSmartPointer< vtkLegiInteractorStyle >::New();
	renderView->GetInteractor()->SetInteractorStyle( style );

	qvtkConnections = vtkEventQtSlotConnect::New();
	qvtkConnections->Connect( renderView->GetRenderWindow()->GetInteractor(),
							vtkCommand::KeyPressEvent, 
							this,
							SLOT ( onKeys(vtkObject*,unsigned long,void*,void*,vtkCommand*) ) );

	m_boxWidget = vtkBoxWidget::New();

	m_boxWidget->SetInteractor(renderView->GetInteractor());
	m_boxWidget->SetPlaceFactor(1.0);
	m_boxWidget->InsideOutOn();

	m_actor = vtkSmartPointer<vtkActor>::New();
	m_haloactor = vtkSmartPointer<vtkActor>::New();
	m_colorbar = vtkSmartPointer<vtkScalarBarActor>::New();

	m_camera = m_render->MakeCamera();
	
	// HSV will not be used for the first doctor meeting
	checkBoxDepthColor->hide();
	label_hue->hide();
	label_saturation->hide();
	label_value->hide();
	doubleSpinBoxHueStart->hide();
	doubleSpinBoxHueEnd->hide();
	doubleSpinBoxSatuStart->hide();
	doubleSpinBoxSatuEnd->hide();
	doubleSpinBoxValueStart->hide();
	doubleSpinBoxValueEnd->hide();

	checkBoxHatching->hide();
	dockWidget->hide();
	menubar->hide();
	statusbar->hide();
	setWindowState(Qt::WindowMaximized);

	/////////////////////////////////////////////////////////////
	addOption('f', true, "input-file-name", "the name of source file"
			" containing geometry and in the format of tgdata");
	addOption('r', true, "tube-radius", "fixed radius of the tubes"
			" to generate");
	addOption('l', true, "lod", "level ot detail controlling the tube"
			" generation, it is expected to impact the smoothness of tubes");
	addOption('p', true, "prompt-text", "a file of interaction help prompt");
	addOption('t', true, "task-list", "a file containing a list of "
			"visualization tasks");
	addOption('s', true, "skeletonic-geometry", "a file containing geometry"
		    " of bundle skeletons also in the format of tgdata");
	addOption('j', true, "boxpos", "files of box position");

	m_taskbox = vtkSmartPointer<CVTKTextbox>::New();
	m_helptext = vtkSmartPointer<CVTKTextbox>::New();

	m_optionPanel = new COptionPanel("Options", centralwidget);
	/*
	m_optionPanel->setOutputStream( &m_cout );
	m_optionPanel->addButton( "Similar" );
	m_optionPanel->addButton( "1 is Higher" );
	m_optionPanel->addButton( "2 is Higher" );
	*/
	m_optionPanel->hide();
}

CLegiMainWindow::~CLegiMainWindow()
{
	delete m_pImgRender;
	delete m_pstlineModel;
	if ( m_boxWidget ) {
		m_boxWidget->Delete();
		m_boxWidget = NULL;
	}

	delete m_optionPanel;

	for (size_t i = 0; i < m_boxes.size(); i++) {
		m_boxes[i]->Delete();
	}
}

int CLegiMainWindow::handleOptions(int optv) 
{
	switch( optv ) {
		case 'f':
			m_fnGeometry= optarg;
			return 0;
		case 'p':
			{
				m_strfnhelp = optarg;
				return 0;
			}
			break;
		case 'r':
			m_fRadius = strtof(optarg, NULL);
			if ( m_fRadius < 0.1 ) {
				cerr << "value for radius is illicit, should be >= .1\n";
				return -1;
			}
			return 0;
		case 'l':
			{
				int lod = atoi(optarg);
				if ( lod >= 2 ) {
					m_lod = lod;
					return 0;
				}
				else {
					cerr << "value for lod is illict, should be >= 2.\n";
					return -1;
				}
			}
			break;
		case 't':
			{
				m_strfntask = optarg;
				return 0;
			}
			break;
		case 's':
			{
				m_strfnskeleton = optarg;
				return 0;
			}
			break;
		case 'j':
			{
				m_strfnBoxes.push_back(optarg);
				return 0;
			}
			break;
		default:
			return CApplication::handleOptions( optv );
	}
	return 1;
}

int CLegiMainWindow::mainstay()
{
	/*
	if ( !m_pstlineModel->Load( m_fnGeometry.c_str(), true ) ) {
		return -1;
	}

	__load_lab();
	*/

	renderView->GetRenderWindow()->FullScreenOn();
	setWindowTitle( "LegiDTI Perceptual Study - pretask" );

	/*
	show();

	__add_axes();
	*/
	renderView->GetRenderWindow()->AddRenderer( m_render );
	QMainWindow::show();

	if ( m_strfntask != "" ) {
		m_taskbox->loadFromFile( m_strfntask.c_str() );
		//m_taskbox->setRenderWindow( renderView->GetRenderWindow() );
		m_taskbox->setUseType( UT_TASKBOX );

		m_render->AddActor2D( m_taskbox );
	}


	/*
	if ( m_strfnhelp != "" ) {
		m_helptext->loadFromFile( m_strfnhelp.c_str() );
		//m_helptext->setRenderWindow( renderView->GetRenderWindow() );
		m_helptext->setUseType( UT_HELPTEXT );

		m_render->AddActor2D( m_helptext );
	}

	__loadBoxpos();

	if ( m_cout.isswitchon() ) {
		m_cout.switchtime(true);

		// start event recording
		m_recorder = vtkSmartPointer<vtkLegiInteractionRecorder>::New();
		m_recorder->SetInteractor( renderView->GetRenderWindow()->GetInteractor() );
		m_recorder->SetOutputStream( &m_cout );
		m_recorder->StudyLogStyleOn();
		m_recorder->EnabledOn();
		m_recorder->Record();
	}

	__drawCanvas(true);
	*/
	if ( m_cout.isswitchon() ) {
		m_cout.switchtime(true);
	}

	m_cout << "started.\n";
	m_render->ResetCamera();
	m_camera->Zoom (2.0);
	m_render->ResetCameraClippingRange();
	renderView->update();

	m_cout << "Loading finished \n";
	return CApplication::mainstay();
}

void CLegiMainWindow::keyPressEvent (QKeyEvent* event)
{
	QMainWindow::keyPressEvent( event );
}

void CLegiMainWindow::resizeEvent( QResizeEvent* event )
{
	QMainWindow::resizeEvent( event );
	if (m_oldsz.isNull())
	{
		m_oldsz = event->size();
	}
	else
	{
		QSize sz = event->size();
		QSize osz = this->m_oldsz;

		float wf = sz.width()*1.0/osz.width();
		float hf = sz.height()*1.0/osz.height();

		QSize orsz = this->renderView->size();
		orsz.setWidth ( orsz.width() * wf );
		orsz.setHeight( orsz.height() * hf );
		//this->renderView->resize( orsz );
		this->renderView->resize( sz );

		this->m_oldsz = sz;
	}
}

void CLegiMainWindow::draw()
{
	onactionGeometryRender();
}

void CLegiMainWindow::show()
{
	QMainWindow::show();
	this->draw();
	m_optionPanel->show();
}

QVTKWidget*& CLegiMainWindow::getRenderView()
{
	return renderView;
}

void CLegiMainWindow::addBoxWidget()
{	
	/*
	if ( m_boxWidget ) {
		m_boxWidget->Delete();
		m_boxWidget = NULL;
	}
	*/
	m_boxWidget->Off();
	m_boxWidget->RemoveAllObservers();

	vtkVolumeMapper* mapper = vtkVolumeMapper::SafeDownCast(m_pImgRender->getVol()->GetMapper());
	vtkBoxWidgetCallback *callback = vtkBoxWidgetCallback::New();
	callback->SetMapper(mapper);
	 
	m_boxWidget->SetInput(mapper->GetInput());
	m_boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);
	callback->Delete();
	m_boxWidget->SetProp3D(m_pImgRender->getVol());
	m_boxWidget->PlaceWidget();
	m_boxWidget->On();
	//m_boxWidget->GetSelectedFaceProperty()->SetOpacity(0.0);
}


void CLegiMainWindow::onactionLoadVolume()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("select volume to load"),
			"/home/chap", QFileDialog::ShowDirsOnly);
	if (dir.isEmpty()) return;

	m_fnVolume = dir.toStdString();
	if ( !m_pImgRender->mount(m_fnVolume.c_str(), true) ) {
		QMessageBox::critical(this, "Error...", "failed to load volume images provided.");
		m_fnVolume = "";
	}
}

void CLegiMainWindow::onactionLoadGeometry()
{
	QString fn = QFileDialog::getOpenFileName(this, tr("select geometry to load"), 
			"/home/chap", tr("Data (*.data);; All (*.*)"));
	if (fn.isEmpty()) return;

	m_fnGeometry = fn.toStdString();
	if ( !m_pstlineModel->Load( m_fnGeometry.c_str() ) ) {
		QMessageBox::critical(this, "Error...", "failed to load geometry data provided.");
		m_fnGeometry = "";
	}
}

void CLegiMainWindow::onactionLoadData()
{
	QString fn = QFileDialog::getOpenFileName(this, tr("select dataset to load"), 
			"/home/chap", tr("NIfTI (*.nii *.nii.gz);; All (*.*)"));
	if (fn.isEmpty()) return;

	m_fnData = fn.toStdString();
	if ( !m_pImgRender->mount(m_fnData.c_str()) ) {
		QMessageBox::critical(this, "Error...", "failed to load composite image data provided.");
		m_fnData = "";
	}
}

void CLegiMainWindow::onactionClose_current()
{
	__removeAllVolumes();
	__removeAllActors();
	cout << "Numer of Actor in the render after removing all: " << (m_render->VisibleActorCount()) << "\n";
	cout << "Numer of Volumes in the render after removing all: " << (m_render->VisibleVolumeCount()) << "\n";
	renderView->update();
}

void CLegiMainWindow::onactionVolumeRender()
{
	if ( m_fnVolume.length() < 1 ) {
		return;
	}

	__removeAllVolumes();

	m_render->AddVolume(m_pImgRender->getVol());
	cout << "Numer of Volumes in the render: " << (m_render->VisibleVolumeCount()) << "\n";

	renderView->GetRenderWindow()->SetAlphaBitPlanes(1);
	renderView->GetRenderWindow()->SetMultiSamples(0);
	m_render->SetUseDepthPeeling(1);
	m_render->SetMaximumNumberOfPeels(100);
	m_render->SetOcclusionRatio(0.1);

	if ( ! renderView->GetRenderWindow()->HasRenderer( m_render ) )
		renderView->GetRenderWindow()->AddRenderer(m_render);

	m_curFocus = FOCUS_VOL;
	renderView->update();
	addBoxWidget();
}

void CLegiMainWindow::onactionGeometryRender()
{
	if ( m_fnGeometry.length() < 1 ) {
		return;
	}

	/*
	if ( m_curFocus != FOCUS_GEO ) 
	{
		__removeAllVolumes();
		if ( m_boxWidget ) {
			m_boxWidget->SetEnabled( false );
		}
		m_bInit = false;
		m_nHaloType = 0;
		__renderTubes(m_nHaloType);
	}
	*/

	__uniformTubeRendering();

	/*
	if ( m_bDepthHalo || m_bDepthColorLAB || m_bDepthValue || m_bDepthTransparency || m_bDepthSize ) {
		__uniformTubeRendering();
	}
	else if (m_bHatching) {
		__add_texture_strokes();
	}
	else {
		__renderTubes(m_nHaloType);
	}

	cout << "Numer of Actor in the render: " << (m_render->VisibleActorCount()) << "\n";

	renderView->GetRenderWindow()->SetAlphaBitPlanes(1);
	renderView->GetRenderWindow()->SetMultiSamples(0);
	m_render->SetUseDepthPeeling(1);
	m_render->SetMaximumNumberOfPeels(100);
	m_render->SetOcclusionRatio(0.5);
	cout << "depth peeling flag: " << m_render->GetLastRenderingUsedDepthPeeling() << "\n";
	*/

	if ( ! renderView->GetRenderWindow()->HasRenderer( m_render ) ) {
		renderView->GetRenderWindow()->AddRenderer(m_render);
	}

	m_curFocus = FOCUS_GEO;
	renderView->update();
}

void CLegiMainWindow::onactionMultipleVolumesRender()
{
	if ( m_fnData.length() < 1 ) {
		return;
	}

	__removeAllVolumes();

	m_render->AddVolume(m_pImgRender->getVol());
	cout << "Numer of Volumes in the render: " << (m_render->VisibleVolumeCount()) << "\n";

	renderView->GetRenderWindow()->SetAlphaBitPlanes(1);
	renderView->GetRenderWindow()->SetMultiSamples(0);
	m_render->SetUseDepthPeeling(1);
	m_render->SetMaximumNumberOfPeels(100);
	m_render->SetOcclusionRatio(0.1);

	if ( ! renderView->GetRenderWindow()->HasRenderer( m_render ) )
		renderView->GetRenderWindow()->AddRenderer(m_render);

	m_curFocus = FOCUS_MVOL;
	renderView->update();
	addBoxWidget();
}

void CLegiMainWindow::onactionCompositeRender()
{
	if ( m_fnData.length() < 1 || m_fnGeometry.length() < 1) {
		return;
	}

	__removeAllVolumes();

	m_render->AddVolume(m_pImgRender->getVol());
	cout << "Numer of Volumes in the render: " << (m_render->VisibleVolumeCount()) << "\n";

	renderView->GetRenderWindow()->SetAlphaBitPlanes(1);
	renderView->GetRenderWindow()->SetMultiSamples(0);
	m_render->SetUseDepthPeeling(1);
	m_render->SetMaximumNumberOfPeels(100);
	m_render->SetOcclusionRatio(0.1);

	if ( ! renderView->GetRenderWindow()->HasRenderer( m_render ) )
		renderView->GetRenderWindow()->AddRenderer(m_render);

	m_bInit = false;
	m_nHaloType = 0;
	if ( m_boxWidget ) {
		m_boxWidget->SetEnabled( false );
	}
	__renderTubes(m_nHaloType);
	m_curFocus = FOCUS_COMPOSITE;
	renderView->update();
	addBoxWidget();
}

void CLegiMainWindow::onactionTF_customize()
{
	if (m_fnVolume.length() < 1 && m_fnData.length() < 1 ) { // the TF widget has never been necessary
		return;
	}

	if ( m_pImgRender->t_graph->isVisible() ) {
		m_pImgRender->t_graph->setVisible( false );
	}
	else {
		m_pImgRender->t_graph->setVisible( true );
	}
}

void CLegiMainWindow::onactionSettings()
{
	if ( dockWidget->isVisible() ) {
		dockWidget->setVisible( false );
	}
	else {
		dockWidget->setVisible( true );
	}
}

void CLegiMainWindow::onKeys(vtkObject* obj,unsigned long eid,void* client_data,void* data2,vtkCommand* command)
{
	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::SafeDownCast( obj );
	command->AbortFlagOn();

	switch (iren->GetKeyCode()){
		case 27:
			this->~CLegiMainWindow();
			m_cout << "quit.";
			m_cout.switchtime(false);
			m_cout << "\n";
			exit(1);
		case 13:
		case 32:
			this->~CLegiMainWindow();
			m_cout << "quit.";
			m_cout.switchtime(false);
			m_cout << "\n";
			exit(0);
		case 'i':
			m_bLighting = ! m_bLighting;
			break;
		case 'h':
			m_nHaloType = 1;
			break;
		case 'j':
			m_nHaloType = 2;
			break;
		case 'k':
			m_nHaloType = 0;
			break;
		case 'l':
			m_nHaloType = 3;
			break;
		case 'm':
			m_nHaloType = 4;
			break;
		case 'd':
			m_nHaloType = 5;
			break;
		case 's':
			m_nHaloType = 6;
			break;
		case 't':
			m_nHaloType = 7;
			break;
		case 'r':
			m_nHaloType = 8;
			break;
		case 'o':
			m_nHaloType = 9;
			break;
		case 'b':
			{
				if ( m_render->GetVolumes()->GetNumberOfItems() < 1 ) {
					cout << "no image volumes added.\n";
					return;
				}

				if ( m_boxWidget ) {
					m_boxWidget->SetEnabled( ! m_boxWidget->GetEnabled() );
				}
			}
			return;
		case 'c':
			m_bCapping = !m_bCapping;
			break;
		case 'v':
			m_bCurveMapping = !m_bCurveMapping;
			break;
		default:
			return;
	}
	onactionGeometryRender();
}

void CLegiMainWindow::onHaloStateChanged(int state)
{
	m_bDepthHalo = ( Qt::Checked == state );
	/*
	if ( m_bDepthHalo ) {
		m_nHaloType = 1;
	}
	else {
		m_nHaloType = 0;
	}
	onactionGeometryRender();
	if ( m_bDepthHalo ) {
		__uniform_halos();
		m_render->AddActor( m_haloactor );
	}
	else {
		m_render->RemoveActor( m_haloactor );
		renderView->update();
	}
	*/
}

void CLegiMainWindow::onVolRenderMethodChanged(int index)
{
	if (m_fnVolume.length() < 1 && m_fnData.length() < 1 ) { // the TF widget has never been necessary
		return;
	}
	m_nCurMethodIdx = index;

	if ( m_curFocus == FOCUS_VOL && m_fnVolume.length() >=1 && m_pImgRender->mount(m_fnVolume.c_str(), true) ) {
		onactionVolumeRender();
		return;
	}
	if ( m_curFocus == FOCUS_MVOL && m_fnData.length() >=1 && m_pImgRender->mount(m_fnData.c_str()) ) {
		onactionMultipleVolumesRender();
		return;
	}
	if ( m_curFocus == FOCUS_COMPOSITE && m_fnData.length() >=1 && m_pImgRender->mount(m_fnData.c_str()) ) {
		onactionCompositeRender();
	}
}

void CLegiMainWindow::onVolRenderPresetChanged(int index)
{
	if (m_fnVolume.length() < 1 && m_fnData.length() < 1 ) { // the TF widget has never been necessary
		return;
	}

	m_nCurPresetIdx = index;

	if ( m_nCurPresetIdx != VP_AUTO ) {
		if ( m_pImgRender->t_graph->isVisible() ) {
			m_pImgRender->t_graph->setVisible( false );
		}
	}
	else {
		if ( !m_pImgRender->t_graph->isVisible() ) {
			m_pImgRender->t_graph->setVisible( true );
		}
	}

	if ( m_curFocus == FOCUS_VOL && m_fnVolume.length() >=1 && m_pImgRender->mount(m_fnVolume.c_str(), true) ) {
		onactionVolumeRender();
		return;
	}
	if ( m_curFocus == FOCUS_MVOL && m_fnData.length() >=1 && m_pImgRender->mount(m_fnData.c_str()) ) {
		onactionMultipleVolumesRender();
		return;
	}
	if ( m_curFocus == FOCUS_COMPOSITE && m_fnData.length() >=1 && m_pImgRender->mount(m_fnData.c_str()) ) {
		onactionCompositeRender();
	}
}

void CLegiMainWindow::onTubeSizeStateChanged(int state)
{
	m_bDepthSize = ( Qt::Checked == state );
	//onactionGeometryRender();
}

void CLegiMainWindow::onTubeSizeChanged(double d)
{
	m_dptsize.size = d;
	//onactionGeometryRender();
}

void CLegiMainWindow::onTubeSizeScaleChanged(double d)
{
	m_dptsize.scale = d;
	//onactionGeometryRender();
}

void CLegiMainWindow::onTubeAlphaStateChanged(int state)
{
	m_bDepthTransparency = ( Qt::Checked == state );
	//onactionGeometryRender();
}

void CLegiMainWindow::onTubeAlphaStartChanged(double d)
{
	m_transparency[0] = d;
	//onactionGeometryRender();
}

void CLegiMainWindow::onTubeAlphaEndChanged(double d)
{
	m_transparency[1] = d;
	//onactionGeometryRender();
}

void CLegiMainWindow::onTubeColorStateChanged(int state)
{
	m_bDepthColor = ( Qt::Checked == state );
	onactionGeometryRender();
}

void CLegiMainWindow::onTubeHueStartChanged(double d)
{
	m_dptcolor.hue[0] = d;
	onactionGeometryRender();
}

void CLegiMainWindow::onTubeHueEndChanged(double d)
{
	m_dptcolor.hue[1] = d;
	onactionGeometryRender();
}

void CLegiMainWindow::onTubeSatuStartChanged(double d)
{
	m_dptcolor.satu[0] = d;
	onactionGeometryRender();
}

void CLegiMainWindow::onTubeSatuEndChanged(double d)
{
	m_dptcolor.satu[1] = d;
	onactionGeometryRender();
}

void CLegiMainWindow::onTubeValueStartChanged(double d)
{
	m_dptcolor.value[0] = d;
	onactionGeometryRender();
}

void CLegiMainWindow::onTubeValueEndChanged(double d)
{
	m_dptcolor.value[1] = d;
	onactionGeometryRender();
}

void CLegiMainWindow::onTubeColorLABStateChanged(int state)
{
	m_bDepthColorLAB = ( Qt::Checked == state );
	/*
	onactionGeometryRender();
	if ( !m_bDepthColorLAB ) {
		m_render->RemoveActor2D ( m_colorbar );
		renderView->update();
	}
	*/
}

void CLegiMainWindow::onTubeDValueStateChanged(int state)
{
	m_bDepthValue = ( Qt::Checked == state );
	//onactionGeometryRender();
}

void CLegiMainWindow::onTubeDValueStartChanged(double d)
{
	m_value[0] = d;
	//onactionGeometryRender();
}

void CLegiMainWindow::onTubeDValueEndChanged(double d)
{
	m_value[1] = d;
	//onactionGeometryRender();
}

void CLegiMainWindow::onHaloWidthChanged(int i)
{
	if ( !m_bDepthHalo ) return;
	m_nHaloWidth = i;
	/*
	if ( m_bDepthHalo ) {
		__uniform_halos();
	}
	onactionGeometryRender();
	*/
}

void CLegiMainWindow::onHatchingStateChanged(int state)
{
	m_bHatching = ( Qt::Checked == state );
	onactionGeometryRender();
}

void CLegiMainWindow::onButtonApply()
{
	if ( m_bDepthHalo ) {
		__uniform_halos();
		m_render->AddActor( m_haloactor );
	}
	else {
		m_render->RemoveActor( m_haloactor );
		renderView->update();
	}
	if ( m_bDepthHalo ) {
		__uniform_halos();
	}
	if ( !m_bDepthColorLAB ) {
		m_render->RemoveActor2D ( m_colorbar );
		renderView->update();
	}
	onactionGeometryRender();
}

void CLegiMainWindow::__renderDepthSortTubes()
{
	vtkSmartPointer<vtkTubeFilter> streamTube = vtkSmartPointer<vtkTubeFilter>::New();
	streamTube->SetInput( m_pstlineModel->GetOutput() );
	streamTube->SetRadius(0.25);
	streamTube->SetNumberOfSides(12);

	vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();

	vtkSmartPointer<vtkDepthSortPolyData> dsort = vtkSmartPointer<vtkDepthSortPolyData>::New();
	dsort->SetInputConnection( streamTube->GetOutputPort() );
	dsort->SetDirectionToBackToFront();
	//dsort->SetDirectionToFrontToBack();
	//dsort->SetDepthSortModeToParametricCenter();
	//dsort->SetDirectionToSpecifiedVector();
	dsort->SetVector(0,0,1);
	dsort->SetCamera( camera );
	dsort->SortScalarsOn();
	dsort->Update();

	//vtkSmartPointer<vtkTubeHaloMapper> mapStreamTube = vtkSmartPointer<vtkTubeHaloMapper>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapStreamTube = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapStreamTube->SetInputConnection(dsort->GetOutputPort());

	//mapStreamTube->SetScalarRange(0, dsort->GetOutput()->GetNumberOfCells());
	mapStreamTube->SetScalarRange(0, dsort->GetOutput()->GetNumberOfStrips());
	/*
	mapStreamTube->SetColorModeToMapScalars();
	mapStreamTube->ScalarVisibilityOn();
	mapStreamTube->MapScalars(0.5);
	mapStreamTube->UseLookupTableScalarRangeOn();
	*/

	vtkSmartPointer<vtkActor> streamTubeActor = vtkSmartPointer<vtkActor>::New();
	streamTubeActor->SetMapper(mapStreamTube);

	vtkSmartPointer<vtkProperty> tubeProperty = vtkSmartPointer<vtkProperty>::New();
	tubeProperty->SetOpacity(.6);
	//tubeProperty->SetOpacity(1.0);
	tubeProperty->SetColor(1,0,0);
	streamTubeActor->SetProperty( tubeProperty );
	streamTubeActor->RotateX( -72 );

	dsort->SetProp3D( streamTubeActor );
  
	m_render->SetActiveCamera( camera );
	m_render->AddActor( streamTubeActor );
	m_render->ResetCamera();
}

void CLegiMainWindow::__depth_dependent_size()
{
	//vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();

	//vtkSmartPointer<vtkDepthSortPolyData> linesort = vtkSmartPointer<vtkDepthSortPolyData>::New();
	vtkSmartPointer<vtkDepthSortPoints> linesort = vtkSmartPointer<vtkDepthSortPoints>::New();
	linesort->SetInput( m_pstlineModel->GetOutput() );
	linesort->SetDirectionToBackToFront();
	linesort->SetVector(0,0,1);
	linesort->SetCamera( m_camera );
	linesort->SortScalarsOn();
	linesort->Update();

	//vtkDataArray * SortScalars = linesort->GetOutput()->GetPointData()->GetAttribute( vtkDataSetAttributes::SCALARS );
	//SortScalars->PrintSelf( cout, vtkIndent());

	vtkSmartPointer<vtkTubeFilterEx> streamTube = vtkSmartPointer<vtkTubeFilterEx>::New();
	streamTube->SetInputConnection( linesort->GetOutputPort() );
	streamTube->SetRadius(0.03);
	streamTube->SetNumberOfSides(12);
	streamTube->SetRadiusFactor( 20 );
	//streamTube->SetVaryRadiusToVaryRadiusByAbsoluteScalar();
	streamTube->SetVaryRadiusToVaryRadiusByScalar();
	//streamTube->UseDefaultNormalOn();
	streamTube->SetCapping( m_bCapping );
	streamTube->Update();

	m_actor->GetMapper()->SetInputConnection(streamTube->GetOutputPort());
	m_actor->GetMapper()->ScalarVisibilityOff();
	m_actor->GetProperty()->SetColor(1,1,1);
	linesort->SetProp3D( m_actor );
	return;

	vtkSmartPointer<vtkPolyDataMapper> mapStreamTube = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapStreamTube->SetInputConnection(streamTube->GetOutputPort());
	//mapStreamTube->SetScalarRange(0, streamTube->GetOutput()->GetNumberOfCells());
	mapStreamTube->ScalarVisibilityOff();
	//mapStreamTube->SetScalarModeToUsePointData();

	vtkSmartPointer<vtkActor> streamTubeActor = vtkSmartPointer<vtkActor>::New();
	streamTubeActor->SetMapper(mapStreamTube);

	vtkSmartPointer<vtkProperty> tubeProperty = vtkSmartPointer<vtkProperty>::New();
	tubeProperty->SetColor(1,1,1);
	streamTubeActor->SetProperty( tubeProperty );

	linesort->SetProp3D( streamTubeActor );
  
	m_render->SetActiveCamera( m_camera );
	m_render->AddActor( streamTubeActor );
	m_render->ResetCamera();
}

void CLegiMainWindow::__renderLines(int type)
{
	vtkSmartPointer<vtkTubeHaloMapper> mapStreamLines = vtkSmartPointer<vtkTubeHaloMapper>::New();
	mapStreamLines->SetInput( m_pstlineModel->GetOutput() );

	vtkSmartPointer<vtkActor> streamLineActor = vtkSmartPointer<vtkActor>::New();
	streamLineActor ->SetMapper(mapStreamLines);
	streamLineActor->GetProperty()->BackfaceCullingOn();

	m_render->AddViewProp( streamLineActor );
}

void CLegiMainWindow::__renderRibbons()
{
	vtkSmartPointer<vtkRibbonFilter> ribbon = vtkSmartPointer<vtkRibbonFilter>::New();
	ribbon->SetInput ( m_pstlineModel->GetOutput() );

	vtkSmartPointer<vtkPolyDataMapper> mapRibbons = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapRibbons->SetInputConnection( ribbon->GetOutputPort() );

	vtkSmartPointer<vtkActor> ribbonActor = vtkSmartPointer<vtkActor>::New();
	ribbonActor->SetMapper(mapRibbons);
	ribbonActor->GetProperty()->BackfaceCullingOn();

	m_render->AddViewProp( ribbonActor );
}

void CLegiMainWindow::__depth_dependent_transparency()
{
	vtkSmartPointer<vtkTubeFilter> streamTube = vtkSmartPointer<vtkTubeFilter>::New();
	streamTube->SetInput( m_pstlineModel->GetOutput() );
	streamTube->SetRadius(0.25);
	streamTube->SetNumberOfSides(5);

	vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();

	//vtkSmartPointer<vtkDepthSortPolyData> dsort = vtkSmartPointer<vtkDepthSortPolyData>::New();
	vtkSmartPointer<vtkDepthSortPoints> dsort = vtkSmartPointer<vtkDepthSortPoints>::New();
	dsort->SetInputConnection( streamTube->GetOutputPort() );
	//dsort->SetInput( m_pstlineModel->GetOutput() );
	dsort->SetDirectionToBackToFront();
	//dsort->SetDirectionToFrontToBack();
	//dsort->SetDepthSortModeToParametricCenter();
	//dsort->SetDirectionToSpecifiedVector();
	dsort->SetVector(0,0,1);
	//dsort->SetOrigin(0,0,0);
	//dsort->SetCamera( camera );
	dsort->SetCamera( m_camera );
	dsort->SortScalarsOn();
	dsort->Update();

	vtkSmartPointer<vtkPolyDataMapper> mapStreamTube = vtkSmartPointer<vtkPolyDataMapper>::New();
	/*
	mapStreamTube->SetInputConnection(dsort->GetOutputPort());
	*/
	m_actor->GetMapper()->SetInputConnection(dsort->GetOutputPort());

	vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
	lut->SetAlphaRange( 0.01, 1.0 );
	lut->SetHueRange( 0.0, 0.0 );
	lut->SetSaturationRange(0.0, 0.0);
	lut->SetValueRange( 0.0, 1.0);
	//lut->SetAlpha( 0.5 );

	//mapStreamTube->SetLookupTable( lut );
	m_actor->GetMapper()->SetLookupTable( lut );
	//mapStreamTube->SetScalarRange( dsort->GetOutput()->GetCellData()->GetScalars()->GetRange());
	//mapStreamTube->SetScalarRange(0, dsort->GetOutput()->GetNumberOfCells());
	//mapStreamTube->SetScalarRange(0, dsort->GetOutput()->GetNumberOfPoints());
	m_actor->GetMapper()->SetScalarRange(0, dsort->GetOutput()->GetNumberOfPoints());

	return;
	//streamTube->GetOutput()->GetCellData()->GetScalars()->PrintSelf(cout, vtkIndent());
	//mapStreamTube->UseLookupTableScalarRangeOn();
	//mapStreamTube->ScalarVisibilityOn();

	vtkSmartPointer<vtkActor> streamTubeActor = vtkSmartPointer<vtkActor>::New();
	streamTubeActor->SetMapper(mapStreamTube);

	vtkSmartPointer<vtkProperty> tubeProperty = vtkSmartPointer<vtkProperty>::New();
	//tubeProperty->SetRepresentationToWireframe();
	//tubeProperty->SetColor(0,0,0);
	//tubeProperty->SetLineWidth(2.0);

	streamTubeActor->SetProperty( tubeProperty );
  
	dsort->SetProp3D( streamTubeActor );

	streamTubeActor->RotateX( -72 );
	m_render->SetActiveCamera( camera );
	m_render->AddActor( streamTubeActor );
	m_render->ResetCamera();
}

void CLegiMainWindow::__depth_dependent_halos()
{
	vtkSmartPointer<vtkTubeFilter> streamTube = vtkSmartPointer<vtkTubeFilter>::New();
	streamTube->SetInput( m_pstlineModel->GetOutput() );
	streamTube->SetRadius(0.25);
	streamTube->SetNumberOfSides(12);

	vtkSmartPointer<vtkTubeHaloMapper> mapStreamTube = vtkSmartPointer<vtkTubeHaloMapper>::New();
	mapStreamTube->SetInputConnection(streamTube->GetOutputPort());

	vtkSmartPointer<vtkActor> streamTubeActor = vtkSmartPointer<vtkActor>::New();
	streamTubeActor->SetMapper(mapStreamTube);

	vtkSmartPointer<vtkProperty> tubeProperty = vtkSmartPointer<vtkProperty>::New();
	//tubeProperty->SetRepresentationToWireframe();
	//tubeProperty->SetColor(0,0,0);
	//tubeProperty->SetLineWidth(2.0);

	streamTubeActor->SetProperty( tubeProperty );
  
	m_render->AddActor( streamTubeActor );

	__addHaloType1();
}

void CLegiMainWindow::__renderTubes(int type)
{
	if ( type == 2 ) {
		__renderDepthSortTubes();
		return;
	}

	if ( type == 3 ) {
		__renderLines();
		return;
	}

	if ( type == 4 ) {
		__depth_dependent_transparency();
		return;
	}

	if ( type == 5 ) {
		__depth_dependent_halos();
		return;
	}

	if ( type == 6 ) {
		__depth_dependent_size();
		return;
	}

	if ( type == 7 ) {
		__add_texture_strokes();
		return;
	}

	if ( type == 8 ) {
		__renderRibbons();
		return;
	}

	if ( type == 9 ) {
		__iso_surface(true);
		return;
	}

	vtkSmartPointer<vtkTubeFilter> streamTube = vtkSmartPointer<vtkTubeFilter>::New();
	streamTube->SetInput( m_pstlineModel->GetOutput() );
	streamTube->SetRadius(0.25);
	streamTube->SetNumberOfSides(12);
	streamTube->SidesShareVerticesOn();
	streamTube->SetCapping( m_bCapping );

	vtkSmartPointer<vtkPolyDataMapper> mapStreamTube = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapStreamTube->SetInputConnection(streamTube->GetOutputPort());

	/*
	vtkSmartPointer<vtkActor> streamTubeActor = vtkSmartPointer<vtkActor>::New();
	streamTubeActor->SetMapper(mapStreamTube);
	*/
	m_actor->SetMapper( mapStreamTube );

	vtkSmartPointer<vtkProperty> tubeProperty = vtkSmartPointer<vtkProperty>::New();
	//tubeProperty->SetRepresentationToWireframe();
	//tubeProperty->SetColor(0.5,0.5,0);
	//tubeProperty->SetLineWidth(2.0);

	//streamTubeActor->SetProperty( tubeProperty );
	m_actor->SetProperty( tubeProperty );

	if ( !m_bInit ) {
		m_render->SetActiveCamera( m_camera );
		//m_render->AddActor( streamTubeActor );
		m_render->AddActor( m_actor );
		/*
		m_render->ResetCamera();
		m_camera->Zoom (2.0);
		*/
		m_bInit = true;
	}

	switch ( m_nHaloType ) {
		case 0:
			break;
		case 1:
			__addHaloType1();
			break;
		case 2:
		default:
			break;
	}
}

void CLegiMainWindow::__addHaloType1()
{
	vtkSmartPointer<vtkTubeFilter> tubeHalos = vtkSmartPointer<vtkTubeFilter>::New();
	tubeHalos->SetInput( m_pstlineModel->GetOutput() );
	tubeHalos->SetRadius(0.25);
	tubeHalos->SetNumberOfSides(12);
	//tubeHalos->CappingOn();

	vtkSmartPointer<vtkPolyDataMapper> mapHalo = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapHalo->SetInputConnection( tubeHalos->GetOutputPort() );

	vtkSmartPointer<vtkProperty> haloProperty = vtkSmartPointer<vtkProperty>::New();
	haloProperty->SetRepresentationToWireframe();
	haloProperty->FrontfaceCullingOn();
	haloProperty->SetColor(0,0,0);
	haloProperty->SetLineWidth(m_nHaloWidth);
	//haloProperty->SetInterpolationToGouraud();

	vtkSmartPointer<vtkActor> haloActor = vtkSmartPointer<vtkActor>::New();
	haloActor->SetMapper( mapHalo );
	haloActor->SetProperty( haloProperty );

	m_render->AddViewProp ( haloActor );
}

void CLegiMainWindow::__uniformTubeRendering()
{
	vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
	vtkSmartPointer<vtkDepthSortPoints> linesort = vtkSmartPointer<vtkDepthSortPoints>::New();
	vtkSmartPointer<vtkDepthSortPoints> dsort = vtkSmartPointer<vtkDepthSortPoints>::New();

	vtkSmartPointer<vtkTubeFilterEx> streamTube = vtkSmartPointer<vtkTubeFilterEx>::New();
	if ( m_bDepthSize ) {
		linesort->SetInput( m_pstlineModel->GetOutput() );
		linesort->SetDirectionToBackToFront();
		linesort->SetVector(0,0,1);
		linesort->SetCamera( m_camera );
		linesort->SortScalarsOn();
		linesort->Update();
		streamTube->SetInputConnection( linesort->GetOutputPort() );
		/*
		streamTube->SetRadius(0.03);
		streamTube->SetRadiusFactor( 20 );
		*/
		streamTube->SetRadius(m_dptsize.size);
		streamTube->SetRadiusFactor(m_dptsize.scale);
		streamTube->SetVaryRadiusToVaryRadiusByScalar();
	}
	else {
		streamTube->SetInput( m_pstlineModel->GetOutput() );
		streamTube->SetRadius(m_fRadius);
		//streamTube->SetRadius(m_dptsize.size);
	}

	streamTube->SetNumberOfSides(m_lod);
	streamTube->SetCapping( m_bCapping );
	streamTube->Update();

	vtkSmartPointer<vtkPolyDataMapper> mapStreamTube = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkProperty> tubeProperty = vtkSmartPointer<vtkProperty>::New();
	/*
	vtkPolyDataMapper* mapStreamTube = vtkPolyDataMapper::SafeDownCast( m_actor->GetMapper() );
	vtkProperty* tubeProperty = m_actor->GetProperty();
	*/

	if ( m_bDepthTransparency || m_bDepthColorLAB || m_bDepthValue ) {
		dsort->SetInputConnection( streamTube->GetOutputPort() );
		dsort->SetDirectionToBackToFront();
		//dsort->SetDirectionToFrontToBack();
		//dsort->SetDepthSortModeToParametricCenter();
		//dsort->SetDirectionToSpecifiedVector();
		dsort->SetVector(0,0,1);
		//dsort->SetOrigin(0,0,0);
		dsort->SetCamera( m_camera );
		dsort->SortScalarsOn();
		dsort->Update();

		mapStreamTube->SetInputConnection(dsort->GetOutputPort());

		vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();

		if ( m_bDepthColorLAB ) {
			vtkIdType total = m_labColors->GetNumberOfPoints();
			lut->SetNumberOfTableValues( total  );
			double labValue[4];
			for (vtkIdType idx = 0; idx < total; idx++ ) {
				labValue[3] = m_bDepthTransparency?((idx+1)*1.0/total):1.0;
				m_labColors->GetPoint( idx, labValue );
				//cout << labValue[0] << "," << labValue[1] << "," << labValue[2] << "\n";
				lut->SetTableValue(idx, labValue);
			}

			if ( m_bCurveMapping ) {
				lut->SetScaleToLog10();
			}
			else {
				lut->SetScaleToLinear();
			}
			/*
			lut->SetHueRange( 0.0, 0.0 );
			lut->SetSaturationRange(0.0, 0.0);
			lut->SetValueRange( 0.0, 1.0);
			*/

			/*
			lut->SetHueRange( m_dptcolor.hue[0], m_dptcolor.hue[1] );
			lut->SetSaturationRange(m_dptcolor.satu[0], m_dptcolor.satu[1]);
			lut->SetValueRange( m_dptcolor.value[0], m_dptcolor.value[1] );
			*/
			//lut->SetAlpha( 0.5 );
			lut->SetTableRange(0,1);
			m_colorbar->SetLookupTable( lut );
			m_colorbar->SetNumberOfLabels( 5 );
			m_colorbar->SetTitle ( "Lab" );
			m_colorbar->SetMaximumWidthInPixels(60);
			m_colorbar->SetMaximumHeightInPixels(300);
			m_colorbar->SetLabelFormat("%.2f");

			m_render->AddActor2D( m_colorbar );
		}
		else {
			lut->SetHueRange( 0.0, 0.0 );
			lut->SetSaturationRange(0.0, 0.0);
			lut->SetValueRange( 1.0, 1.0);
			lut->SetAlphaRange( 1.0, 1.0 );
		}

		if ( m_bDepthValue ) {
			lut->SetValueRange( m_value[0], m_value[1] );
		}
		else {
			lut->SetValueRange( 1.0, 1.0);
		}

		if ( m_bDepthTransparency ) {
			//lut->SetAlphaRange( 0.01, 1.0 );
			lut->SetAlphaRange( m_transparency[0], m_transparency[1] );
		}
		else {
			lut->SetAlphaRange( 1.0, 1.0 );
		}

		mapStreamTube->SetLookupTable( lut );

		//mapStreamTube->SetScalarRange( dsort->GetOutput()->GetCellData()->GetScalars()->GetRange());
		//mapStreamTube->SetScalarRange(0, dsort->GetOutput()->GetNumberOfCells());
		mapStreamTube->SetScalarRange(0, dsort->GetOutput()->GetNumberOfPoints());

		//streamTube->GetOutput()->GetCellData()->GetScalars()->PrintSelf(cout, vtkIndent());
		//mapStreamTube->UseLookupTableScalarRangeOn();
		//mapStreamTube->ScalarVisibilityOn();

		//tubeProperty->SetColor(1,1,1);
		//tubeProperty->SetRepresentationToWireframe();
		//tubeProperty->SetColor(0,0,0);
		//tubeProperty->SetLineWidth(2.0);
	}
	else {
		//cout << "Fa color set.\n";
		mapStreamTube->SetInputConnection(streamTube->GetOutputPort());

		//mapStreamTube->SetLookupTable ( m_pstlineModel->GetColorTable() );
		//mapStreamTube->SetScalarRange(0, streamTube->GetOutput()->GetNumberOfPoints() );
		mapStreamTube->ScalarVisibilityOn();
		mapStreamTube->SetScalarModeToUsePointFieldData();
		mapStreamTube->SelectColorArray("FA COLORS");

		/*
		vtkSmartPointer<vtkPolyDataMapper2D> mm = vtkSmartPointer<vtkPolyDataMapper2D>::New();
		mm->SetInput( m_pstlineModel->GetOutput() );
		m_colorbar->SetMapper( mm );
		*/

		m_colorbar->SetLookupTable( m_pstlineModel->GetColorTable() );
		//m_colorbar->SetLookupTable( mapStreamTube->GetLookupTable() );
		m_colorbar->SetNumberOfLabels( 10 );
		m_colorbar->SetTitle ( "FA color map" );
		m_colorbar->SetMaximumWidthInPixels(60);
		m_colorbar->SetMaximumHeightInPixels(400);
		m_colorbar->SetLabelFormat("%.2f");
		m_colorbar->GetLabelTextProperty()->SetJustificationToRight();
		m_colorbar->GetTitleTextProperty()->SetFontSize(10);
		m_colorbar->GetProperty()->SetDisplayLocationToForeground();

		QRect parentRect = qApp->desktop()->screenGeometry();
		m_colorbar->SetDisplayPosition( parentRect.x() + parentRect.width() - 200,
										parentRect.y() + parentRect.height() - 460 );

		m_render->AddActor2D( m_colorbar );
	}

	m_actor->SetMapper( mapStreamTube );
	m_actor->SetProperty( tubeProperty );
	m_render->SetActiveCamera( m_camera );
	m_render->AddActor( m_actor );
	/*
	if ( m_bDepthSize ) {
		linesort->SetProp3D( m_actor );
	}
	if ( m_bDepthTransparency ) {
		dsort->SetProp3D( m_actor );
		//streamTubeActor->RotateX( -72 );
	}
	*/

	if ( m_bDepthHalo ) {
		__uniform_halos();
	}

	renderView->update();
	cout << "uniform_rendering finished.\n";

	return;

	vtkSmartPointer<vtkActor> streamTubeActor = vtkSmartPointer<vtkActor>::New();
	streamTubeActor->SetMapper(mapStreamTube);
	streamTubeActor->SetProperty( tubeProperty );

	if ( m_bDepthSize ) {
		linesort->SetProp3D( streamTubeActor );
	}
	if ( m_bDepthTransparency ) {
		dsort->SetProp3D( streamTubeActor );
		//streamTubeActor->RotateX( -72 );
	}

	m_render->SetActiveCamera( camera );
	m_render->AddActor( streamTubeActor );
	m_render->ResetCamera();
}

void CLegiMainWindow::__uniform_halos()
{
	vtkSmartPointer<vtkDepthSortPoints> linesort = vtkSmartPointer<vtkDepthSortPoints>::New();
	vtkSmartPointer<vtkDepthSortPoints> dsort = vtkSmartPointer<vtkDepthSortPoints>::New();

	vtkSmartPointer<vtkTubeFilterEx> streamTube = vtkSmartPointer<vtkTubeFilterEx>::New();
	if ( m_bDepthSize ) {
		linesort->SetInput( m_pstlineModel->GetOutput() );
		linesort->SetDirectionToBackToFront();
		linesort->SetVector(0,0,1);
		linesort->SetCamera( m_camera );
		linesort->SortScalarsOn();
		linesort->Update();
		streamTube->SetInputConnection( linesort->GetOutputPort() );
		streamTube->SetRadius(m_dptsize.size);
		streamTube->SetRadiusFactor(m_dptsize.scale);
		streamTube->SetVaryRadiusToVaryRadiusByScalar();
	}
	else {
		streamTube->SetInput( m_pstlineModel->GetOutput() );
		streamTube->SetRadius(m_fRadius);
	}

	streamTube->SetNumberOfSides(m_lod);
	streamTube->SetCapping( m_bCapping );
	streamTube->Update();

	vtkPolyDataMapper* mapStreamTube = m_bFirstHalo? vtkPolyDataMapper::New() :
			vtkPolyDataMapper::SafeDownCast( m_haloactor->GetMapper() );
	vtkProperty* tubeProperty = m_bFirstHalo? vtkProperty::New() : m_haloactor->GetProperty();

	tubeProperty->SetRepresentationToWireframe();
	tubeProperty->FrontfaceCullingOn();
	tubeProperty->SetColor(0,0,0);
	tubeProperty->SetLineWidth(m_nHaloWidth);

	if ( m_bDepthTransparency || m_bDepthColor ) {
		dsort->SetInputConnection( streamTube->GetOutputPort() );
		dsort->SetDirectionToBackToFront();
		dsort->SetVector(0,0,1);
		dsort->SetCamera( m_camera );
		dsort->SortScalarsOn();
		dsort->Update();

		mapStreamTube->SetInputConnection(dsort->GetOutputPort());

		vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
		if ( m_bDepthTransparency ) {
			//lut->SetAlphaRange( 0.01, 1.0 );
			lut->SetAlphaRange( m_transparency[0], m_transparency[1] );
			lut->SetValueRange( 1.0, 0.0);
		}
		else {
			lut->SetAlphaRange( 1.0, 1.0 );
			lut->SetValueRange( 0.0, 0.0);
		}

		lut->SetHueRange( 0.0, 0.0 );
		lut->SetSaturationRange(0.0, 0.0);

		mapStreamTube->SetLookupTable( lut );
		mapStreamTube->SetScalarRange(0, dsort->GetOutput()->GetNumberOfPoints());
	}
	else {
		mapStreamTube->SetInputConnection(streamTube->GetOutputPort());
		mapStreamTube->ScalarVisibilityOff();
	}

	if ( m_bFirstHalo ) {
		m_haloactor->SetMapper( mapStreamTube );
		mapStreamTube->Delete();

		m_haloactor->SetProperty( tubeProperty );
		tubeProperty->Delete();

		m_bFirstHalo = false;
	}

	if ( m_bDepthSize ) {
		linesort->SetProp3D( m_haloactor );
	}
	if ( m_bDepthTransparency ) {
		dsort->SetProp3D( m_haloactor );
		//streamTubeActor->RotateX( -72 );
	}

	renderView->update();
}

void CLegiMainWindow::__removeAllActors()
{
	m_render->RemoveAllViewProps();
	vtkActorCollection* allactors = m_render->GetActors();
	vtkActor * actor = allactors->GetNextActor();
	while ( actor ) {
		m_render->RemoveActor( actor );
		actor = allactors->GetNextActor();
	}
	cout << "Numer of Actor in the render after removing all: " << (m_render->VisibleActorCount()) << "\n";
}

void CLegiMainWindow::__removeAllVolumes()
{
	m_render->RemoveAllViewProps();
	vtkVolumeCollection* allvols = m_render->GetVolumes();
	vtkVolume * vol = allvols->GetNextVolume();
	while ( vol ) {
		m_render->RemoveVolume(vol);
		vol = allvols->GetNextVolume();
	}

	cout << "Numer of Volumes in the render after removing all: " << (m_render->VisibleVolumeCount()) << "\n";
}

void CLegiMainWindow::__init_volrender_methods()
{
	for (int idx = VM_START + 1; idx < VM_END; idx++) {
		comboBoxVolRenderMethods->addItem( QString(g_volrender_methods[ idx ]) );
	}
}

void CLegiMainWindow::__init_tf_presets()
{
	for (int idx = VP_START + 1; idx < VP_END; idx++) {
		comboBoxVolRenderPresets->addItem( QString(g_volrender_presets[ idx ]) );
	}
}

void CLegiMainWindow::__add_texture_strokes()
{
	vtkSmartPointer<vtkTubeFilter> streamTube = vtkSmartPointer<vtkTubeFilter>::New();
	streamTube->SetInput( m_pstlineModel->GetOutput() );
	streamTube->SetRadius(0.25);
	streamTube->SetNumberOfSides(12);
	streamTube->SetCapping( m_bCapping );
	streamTube->SetGenerateTCoordsToUseLength();
	streamTube->SetTextureLength(1.0);
	streamTube->SetGenerateTCoordsToUseScalars();

	vtkSmartPointer<vtkTextureMapToCylinder> tmapper = vtkSmartPointer<vtkTextureMapToCylinder>::New();
	//vtkSmartPointer<vtkTextureMapToPlane> tmapper = vtkSmartPointer<vtkTextureMapToPlane>::New();
	//vtkSmartPointer<vtkTextureMapToSphere> tmapper = vtkSmartPointer<vtkTextureMapToSphere>::New();
	tmapper->SetInputConnection( streamTube->GetOutputPort() );

	vtkSmartPointer<vtkTransformTextureCoords> xform = vtkSmartPointer<vtkTransformTextureCoords>::New();
	xform->SetInputConnection(tmapper->GetOutputPort());
	xform->SetScale(10, 10, 10);
	//xform->SetScale(100, 100, 100);
	//xform->FlipROn();
	xform->SetOrigin(0,0,0);


	vtkSmartPointer<vtkTexture> atex = vtkSmartPointer<vtkTexture>::New();
	vtkSmartPointer<vtkPNGReader> png = vtkSmartPointer<vtkPNGReader>::New();
	png->SetFileName("/home/chap/session2.png");
	//png->SetFileName("/home/chap/wait.png");

	/*
	vtkSmartPointer<vtkPolyDataReader> png = vtkSmartPointer<vtkPolyDataReader>::New();
	png->SetFileName("/home/chap/hello.vtk");
	*/


	atex->SetInputConnection( png->GetOutputPort() );
	atex->RepeatOn();
	atex->InterpolateOn();

	vtkSmartPointer<vtkPolyDataMapper> mapStreamTube = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapStreamTube->SetInputConnection(xform->GetOutputPort());
	//mapStreamTube->SetInputConnection(streamTube->GetOutputPort());

	vtkSmartPointer<vtkActor> streamTubeActor = vtkSmartPointer<vtkActor>::New();
	streamTubeActor->SetMapper(mapStreamTube);

	vtkSmartPointer<vtkProperty> tubeProperty = vtkSmartPointer<vtkProperty>::New();
	//tubeProperty->SetRepresentationToWireframe();
	//tubeProperty->SetColor(0.5,0.5,0);
	//tubeProperty->SetLineWidth(2.0);
	streamTubeActor->SetProperty( tubeProperty );
	streamTubeActor->SetTexture( atex );
  
	m_render->AddActor( streamTubeActor );
}

void CLegiMainWindow::__iso_surface(bool outline)
{
	vtkSmartPointer<vtkTubeFilter> streamTube = vtkSmartPointer<vtkTubeFilter>::New();
	streamTube->SetInput( m_pstlineModel->GetOutput() );
	streamTube->SetRadius(0.25);
	streamTube->SetNumberOfSides(12);

	vtkSmartPointer<vtkContourFilter> skinExtractor =
		vtkSmartPointer<vtkContourFilter>::New();
	skinExtractor->SetInputConnection(streamTube->GetOutputPort());
	//skinExtractor->SetInputConnection( m_pImgRender->dicomReader->GetOutputPort() );
	//skinExtractor->SetInputConnection( m_pImgRender->niftiImg->GetOutputPort() );
	skinExtractor->UseScalarTreeOn();
	skinExtractor->ComputeGradientsOn();
	skinExtractor->SetValue(0, -.01);
	//skinExtractor->SetValue(0, 500);
	//skinExtractor->GenerateValues(10, 0, 1000);
	skinExtractor->ComputeNormalsOn();

	vtkSmartPointer<vtkPolyDataNormals> skinNormals =
		vtkSmartPointer<vtkPolyDataNormals>::New();
	skinNormals->SetInputConnection(skinExtractor->GetOutputPort());
	skinNormals->SetFeatureAngle(60.0);

	vtkSmartPointer<vtkPolyDataMapper> skinMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	skinMapper->SetInputConnection(skinNormals->GetOutputPort());
	//skinMapper->SetInputConnection(skinExtractor->GetOutputPort());
	skinMapper->ScalarVisibilityOff();

	vtkSmartPointer<vtkActor> skin =
		vtkSmartPointer<vtkActor>::New();
	skin->SetMapper(skinMapper);

	vtkSmartPointer<vtkCamera> aCamera =
		vtkSmartPointer<vtkCamera>::New();
	aCamera->SetViewUp (0, 0, -1);
	aCamera->SetPosition (0, 1, 0);
	aCamera->SetFocalPoint (0, 0, 0);
	aCamera->ComputeViewPlaneNormal();
	aCamera->Azimuth(30.0);
	aCamera->Elevation(30.0);

	if ( outline ) {
		vtkSmartPointer<vtkOutlineFilter> outlineData =
			vtkSmartPointer<vtkOutlineFilter>::New();
		outlineData->SetInputConnection(streamTube->GetOutputPort());

		vtkSmartPointer<vtkPolyDataMapper> mapOutline =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		mapOutline->SetInputConnection(outlineData->GetOutputPort());

		vtkSmartPointer<vtkActor> outline =
			vtkSmartPointer<vtkActor>::New();
		outline->SetMapper(mapOutline);
		outline->GetProperty()->SetColor(0,0,0);


		m_render->AddActor(outline);
	}

	m_render->AddActor(skin);
	/*
	m_render->SetActiveCamera(aCamera);
	m_render->ResetCamera ();
	aCamera->Dolly(1.5);
	*/

	renderView->update();
}

void CLegiMainWindow::__load_lab()
{
	m_labColors = vtkSmartPointer<vtkPoints>::New();

	ifstream ifs ("lab.txt");
	if (!ifs.is_open()) {
		cerr << "can not load LAB color values from lab.txt, ignored.\n";
	}

	double labValue[3];
	int ilabTotal = 0;
	while (ifs) {
		ifs >> labValue[0] >> labValue[1] >> labValue[2];
		m_labColors->InsertNextPoint( labValue );
		ilabTotal ++;
	}
	cout << m_labColors->GetNumberOfPoints() << " LAB color loaded.\n";
}

void CLegiMainWindow::__add_axes()
{
	vtkSmartPointer<vtkAnnotatedCubeActor> cube = vtkSmartPointer<vtkAnnotatedCubeActor>::New();
	cube->SetXPlusFaceText ( "A" );
	cube->SetXMinusFaceText( "P" );
	cube->SetYPlusFaceText ( "L" );
	cube->SetYMinusFaceText( "R" );
	cube->SetZPlusFaceText ( "S" );
	cube->SetZMinusFaceText( "I" );
	cube->SetFaceTextScale( 0.65 );

	vtkProperty* property = cube->GetCubeProperty();
	property->SetColor( 0.5, 1, 1 );

	property = cube->GetTextEdgesProperty();
	property->SetLineWidth( 1 );
	property->SetDiffuse( 0 );
	property->SetAmbient( 1 );
	property->SetColor( 0.1800, 0.2800, 0.2300 );

	vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();

	property = cube->GetXPlusFaceProperty();
	property->SetColor(0, 0, 1);
	property->SetInterpolationToFlat();
	property = cube->GetXMinusFaceProperty();
	property->SetColor(0, 0, 1);
	property->SetInterpolationToFlat();
	property = cube->GetYPlusFaceProperty();
	property->SetColor(0, 1, 0);
	property->SetInterpolationToFlat();
	property = cube->GetYMinusFaceProperty();
	property->SetColor(0, 1, 0);
	property->SetInterpolationToFlat();
	property = cube->GetZPlusFaceProperty();
	property->SetColor(1, 0, 0);
	property->SetInterpolationToFlat();
	property = cube->GetZMinusFaceProperty();
	property->SetColor(1, 0, 0);
	property->SetInterpolationToFlat();

	vtkSmartPointer<vtkAxesActor> axes2 = vtkSmartPointer<vtkAxesActor>::New();
	axes2->SetShaftTypeToCylinder();
	axes2->SetXAxisLabelText( "x" );
	axes2->SetYAxisLabelText( "y" );
	axes2->SetZAxisLabelText( "z" );

	axes2->SetTotalLength( 3.5, 3.5, 3.5 );
	axes2->SetCylinderRadius( 0.500 * axes2->GetCylinderRadius() );
	axes2->SetConeRadius    ( 1.025 * axes2->GetConeRadius() );
	axes2->SetSphereRadius  ( 1.500 * axes2->GetSphereRadius() );

	vtkTextProperty* tprop = axes2->GetXAxisCaptionActor2D()-> GetCaptionTextProperty();
	tprop->ItalicOn();
	tprop->ShadowOn();
	tprop->SetFontFamilyToTimes();

	axes2->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->ShallowCopy( tprop );
	axes2->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->ShallowCopy( tprop );

	vtkSmartPointer<vtkPropAssembly> assembly = vtkSmartPointer<vtkPropAssembly>::New();
	assembly->AddPart( axes2 );
	assembly->AddPart( cube );

	// add orientation widget
	m_orientationWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	m_orientationWidget->SetOutlineColor( 0.9300, 0.5700, 0.1300 );
	m_orientationWidget->SetOrientationMarker( assembly );

	vtkRenderWindowInteractor* iren = renderView->GetRenderWindow()->GetInteractor();
	//iren->PrintSelf( cout, vtkIndent() );
	m_orientationWidget->SetInteractor( iren );

	m_orientationWidget->SetViewport( -0.1, -0.1, 0.3, 0.3 );
	m_orientationWidget->EnabledOn();
	m_orientationWidget->InteractiveOff();

	// add vtkButtons
	/*
	vtkSmartPointer<vtkRectangularButtonSource> btn = vtkSmartPointer<vtkRectangularButtonSource>::New();
	btn->SetWidth( 20 );
	btn->SetHeight( 10 );

	vtkSmartPointer<vtkPolyDataMapper> bmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	bmapper->SetInputConnection( btn->GetOutputPort() );

	vtkSmartPointer<vtkActor> bactor = vtkSmartPointer<vtkActor>::New();
	bactor->SetMapper( bmapper );

	m_render->AddActor( bactor );
	*/
}

int CLegiMainWindow::__loadBoxpos()
{
	int nBoxes = (int)m_strfnBoxes.size();
	int nLineInbox = 0;

	string discard;
	double* faValues = new double[nBoxes];
	double bounds[6];
	//cout << "FAInfo: ";
	for (int i = 0; i < nBoxes; i++) {
		ifstream ifs(m_strfnBoxes[i].c_str());
		if ( !ifs.is_open() ) {
			return -1;
		}

		// we only care about the 3 lines of the file
		// 1st line: the task key, but not used in this version, so discarded
		if ( !ifs ) return -1;
		ifs >> discard;

		// 2nd line: the coordinate of the minor corner of the tumor bounding box
		if ( !ifs ) return -1;
		ifs >> fixed >> setprecision(6);
		ifs >> bounds[0] >> bounds[2] >> bounds[4];

		// 3rd line: the coordinate of the major corner of the tumor bounding box
		if ( !ifs ) return -1;
		ifs >> bounds[1] >> bounds[3] >> bounds[5];

		ifs.close();

		vtkCubeSource* box = vtkCubeSource::New();
		box->SetBounds( bounds );

		vtkSmartPointer<vtkTransformPolyDataFilter> tfilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

		double extent[6];
		m_pstlineModel->GetOutput()->GetBounds(extent);
		/*
		for (int i = 0; i < 6; i++) {
			cout << extent[i] << " ";
		}
		cout << endl;
		*/
		transform->Translate( (extent[0] + extent[1])/2.0, (extent[2] + extent[3])/2.0, (extent[4]+extent[5])/2.0 );

		//tfilter->SetInputConnection( box->GetOutputPort() );
		tfilter->SetInput( box->GetOutput() );
		tfilter->SetTransform( transform );
		tfilter->Update();


		//vtkSmartPointer<vtkPolyDataMapper> boxmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		vtkSmartPointer<vtkLegiPolyDataMapper> boxmapper = vtkSmartPointer<vtkLegiPolyDataMapper>::New();
		//boxmapper->SetInput( box->GetOutput() );
		boxmapper->SetInputConnection( tfilter->GetOutputPort() );

		//vtkSmartPointer<vtkProperty> boxproperty = vtkSmartPointer<vtkProperty>::New();
		vtkSmartPointer<vtkLegiProperty> boxproperty = vtkSmartPointer<vtkLegiProperty>::New();
		boxproperty->SetRepresentationToSurface();
		boxproperty->EdgeVisibilityOn();
		boxproperty->SetColor(1,1,1);
		boxproperty->SetOpacity(0.6);
		boxproperty->SetEdgeColor(1,1,1);
		boxproperty->LightingOff();
		boxproperty->ShadingOff();
		boxproperty->FrontfaceCullingOn();

		vtkSmartPointer<vtkActor> boxactor = vtkSmartPointer<vtkActor>::New();
		boxactor->SetMapper( boxmapper );
		boxactor->SetProperty( boxproperty );

		m_render->AddActor( boxactor );

		vtkSmartPointer<vtkProperty> boxproperty2 = vtkSmartPointer<vtkProperty>::New();
		boxproperty2->SetRepresentationToWireframe();
		boxproperty2->SetColor(1,1,0);
		boxproperty2->LightingOff();
		boxproperty2->ShadingOff();
		boxproperty2->BackfaceCullingOn();
		boxproperty2->SetLineWidth(2);

		/*
		vtkCubeSource* box2 = vtkCubeSource::New();
		box2->SetBounds( bounds );

		vtkSmartPointer<vtkTransformPolyDataFilter> tfilter2 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		vtkSmartPointer<vtkTransform> transform2 = vtkSmartPointer<vtkTransform>::New();

		transform2->Translate( (extent[0] + extent[1])/2.0, (extent[2] + extent[3])/2.0, (extent[4]+extent[5])/2.0 );

		tfilter2->SetInputConnection( box2->GetOutputPort() );
		tfilter2->SetTransform( transform2 );
		tfilter2->Update();
		*/

		vtkSmartPointer<vtkPolyDataMapper> boxmapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
		boxmapper2->SetInputConnection( tfilter->GetOutputPort() );

		vtkSmartPointer<vtkActor> boxactor2 = vtkSmartPointer<vtkActor>::New();
		boxactor2->SetMapper( boxmapper2 );
		boxactor2->SetProperty ( boxproperty2 );

		m_render->AddActor( boxactor2 );

		m_boxes.push_back ( box );

		// add box identifier
		vtkSmartPointer<vtkCaptionActor2D> tag = vtkSmartPointer<vtkCaptionActor2D>::New();
		vtkSmartPointer<vtkTextProperty> tagproperty = vtkSmartPointer<vtkTextProperty>::New();
		tagproperty->SetFontFamilyToCourier();
		tagproperty->SetColor( 0,1,1 );
		tagproperty->SetFontSize( 5 );
		tagproperty->BoldOn();

		ostringstream ostr;
		ostr << (1+i) << ends;
		tag->SetCaption( ostr.str().c_str() );
		tag->SetCaptionTextProperty( tagproperty );

		tag->LeaderOff();
		tag->BorderOff();
		tag->AttachEdgeOnlyOn();

		tfilter->GetOutput()->GetBounds(extent);

		tag->SetAttachmentPoint( extent[0], extent[2], extent[4] );

		m_render->AddActor2D( tag );
		//_calBlockAvgFA( extent, nLineInbox );

		faValues[i] = _calBlockAvgFA(bounds, nLineInbox);
		//cout << faValues[i] << " " << nLineInbox << " ";
	}

	m_nKey = (faValues[0] > faValues[1])?2:3;

	double faDiff = fabs(faValues[0] - faValues[1]);
	if (faDiff < 1e-2 ) {
		m_nKey = 1;
	}

	m_optionPanel->setKey( m_nKey );
	//cout << faDiff << "\n";
	//exit(0);

	//cout << "max FA: " << maxFA << " key box: " << m_nKey << "\n";
	delete [] faValues;

	return 0;
}

double CLegiMainWindow::_calBlockAvgFA(double* boxExtent, int & nLineInbox)
{
	//vtkPolyData* tubes = vtkPolyDataMapper::SafeDownCast(m_actor->GetMapper())->GetInput();
	vtkPolyData* tubes = m_pstlineModel->GetOutput();
	vtkCellArray* alllines = tubes->GetLines();
	vtkIdType szTotal = alllines->GetNumberOfCells();
	vtkDataArray* CL = tubes->GetPointData()->GetArray("Linear Anisotropy");
	if ( !CL ) {
		cerr << "Fatal: in _calBlockAvgFA, FA value array not found in polyData of the geometry.\n";
		return .0;
	}
	/*
	cout << "size of lookuptable: "  <<
		vtkLookupTable::SafeDownCast( vtkPolyDataMapper::SafeDownCast(m_actor->GetMapper())->GetLookupTable() )->GetNumberOfTableValues() << "\n";
	cout << "#tubes: " << szTotal << "\n";
	*/
		
	double extent[6];
	double faTotal = .0, ptTotal = 0.0;
	double point[3];
	vtkIdType* line;
	vtkIdType lineCnt = 0, szPts;

	tubes->GetBounds(extent);

	nLineInbox = 0;
	/* if this was not invoked first, GetNextCell, GetCell will both refuse to work */
	alllines->InitTraversal();
	while ( alllines->GetNextCell(szPts, line) ) {
		double npt = 0;
		for (vtkIdType idx = 0; idx < szPts; idx++) {
			tubes->GetPoint( line[idx], point );
			point[0] -= (extent[0] + extent[1])/2.0;
			point[1] -= (extent[2] + extent[3])/2.0;
			point[2] -= (extent[4]+extent[5])/2.0;

			// box inclusion judgement
			if ( point[0] >= boxExtent[0] && point[0] <= boxExtent[1] &&
				 point[1] >= boxExtent[2] && point[1] <= boxExtent[3] &&
				 point[2] >= boxExtent[4] && point[2] <= boxExtent[5] ) {

				faTotal += CL->GetTuple1( line[idx] );
				npt ++;
			}
		}
		ptTotal += npt;
		if ( npt > 0 ) {
			nLineInbox ++;
		}

		lineCnt ++;
	}

	if ( lineCnt != szTotal ) {
		cerr << "Internal data error in _calBlockAvgFA #cell=" << lineCnt << ", but expected num=" << szTotal << "\n";
	}

	if ( ptTotal < 1.0 ) return -1.0;
	return faTotal / ptTotal;
}

void CLegiMainWindow::__drawCanvas(bool bGrid)
{
	double extent[6];
	m_pstlineModel->GetOutput()->GetBounds(extent);
		
	vtkSmartPointer<vtkRenderer> canvasRenderer = vtkSmartPointer<vtkRenderer>::New();
	//canvasRenderer->SetViewport( 0.0,0.0,1.0,1.0 );
	//canvasRenderer->SetViewPoint(0.0, 0.0, 200 );
	m_render->SetLayer(1);
	canvasRenderer->SetLayer( 0 );
	canvasRenderer->InteractiveOff();
	canvasRenderer->SetBackground(  0.4392, 0.5020, 0.5647 );

	vtkSmartPointer<vtkLight> canvasLight = vtkSmartPointer<vtkLight>::New();
	canvasLight->SetLightTypeToCameraLight();
	canvasLight->SetPosition(0, extent[3], 0);
	canvasRenderer->AddLight( canvasLight );

	extent[1] *= 1;
	GLdouble fscale = 1.5;
	GLdouble h = extent[5];
	GLdouble w = extent[1];
	GLdouble valueY = 60 - (extent[2]+extent[3])/2.0, valueY2 = valueY+.5;
	valueY = -1, valueY2 = -.5;

	vtkSmartPointer<vtkPoints> basePts = vtkSmartPointer<vtkPoints>::New();
	basePts->InsertNextPoint( -w * fscale, valueY, -h*8 * fscale);
	basePts->InsertNextPoint( -w * fscale, valueY, h * fscale);
	basePts->InsertNextPoint( w * fscale, valueY, h * fscale);
	basePts->InsertNextPoint( w * fscale, valueY, -h*8 * fscale);

	vtkSmartPointer<vtkCellArray> baseLines = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkCellArray> basePolys= vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkPolygon> outline = vtkSmartPointer<vtkPolygon>::New();
	outline->GetPointIds()->SetNumberOfIds(4);
	vtkIdType id;
	for (id = 0; id < 4; id++) {
		outline->GetPointIds()->SetId( id, id );
	}
	basePolys->InsertNextCell( outline );

	vtkSmartPointer<vtkLine> vtkln = vtkSmartPointer<vtkLine>::New();
	id = 0;

	if ( bGrid ) {
		double viewmat[4] = {0,0, extent[1]*2, extent[5]*2};
		/*
		cout << viewmat[0] << "," << viewmat[1] << "," << viewmat[2] << "," << viewmat[3] << "\n";
		canvasRenderer->GetViewport( viewmat );
		viewmat[2] = viewmat[2] - viewmat[0];
		viewmat[3] = viewmat[3] - viewmat[1];
		*/
		GLfloat gridCellW = viewmat[2]*1.0/30, gridCellH = gridCellW * 8 * viewmat[3]/viewmat[2];

		// vertical grid lines
		for (int i = 1; i <= 2*w*fscale / gridCellW; i++) {
			basePts->InsertNextPoint(-w*fscale + i*gridCellW, valueY2, h*fscale);
			basePts->InsertNextPoint(-w*fscale + i*gridCellW, valueY2, -h*8*fscale);
			vtkln->GetPointIds()->SetNumberOfIds(2);
			vtkln->GetPointIds()->SetId(0, id++);
			vtkln->GetPointIds()->SetId(1, id++);
			baseLines->InsertNextCell( vtkln );
		}

		// horizontal grid lines
		for (int i = 1; i <= h*9*fscale / gridCellH; i++) {
			basePts->InsertNextPoint(-w*fscale, valueY2, -h*8*fscale + i*gridCellH);
			basePts->InsertNextPoint(w*fscale, valueY2, -h*8*fscale + i*gridCellH);
			vtkln->GetPointIds()->SetNumberOfIds(2);
			vtkln->GetPointIds()->SetId(0, id++);
			vtkln->GetPointIds()->SetId(1, id++);
			baseLines->InsertNextCell( vtkln );
		}
	}

	vtkSmartPointer<vtkPolyData> polycanvas = vtkSmartPointer<vtkPolyData>::New();
	polycanvas->SetPoints( basePts );
	polycanvas->SetLines ( baseLines );
	polycanvas->SetPolys( basePolys );

	/*
	polycanvas->SetWholeExtent(-extent[1], extent[1], -extent[3]*10, extent[3]*10, -extent[5], extent[5]);
	polycanvas->SetWholeBoundingBox(-extent[1], extent[1], -extent[3]*10, extent[3]*10, -extent[5], extent[5]);
	*/

	vtkSmartPointer<vtkTransformPolyDataFilter> tfilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->Translate(0,  60 -(extent[2]+extent[3])/2.0, - (extent[4]+extent[5])/2.0);
	tfilter->SetInput ( polycanvas );
	tfilter->SetTransform( transform );
	tfilter->Update();

	/*
	vtkCanvasWidget* pw = vtkCanvasWidget::New();
	//pw->SetInput( polycanvas );
	pw->SetInput( tfilter->GetOutput() );
	pw->PlaceWidget( extent );
	vtkRenderWindowInteractor* iren = renderView->GetRenderWindow()->GetInteractor();
	pw->SetInteractor( iren );
	//pw->RemoveAllObservers();
	pw->EnabledOn();
	return;
	*/


	//vtkSmartPointer<vtkPolyDataMapper2D> mapCanvas = vtkSmartPointer<vtkPolyDataMapper2D>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapCanvas = vtkSmartPointer<vtkPolyDataMapper>::New();
	//mapCanvas->SetInput ( polycanvas );
	mapCanvas->SetInputConnection ( tfilter->GetOutputPort() );
	/*
	vtkSmartPointer<vtkCoordinate> coord = vtkSmartPointer<vtkCoordinate>::New();
	coord->SetViewport( m_render );
	coord->SetCoordinateSystemToViewport();
	mapCanvas->SetTransformCoordinate( coord );
	*/

	//vtkSmartPointer<vtkActor2D> actorCanvas = vtkSmartPointer<vtkActor2D>::New();
	vtkSmartPointer<vtkActor> actorCanvas = vtkSmartPointer<vtkActor>::New();
	actorCanvas->SetMapper( mapCanvas );

	//vtkSmartPointer<vtkProperty2D> canvasProp = vtkSmartPointer<vtkProperty2D>::New();
	vtkSmartPointer<vtkProperty> canvasProp = vtkSmartPointer<vtkProperty>::New();
	if ( bGrid ) {
		canvasProp->EdgeVisibilityOn();
		canvasProp->SetEdgeColor( 0.2, 0.2, 0.2 );
	}
	canvasProp->SetColor( 80.0/255, 80.0/255, 80.0/255 );
	canvasProp->SetRepresentationToSurface();

	actorCanvas->SetProperty( canvasProp );

	/*
	vtkSmartPointer<vtkMatrix4x4> rot = vtkSmartPointer<vtkMatrix4x4>::New();
	rot->Identity();
	actorCanvas->PokeMatrix( rot );
	actorCanvas->SetUserMatrix( rot );

	vtkSmartPointer<vtkViewKeyEventCallback> nil = vtkSmartPointer<vtkViewKeyEventCallback>::New();

	actorCanvas->AddObserver(vtkCommand::MouseMoveEvent, nil);
	m_render->AddActor2D( actorCanvas );
	*/

	//actorCanvas->SetOrigin( m_actor->GetOrigin() );
	vtkSmartPointer<vtkCamera> canvasCamera = canvasRenderer->MakeCamera();

	//canvasCamera->Zoom( 20.0 );
	//canvasCamera->SetFocalPoint(0,0,(extent[4]+extent[5])/2.0);

	canvasRenderer->SetActiveCamera( canvasCamera );
	//actorCanvas->SetScale(2);
	//canvasRenderer->AddViewProp( actorCanvas );
	actorCanvas->RotateX(1);
	canvasRenderer->AddActor( actorCanvas );

	/* the following camera resetting was the bane that caused the failure of 
	 * displaying the canvas before the first mouse rotation operation */
	//canvasRenderer->ResetCamera(); 

	canvasCamera->SetFocalPoint(m_camera->GetFocalPoint());
	canvasCamera->SetPosition(0,0,(extent[4]+extent[5])/2.0*3.0);
	canvasCamera->SetViewUp( 0,1,0 );
	//canvasCamera->Zoom( 2.0 );

	renderView->GetRenderWindow()->AddRenderer( canvasRenderer );
}

void CLegiMainWindow::__projectSkeletons()
{
}

/* sts=8 ts=8 sw=80 tw=8 */

