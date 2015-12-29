// ----------------------------------------------------------------------------
// vtkDepthSortPoints.h : extension to vtkDepthSortPolyData for supporting 
//					point-wise, rather than cell-wise, sorting
//
// Creation : Nov. 22nd 2011
// Revisions:
//	Jan. 9th - Add paralell sorting support via the XKAAPI
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------

#ifndef __vtkDepthSortPoints_h
#define __vtkDepthSortPoints_h

#include "vtkDepthSortPolyData.h"
#include "vtkUnsignedIntArray.h"

//#include "kaapi++"
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

class vtkCamera;
class vtkProp3D;
class vtkTransform;

class vtkMultiProcessController;

typedef struct _vtkSortValues {
  float z;
  //vtkIdType ptId;
  int ptId;

  _vtkSortValues & operator =  (const _vtkSortValues& other) {
	  this->z = other.z;
	  this->ptId = other.ptId;
	  return *this;
  }

  bool operator == (const _vtkSortValues& other) {
	  return this->ptId == other.ptId;
  }

  bool operator == (int oid) {
	  return this->ptId == oid;
  }

} vtkSortValues;

class VTK_HYBRID_EXPORT vtkDepthSortPoints : public vtkDepthSortPolyData 
{
public:
  static vtkDepthSortPoints*New();

  vtkTypeMacro(vtkDepthSortPoints, vtkDepthSortPolyData);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkGetObjectMacro(Controller, vtkMultiProcessController);
  void SetController(vtkMultiProcessController *controller);

protected:
  vtkDepthSortPoints();
  ~vtkDepthSortPoints();

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  void ComputeProjectionVector(double vector[3], double origin[3]);

private:
  vtkDepthSortPoints(const vtkDepthSortPoints&);  // Not implemented.
  void operator=(const vtkDepthSortPoints&);  // Not implemented.

  vtkSortValues *depth ;
  vtkUnsignedIntArray *sortScalars ;

  vtkMultiProcessController *Controller;
};

#endif
