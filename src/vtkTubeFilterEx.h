// ----------------------------------------------------------------------------
// vtkTubeFilterEx.h : extension to vtkTubeFilter for supporting depth-dependent
//						streamtube geometric properties
//
// Creation : Nov. 22nd 2011
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------

#ifndef __vtkTubeFilterEx_h
#define __vtkTubeFilterEx_h

#include "vtkTubeFilter.h"

class vtkCellArray;
class vtkCellData;
class vtkDataArray;
class vtkFloatArray;
class vtkPointData;
class vtkPoints;

class VTK_GRAPHICS_EXPORT vtkTubeFilterEx : public vtkTubeFilter 
{
public:
  vtkTypeMacro(vtkTubeFilterEx, vtkTubeFilter);

  // Description:
  // Construct object with radius 0.5, radius variation turned off, the
  // number of sides set to 3, and radius factor of 10.
  static vtkTubeFilterEx *New();

protected:
  vtkTubeFilterEx();
  ~vtkTubeFilterEx() {}

  // Usual data generation method
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  // Helper methods
  int GeneratePoints(vtkIdType offset, vtkIdType npts, vtkIdType *pts,
                     vtkPoints *inPts, vtkPoints *newPts, 
                     vtkPointData *pd, vtkPointData *outPD,
                     vtkFloatArray *newNormals, vtkDataArray *inScalars,
                     double range[2], vtkDataArray *inVectors, double maxNorm, 
                     vtkDataArray *inNormals);
private:
  vtkTubeFilterEx(const vtkTubeFilterEx&);  // Not implemented.
  void operator=(const vtkTubeFilterEx&);  // Not implemented.
};

#endif
