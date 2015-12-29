// ----------------------------------------------------------------------------
// imgVolRender.cpp : algorithms for volume rendering
//
// Creation : Nov. 12th 2011
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#include "imgVolRender.h"

#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

//////////////////////////////////////////////////////////////////////
// implementation of class imgVolRender 
//////////////////////////////////////////////////////////////////////
imgVolRender::imgVolRender(CLegiMainWindow* parent) :
	m_parent( parent ),
	m_vol ( NULL ),
	t_graph ( new TransferFunctionWidget(parent) ),
	m_volMapper(NULL),
	niftiImg(NULL),
	dicomReader(NULL),
	scalar_opacity_tf( vtkSmartPointer<vtkPiecewiseFunction>::New() ),
	gradient_opacity_tf ( vtkSmartPointer<vtkPiecewiseFunction>::New() ),
	color_tf( vtkSmartPointer<vtkColorTransferFunction>::New() )
{
	parent->connect(t_graph, SIGNAL( onTfChanged() ), this, SLOT( OnTGraphUpdate() ));
	parent->connect(parent, SIGNAL( close() ), t_graph, SLOT( close() ));
	parent->connect(t_graph, SIGNAL( close() ), parent, SLOT( close() ));

	t_graph->setWindowTitle("transfer function tunning...");
}

imgVolRender::~imgVolRender()
{
	cleanup();
	delete t_graph;
}

void imgVolRender::setImgData (void* data, size_t size, nifti_1_header* header)
{
	if ( niftiImg == NULL || !header ) return;

	int numComponents = 1;

	switch (header->datatype)
	{
		case DT_INT8:
		case DT_UINT8:
			niftiImg->SetDataScalarTypeToUnsignedChar();
			break;
		case DT_INT16:
			niftiImg->SetDataScalarTypeToShort();
			break;
		case DT_UINT16:
			niftiImg->SetDataScalarTypeToUnsignedShort();
			break;
		case DT_INT32:
		case DT_UINT32:
		case DT_INT64: 
		case DT_UINT64:
			niftiImg->SetDataScalarTypeToInt();
			break;
		case DT_FLOAT32:
			niftiImg->SetDataScalarTypeToFloat();
		case DT_FLOAT64:
		case DT_FLOAT128:
			niftiImg->SetDataScalarTypeToDouble();
		default:
			niftiImg->SetDataScalarTypeToShort();
			break;
	}

	niftiImg->CopyImportVoidPointer( data, size );
		
	int* extent = niftiImg->GetDataExtent();
	short* dim = header->dim;
	cout << "niftiImg dimension: ";
	for (int i=0; i<8; i++)
	{
		cout << dim[i] << " ";
	}
	float* pixdim = header->pixdim;
	niftiImg->SetDataExtent(extent[0], extent[0] + dim[1] - 1,
						extent[2], extent[2] + dim[2] - 1,
						extent[4], extent[4] + dim[3] - 1);
	niftiImg->SetWholeExtent(extent[0], extent[0] + dim[1] - 1,
					 extent[2], extent[2] + dim[2] - 1,
					 extent[4], extent[4] + dim[3] - 1);

	niftiImg->SetNumberOfScalarComponents(numComponents);
	niftiImg->SetDataScalarTypeToUnsignedShort();
	niftiImg->SetDataSpacing( pixdim[1], pixdim[2], pixdim[3] );
	//niftiImg->SetDataOrigin( dim[0]*pixdim[0], dim[1]*pixdim[1], dim[2]*pixdim[2]);
	//niftiImg->SetDataOrigin( -0,255,-0 );
	niftiImg->SetDataOrigin( 0,0,0 );

	extent = niftiImg->GetDataExtent();
	cout << "data extent: ";
	for (int i=0; i<6; i++)
	{
		cout << extent[i] << " ";
	}
	cout << "\n";
}

bool imgVolRender::addVol(vtkImageAlgorithm* image, int mapperType, int presetType)
{
	float pix_diag = 5.0/10.0;

	if ( m_volMapper ) {
		m_volMapper->Delete();
	}

	switch ( mapperType ) {
		case VM_AUTO:
			m_volMapper = vtkSmartVolumeMapper::New();
			break;
		case VM_CPU_RC:
			{
				m_volMapper = vtkVolumeRayCastMapper::New();
				vtkSmartPointer<vtkVolumeRayCastCompositeFunction>
					compositeFunction = vtkSmartPointer<vtkVolumeRayCastCompositeFunction>::New();
				compositeFunction->SetCompositeMethodToInterpolateFirst();
				((vtkVolumeRayCastMapper*)m_volMapper)->SetVolumeRayCastFunction(compositeFunction);
				((vtkVolumeRayCastMapper*)m_volMapper)->AutoAdjustSampleDistancesOn();
				((vtkVolumeRayCastMapper*)m_volMapper)->SetSampleDistance(pix_diag / 5.0);
				((vtkVolumeRayCastMapper*)m_volMapper)->SetImageSampleDistance( 1.0 );
			}
			break;
		case VM_VTKGPU_RC:
			{
				m_volMapper = vtkGPUVolumeRayCastMapper::New();
				((vtkGPUVolumeRayCastMapper*)m_volMapper)->SetSampleDistance(pix_diag / 5.0);
				((vtkGPUVolumeRayCastMapper*)m_volMapper)->SetImageSampleDistance( 1.0 );
				((vtkGPUVolumeRayCastMapper*)m_volMapper)->SetBlendModeToComposite();
			}
			break;
		case VM_OPENGLGPU_RC:
			{
				m_volMapper = vtkOpenGLGPUVolumeRayCastMapper::New();
				((vtkOpenGLGPUVolumeRayCastMapper*)m_volMapper)->SetSampleDistance(pix_diag / 5.0);
				((vtkOpenGLGPUVolumeRayCastMapper*)m_volMapper)->SetImageSampleDistance( 1.0 );
			}
			break;
		case VM_FIXED_RC:
			{
				m_volMapper = vtkFixedPointVolumeRayCastMapper::New();
				((vtkFixedPointVolumeRayCastMapper*)m_volMapper)->SetSampleDistance(pix_diag / 5.0);
				((vtkFixedPointVolumeRayCastMapper*)m_volMapper)->SetImageSampleDistance( 1.0 );
			}
			break;
		case VM_TEXTURE_2D:
			{
				m_volMapper = vtkOpenGLVolumeTextureMapper2D::New();
			}
			break;
		case VM_TEXTURE_3D:
			{
				m_volMapper = vtkOpenGLVolumeTextureMapper3D::New();
				((vtkOpenGLVolumeTextureMapper3D*)m_volMapper)->SetSampleDistance(pix_diag / 5.0);
				((vtkOpenGLVolumeTextureMapper3D*)m_volMapper)->SetPreferredMethodToNVidia();
			}
			break;
		default:
			cerr << "unrecognized render type.\n";
			return false;
			break;
	}

	m_volMapper->SetInputConnection( image->GetOutputPort() );
	m_volMapper->SetBlendModeToComposite();

	vtkSmartPointer<vtkVolumeProperty> volProperty = vtkSmartPointer<vtkVolumeProperty>::New();

	if ( presetType == VP_AUTO ) {
		OnTGraphUpdate();
		volProperty->SetScalarOpacityUnitDistance(pix_diag/5.0);
		volProperty->SetAmbient(0.1);
		volProperty->SetDiffuse(0.9);
		volProperty->SetSpecular(0.2);
		volProperty->SetSpecularPower(10.0);
		volProperty->SetScalarOpacityUnitDistance(0.8919);
		if (this->m_parent->m_bLighting)
		{
			volProperty->ShadeOn();
		}
	}
	else {
		this->color_tf->RemoveAllPoints();
		this->scalar_opacity_tf->RemoveAllPoints();
		this->gradient_opacity_tf->RemoveAllPoints();

		switch (presetType) {
			case VP_CT_SKIN:
				{
					/*
					color_tf->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
					color_tf->AddRGBPoint( -1000, .62, .36, .18, 0.5, 0.0 );
					color_tf->AddRGBPoint( -500, .88, .60, .29, 0.33, 0.45 );
					color_tf->AddRGBPoint( 3071, .83, .66, 1, 0.5, 0.0 );
					
					scalar_opacity_tf->AddPoint(-3024, 0, 0.5, 0.0 );
					scalar_opacity_tf->AddPoint(-1000, 0, 0.5, 0.0 );
					scalar_opacity_tf->AddPoint(-500, 1.0, 0.33, 0.45 );
					scalar_opacity_tf->AddPoint(3071, 1.0, 0.5, 0.0);
					*/
					this->GetPiecewiseFunctionFromString("12 -1000 0 19 0 20 0.15 100 0.15 101 0 3952 0",scalar_opacity_tf);
					this->GetPiecewiseFunctionFromString("6 0 1 985.12 1 988 1",gradient_opacity_tf);
					this->GetColorTransferFunctionFromString("36 -1000 0.3 0.3 1 -488 0.3 1 0.3 23.15 0 0 1 45.96 0 1 0"
						   " 75.98 1 1 0 98.8 1 0 0 463.28 1 0 0 659.15 1 0.912535 0.0374849 952 1 0.300267 0.299886",color_tf);

					volProperty->ShadeOff();
					volProperty->SetAmbient(0.1);
					volProperty->SetDiffuse(0.7);
					volProperty->SetSpecular(0.2);
					volProperty->SetSpecularPower(10.0);
					volProperty->SetInterpolationType(1);
				}
				break;
			case VP_CT_BONE:
				{
					/*
					color_tf->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
					color_tf->AddRGBPoint( -16, 0.73, 0.25, 0.30, 0.49, .61 );
					color_tf->AddRGBPoint( 641, .90, .82, .56, .5, 0.0 );
					color_tf->AddRGBPoint( 3071, 1, 1, 1, .5, 0.0 );
					
					scalar_opacity_tf->AddPoint(-3024, 0, 0.5, 0.0 );
					scalar_opacity_tf->AddPoint(-16, 0, .49, .61 );
					scalar_opacity_tf->AddPoint(641, .72, .5, 0.0 );
					scalar_opacity_tf->AddPoint(3071, .71, 0.5, 0.0);
					*/
					this->GetPiecewiseFunctionFromString("8 -1000 0 152.19 0 278.93 0.190476 952 0.2",scalar_opacity_tf);
					this->GetPiecewiseFunctionFromString("4 0 1 985.12 1",gradient_opacity_tf);
					this->GetColorTransferFunctionFromString("20 -1000 0.3 0.3 1 -488 0.3 1 0.3 463.28 1 0 0 659.15 1 0.912535 0.0374849 953 1 0.3 0.3",color_tf);

					volProperty->ShadeOn();
					volProperty->SetAmbient(0.2);
					volProperty->SetDiffuse(1.0);
					volProperty->SetSpecular(0.0);
					volProperty->SetSpecularPower(1.0);
					volProperty->SetInterpolationType(1);
				}
				break;
			case VP_CT_MUSCLE:
				{
					/*
					color_tf->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
					color_tf->AddRGBPoint( -155, .55, .25, .15, 0.5, .92 );
					color_tf->AddRGBPoint( 217, .88, .60, .29, 0.33, 0.45 );
					color_tf->AddRGBPoint( 420, 1, .94, .95, 0.5, 0.0 );
					color_tf->AddRGBPoint( 3071, .83, .66, 1, 0.5, 0.0 );
					
					scalar_opacity_tf->AddPoint(-3024, 0, 0.5, 0.0 );
					scalar_opacity_tf->AddPoint(-155, 0, 0.5, 0.92 );
					scalar_opacity_tf->AddPoint(217, .68, 0.33, 0.45 );
					scalar_opacity_tf->AddPoint(420,.83, 0.5, 0.0);
					scalar_opacity_tf->AddPoint(3071, .80, 0.5, 0.0);
					*/
					this->GetPiecewiseFunctionFromString("10 -3024 0 -155.407 0 217.641 0.676471 419.736 0.833333 3071 0.803922",scalar_opacity_tf);
					this->GetPiecewiseFunctionFromString("4 0 1 255 1",gradient_opacity_tf);
					this->GetColorTransferFunctionFromString("20 -3024 0 0 0 -155.407 0.54902 0.25098 0.14902 217.641 0.882353 "
							"0.603922 0.290196 419.736 1 0.937033 0.954531 3071 0.827451 0.658824 1",color_tf);

					volProperty->ShadeOn();
					volProperty->SetAmbient(0.1);
					volProperty->SetDiffuse(0.9);
					volProperty->SetSpecular(0.2);
					volProperty->SetSpecularPower(10.0);
					volProperty->SetInterpolationType(1);
				}
				break;
			case VP_CT_AAA:
				{
					this->GetPiecewiseFunctionFromString("16 -3024 0 129.542 0 145.244 0.166667 157.02 0.5 169.918 0.627451 395.575 0.8125 1578.73 0.8125 3071 0.8125",scalar_opacity_tf);
					this->GetPiecewiseFunctionFromString("4 0 1 255 1",gradient_opacity_tf);
					this->GetColorTransferFunctionFromString("32 -3024 0 0 0 129.542 0.54902 0.25098 0.14902 145.244 0.6 0.627451 0.843137 157.02 0.890196 0.47451 "
							"0.6 169.918 0.992157 0.870588 0.392157 395.575 1 0.886275 0.658824 1578.73 1 0.829256 0.957922 3071 0.827451 0.658824 1",color_tf);

					volProperty->ShadeOn();
					volProperty->SetAmbient(0.1);
					volProperty->SetDiffuse(0.9);
					volProperty->SetSpecular(0.2);
					volProperty->SetSpecularPower(10.0);
					volProperty->SetInterpolationType(1);
				}
				break;
			case VP_CT_CARDIAC:
				{
					this->GetPiecewiseFunctionFromString("12 -3024 0 42.8964 0 163.488 0.428571 277.642 0.776786 1587 0.754902 3071 0.754902",scalar_opacity_tf);
					this->GetPiecewiseFunctionFromString("4 0 1 255 1",gradient_opacity_tf);
					this->GetColorTransferFunctionFromString("24 -3024 0 0 0 42.8964 0.54902 0.25098 0.14902 163.488 0.917647 0.639216 0.0588235 "
							"277.642 1 0.878431 0.623529 1587 1 1 1 3071 0.827451 0.658824 1" ,color_tf);

					volProperty->ShadeOn();
					volProperty->SetAmbient(0.1);
					volProperty->SetDiffuse(0.9);
					volProperty->SetSpecular(0.2);
					volProperty->SetSpecularPower(10.0);
					volProperty->SetInterpolationType(1);
				}
				break;
			case VP_CT_CORONARY:
				{
					this->GetPiecewiseFunctionFromString("14 -2048 0 142.677 0 145.016 0.116071 192.174 0.5625 217.24 0.776786 384.347 0.830357 3661 0.830357",scalar_opacity_tf);
					this->GetPiecewiseFunctionFromString("4 0 1 255 1",gradient_opacity_tf);
					this->GetColorTransferFunctionFromString("28 -2048 0 0 0 142.677 0 0 0 145.016 0.615686 0 0.0156863 192.174 0.909804 0.454902 0 217.24 "
							"0.972549 0.807843 0.611765 384.347 0.909804 0.909804 1 3661 1 1 1",color_tf);

					volProperty->ShadeOn();
					volProperty->SetAmbient(0.2);
					volProperty->SetDiffuse(1.0);
					volProperty->SetSpecular(0);
					volProperty->SetSpecularPower(1.0);
					volProperty->SetInterpolationType(1);
				}
				break;
			case VP_SPLPNL_ATLAS:
				{
					this->GetPiecewiseFunctionFromString("10 0 0 17.2455 0 50.7784 0.547619 320 0.904762 321 0.2",scalar_opacity_tf);
					this->GetPiecewiseFunctionFromString("6 0 1 12.6946 0.333333 80 0.214286",gradient_opacity_tf);
					this->GetColorTransferFunctionFromString("16 0 0.074766 0.0740317 0.0718289 25.5 1 0.764847 0.514087 116.886 1 0.493789 0.342006 320 1 0.3 0.3",color_tf);

					volProperty->ShadeOn();
					volProperty->SetAmbient(0.3);
					volProperty->SetDiffuse(0.6);
					volProperty->SetSpecular(0.2);
					volProperty->SetSpecularPower(40.0);
					volProperty->SetInterpolationType(1);
				}
				break;
			case VP_MR_ANGIO:
				{
					this->GetPiecewiseFunctionFromString("12 -2048 0 151.354 0 158.279 0.4375 190.112 0.580357 200.873 0.732143 3661 0.741071",scalar_opacity_tf);
					this->GetPiecewiseFunctionFromString("4 0 1 255 1",gradient_opacity_tf);
					this->GetColorTransferFunctionFromString("24 -2048 0 0 0 151.354 0 0 0 158.279 0.74902 0.376471 0 190.112 1 0.866667 0.733333 "
							"200.873 0.937255 0.937255 0.937255 3661 1 1 1",color_tf);

					volProperty->ShadeOn();
					volProperty->SetAmbient(0.2);
					volProperty->SetDiffuse(1.0);
					volProperty->SetSpecular(0);
					volProperty->SetSpecularPower(1.0);
					volProperty->SetInterpolationType(1);
				}
				break;
			case VP_MR_MIP:
				{
					this->GetPiecewiseFunctionFromString("8 0 0 98.3725 0 416.637 1 2800 1",scalar_opacity_tf);
					this->GetPiecewiseFunctionFromString("4 0 1 255 1",gradient_opacity_tf);
					this->GetColorTransferFunctionFromString("16 0 1 1 1 98.3725 1 1 1 416.637 1 1 1 2800 1 1 1",color_tf);

					volProperty->ShadeOn();
					volProperty->SetAmbient(0.2);
					volProperty->SetDiffuse(1.0);
					volProperty->SetSpecular(0);
					volProperty->SetSpecularPower(1.0);
					volProperty->SetInterpolationType(1);
				}
				break;
			case VP_CHEST_HIGHCONTRAST:
				{
					this->GetPiecewiseFunctionFromString("10 -3024 0 67.0106 0 251.105 0.446429 439.291 0.625 3071 0.616071",scalar_opacity_tf);
					this->GetPiecewiseFunctionFromString("4 0 1 255 1",gradient_opacity_tf);
					this->GetColorTransferFunctionFromString("20 -3024 0 0 0 67.0106 0.54902 0.25098 0.14902 251.105 0.882353 0.603922 0.290196 "
							"439.291 1 0.937033 0.954531 3071 0.827451 0.658824 1",color_tf);

					volProperty->ShadeOn();
					volProperty->SetAmbient(0.1);
					volProperty->SetDiffuse(0.9);
					volProperty->SetSpecular(0.2);
					volProperty->SetSpecularPower(10.0);
					volProperty->SetInterpolationType(1);
				}
				break;
			default:
				cerr << "unrecognized transfer function preset.\n";
				return false;
				break;
		}
	}

	volProperty->SetColor(this->color_tf);
	volProperty->SetScalarOpacity(this->scalar_opacity_tf);
	if ( presetType == VP_AUTO ) {
		volProperty->SetGradientOpacity(this->scalar_opacity_tf);
	}
	else {
		volProperty->SetGradientOpacity(this->gradient_opacity_tf);
	}

	m_vol = vtkVolume::New(); 
	m_vol->SetMapper(m_volMapper);
	m_vol->SetProperty(volProperty);

	cout << "Volume prepared.\n";
	return true;
}

vtkVolume* & imgVolRender::getVol()
{
	return this->m_vol;
}

void imgVolRender::cleanup()
{
	if ( niftiImg ) {
		//niftiImg->RemoveAllInputs();
		niftiImg->Delete();
		niftiImg = NULL;
	}

	if ( dicomReader ) {
		dicomReader->Delete();
		dicomReader = NULL;
	}

	if ( m_vol ) {
		m_vol->Delete();
		m_vol = NULL;
	}

	if ( m_volMapper ) {
		//m_volMapper->RemoveAllClippingPlanes();
		m_volMapper->Delete();
		m_volMapper = NULL;
	}
}

bool imgVolRender::mount(const char* fndata, bool dicom)
{
	cleanup();

	if (!fndata) return false;

	if ( dicom ) {
		if (!this->LoadDicoms(fndata)) return false;
		addVol( dicomReader, m_parent->m_nCurMethodIdx, m_parent->m_nCurPresetIdx);
	}
	else {
		if (!this->LoadNifti(fndata)) return false;
		addVol( niftiImg, m_parent->m_nCurMethodIdx, m_parent->m_nCurPresetIdx);
	}

	if ( m_parent->m_nCurPresetIdx == VP_AUTO ) {
		this->OnTGraphUpdate();

		this->t_graph->move(500,0);
		this->t_graph->show();
	}

	return true;
}

void imgVolRender::OnTGraphUpdate()
{
	if ( m_parent->m_nCurPresetIdx != VP_AUTO ) {
		return;
	}
	static int visit = 0;
	this->color_tf->RemoveAllPoints();
	this->scalar_opacity_tf->RemoveAllPoints();
	this->gradient_opacity_tf->RemoveAllPoints();
	const vector< TransferPoint >& points = this->t_graph->getPoints();

	for ( size_t i = 0; i < points.size(); i++)
	{
		const color_t& rgba = points[i].get_rgba();
		this->color_tf->AddRGBPoint(points[i].value, rgba[0]/255.0, 
						 rgba[1]/255.0, rgba[2]/255.0);

		this->scalar_opacity_tf->AddPoint(points[i].value, rgba[3]);
	}

	if (visit > 0)
	{
		this->m_parent->getRenderView()->repaint();
	}
	visit ++;
}

bool imgVolRender::LoadNifti(const char* fndata)
{
	if (!is_nifti_file(fndata)) return false;

	nifti_1_header* header = nifti_read_header( fndata, NULL, 1 );
	if ( !header ) {
		cerr << "Failed to read NIfTI header from " << fndata << "\n";
		return false;
	}
	nifti_image* nim = nifti_image_read( fndata, 1 );
	if ( ! nim )
	{
		cerr << "Failed to read NIfTI image from " << fndata << "\n";
		return false;
	}

	/*
		elif len(nim.data.shape) == 4: # multiple volume
			alldata = numpy.array(nim.data[0])
			print alldata.shape
			for i in range(1, len(nim.data)):
				#this->addVol( nim.data[i] )
				#alldata = numpy.append( alldata, nim.data[i], axis=0)
				alldata = numpy.add( alldata, nim.data[i])
			print alldata.shape
			img_data = alldata
		elif len(nim.data.shape) == 5: # tensor field volume
			alldata = numpy.array(nim.data[0][0])
			print alldata.shape
			for i in range(1, len(nim.data)):
				#this->addVol( nim.data[i] )
				#alldata = numpy.append( alldata, nim.data[i][0], axis=0)
				alldata = numpy.add( alldata, nim.data[i][0])
			print alldata.shape
			img_data = alldata
	*/

	cout << "number of voxels: " << nim->nvox << "\n";
	niftiImg = vtkImageImport::New();
	setImgData(nim->data, 
			/*(nim->slice_end - nim->slice_start + 1)*/nim->nvox*nim->nbyper, 
			header);

	nifti_image_free( nim );
	return true;
}

bool imgVolRender::LoadDicoms(const char* dicomdir)
{
    dicomReader = vtkDICOMImageReader::New();
    dicomReader->SetDirectoryName(dicomdir);
    dicomReader->Update();

	if ( dicomReader->GetErrorCode() != 0 ) return false;

	dicomReader->SetDataScalarTypeToUnsignedShort ();
	double * spacing = dicomReader->GetPixelSpacing();
	cout << "dicom pixel dimension: ";
	for (int i=0; i<3; i++)
	{
		cout << spacing[i] << " ";
	}
	cout << "\n";
	return true;
}

bool imgVolRender::LoadPresets(const char* fnpreset)
{
	if ( strlen(fnpreset) < 1 ) {
		return false;
	}
	else {
		vtkXMLParser* parser = vtkXMLParser::New();

		parser->SetFileName( fnpreset );

		if ( !parser->Parse() ) {
			cerr << "failed to parse " << fnpreset << " for loading transfer function presets.\n";
			return false;
		}

		parser->PrintSelf( cout, vtkIndent() );

		parser->Delete();
	}
	return true;
}

void imgVolRender::GetPiecewiseFunctionFromString(std::string str,vtkSmartPointer<vtkPiecewiseFunction>& result)
{
    std::stringstream stream;
    stream<<str;
    int size=0;

    stream>>size;

    if (size==0)
    {
        return;
    }
    double *data=new double[size];
    for(int i=0;i<size;i++)
    {
        stream>>data[i];
    }
    result->FillFromDataPointer(size/2,data);
    delete[] data;
}

void imgVolRender::GetColorTransferFunctionFromString(std::string str, vtkSmartPointer<vtkColorTransferFunction>& result)
{
    std::stringstream stream;
    stream<<str;
    int size=0;

    stream>>size;

    if (size==0)
    {
        return;
    }
    double *data=new double[size];
    for(int i=0;i<size;i++)
    {
        stream>>data[i];
    }
    result->FillFromDataPointer(size/4,data);
    delete[] data;
}

/* sts=8 ts=8 sw=80 tw=8 */

