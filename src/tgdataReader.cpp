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
	__polycolor = vtkSmartPointer<vtkPolyData>::New();
}

vtkTgDataReader::~vtkTgDataReader()
{
}

void vtkTgDataReader::SetParallelParams(int numproc, int procid, bool bLoadColor)
{
	m_numproc = numproc;
	m_procid = procid;

	vtkIdType szTotal = __polydata->GetNumberOfCells();

	vtkIdType sidx, eidx;

	sidx = szTotal / m_numproc * m_procid;

	if ( m_procid == m_numproc - 1 ) {
		eidx = szTotal;
	}
	else {
		eidx = szTotal / m_numproc * (m_procid + 1);
	}

	vtkCellArray* allLines = __polydata->GetLines();
	vtkSmartPointer<vtkCellArray> newallLines = vtkSmartPointer<vtkCellArray>::New();

	vtkSmartPointer<vtkPoints> lcPoints = vtkSmartPointer<vtkPoints>::New();
	
	vtkIdType npts, *line;
	allLines->InitTraversal();
	vtkIdType idx = 0;
	int colorTotal = 0;

	GLfloat x,y,z;

	while ( allLines->GetNextCell ( npts, line) ) {
		if (idx >= sidx && idx < eidx ) {
			vtkSmartPointer<vtkPolyLine> vtkln = vtkSmartPointer<vtkPolyLine>::New();

			for (vtkIdType jdx = 0; jdx < npts; jdx ++) {
				double *pt = __polydata->GetPoint( line[jdx] );
				x = pt[0], y = pt[1], z = pt[2];

				lcPoints->InsertNextPoint(x,y,z);

				/*
				 * This issue has been torturing me for at least three days - even when I explicitly indicated color
				 * arrays for the local geometry partition for FA color mapping, the coloring keeps inveterately
				 * refusing to work properly but it works well only on the root process (now it turns out that this is
				 * because the root process always got the first partition, inheriting the naturally correct point ids
				 * assigned.
				 *
				 * The culprit is the following "Point Ids" stuff. Before I got this fix, the issues lie at that, for
				 * the local geometry rendering with FA color mapping, the geometry partition did not get assigned with
				 * correct Point Ids in the "line" (cell) structure.  In a simple word, we must adopt to the tricks used
				 * by VTK designer to tame it in its applications!
				 *
				 * Initially bothered by this issue: Feburary 3rd, 2012
				 * Finally got this fix and beat it: Febuary 16th, 2012 (today)
				 *
				 */
				vtkln->GetPointIds()->InsertNextId( jdx + colorTotal );
			}
			newallLines->InsertNextCell( vtkln );
			colorTotal += npts;
		}
		idx ++;
	}

	__polydata->SetLines ( newallLines );
	__polydata->SetPoints( lcPoints );

	/*
	szTotal = __polydata->GetNumberOfCells();
	szTotal = __polydata->GetNumberOfPoints();
	cout << "szTotal = " << szTotal << "\n";
	*/

	if ( !bLoadColor ) {
		return;
	}

	int colorIdx = 0;

	vtkSmartPointer<vtkUnsignedCharArray> faColors = vtkSmartPointer<vtkUnsignedCharArray>::New();
	faColors->SetName("FA COLORS");
	faColors->SetNumberOfComponents(3);
	faColors->SetNumberOfTuples( colorTotal );

	vtkSmartPointer<vtkFloatArray> CL = vtkSmartPointer<vtkFloatArray>::New();
	CL->SetName("Linear Anisotropy");
	CL->SetNumberOfComponents(1);
	CL->SetNumberOfTuples( colorTotal );

	idx = 0;
	vtkCellArray* allcrLines = __polycolor->GetLines();
	allcrLines->InitTraversal();
	while ( allcrLines->GetNextCell ( npts, line) ) {
		if (idx >= sidx && idx < eidx ) {
			for (vtkIdType jdx = 0; jdx < npts; jdx ++) {
				double *pt = __polycolor->GetPoint( line[jdx] );
				x = pt[0], y = pt[1], z = pt[2];

				faColors->InsertTuple3( colorIdx, int(255*x), int(255*(y)), int(255*(y)) );
				CL->InsertTuple1( colorIdx, 1.0-y);
				colorIdx++;
			}
		}
		idx ++;
	}

	/*
	cout << "in proc: " << procid << " idx=" << idx << " colortotal=" << colorTotal << " colorIdx=" << colorIdx << "\n";
	*/

	__polydata->GetPointData()->RemoveArray("FA COLORS");
	__polydata->GetPointData()->RemoveArray("Linear Anisotropy");
	__polydata->GetPointData()->AddArray( faColors );
	__polydata->GetPointData()->SetActiveScalars("FA COLORS");
	__polydata->GetPointData()->AddArray( CL );

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

	unsigned long szTotal = m_loader.getSize();

	__colortable->SetNumberOfTableValues( szTotal );

	vtkSmartPointer<vtkPoints> crPoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> crLines = vtkSmartPointer<vtkCellArray>::New();

	for (unsigned long idx = 0; idx < szTotal; ++idx) {
		const vector<GLfloat> & line = m_loader.getElement( idx );
		unsigned long szPts = static_cast<unsigned long>( line.size()/6 );
		GLfloat x,y,z;

		vtkSmartPointer<vtkPolyLine> vtkln = vtkSmartPointer<vtkPolyLine>::New();
		vtkSmartPointer<vtkPolyLine> crvtkln = vtkSmartPointer<vtkPolyLine>::New();

		vtkln->GetPointIds()->SetNumberOfIds(szPts);
		crvtkln->GetPointIds()->SetNumberOfIds(szPts);
		for (unsigned long jdx = 0; jdx < szPts; jdx++) {
			x = line [ jdx*6 + 3 ], 
			y = line [ jdx*6 + 4 ], 
			z = line [ jdx*6 + 5 ];

			allPoints->InsertNextPoint( x, y, z );
			vtkln->GetPointIds()->SetId( jdx, jdx + startPtId );

			crvtkln->GetPointIds()->SetId( jdx, jdx + startPtId );
			if (bLoadColor) {
				x = line [ jdx*6 + 0 ], 
				y = line [ jdx*6 + 1 ], 
				z = line [ jdx*6 + 2 ];

				crPoints->InsertNextPoint( x, y, z );
			}
		}
		allLines->InsertNextCell( vtkln );
		if ( bLoadColor ) {
			crLines->InsertNextCell( crvtkln );
		}
		startPtId += szPts;

		__colortable->SetTableValue( idx, 1, 1-(idx*1.0/szTotal) , 1-(idx*1.0/szTotal) );
	}

	__polydata->SetPoints( allPoints );
	__polydata->SetLines( allLines );

	__polycolor->SetPoints( crPoints );
	__polycolor->SetLines( crLines );

	__colortable->SetTableRange(0,1);

	return true;
}

/* sts=8 ts=8 sw=80 tw=8 */

