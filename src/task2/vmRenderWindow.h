// ----------------------------------------------------------------------------
// vmRenderWindow.h : volume rendering Window with Qt Mainwindow
//
// Creation : Nov. 12th 2011
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#ifndef _VMRENDERWINDOW_H_
#define _VMRENDERWINDOW_H_

#include <QMainWindow>
#include <QSize>
#include <QSizePolicy>
#include <QListWidget>
#include <QFile>

#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkBoxWidget.h>
#include <vtkVolume.h>
#include <vtkVolumeCollection.h>
#include <vtkVolumeMapper.h>
#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkTubeFilter.h>
#include <vtkPolyData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkInteractorStyle.h>

#include <vtkDepthSortPolyData.h>
#include <vtkCamera.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkIndent.h>

#include <vtkPlanes.h>

#include <vtkTextureMapToCylinder.h>
#include <vtkTextureMapToSphere.h>
#include <vtkTextureMapToPlane.h>
#include <vtkPNGReader.h>
#include <vtkTexture.h>
#include <vtkTransformTextureCoords.h>
#include <vtkPolyDataReader.h>
#include <vtkRibbonFilter.h>

#include <vtkOutlineFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkContourFilter.h>

#include <QVTKInteractor.h>

#include <vtkScalarBarActor.h>
#include <vtkTextProperty.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtkAppendPolyData.h>
#include <vtkAxesActor.h>
#include <vtkPropAssembly.h>
#include <vtkPropCollection.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkCaptionActor2D.h>
#include <vtkAppendPolyData.h>
#include <vtkMath.h>
#include <vtkRectangularButtonSource.h>
#include <vtkCubeSource.h>
#include <vtkProperty2D.h>

#include <vtkDataArray.h>

#include "vtkTubeHaloMapper.h" 
#include "vtkTubeFilterEx.h"
#include "vtkDepthSortPoints.h"
#include "vtkLegiInteractionRecorder.h"
#include "ui_testbed.h"
#include "imgVolRender.h"

#include "tgdataReader.h"
#include "cppmoth.h"
#include "point.h"
#include "Gadgets.h"

#include <string>
#include <iostream>
#include <vector>
#include <set>

class imgVolRender;

// Callback for moving the planes from the box widget to the mapper
class vtkBoxWidgetCallback : public vtkCommand
{
public:
  static vtkBoxWidgetCallback *New();
  virtual void Execute(vtkObject *caller, unsigned long, void*);
  void SetMapper(vtkVolumeMapper* m);

protected:
  vtkBoxWidgetCallback();

private:
  vtkVolumeMapper *Mapper;
};

// Callback for keyPressEvent handling emitted from the vtk rendering view
class vtkViewKeyEventCallback: public vtkCommand
{
public:
  static vtkViewKeyEventCallback *New();
  virtual void Execute(vtkObject *caller, unsigned long, void*);

protected:
  vtkViewKeyEventCallback();

private:
};

typedef struct _depth_color_t {
	vector_t hue;
	vector_t satu;
	vector_t value;

	_depth_color_t() :
		hue(0.0,0.0),
		satu(0.0,0.0),
		value(0.0,1.0){
	}
} depth_color_t;

typedef struct _depth_size_t {
	double size;
	double scale;

	_depth_size_t() {
		size = 0.03;
		scale = 20;
	}
} depth_size_t;

enum {
	FOCUS_VOL = 0,
	FOCUS_MVOL,
	FOCUS_GEO,
	FOCUS_COMPOSITE
};

enum {
	SHAPE_TUBE,
	SHAPE_SUPERELLIPSOID,
	SHAPE_CONE
};

enum {
	ORT_NONE,
	ORT_SHAPE,
	ORT_COLOR,
	ORT_SAT
};

class CLegiMainWindow : public QMainWindow, public Ui::TestbedWindow, public CApplication
{
	Q_OBJECT
public:
	friend class imgVolRender;

	CLegiMainWindow(int argc, char **argv, QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~CLegiMainWindow();
	int handleOptions(int optv);
	int mainstay();

	void keyPressEvent (QKeyEvent* event);
	void resizeEvent( QResizeEvent* event );

	void draw();
	void show();

	QVTKWidget*& getRenderView();

	void addBoxWidget();

	MyCout& getcout() { return m_cout; }

protected:
	/* source file holding helper info*/
	string m_strfnhelp;

	/* text file holding a list of tasks for a single session */
	string m_strfntask;

	string m_strfnskeleton;

	/* number of selection box */
	int	m_nselbox;
	int m_lod;
	double m_fRadius;

signals:
	void close();

public slots:
	void onactionLoadVolume();
	void onactionLoadGeometry();
	void onactionLoadData();
	void onactionClose_current();

	void onactionVolumeRender();
	void onactionGeometryRender();
	void onactionMultipleVolumesRender();
	void onactionCompositeRender();
	void onactionTF_customize();
	void onactionSettings();

	void onKeys(vtkObject* obj,unsigned long,void* client_data,void*,vtkCommand* command);

	/* settings .... */
	void onVolRenderMethodChanged(int index);
	void onVolRenderPresetChanged(int index);

	void onHaloStateChanged(int state);

	void onTubeSizeStateChanged(int state);
	void onTubeSizeChanged(double d);
	void onTubeSizeScaleChanged(double d);

	void onTubeAlphaStateChanged(int state);
	void onTubeAlphaStartChanged(double d);
	void onTubeAlphaEndChanged(double d);

	void onTubeColorStateChanged(int state);
	void onTubeHueStartChanged(double d);
	void onTubeHueEndChanged(double d);
	void onTubeSatuStartChanged(double d);
	void onTubeSatuEndChanged(double d);
	void onTubeValueStartChanged(double d);
	void onTubeValueEndChanged(double d);

	void onTubeDValueStateChanged(int state);
	void onTubeDValueStartChanged(double d);
	void onTubeDValueEndChanged(double d);

	void onTubeColorLABStateChanged(int state);

	void onHaloWidthChanged(int i);

	void onHatchingStateChanged(int state);


	void onButtonApply();

	int onTaskFinished() { return 1; }
private:
	imgVolRender* m_pImgRender;
	vtkTgDataReader* m_pstlineModel;
	vtkSmartPointer<vtkRenderer> m_render;
	vtkEventQtSlotConnect*	qvtkConnections;
	vtkBoxWidget* m_boxWidget;
	vtkSmartPointer<vtkCamera> m_camera;
	vtkSmartPointer<vtkActor> m_actor;
	vtkSmartPointer<vtkActor> m_haloactor;
	vtkSmartPointer<vtkScalarBarActor> m_colorbar;
	vtkSmartPointer<vtkOrientationMarkerWidget> m_orientationWidget;
	vtkSmartPointer<vtkLegiInteractionRecorder> m_recorder;

	QSize	m_oldsz;

	std::string	m_fnData;
	std::string m_fnVolume;
	std::string m_fnGeometry;

	int		m_nHaloType;
	int		m_nHaloWidth;

	bool	m_bDepthHalo;
	bool	m_bDepthSize;
	bool	m_bDepthTransparency;
	bool	m_bDepthColor;
	bool	m_bDepthColorLAB;
	bool	m_bDepthValue;

	vector_t	m_transparency;
	depth_color_t m_dptcolor;
	depth_size_t m_dptsize;
	vector_t	m_value;

	int			m_nShapeType;
	int			m_nOrientationType;

	int	m_nCurMethodIdx;
	int m_nCurPresetIdx;

	bool	m_bLighting;
	bool	m_bCapping;
	bool	m_bHatching;

	int		m_curFocus;
	bool	m_bInit;
	bool	m_bFirstHalo;

	bool	m_bCurveMapping;
	vtkSmartPointer<vtkPoints>	m_labColors;
	vtkDepthSortPoints* m_dsort;
	vtkDepthSortPoints* m_linesort;
	vtkTubeFilter*	m_streamtube;

	vtkSmartPointer<CVTKTextbox> m_taskbox;
	vtkSmartPointer<CVTKTextbox> m_helptext;

	COptionPanel*				m_optionPanel;

	std::vector< vtkCubeSource* >	m_boxes;
	std::vector< std::string >		m_strfnBoxes;

	int		m_nKey;

	// file containing indices of expectedly selected fibers
	std::string m_strfnFiberIdx;

	// set of the loaded fiber indices
	std::set<vtkIdType> m_expectedFiberIndices;
private:
	/**
	 *@param type - indicate how to render geometries 
	 *@note 
	 *	0 - no halo
	 *	1 - consistent halo
	 *	2 - depth dependent halo
	 */
	void __renderTubes(int type=0);
	void __renderLines(int type=0);
	void __renderRibbons();
	void __depth_dependent_size();
	void __depth_dependent_transparency();
	void __depth_dependent_halos();

	void __renderDepthSortTubes();

	void __addHaloType1();

	void __uniformTubeRendering();
	void __uniform_halos();

	void __removeAllActors();
	void __removeAllVolumes();

private:
	void __init_volrender_methods();
	void __init_tf_presets();

	void __add_texture_strokes();
	void __iso_surface(bool);
	void __load_lab();
	void __add_axes();
	void __add_colorsphere(int schemeIdx);
	int __loadBoxpos();

	void __drawCanvas(bool bGrid);
	void __projectSkeletons();

	// draw markers at start points of a predefined fiber bundle
	void _drawMarkers();

	// load fiber indices from a file where the indices of fibers constituting a
	// predefined fiber bundle are prepared therein.
	int _loadFiberIdx();

	void __drawSuperEllipsoids();
	void __drawCones();

	void __getLABColorArrayFromFAcolorArray(vtkDataArray* in, vtkDataArray* out);
	void __addColorBar();
	point_t m_scaleMinMax;
	void __addScaleLegend();
};

#endif // _VMRENDERWINDOW_H_

/* sts=8 ts=8 sw=80 tw=8 */

