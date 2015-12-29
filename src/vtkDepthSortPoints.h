// ----------------------------------------------------------------------------
// vtkDepthSortPoints.h : extension to vtkDepthSortPolyData for supporting 
//					point-wise, rather than cell-wise, sorting
//
// Creation : Nov. 22nd 2011
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------

#ifndef __vtkDepthSortPoints_h
#define __vtkDepthSortPoints_h

#include "vtkDepthSortPolyData.h"

class vtkCamera;
class vtkProp3D;
class vtkTransform;

class VTK_HYBRID_EXPORT vtkDepthSortPoints : public vtkDepthSortPolyData 
{
public:
  static vtkDepthSortPoints*New();

  vtkTypeMacro(vtkDepthSortPoints, vtkDepthSortPolyData);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkDepthSortPoints();
  ~vtkDepthSortPoints();

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  void ComputeProjectionVector(double vector[3], double origin[3]);

private:
  vtkDepthSortPoints(const vtkDepthSortPoints&);  // Not implemented.
  void operator=(const vtkDepthSortPoints&);  // Not implemented.
};

#endif
