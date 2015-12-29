// ----------------------------------------------------------------------------
// imgVolRender.h : algorithms for volume rendering
//
// Creation : Nov. 12th 2011
// revision : 
//		.Nov. 25th 2011 Add volume rendering algorithm options 
//			i.e. the different type of volume mapper in vtk
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#ifndef _IMGVOLRENDER_H_
#define _IMGVOLRENDER_H_

#include <QObject>

#include <vtkSmartPointer.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkProperty.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkImageData.h>

#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkCommand.h>
#include <vtkImageImport.h>
#include <vtkVolume.h>
#include <vtkVolumeMapper.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageAlgorithm.h>

#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkOpenGLGPUVolumeRayCastMapper.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkOpenGLVolumeTextureMapper2D.h>
#include <vtkOpenGLVolumeTextureMapper3D.h>

#include <vtkXMLParser.h>

#include <nifti1_io.h>
#include "tfWidget.h"
#include "vmRenderWindow.h"

const char* const g_volrender_methods[] = {
	"Auto",
	"CPU Ray-Cast",
	"VTK GPU Ray-Cast",
	"OpenGL GPU Ray-Cast",
	"Fixed-point Ray-Cast",
	"2D Texture Mapping",
	"3D Texture Mapping"
};

enum _volrender_index {
	VM_START = -1,
	VM_AUTO = 0,
	VM_CPU_RC,
	VM_VTKGPU_RC,
	VM_OPENGLGPU_RC,
	VM_FIXED_RC,
	VM_TEXTURE_2D,
	VM_TEXTURE_3D,
	VM_END
};

const char* const g_volrender_presets[] = {
	"Auto",
	"CT_Skin",
	"CT_Bone",
	"CT_Muscle",
	"CT_AAA",
	"CT_Cardiac",
	"CT_Coronary",
	"SPLPNL-Atlas",
	"MR_Angio",
	"MR_MIP",
	"CHEST_HighConstrast"
};

enum _volpreset_index {
	VP_START = -1,
	VP_AUTO = 0,
	VP_CT_SKIN,
	VP_CT_BONE,
	VP_CT_MUSCLE,
	VP_CT_AAA,
	VP_CT_CARDIAC,
	VP_CT_CORONARY,
	VP_SPLPNL_ATLAS,
	VP_MR_ANGIO,
	VP_MR_MIP,
	VP_CHEST_HIGHCONTRAST,
	VP_END
};

class CLegiMainWindow;

////////////////////////////////////////////////////////////////////
class imgVolRender : public QObject {
	Q_OBJECT

public:
	friend class CLegiMainWindow;

	imgVolRender(CLegiMainWindow* parent);
	virtual ~imgVolRender();

	void setImgData (void* data, size_t size, nifti_1_header* header);

	bool addVol(vtkImageAlgorithm* image, int mapperType = 0, int presetType = 0);
	vtkVolume* & getVol();

	void cleanup();
	bool mount(const char* fndata, bool dicom=false);

	bool LoadNifti(const char* fndata);

	bool LoadDicoms(const char* dicomdir);

	bool LoadPresets(const char* fnpreset);

public slots:
	void OnTGraphUpdate();

private:
	CLegiMainWindow *m_parent;

	vtkVolume* m_vol;
	TransferFunctionWidget *t_graph;

	vtkVolumeMapper* m_volMapper;

	vtkImageImport* niftiImg;
	vtkDICOMImageReader* dicomReader;
	vtkSmartPointer<vtkPiecewiseFunction> scalar_opacity_tf;
	vtkSmartPointer<vtkPiecewiseFunction> gradient_opacity_tf;
	vtkSmartPointer<vtkColorTransferFunction> color_tf;

private:
	void GetPiecewiseFunctionFromString(std::string str, vtkSmartPointer<vtkPiecewiseFunction>& result);
	void GetColorTransferFunctionFromString(std::string str, vtkSmartPointer<vtkColorTransferFunction>& result);
};

#endif // _IMGVOLRENDER_H_

/* sts=8 ts=8 sw=80 tw=8 */

