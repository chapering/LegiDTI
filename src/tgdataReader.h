// ----------------------------------------------------------------------------
// tgdataReader.h : an extension to vtkPolyData capable of reading tgdata 
//					geometry
//
// Creation : Nov. 13th 2011
// revision : 
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

class vtkTgDataReader
//: public vtkPolyData
{
public:
	vtkTgDataReader();
	virtual ~vtkTgDataReader();

	bool Load(const char* fndata, bool bLoadColor=false);

	/*
	vtkSmartPointer<vtkPolyData> GetOutput() const {
		return __polydata;
	}
	*/

	vtkSmartPointer<vtkPolyData>& GetOutput() {
		return __polydata;
	}

	/*
	vtkSmartPointer<vtkPolyData> GetPolyColor() const {
		return __polycolor;
	}
	*/

	vtkSmartPointer<vtkPolyData>& GetPolyColor() {
		return __polycolor;
	}

	void SetParallelParams(int numproc, int procid, bool bLoadColor=false);

	vtkSmartPointer<vtkLookupTable> GetColorTable() {
		return __colortable;
	}
protected:

private:
	//vtkSmartPointer<vtkPoints> allPoints;
	//vtkSmartPointer<vtkCellArray> allLines;

	vtkSmartPointer<vtkPolyData> __polydata;
	vtkSmartPointer<vtkLookupTable> __colortable;

	vtkSmartPointer<vtkPolyData> __polycolor;

	int m_numproc;
	int m_procid;
};

#endif // _TGDATAREADER_H_

/* sts=8 ts=8 sw=80 tw=8 */

