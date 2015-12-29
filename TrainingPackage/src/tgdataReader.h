// ----------------------------------------------------------------------------
// tgdataReader.h : an extension to vtkPolyData capable of reading tgdata 
//					geometry
//
// Creation : Nov. 13th 2011
// revision : 
//		May 30 => add __polydata2 and Load2 for modified geometry for the 
//		purpose of tailed glyph visualization 
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#ifndef _TGDATAREADER_H_
#define _TGDATAREADER_H_

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkLookupTable.h>

#include <set>

class vtkTgDataReader
//: public vtkPolyData
{
public:
	vtkTgDataReader();
	virtual ~vtkTgDataReader();

	bool Load(const char* fndata, bool bLoadColor=false);
	bool Load2(const char* fndata, bool bLoadColor=false, int step = 6, double interval=0.5);

	vtkSmartPointer<vtkPolyData> GetOutput() {
		return __polydata;
	}

	vtkSmartPointer<vtkPolyData> GetOutput2() {
		return __polydata2;
	}

	vtkSmartPointer<vtkLookupTable> GetColorTable() {
		return __colortable;
	}

	void setFocusedIndices( std::set<vtkIdType>& in ) {
		__focusedFiberIndicesIn = in;
	}

	std::set<vtkIdType>& getFocusedIndices() {
		return __focusedFiberIndicesOut;
	}
protected:

private:
	//vtkSmartPointer<vtkPoints> allPoints;
	//vtkSmartPointer<vtkCellArray> allLines;

	vtkSmartPointer<vtkPolyData> __polydata;
	vtkSmartPointer<vtkLookupTable> __colortable;
	vtkSmartPointer<vtkPolyData> __polydata2;

	std::set<vtkIdType> __focusedFiberIndicesIn;
	std::set<vtkIdType> __focusedFiberIndicesOut;
};

#endif // _TGDATAREADER_H_

/* sts=8 ts=8 sw=80 tw=8 */

