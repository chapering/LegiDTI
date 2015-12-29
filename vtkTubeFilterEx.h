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

#define VTK_DIRECTION_BACK_TO_FRONT 0
#define VTK_DIRECTION_FRONT_TO_BACK 1
#define VTK_DIRECTION_SPECIFIED_VECTOR 2

#define VTK_SORT_FIRST_POINT 0
#define VTK_SORT_BOUNDS_CENTER 1
#define VTK_SORT_PARAMETRIC_CENTER 2

class vtkCamera;
class vtkProp3D;
class vtkTransform;

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

  // Description:
  // Specify the sort method for the polygonal primitives. By default, the
  // poly data is sorted from back to front.
  vtkSetMacro(Direction,int);
  vtkGetMacro(Direction,int);
  void SetDirectionToFrontToBack() 
    {this->SetDirection(VTK_DIRECTION_FRONT_TO_BACK);}
  void SetDirectionToBackToFront() 
    {this->SetDirection(VTK_DIRECTION_BACK_TO_FRONT);}
  void SetDirectionToSpecifiedVector() 
    {this->SetDirection(VTK_DIRECTION_SPECIFIED_VECTOR);}

  // Description:
  // Specify the point to use when sorting. The fastest is to just
  // take the first cell point. Other options are to take the bounding
  // box center or the parametric center of the cell. By default, the
  // first cell point is used.
  vtkSetMacro(DepthSortMode,int);
  vtkGetMacro(DepthSortMode,int);
  void SetDepthSortModeToFirstPoint() 
    {this->SetDepthSortMode(VTK_SORT_FIRST_POINT);}
  void SetDepthSortModeToBoundsCenter() 
    {this->SetDepthSortMode(VTK_SORT_BOUNDS_CENTER);}
  void SetDepthSortModeToParametricCenter() 
    {this->SetDepthSortMode(VTK_SORT_PARAMETRIC_CENTER);}

  // Description:
  // Specify a camera that is used to define a view direction along which
  // the cells are sorted. This ivar only has effect if the direction is set
  // to front-to-back or back-to-front, and a camera is specified.
  virtual void SetCamera(vtkCamera*);
  vtkGetObjectMacro(Camera,vtkCamera);

  // Description:
  // Specify a transformation matrix (via the vtkProp3D::GetMatrix() method)
  // that is used to include the effects of transformation. This ivar only
  // has effect if the direction is set to front-to-back or back-to-front,
  // and a camera is specified. Specifying the vtkProp3D is optional.
  void SetProp3D(vtkProp3D *);
  vtkProp3D *GetProp3D();

  // Description:
  // Set/Get the sort direction. This ivar only has effect if the sort
  // direction is set to SetDirectionToSpecifiedVector(). The sort occurs
  // in the direction of the vector.
  vtkSetVector3Macro(Vector,double);
  vtkGetVectorMacro(Vector,double,3);

  // Description:
  // Set/Get the sort origin. This ivar only has effect if the sort
  // direction is set to SetDirectionToSpecifiedVector(). The sort occurs
  // in the direction of the vector, with this point specifying the
  // origin.
  vtkSetVector3Macro(Origin,double);
  vtkGetVectorMacro(Origin,double,3);

  // Description:
  // Set/Get a flag that controls the generation of scalar values
  // corresponding to the sort order. If enabled, the output of this
  // filter will include scalar values that range from 0 to (ncells-1),
  // where 0 is closest to the sort direction.
  vtkSetMacro(SortScalars, int);
  vtkGetMacro(SortScalars, int);
  vtkBooleanMacro(SortScalars, int);

  // Description:
  // Return MTime also considering the dependent objects: the camera
  // and/or the prop3D.
  unsigned long GetMTime();

protected:
  vtkTubeFilterEx();
  ~vtkTubeFilterEx();

  // Usual data generation method
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  // Helper methods
  int GeneratePoints(vtkIdType offset, vtkIdType npts, vtkIdType *pts,
                     vtkPoints *inPts, vtkPoints *newPts, 
                     vtkPointData *pd, vtkPointData *outPD,
                     vtkFloatArray *newNormals, vtkDataArray *inScalars,
                     double range[2], vtkDataArray *inVectors, double maxNorm, 
                     vtkDataArray *inNormals);

  void ComputeProjectionVector(double vector[3], double origin[3]);

  int Direction;
  int DepthSortMode;
  vtkCamera *Camera;
  vtkProp3D *Prop3D;
  vtkTransform *Transform;
  double Vector[3];
  double Origin[3];
  int SortScalars;

private:
  vtkTubeFilterEx(const vtkTubeFilterEx&);  // Not implemented.
  void operator=(const vtkTubeFilterEx&);  // Not implemented.
};

#endif
