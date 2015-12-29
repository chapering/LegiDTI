// ----------------------------------------------------------------------------
// tgdataReader.cpp : an extension to vtkPolyData capable of reading tgdata 
//					geometry
//
// Creation : Nov. 13th 2011
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#include "tgdataReader.h"
#include "GLoader.h"

#include <vtkPolyLine.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>

#include <vector>
#include <iostream>

using namespace std;

vtkTgDataReader::vtkTgDataReader()
{
	//allPoints = vtkSmartPointer<vtkPoints>::New();
	//allLines = vtkSmartPointer<vtkCellArray>::New();

	__polydata = vtkSmartPointer<vtkPolyData>::New();
	__colortable = vtkSmartPointer<vtkLookupTable>::New();
}

vtkTgDataReader::~vtkTgDataReader()
{
}

bool vtkTgDataReader::Load(const char* fndata, bool bLoadColor)
{
	vtkSmartPointer<vtkPoints> allPoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> allLines = vtkSmartPointer<vtkCellArray>::New();
	/*
	allPoints->SetNumberOfPoints(0);
	allLines->SetNumberOfCells(0);
	*/
	CTgdataLoader m_loader;
	if ( 0 != m_loader.load(fndata) ) {
		cout << "Loading geometry failed - GLApp aborted abnormally.\n";
		return false;
	}

	int startPtId = 0;
	int colorTotal = 0;

	unsigned long szTotal = m_loader.getSize();
	for (unsigned long idx = 0; idx < szTotal; ++idx) {
		const vector<GLfloat> & line = m_loader.getElement( idx );
		unsigned long szPts = static_cast<unsigned long>( line.size()/6 );
		GLfloat x,y,z;

		vtkSmartPointer<vtkPolyLine> vtkln = vtkSmartPointer<vtkPolyLine>::New();

		vtkln->GetPointIds()->SetNumberOfIds(szPts);
		for (unsigned long idx = 0; idx < szPts; idx++) {
			x = line [ idx*6 + 3 ], 
			y = line [ idx*6 + 4 ], 
			z = line [ idx*6 + 5 ];

			allPoints->InsertNextPoint( x, y, z );
			vtkln->GetPointIds()->SetId( idx, idx + startPtId );
			colorTotal ++;
		}
		allLines->InsertNextCell( vtkln );
		startPtId += szPts;
	}

	//this->SetPoints( allPoints );
	//this->SetLines( allLines );
	__polydata->SetPoints( allPoints );
	__polydata->SetLines( allLines );

	if ( !bLoadColor ) {
		return true;
	}

	int colorIdx = 0;
	//__colortable->SetNumberOfTableValues( colorTotal );
	__colortable->SetNumberOfTableValues( szTotal );

	vtkSmartPointer<vtkUnsignedCharArray> faColors = vtkSmartPointer<vtkUnsignedCharArray>::New();
	faColors->SetName("FA COLORS");
	faColors->SetNumberOfComponents(3);
	faColors->SetNumberOfTuples( colorTotal );

	vtkSmartPointer<vtkFloatArray> CL = vtkSmartPointer<vtkFloatArray>::New();
	CL->SetName("Linear Anisotropy");
	CL->SetNumberOfComponents(1);
	CL->SetNumberOfTuples( colorTotal );

	for (unsigned long idx = 0; idx < szTotal; ++idx) {
		const vector<GLfloat> & line = m_loader.getElement( idx );
		unsigned long szPts = static_cast<unsigned long>( line.size()/6 );
		GLfloat x,y,z;

		for (unsigned long jdx = 0; jdx < szPts; jdx++) {
			x = line [ jdx*6 + 0 ], 
			y = line [ jdx*6 + 1 ], 
			z = line [ jdx*6 + 2 ];

			//__colortable->SetTableValue( colorIdx, x,1-y,1-z);
			faColors->InsertTuple3( colorIdx, int(255*x), int(255*(y)), int(255*(y)) );
			CL->InsertTuple1( colorIdx, 1.0-y);
			colorIdx++;
		}

		__colortable->SetTableValue( idx, 1, 1-(idx*1.0/szTotal) , 1-(idx*1.0/szTotal) );
	}

	__polydata->GetPointData()->AddArray( faColors );
	__polydata->GetPointData()->SetActiveScalars("FA COLORS");
	__polydata->GetPointData()->AddArray( CL );

	__colortable->SetTableRange(0,1);
	//__colortable->SetValueRange(1,1);
	//__colortable->PrintSelf(cout, vtkIndent() );
	return true;
}

/* sts=8 ts=8 sw=80 tw=8 */

