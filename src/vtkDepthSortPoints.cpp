// ----------------------------------------------------------------------------
// vtkDepthSortPoints.h : extension to vtkDepthSortPolyData for supporting 
//					point-wise, rather than cell-wise, sorting
//
// Creation : Nov. 22nd 2011
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#include "vtkDepthSortPoints.h"

#include "vtkCamera.h"
#include "vtkCellData.h"
#include "vtkGenericCell.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkProp3D.h"
#include "vtkTransform.h"
#include "vtkUnsignedIntArray.h"

vtkStandardNewMacro(vtkDepthSortPoints);

vtkDepthSortPoints::vtkDepthSortPoints()
{
	//cout << "vtkDepthSortPoints instantiated.\n";
}

vtkDepthSortPoints::~vtkDepthSortPoints()
{
}

typedef struct _vtkSortValues {
  double z;
  vtkIdType ptId;
} vtkSortValues;

extern "C" 
{
  int vtkCompareBackToFront(const void *val1, const void *val2)
  {
    if (((vtkSortValues *)val1)->z > ((vtkSortValues *)val2)->z)
      {
      return (-1);
      }
    else if (((vtkSortValues *)val1)->z < ((vtkSortValues *)val2)->z)
      {
      return (1);
      }
    else
      {
      return (0);
      }
  }
}

extern "C" 
{
  int vtkCompareFrontToBack(const void *val1, const void *val2)
  {
    if (((vtkSortValues *)val1)->z < ((vtkSortValues *)val2)->z)
      {
      return (-1);
      }
    else if (((vtkSortValues *)val1)->z > ((vtkSortValues *)val2)->z)
      {
      return (1);
      }
    else
      {
      return (0);
      }
  }
}

int vtkDepthSortPoints::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and output
  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkSortValues *depth;
  vtkIdType cellId, id;
  vtkGenericCell *cell;
  vtkIdType numCells=input->GetNumberOfCells();
  vtkCellData *inCD=input->GetCellData();
  vtkCellData *outCD=output->GetCellData();
  vtkUnsignedIntArray *sortScalars = NULL;
  unsigned int *scalars = NULL;
  double x[3];
  //double p[3], *bounds, *w = NULL, xf[3];
  double *w = NULL;
  double vector[3];
  double origin[3];
  //int type, npts, subId;
  int type, npts;
  vtkIdType newId;
  vtkIdType *pts;

  vtkIdType numPoints = input->GetNumberOfPoints();
  vtkIdType ptId;
  
  // Initialize
  //
  vtkDebugMacro(<<"Sorting polygonal data");

  // Compute the sort vector
  if ( this->Direction == VTK_DIRECTION_SPECIFIED_VECTOR )
    {
    for (int i=0; i<3; i++)
      {
      vector[i] = this->Vector[i];
      origin[i] = this->Origin[i];
      }
    }
  else //compute view vector
    {
    if ( this->Camera == NULL)
      {
      vtkErrorMacro(<<"Need a camera to sort");
      return 0;
      }
  
    this->ComputeProjectionVector(vector, origin);
    }
  cell=vtkGenericCell::New();

  if ( this->DepthSortMode == VTK_SORT_PARAMETRIC_CENTER )
    {
    w = new double [input->GetMaxCellSize()];
    }

  // Create temporary input
  vtkPolyData *tmpInput = vtkPolyData::New();
  tmpInput->CopyStructure(input);

  // Compute the depth value per points, so this is done regardless of the DepthSortMode
  depth = new vtkSortValues [numPoints];
  for ( ptId=0; ptId < numPoints; ptId++ )
  {
	  input->GetPoint(ptId, x);
	  depth[ptId].z = vtkMath::Dot(x,vector);
	  depth[ptId].ptId= ptId;
  }

  this->UpdateProgress(0.20);

  // Sort the depths
  if ( this->Direction == VTK_DIRECTION_FRONT_TO_BACK )
    {
    qsort((void *)depth, numPoints, sizeof(vtkSortValues), 
          vtkCompareFrontToBack);
    }
  else
    {
    qsort((void *)depth, numPoints, sizeof(vtkSortValues), 
          vtkCompareBackToFront);
    }
  this->UpdateProgress(0.60);

  // Generate sorted output
  if ( this->SortScalars )
    {
    sortScalars = vtkUnsignedIntArray::New();
    sortScalars->SetNumberOfTuples(numPoints);
    scalars = sortScalars->GetPointer(0);
	for ( ptId=0; ptId < numPoints; ptId++ ) {
		scalars[ depth[ptId].ptId ] = ptId;
	}
    }

  this->UpdateProgress(0.75);

  outCD->CopyAllocate(inCD);
  output->Allocate(tmpInput,numCells);
  // here we are not sorting the cells but points, so the cell order will keep unchanged
  for ( cellId=0; cellId < numCells; cellId++ )
    {
    //id = depth[cellId].cellId;
    id = cellId;
    tmpInput->GetCell(id, cell);
    type = cell->GetCellType();
    npts = cell->GetNumberOfPoints();
    pts = cell->GetPointIds()->GetPointer(0);

    // copy cell data
    newId = output->InsertNextCell(type, npts, pts);
    outCD->CopyData(inCD, id, newId);
    }
  this->UpdateProgress(0.90);

  // Points are left alone
  output->SetPoints(input->GetPoints());
  output->GetPointData()->PassData(input->GetPointData());
  if ( this->SortScalars )
    {
	/*
    int idx = output->GetCellData()->AddArray(sortScalars);
    output->GetCellData()->SetActiveAttribute(idx, vtkDataSetAttributes::SCALARS);
	*/
	int idx = output->GetPointData()->AddArray(sortScalars);
    output->GetPointData()->SetActiveAttribute(idx, vtkDataSetAttributes::SCALARS);
	//output->GetPointData()->SetScalars( sortScalars );
    sortScalars->Delete();
    }

  // Clean up and get out    
  tmpInput->Delete();
  delete [] depth;
  cell->Delete();
  output->Squeeze();

  return 1;
}

void vtkDepthSortPoints::ComputeProjectionVector(double vector[3], 
                                                   double origin[3])
{
  double *focalPoint = this->Camera->GetFocalPoint();
  double *position = this->Camera->GetPosition();
 
  // If a camera is present, use it
  if ( !this->Prop3D )
    {
    for(int i=0; i<3; i++)
      { 
      vector[i] = focalPoint[i] - position[i];
      origin[i] = position[i];
      }
    }

  else  //Otherwise, use Prop3D
    {
    double focalPt[4], pos[4];
    int i;

    this->Transform->SetMatrix(this->Prop3D->GetMatrix());
    this->Transform->Push();
    this->Transform->Inverse();

    for(i=0; i<4; i++)
      {
      focalPt[i] = focalPoint[i];
      pos[i] = position[i];
      }

    this->Transform->TransformPoint(focalPt,focalPt);
    this->Transform->TransformPoint(pos,pos);

    for (i=0; i<3; i++) 
      {
      vector[i] = focalPt[i] - pos[i];
      origin[i] = pos[i];
      }
    this->Transform->Pop();
  }
}

void vtkDepthSortPoints::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
