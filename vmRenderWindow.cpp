// ----------------------------------------------------------------------------
// vmRenderWindow.cpp : volume rendering Window with Qt Mainwindow
//
// Creation : Nov. 12th 2011
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#include "vmRenderWindow.h"
#include <vtkXMLMaterial.h>
#include <vtkXMLDataParser.h>
#include <vtkLegiInteractorStyle.h>

#include "vtkCubeSource.h"
#include "vtkWindowToImageFilter.h"
#include "vtkPNGReader.h"
#include "vtkPNGWriter.h"
#include "vtkImageMapper.h"
#include "vtkImageActor.h"
#include "vtksys/SystemTools.hxx"

#include "vtkTriangleFilter.h"
#include "vtkStripper.h"

#include <sstream>

using namespace std;

#define TUBE_LOD 5

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
CLegiMainWindow::CLegiMainWindow(QWidget* parent, Qt::WindowFlags f)
	: QMainWindow(parent, f),
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
	m_bCurveMapping(false)
{
	setWindowTitle( "GUItestbed for LegiDTI v1.0" );

	setupUi( this );
	statusbar->setStatusTip ( "ready to load" );

	__init_volrender_methods();
	__init_tf_presets();
	/*
	vtkRenderWindow* renwin = vtkRenderWindow::New();
	renderView->SetRenderWindow( renwin );
	renwin->Delete();
	*/
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

	/*
	vtkViewKeyEventCallback * callback = vtkViewKeyEventCallback::New();
	m_render->AddObserver( vtkCommand::KeyPressEvent, callback );
	callback->Delete();
	*/

	//vtkSmartPointer< vtkInteractorStyleTrackballCamera > style = vtkSmartPointer< vtkInteractorStyleTrackballCamera >::New();
	//vtkSmartPointer< vtkLegiInteractorStyle > style = vtkSmartPointer< vtkLegiInteractorStyle >::New();
	vtkLegiInteractorStyle *style = vtkLegiInteractorStyle::New();
	m_dsort = vtkDepthSortPoints::New();
	style->agent = m_dsort;

	m_linesort = vtkDepthSortPoints::New();
	style->agent2 = m_linesort;

	m_streamtube = vtkTubeFilter::New();
	style->streamtube = m_streamtube;

	m_actor = vtkSmartPointer<vtkActor>::New();
	style->actor = m_actor;

	renderView->GetInteractor()->SetInteractorStyle( style );

	qvtkConnections = vtkEventQtSlotConnect::New();
	qvtkConnections->Connect( renderView->GetRenderWindow()->GetInteractor(),
							vtkCommand::KeyPressEvent, 
							this,
							SLOT ( onKeys(vtkObject*,unsigned long,void*,void*,vtkCommand*) ) );

	// for quick test/debug

	m_fnGeometry = "/home/chap/test.data";
	if ( !m_pstlineModel->Load( m_fnGeometry.c_str() ) ) {
		QMessageBox::critical(this, "Error...", "failed to load geometry data provided.");
		m_fnGeometry = "";
	}

	m_boxWidget = vtkBoxWidget::New();

	m_boxWidget->SetInteractor(renderView->GetInteractor());
	m_boxWidget->SetPlaceFactor(1.0);
	m_boxWidget->InsideOutOn();


	m_haloactor = vtkSmartPointer<vtkActor>::New();
	m_colorbar = vtkSmartPointer<vtkScalarBarActor>::New();

	//m_camera = vtkSmartPointer<vtkCamera>::New();
	m_camera = m_render->MakeCamera();
	//m_pImgRender->LoadPresets("presets.xml");
	
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

}

CLegiMainWindow::~CLegiMainWindow()
{
	delete m_pImgRender;
	delete m_pstlineModel;
	if ( m_boxWidget ) {
		m_boxWidget->Delete();
		m_boxWidget = NULL;
	}

	m_dsort->Delete();
	m_linesort->Delete();
	m_streamtube->Delete();
	qvtkConnections->Delete();
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
	/*
	LoadImages( "Screenshot_d", 5 );
	m_render->ResetCamera();
	m_camera->Zoom (2.0);
	return;
	*/
	this->draw();

	// capture views around the x-axis
	/*
	m_bDepthColorLAB = true;
	onButtonApply();
	*/
}

void CLegiMainWindow::LoadImages(const char* prefix, int range)
{
	vtkSmartPointer<vtkPNGReader> reader = vtkSmartPointer<vtkPNGReader>::New();

	//vtkSmartPointer<vtkImageMapper> imap = vtkSmartPointer<vtkImageMapper>::New();

	vtkSmartPointer<vtkImageActor> imgActor = vtkSmartPointer<vtkImageActor>::New();
	//imgActor->SetMapper( imap );

	m_render->SetActiveCamera( m_camera );
	m_render->AddActor( imgActor );
	//m_render->ResetCamera();
	//m_camera->Zoom (2.0);

	if ( ! renderView->GetRenderWindow()->HasRenderer( m_render ) )
		renderView->GetRenderWindow()->AddRenderer(m_render);

	for (int i=1;i<=range;i++) {
		ostringstream ostrName;
		ostrName << prefix << i << ".png" << ends;
		if ( reader->CanReadFile( ostrName.str().c_str() ) == 0 ) {
			cerr << "Failed to load image in file: " << ostrName.str() << "\n";
			continue;
		}
		reader->SetFileName(ostrName.str().c_str());

		//imap->SetInput( reader->GetOutput() );
		imgActor->SetInput( reader->GetOutput() );
		vtksys::SystemTools::Delay(1000);
	}

	renderView->update();
}

void CLegiMainWindow::selfRotate(int range)
{
	vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
	windowToImageFilter->SetInput(renderView->GetRenderWindow());
	//windowToImageFilter->SetMagnification(3); //set the resolution of the output image (3 times the current resolution of vtk render window)
	windowToImageFilter->SetInputBufferTypeToRGBA(); //also record the alpha (transparency) channel
	windowToImageFilter->FixBoundaryOn();
	windowToImageFilter->Update();
	// Screenshot  
	windowToImageFilter->ShouldRerenderOff();

	vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();

	for (int i=1;i<=range;i++) {
		ostringstream ostrName;
		ostrName << "Screenshot_d" << i << ".png" << ends;
		writer->SetFileName(ostrName.str().c_str());
		writer->SetInput(windowToImageFilter->GetOutput());
		writer->Write();

		m_camera->Azimuth(360/range);
	}
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

	if ( m_curFocus != FOCUS_GEO ) {
		__removeAllVolumes();
		if ( m_boxWidget ) {
			m_boxWidget->SetEnabled( false );
		}
		m_bInit = false;
		m_nHaloType = 0;
		__renderTubes(m_nHaloType);
	}

	//__removeAllActors();

	//if ( !m_bDepthHalo && (m_bDepthColor || m_bDepthTransparency || m_bDepthSize) ) {
	if ( m_bDepthHalo || m_bDepthColorLAB || m_bDepthValue || m_bDepthTransparency || m_bDepthSize ) {
		__uniformTubeRendering();
	}
	else if (m_bHatching) {
		__add_texture_strokes();
	}
	else {
		__renderTubes(m_nHaloType);
	}

	/*
	cout << "Numer of Actor in the render: " << (m_render->VisibleActorCount()) << "\n";

	renderView->GetRenderWindow()->SetAlphaBitPlanes(1);
	renderView->GetRenderWindow()->SetMultiSamples(0);
	m_render->SetUseDepthPeeling(1);
	m_render->SetMaximumNumberOfPeels(100);
	m_render->SetOcclusionRatio(0.5);
	cout << "depth peeling flag: " << m_render->GetLastRenderingUsedDepthPeeling() << "\n";
	*/

	if ( ! renderView->GetRenderWindow()->HasRenderer( m_render ) )
		renderView->GetRenderWindow()->AddRenderer(m_render);

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
	__uniformTubeRendering();
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

				/*
				static int i = 0;
				if ( i == 0 ) {
					addBoxWidget();
				}
				if (i++ % 2 == 0) {
					m_boxWidget->EnabledOn();
				}
				else {
					m_boxWidget->EnabledOff();
				}
				*/
			}
			return;
		case 'c':
			m_bCapping = !m_bCapping;
			//m_actor->GetMapper()->GetInput()->Update();
			break;
		case 'v':
			m_bCurveMapping = !m_bCurveMapping;
			break;
		case 'y':
			m_actor->GetMapper()->GetInput()->Modified();
			break;
		case 'x': 
			{
				vtkSmartPointer<vtkCubeSource> cube = vtkSmartPointer<vtkCubeSource>::New();
				cube->SetBounds(0,100,0,100,0,100);
				m_actor->GetMapper()->SetInputConnection( cube->GetOutputPort() );
				return;
			}
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
	/*
	if ( m_bDepthHalo ) {
		__uniform_halos();
	}
	*/
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
	streamTube->SetNumberOfSides(TUBE_LOD);

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
	streamTube->SetNumberOfSides(TUBE_LOD);
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
	streamTube->SetNumberOfSides(TUBE_LOD);

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
	streamTube->SetNumberOfSides(TUBE_LOD);

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
	streamTube->SetNumberOfSides(TUBE_LOD);
	streamTube->SidesShareVerticesOn();
	streamTube->SetCapping( m_bCapping );

	vtkSmartPointer<vtkPolyDataMapper> mapStreamTube = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapStreamTube->ImmediateModeRenderingOn();
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
		m_render->ResetCamera();
		m_camera->Zoom (2.0);
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
	tubeHalos->SetNumberOfSides(TUBE_LOD);
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
	//vtkSmartPointer<vtkDepthSortPoints> linesort = vtkSmartPointer<vtkDepthSortPoints>::New();
	vtkDepthSortPoints* linesort = m_linesort;

	//vtkSmartPointer<vtkDepthSortPoints> dsort = vtkSmartPointer<vtkDepthSortPoints>::New();
	vtkDepthSortPoints* dsort = m_dsort;

	//vtkSmartPointer<vtkTubeFilterEx> streamTube = vtkSmartPointer<vtkTubeFilterEx>::New();
	//vtkSmartPointer<vtkTubeFilter> streamTube = vtkSmartPointer<vtkTubeFilter>::New();
	vtkTubeFilter *streamTube = m_streamtube;

	if ( m_bDepthSize ) {
		linesort->SetInput( m_pstlineModel->GetOutput() );
		linesort->SetDirectionToBackToFront();
		linesort->SetVector(0,0,1);
		linesort->SetCamera( m_camera );
		linesort->SortScalarsOn();
		linesort->Update();
		linesort->GetOutput()->ReleaseDataFlagOn();

		//streamTube->SetInputConnection( linesort->GetOutputPort() );
		streamTube->SetInput( m_pstlineModel->GetOutput() );
		/*
		streamTube->SetRadius(0.03);
		streamTube->SetRadiusFactor( 20 );
		*/
		streamTube->SetRadius(m_dptsize.size);
		streamTube->SetRadiusFactor(m_dptsize.scale);
		streamTube->SetVaryRadiusToVaryRadiusByScalar();
		streamTube->SidesShareVerticesOn();
	}
	else {
		streamTube->SetInput( m_pstlineModel->GetOutput() );
		streamTube->SetRadius(0.25);
		streamTube->SetVaryRadiusToVaryRadiusOff();
		//streamTube->SetRadius(m_dptsize.size);
	}

	streamTube->SetNumberOfSides(TUBE_LOD);
	streamTube->SetCapping( m_bCapping );
	streamTube->Update();

	/*
	m_pstlineModel->GetOutput()->ReleaseDataFlagOn();

	vtkSmartPointer<vtkTriangleFilter> tris = vtkSmartPointer<vtkTriangleFilter>::New();
	tris->SetInput( streamTube->GetOutput() );
	//tris->GetOutput()->ReleaseDataFlagOn();

	vtkSmartPointer<vtkStripper> strip = vtkSmartPointer<vtkStripper>::New();
	strip->SetInput( tris->GetOutput() );
	//strip->GetOutput()->ReleaseDataFlagOn();
	//strip->Update();
	*/

	/*
	vtkSmartPointer<vtkPolyDataMapper> mapStreamTube = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkProperty> tubeProperty = m_actor->GetProperty();
	*/
	vtkPolyDataMapper* mapStreamTube = vtkPolyDataMapper::SafeDownCast( m_actor->GetMapper() );
	mapStreamTube->GlobalImmediateModeRenderingOn();
	mapStreamTube->ImmediateModeRenderingOn();
	vtkProperty* tubeProperty = m_actor->GetProperty();
	if ( m_bDepthSize ) {
		mapStreamTube->StaticOff();
	}
	else {
		mapStreamTube->StaticOn();
	}

	/* use shaders 
	//tubeProperty->LoadMaterial( "/home/chap/DTI-SVL/hcai/LegiDTI/pointlight.xml" );
	tubeProperty->LoadMaterial( "./pointlight.xml" );
	tubeProperty->AddShaderVariable("rate", 1.0);
	tubeProperty->ShadingOn();
	vtkXMLMaterial* xm = tubeProperty->GetMaterial();
	if ( xm ) {
		//xm->PrintSelf(cout, vtkIndent());
	}
	*/

	if ( m_bDepthTransparency || m_bDepthColorLAB || m_bDepthValue ) {
		dsort->SetInputConnection( streamTube->GetOutputPort() );
		//dsort->SetInputConnection( strip->GetOutputPort() );
		dsort->SetDirectionToBackToFront();
		//dsort->SetDirectionToFrontToBack();
		//dsort->SetDepthSortModeToParametricCenter();
		//dsort->SetDirectionToSpecifiedVector();
		dsort->SetVector(0,0,1);
		//dsort->SetOrigin(0,0,0);
		dsort->SetCamera( m_camera );
		dsort->SortScalarsOn();
		dsort->Update();
		dsort->GetOutput()->ReleaseDataFlagOn();
		/*
		streamTube->SetDirectionToBackToFront();
		streamTube->SetVector(0,0,1);
		streamTube->SetCamera( m_camera );
		streamTube->SortScalarsOn();
		*/

		//mapStreamTube->SetInputConnection(dsort->GetOutputPort());

		mapStreamTube->SetInputConnection(streamTube->GetOutputPort());
		//mapStreamTube->SetInputConnection(strip->GetOutputPort());

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

			//m_render->AddActor2D( m_colorbar );
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

		//mapStreamTube->SetScalarRange(0, dsort->GetOutput()->GetNumberOfPoints());
		mapStreamTube->ScalarVisibilityOn();
		mapStreamTube->SetScalarRange(0, streamTube->GetOutput()->GetNumberOfPoints());

		//streamTube->GetOutput()->GetCellData()->GetScalars()->PrintSelf(cout, vtkIndent());
		//mapStreamTube->UseLookupTableScalarRangeOn();
		//mapStreamTube->ScalarVisibilityOn();

		//tubeProperty->SetColor(1,1,1);
		//tubeProperty->SetRepresentationToWireframe();
		//tubeProperty->SetColor(0,0,0);
		//tubeProperty->SetLineWidth(2.0);
	}
	else {
		mapStreamTube->SetInputConnection(streamTube->GetOutputPort());
		//mapStreamTube->SetInputConnection(strip->GetOutputPort());

		//mapStreamTube->SetScalarRange(0, streamTube->GetOutput()->GetNumberOfCells());
		mapStreamTube->ScalarVisibilityOff();
		//mapStreamTube->SetScalarModeToUsePointData();
	}
	/*
	if ( m_bDepthSize ) {
		linesort->SetProp3D( m_actor );
	}
	if ( m_bDepthTransparency ) {
		dsort->SetProp3D( m_actor );
		//streamTubeActor->RotateX( -72 );
	}
	mapStreamTube->SetNumberOfPieces( 100 );
	mapStreamTube->SetPiece(1);
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
		//dsort->SetProp3D( streamTubeActor );

		//streamTubeActor->RotateX( -72 );
	}

	m_render->SetActiveCamera( camera );
	m_render->AddActor( streamTubeActor );
	m_render->ResetCamera();
}

void CLegiMainWindow::__uniform_halos()
{
	//vtkSmartPointer<vtkDepthSortPoints> linesort = vtkSmartPointer<vtkDepthSortPoints>::New();
	//vtkDepthSortPoints* linesort = m_linesort;

	//vtkSmartPointer<vtkDepthSortPoints> dsort = vtkSmartPointer<vtkDepthSortPoints>::New();
	//vtkDepthSortPoints* dsort = m_dsort;

	//vtkSmartPointer<vtkTubeFilterEx> streamTube = vtkSmartPointer<vtkTubeFilterEx>::New();
	vtkTubeFilter* streamTube = m_streamtube;

	/*
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
		streamTube->SetRadius(0.25);
	}
	*/

	streamTube->SetNumberOfSides(TUBE_LOD);
	streamTube->SetCapping( m_bCapping );
	streamTube->Update();

	vtkPolyDataMapper* mapStreamTube = m_bFirstHalo? vtkPolyDataMapper::New() :
			vtkPolyDataMapper::SafeDownCast( m_haloactor->GetMapper() );
	mapStreamTube->ImmediateModeRenderingOn();
	vtkProperty* tubeProperty = m_bFirstHalo? vtkProperty::New() : m_haloactor->GetProperty();

	tubeProperty->SetRepresentationToWireframe();
	tubeProperty->FrontfaceCullingOn();
	tubeProperty->SetColor(0,0,0);
	tubeProperty->SetLineWidth(m_nHaloWidth);

	mapStreamTube->SetInputConnection(streamTube->GetOutputPort());
	mapStreamTube->ScalarVisibilityOff();

	/*
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
		mapStreamTube->ScalarVisibilityOn();
		mapStreamTube->SetScalarRange(0, dsort->GetOutput()->GetNumberOfPoints());
	}
	else {
		mapStreamTube->SetInputConnection(streamTube->GetOutputPort());
		mapStreamTube->ScalarVisibilityOff();
	}
	*/

	if ( m_bFirstHalo ) {
		m_haloactor->SetMapper( mapStreamTube );
		mapStreamTube->Delete();

		m_haloactor->SetProperty( tubeProperty );
		tubeProperty->Delete();

		m_bFirstHalo = false;
	}

	/*
	if ( m_bDepthSize ) {
		linesort->SetProp3D( m_haloactor );
	}
	if ( m_bDepthTransparency ) {
		dsort->SetProp3D( m_haloactor );
		//streamTubeActor->RotateX( -72 );
	}
	*/

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
	streamTube->SetNumberOfSides(TUBE_LOD);
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
	streamTube->SetNumberOfSides(TUBE_LOD);

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

/* sts=8 ts=8 sw=80 tw=8 */

