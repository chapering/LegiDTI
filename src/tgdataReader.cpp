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
#include "point.h"

#include <vtkPolyLine.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkUnsignedIntArray.h>

#include <vector>
#include <iostream>

using namespace std;

vtkTgDataReader::vtkTgDataReader()
{
	//allPoints = vtkSmartPointer<vtkPoints>::New();
	//allLines = vtkSmartPointer<vtkCellArray>::New();

	__polydata = vtkSmartPointer<vtkPolyData>::New();
	__polydata2 = vtkSmartPointer<vtkPolyData>::New();
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
	for (unsigned long jdx = 0; jdx < szTotal; ++jdx) {
		const vector<GLfloat> & line = m_loader.getElement( jdx );
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

	point_t zax;
	zax.update(0,0,-1);

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

	vtkSmartPointer<vtkUnsignedIntArray> faSizes = vtkSmartPointer<vtkUnsignedIntArray>::New();
	faSizes->SetName("size scalars");
	faSizes->SetNumberOfComponents(1);
	faSizes->SetNumberOfTuples( colorTotal );

	vtkSmartPointer<vtkUnsignedCharArray> ortColors = vtkSmartPointer<vtkUnsignedCharArray>::New();
	ortColors->SetName("orientation colors");
	ortColors->SetNumberOfComponents(3);
	ortColors->SetNumberOfTuples( colorTotal );

	vtkSmartPointer<vtkUnsignedCharArray> satColors = vtkSmartPointer<vtkUnsignedCharArray>::New();
	satColors->SetName("saturation colors");
	satColors->SetNumberOfComponents(3);
	satColors->SetNumberOfTuples( colorTotal );

	unsigned long nextIdx = 1;
	point_t vp;

	for (unsigned long idx = 0; idx < szTotal; ++idx) {
		const vector<GLfloat> & line = m_loader.getElement( idx );
		unsigned long szPts = static_cast<unsigned long>( line.size()/6 );
		GLfloat x,y,z;
		GLfloat px,py,pz;
		GLfloat nx,ny,nz; // next point to (px,py,pz)
		GLfloat vd[3];

		for (unsigned long jdx = 0; jdx < szPts; jdx++) {
			x = line [ jdx*6 + 0 ], 
			y = line [ jdx*6 + 1 ], 
			z = line [ jdx*6 + 2 ];

			px = line [ jdx*6 + 3 ], 
			py = line [ jdx*6 + 4 ], 
			pz = line [ jdx*6 + 5 ];

			if ( jdx == szPts - 1 ) {
				nextIdx = jdx - 1;
			}
			else {
				nextIdx = jdx + 1;
			}

			nx = line [ nextIdx*6 + 3 ], 
			ny = line [ nextIdx*6 + 4 ], 
			nz = line [ nextIdx*6 + 5 ];

			//__colortable->SetTableValue( colorIdx, x,1-y,1-z);
			faColors->InsertTuple3( colorIdx, int(255*x), int(255*(y)), int(255*(y)) );
			CL->InsertTuple1( colorIdx, 1.0-y);
			faSizes->InsertTuple1( colorIdx, int( (1.0-y)*100 ) );

			//cout << px << "," << py << "," << pz << endl;
			//cout << nx << "," << ny << "," << nz << endl;

			if ( jdx == szPts - 1 ) {
				vd[0] = px - nx, vd[1] = py - ny, vd[2] = pz - nz;
			}
			else {
				vd[0] = nx - px, vd[1] = ny - py, vd[2] = nz - pz;
			}
			vp.update(vd[0], vd[1], vd[2]);
			//cout << vp[0] << "," << vp[1] << "," << vp[2] << endl;
			vp.normalize(true);
			//cout << "after: " <<  vp[0] << "," << vp[1] << "," << vp[2] << endl;
			ortColors->InsertTuple3( colorIdx, fabs(vp[0])*255, fabs(vp[1])*255, fabs(vp[2])*255 );
			//satColors->InsertTuple3( colorIdx, 0.5, (vp[0]+vp[1]+vp[2])/3.0, 0.5 );
			//satColors->InsertTuple3( colorIdx, 0.5*255, 0.5*255, fabs( min( min(vp[0],vp[1]), vp[2]) )*255);
			//satColors->InsertTuple3( colorIdx, 0.0*255, 0.0*255, pz*1.5);
			double val = fabs(zax.angleTo( vp ))/3.1415*255;
			//satColors->InsertTuple3( colorIdx, val, val, 255  );
			satColors->InsertTuple3( colorIdx, 255, val, val );
			/*
			satColors->InsertTuple3( colorIdx, 0.0, 0.0, fabs(zax.angleTo( vp ))/3.1415*255 );
			*/

			colorIdx++;
		}

		__colortable->SetTableValue( idx, 1, 1-(idx*1.0/szTotal) , 1-(idx*1.0/szTotal) );
	}

	__polydata->GetPointData()->AddArray( faColors );
	__polydata->GetPointData()->SetActiveScalars("FA COLORS");
	__polydata->GetPointData()->AddArray( CL );
	__polydata->GetPointData()->AddArray( faSizes );

	__polydata->GetPointData()->AddArray( ortColors );
	__polydata->GetPointData()->AddArray( satColors );

	__colortable->SetTableRange(0,1);
	//__colortable->SetValueRange(1,1);
	//__colortable->PrintSelf(cout, vtkIndent() );
	return true;
}

bool vtkTgDataReader::Load2(const char* fndata, bool bLoadColor, int step, double interval)
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
	point_t norm;
	GLfloat x,y,z, nx, ny, nz;
	unsigned long lidx = 0;

	for (unsigned long jdx = 0; jdx < szTotal; ++jdx) {
		const vector<GLfloat> & line = m_loader.getElement( jdx );
		unsigned long szPts = static_cast<unsigned long>( line.size()/6 );

		bool bKeyFiber = ( __focusedFiberIndicesIn.find( jdx ) != __focusedFiberIndicesIn.end() );

		unsigned long ltotal = 0;
		vtkPolyLine* vtkln = vtkPolyLine::New();
		for (unsigned long idx = 0; idx < szPts; idx++) {
			/*
			if ( idx >= szPts ) {
				idx = szPts - 1;
			}
			*/
			x = line [ idx*6 + 3 ], 
			y = line [ idx*6 + 4 ], 
			z = line [ idx*6 + 5 ];

			if ( idx < szPts - 1 && (0 == idx % step) && szPts - 1 - idx > 3 ) {
				if ( idx != 0 ) {
					// one more line segment
					allPoints->InsertNextPoint( x, y, z );
					vtkln->GetPointIds()->InsertNextId( ltotal + startPtId );
					ltotal++;

					allLines->InsertNextCell( vtkln );
					if ( bKeyFiber ) {
						__focusedFiberIndicesOut.insert( lidx );
					}
					lidx++;
					//cout << "line inserted.\n";
					vtkln->Delete();
					vtkln = vtkPolyLine::New();
				}

				unsigned long idx2 = idx + 1;
				nx = line [ idx2*6 + 3 ], 
				ny = line [ idx2*6 + 4 ], 
				nz = line [ idx2*6 + 5 ];
				/*
				norm.update( nx - x, ny - y, nz - z );
				float len = norm.magnitude();
				norm.normalize();
				norm = norm*( len > interval? len - interval : len/3.0 );
				allPoints->InsertNextPoint( nx - norm.x, ny - norm.y, nz - norm.z );
				vtkln->GetPointIds()->InsertNextId( ltotal + startPtId );
				ltotal++;
				*/

				/*
				allPoints->InsertNextPoint( nx, ny, nz );

				vtkln->GetPointIds()->SetNumberOfIds(2);
				vtkln->GetPointIds()->SetId( 0, ltotal + startPtId );
				vtkln->GetPointIds()->SetId( 1, ltotal + startPtId + 1 );
				allLines->InsertNextCell( vtkln );

				ltotal += 2;
				*/
				idx += (int)interval;
			}
			else {
				allPoints->InsertNextPoint( x, y, z );
				vtkln->GetPointIds()->InsertNextId( ltotal + startPtId );
				ltotal ++;
			}
			colorTotal ++;

			/*
			if ( szPts - 1 == idx ) break;
			*/
			if ( szPts - 1 == idx ) {
				// one more line segment
				/*
				allPoints->InsertNextPoint( x, y, z );
				vtkln->GetPointIds()->InsertNextId( ltotal + startPtId );
				ltotal++;
				*/

				allLines->InsertNextCell( vtkln );
				if ( bKeyFiber ) {
					__focusedFiberIndicesOut.insert( lidx );
				}
				lidx++;
				//cout << "line inserted.\n";
				vtkln->Delete();
				break;
			}
		}
		startPtId += ltotal;
		//cout << ltotal << " points inserted.\n";
	}

	//this->SetPoints( allPoints );
	//this->SetLines( allLines );
	__polydata2->SetPoints( allPoints );
	__polydata2->SetLines( allLines );

	if ( !bLoadColor ) {
		return true;
	}

	point_t zax;
	zax.update(0,0,-1);
	// total number of lines in the new geometry model
	unsigned long lTotal = lidx;
	lidx = 0;

	int colorIdx = 0;
	//__colortable->SetNumberOfTableValues( colorTotal );
	__colortable->SetNumberOfTableValues( lTotal );

	vtkSmartPointer<vtkUnsignedCharArray> faColors = vtkSmartPointer<vtkUnsignedCharArray>::New();
	faColors->SetName("FA COLORS");
	faColors->SetNumberOfComponents(3);
	//faColors->SetNumberOfTuples( colorTotal );

	vtkSmartPointer<vtkFloatArray> CL = vtkSmartPointer<vtkFloatArray>::New();
	CL->SetName("Linear Anisotropy");
	CL->SetNumberOfComponents(1);
	//CL->SetNumberOfTuples( colorTotal );

	vtkSmartPointer<vtkUnsignedIntArray> faSizes = vtkSmartPointer<vtkUnsignedIntArray>::New();
	faSizes->SetName("size scalars");
	faSizes->SetNumberOfComponents(1);
	//faSizes->SetNumberOfTuples( colorTotal );

	vtkSmartPointer<vtkUnsignedCharArray> ortColors = vtkSmartPointer<vtkUnsignedCharArray>::New();
	ortColors->SetName("orientation colors");
	ortColors->SetNumberOfComponents(3);
	//ortColors->SetNumberOfTuples( colorTotal );

	vtkSmartPointer<vtkUnsignedCharArray> satColors = vtkSmartPointer<vtkUnsignedCharArray>::New();
	satColors->SetName("saturation colors");
	satColors->SetNumberOfComponents(3);
	//satColors->SetNumberOfTuples( colorTotal );

	unsigned long nextIdx = 1;
	point_t vp;

	for (unsigned long idx = 0; idx < szTotal; ++idx) {
		const vector<GLfloat> & line = m_loader.getElement( idx );
		unsigned long szPts = static_cast<unsigned long>( line.size()/6 );
		GLfloat x,y,z;
		GLfloat px,py,pz;
		GLfloat nx,ny,nz; // next point to (px,py,pz)
		GLfloat vd[3];

		for (unsigned long jdx = 0; jdx < szPts; jdx++) {
			x = line [ jdx*6 + 0 ], 
			y = line [ jdx*6 + 1 ], 
			z = line [ jdx*6 + 2 ];

			px = line [ jdx*6 + 3 ], 
			py = line [ jdx*6 + 4 ], 
			pz = line [ jdx*6 + 5 ];

			if ( jdx == szPts - 1 ) {
				nextIdx = jdx - 1;
			}
			else {
				nextIdx = jdx + 1;
			}

			nx = line [ nextIdx*6 + 3 ], 
			ny = line [ nextIdx*6 + 4 ], 
			nz = line [ nextIdx*6 + 5 ];

			//cout << px << "," << py << "," << pz << endl;
			//cout << nx << "," << ny << "," << nz << endl;

			if ( jdx == szPts - 1 ) {
				vd[0] = px - nx, vd[1] = py - ny, vd[2] = pz - nz;
			}
			else {
				vd[0] = nx - px, vd[1] = ny - py, vd[2] = nz - pz;
			}
			vp.update(vd[0], vd[1], vd[2]);
			//cout << vp[0] << "," << vp[1] << "," << vp[2] << endl;
			vp.normalize(true);
			//cout << "after: " <<  vp[0] << "," << vp[1] << "," << vp[2] << endl;

			if ( jdx < szPts - 1 && (0 == jdx % step) && szPts - 1 - jdx > 3 ) {
				if ( jdx != 0 ) {
					faColors->InsertTuple3( colorIdx, int(255*x), int(255*(y)), int(255*(y)) );
					CL->InsertTuple1( colorIdx, 1.0-y);
					faSizes->InsertTuple1( colorIdx, int( (1.0-y)*100 ) );
					ortColors->InsertTuple3( colorIdx, fabs(vp[0])*255, fabs(vp[1])*255, fabs(vp[2])*255 );
					//satColors->InsertTuple3( colorIdx, 0.5*255, 0.5*255, fabs( min( min(vp[0],vp[1]), vp[2]) )*255);
					//satColors->InsertTuple3( colorIdx, 0.0*255, 0.0*255, pz*2.0);
					satColors->InsertTuple3( colorIdx, 0.0, 0.0, fabs(zax.angleTo( vp ))/3.1415*255 );
					colorIdx++;

					__colortable->SetTableValue( lidx, 1, 1-(lidx*1.0/lTotal) , 1-(lidx*1.0/lTotal) );
					lidx++;
				}

				jdx += (int)interval;
			}
			else {
				faColors->InsertTuple3( colorIdx, int(255*x), int(255*(y)), int(255*(y)) );
				CL->InsertTuple1( colorIdx, 1.0-y);
				faSizes->InsertTuple1( colorIdx, int( (1.0-y)*100 ) );
				ortColors->InsertTuple3( colorIdx, fabs(vp[0])*255, fabs(vp[1])*255, fabs(vp[2])*255 );
				//satColors->InsertTuple3( colorIdx, 0.5*255, 0.5*255, fabs( min( min(vp[0],vp[1]), vp[2]) )*255);
				//satColors->InsertTuple3( colorIdx, 0.0*255, 0.0*255, pz*2.0);
				satColors->InsertTuple3( colorIdx, 0.0, 0.0, fabs(zax.angleTo( vp ))/3.1415*255 );
				colorIdx++;
			}

			if ( szPts - 1 == jdx ) {
				/*
				faColors->InsertTuple3( colorIdx, int(255*x), int(255*(y)), int(255*(y)) );
				CL->InsertTuple1( colorIdx, 1.0-y);
				faSizes->InsertTuple1( colorIdx, int( (1.0-y)*100 ) );
				ortColors->InsertTuple3( colorIdx, fabs(vp[0])*255, fabs(vp[1])*255, fabs(vp[2])*255 );
				satColors->InsertTuple3( colorIdx, 0.5*255, 0.5*255, fabs( min( min(vp[0],vp[1]), vp[2]) )*255);
				colorIdx++;
				*/

				__colortable->SetTableValue( lidx, 1, 1-(lidx*1.0/lTotal) , 1-(lidx*1.0/lTotal) );
				lidx++;
				break;
			}
		}
	}

	__polydata2->GetPointData()->AddArray( faColors );
	__polydata2->GetPointData()->SetActiveScalars("FA COLORS");
	__polydata2->GetPointData()->AddArray( CL );
	__polydata2->GetPointData()->AddArray( faSizes );

	__polydata2->GetPointData()->AddArray( ortColors );
	__polydata2->GetPointData()->AddArray( satColors );

	__colortable->SetTableRange(0,1);
	//__colortable->SetValueRange(1,1);
	//__colortable->PrintSelf(cout, vtkIndent() );
	return true;
}

/* sts=8 ts=8 sw=80 tw=8 */

