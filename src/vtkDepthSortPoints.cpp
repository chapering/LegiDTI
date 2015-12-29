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
#include "vtkFloatArray.h"
#include "vtkSmartPointer.h"

#include "vtkMultiProcessController.h"

#include "vtkCommunicator.h"
#include "vtkMPICommunicator.h"

#include <sys/time.h>

#include <algorithm>
#include <iostream>
using namespace std;

enum {
	TAG_SENT_DEPTH_SIZE = 4122,
	TAG_SENT_DEPTH = 4123,
	TAG_SENT_DEPTH_SORTED = 4124
};

vtkStandardNewMacro(vtkDepthSortPoints);

//----------------------- parallel sorting beginnign ---------------------------------------
#define CONFIG_LEAF_SIZE 0x20
#define CONFIG_USE_TIME 0

typedef vtkSortValues elem_type;
typedef elem_type* iterator_type;

int operator < (const vtkSortValues& a, const vtkSortValues& b) {
	return a.z > b.z;
}

int operator > (const vtkSortValues& a, const vtkSortValues& b) {
	return a.z < b.z;
}

ostream& operator << (ostream& os, const vtkSortValues& a) {
	os << a.z;
	return os;
}

#if 0

// sequential code
static void naivesort(iterator_type i, iterator_type j)
{
  iterator_type m, n;
  iterator_type pos;
  elem_type val;

  for (m = i; m != j; ++m)
  {
    pos = m;
    val = *m;

    for (n = m + 1; n != j; ++n)
    {
      if (*n < val)
      {
	val = *n;
	pos = n;
      }
    }

    if (pos != m)
    {
      *pos = *m;
      *m = val;
    }
  }
}

// naivesort kaapi task

struct naivesortTask : public ka::Task<1>::Signature
< ka::RW<ka::range1d<elem_type> > > {};

template<> struct TaskBodyCPU<naivesortTask>
{
  void operator()(ka::range1d_rw<elem_type> r)
  {
    naivesort(r.ptr(), r.ptr() + r.size());
  }
};

// merge kaapi task
struct mergeTask : public ka::Task<2>::Signature
<
  ka::RW<ka::range1d<elem_type> >,
  ka::RW<ka::range1d<elem_type> >
> {};

template<> struct TaskBodyCPU<mergeTask>
{
  void operator()
  (ka::range1d_rw<elem_type> fu, ka::range1d_rw<elem_type> bar)
  {
    std::inplace_merge(fu.ptr(), bar.ptr(), bar.ptr() + bar.size());
  }
};

struct mergesortTask : public ka::Task<1>::Signature
< ka::RPWP<ka::range1d<elem_type> > > {};

template<> struct TaskBodyCPU<mergesortTask>
{
  void operator()(ka::range1d_rpwp<elem_type> range)
  {
    const size_t d = range.size();

    if (d <= CONFIG_LEAF_SIZE)
    {
      ka::Spawn<naivesortTask>()(range);
      return ;
    }

    const size_t pivot = d / 2;

    ka::array<1, elem_type> fu((elem_type*)range.ptr(), pivot);
    ka::Spawn<mergesortTask>()(fu);

    ka::array<1, elem_type> bar((elem_type*)range.ptr() + pivot, d - pivot);
    ka::Spawn<mergesortTask>()(bar);

    ka::Spawn<mergeTask>()(fu, bar);
  }
};


// parallel mergesort entrypoint
static void mergesort_par(iterator_type i, iterator_type j)
{
  ka::array<1, elem_type> fubar(i, j - i);
  // ka::Spawn<mergesortTask>(ka::SetStaticSched())(fubar);
  ka::Spawn<mergesortTask>()(fubar);
  ka::Sync();
}

struct doit
{
  void operator()(int count, char** _v)
  {
	vtkSortValues* v = (vtkSortValues*)*_v;
#if CONFIG_USE_TIME
    double t0, t1, tt = 0;

	/*
	cout << "before sort\n";
	for ( iterator_type s = v; s < v+count; s++ ) {
		cout << *s << "\n";
	}
	*/
#endif

#if CONFIG_USE_TIME
      t0 = kaapi_get_elapsedtime();
#endif

      mergesort_par(v, v + count);

#if CONFIG_USE_TIME
      t1 = kaapi_get_elapsedtime();
      // do not time first run
      tt += t1 - t0;

	  /*
	  cout << "after sort\n";
	  for ( iterator_type s = v; s < v+count; s++ ) {
		  cout << *s << "\n";
	  }
	  */
    std::cout << tt << std::endl; // seconds
#endif
  }
};
#endif

//----------------------- parallel sorting end ---------------------------------------
//
vtkDepthSortPoints::vtkDepthSortPoints()
{
	//cout << "vtkDepthSortPoints instantiated.\n";
	depth = NULL;
	sortScalars = NULL;
}

vtkDepthSortPoints::~vtkDepthSortPoints()
{
	if ( sortScalars ) sortScalars->Delete();
	if ( depth ) delete [] depth;
}

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
  //cout << "requested.\n";
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  //vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and output
  vtkPolyData *input = vtkPolyData::SafeDownCast( inInfo->Get(vtkDataObject::DATA_OBJECT()));
  //vtkPolyData *output = vtkPolyData::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));

  //static vtkSortValues *depth = NULL;
  //vtkIdType cellId, id;
  //vtkGenericCell *cell;
  //vtkIdType numCells=input->GetNumberOfCells();
  //vtkCellData *inCD=input->GetCellData();
  //vtkCellData *outCD=output->GetCellData();
  //static vtkUnsignedIntArray *sortScalars = NULL;
  unsigned int *scalars = NULL;
  double x[3];
  //double p[3], *bounds, *w = NULL, xf[3];
  double vector[3];
  double origin[3];
  //int type, npts, subId;
  //vtkIdType newId;
  //vtkIdType *pts;

  int numPoints = input->GetNumberOfPoints();
  int ptId;

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

  /*
  cell=vtkGenericCell::New();

  if ( this->DepthSortMode == VTK_SORT_PARAMETRIC_CENTER )
    {
    w = new double [input->GetMaxCellSize()];
    }
  */

  // Create temporary input
  /*
  static vtkPolyData *tmpInput = NULL;
  if ( tmpInput == NULL ) {
	  tmpInput = vtkPolyData::New();
  }
  tmpInput->CopyStructure(input);
  */

  int numProcs = this->Controller->GetNumberOfProcesses();
  int myId = this->Controller->GetLocalProcessId();


  /* each process needs to know the numPoints of all other processes to determine the offset of depth[].ptId 
   * because we will need to do the global depth sorting */
  int *allNumPoints = new int[numProcs];
  this->Controller->AllGather( (int*)&numPoints, (int*)allNumPoints, 1);

  this->Controller->Barrier();
  /*
  cout << "in process No. " << myId << "\n";
  for (vtkIdType id = 0; id < numProcs; id++) {
	  cout << allNumPoints[id] << "\n";
  }
  */

  // Compute the depth value per points, so this is done regardless of the DepthSortMode
  if ( depth == NULL ) 
  {
	  depth = new vtkSortValues [numPoints];
  }

  int idoffset = 0;
  for (vtkIdType id = 0; id < myId; id++) {
	  idoffset += allNumPoints[id];
  }

  for ( ptId=0; ptId < numPoints; ptId++ )
  {
	  input->GetPoint(ptId, x);
	  depth[ptId].z = vtkMath::Dot(x,vector);
	  depth[ptId].ptId= ptId + idoffset;
  }

  //this->UpdateProgress(0.20);

  /*
  //time_t ts, te;
  clock_t ts, te;
  //ts = (time_t) time (NULL);
  ts = (clock_t) clock();
  */

  // Sort the depths
  if ( this->Direction == VTK_DIRECTION_FRONT_TO_BACK )
    {
		qsort((void *)depth, numPoints, sizeof(vtkSortValues), vtkCompareFrontToBack);
		/*
		ka::SpawnMain<doit>()(numPoints, (char**)&depth); 
		ka::Sync();
		*/
    }
  else
    {
		/*
		ka::SpawnMain<doit>()(numPoints, (char**)&depth); 
		ka::Sync();
		*/
		qsort((void *)depth, numPoints, sizeof(vtkSortValues), vtkCompareBackToFront);
    }
  //this->UpdateProgress(0.60);
  //te = (time_t) time (NULL);
  /*
  te = (time_t) clock();
  //cout << "elapsed: " << (te - ts) << "\n";
  cout << "elapsed: " << (te - ts)/CLOCKS_PER_SEC << "\n";
  cout << "elapsed: " << (te - ts) << "\n";
  */

  int tnumPoints = 0;
  if ( this-Controller->AllReduce(&numPoints, &tnumPoints, 1, vtkCommunicator::SUM_OP) == 0 ) {
	  cout << "Reducing number of depth values from all computing nodes to Root process failed.\n";
	  delete [] allNumPoints;
	  return 1;
  }

  int *lut = new int[tnumPoints];
  this->Controller->Barrier();

  if ( myId == 0 ) {
	  int offset = 0;

	  vtkSortValues* tdepth = new vtkSortValues [tnumPoints];

	  std::copy( depth, depth + numPoints, tdepth );

	  /*
	  int ret = this->Controller->GetCommunicator()->GatherVoidArray( (char*)depth, (char*)tdepth, numPoints*sizeof(vtkSortValues), VTK_CHAR, 0);
	  cout << "ret= " << ret << "\n";
	  */
	  //vtkSmartPointer<vtkPolyData> tmpdata = vtkSmartPointer<vtkPolyData>::New();
	  int osize = 0;
	  
	  //std::vector<int> allsizes;
	  
	  for ( int id = 1; id < numProcs; ++id ) {
		  // receive data size firstly

		  /*
		  int ret = this->Controller->Receive(
				  (int*)&osize,
				  1,
				  id,
				  TAG_SENT_DEPTH_SIZE);
		  if ( !ret ) {
			  cout << "Rank #" << myId << ": receiving size of sorted depth values from Rank #" << id << " failed.\n";
		  }
		  */
		  osize = allNumPoints[id];

		  int ret = this->Controller->GetCommunicator()->ReceiveVoidArray(
				  (char*)(tdepth + numPoints + offset),
				  osize*sizeof(vtkSortValues),
				  VTK_CHAR,
				  id,
				  TAG_SENT_DEPTH);
		  if ( !ret ) {
			  cout << "Rank #" << myId << ": receiving sorted depth values from Rank #" << id << " failed.\n";
		  }
		  /*
		  ret = this->Controller->Receive(tmpdata, id, TAG_SENT_DEPTH);
		  cout << "ret3 = " << ret << "\n";
		  */

		  // merge_sort in place
		  std::inplace_merge( tdepth, tdepth + numPoints + offset, tdepth + numPoints + osize + offset );

		  //allsizes.push_back ( osize );
		  offset += osize;
	  }

	  /*
	  for (vtkIdType id = 0; id < tnumPoints; id ++ ) {
		  cout << tdepth[id].z << " " << tdepth[id].ptId << "\n";
	  }
	  */
	  for (vtkIdType id = 0; id < tnumPoints; id ++ ) {
		  lut[ tdepth[id].ptId ] = id;
	  }
	  delete [] tdepth;

	  /*
	  this->Controller->GetCommunicator()->BroadcastVoidArray(
			  (char*)tdepth,
			  tnumPoints*sizeof(vtkSortValues),
			  VTK_CHAR,
			  0);
	  */

	  this->Controller->GetCommunicator()->BroadcastVoidArray(
			  (int*)lut,
			  tnumPoints,
			  VTK_INT,
			  0);
	  /*
	  offset = 0;
	  for ( int id = 1; id < numProcs; ++id ) {
		  int ret = this->Controller->GetCommunicator()->SendVoidArray(
				  (char*)(tdepth + numPoints + offset),
				  allsizes[id-1]*sizeof(vtkSortValues),
				  VTK_CHAR,
				  id,
				  TAG_SENT_DEPTH_SORTED);
		  if ( !ret ) {
			  cout << "Rank #" << myId << ": sending globally sorted depth values to Rank #" << id << " failed.\n";
		  }
		  offset += allsizes[id-1];
	  }

	  for (vtkIdType id = 0; id < numPoints; id ++ ) {
		  depth[id].ptId = tdepth[id].ptId;
	  }
	  std::copy( tdepth, tdepth + numPoints, depth );
	  delete [] tdepth;
	  */
  }
  else {
	  // send data size firstly
	  /*
	  int ret = this->Controller->Send( 
			  (int*)&numPoints,
			  1,
			  0,
			  TAG_SENT_DEPTH_SIZE);
	  //cout << "ret = " << ret << "\n";
	  if ( !ret ) {
		  cout << "Rank #" << myId << ": sending size of locally sorted depth values failed.\n";
	  }
	  */

	  /*
	  vtkSmartPointer<vtkPoints> allPoints = vtkSmartPointer<vtkPoints>::New();
	  for (vtkIdType id = 0; id < numPoints; id ++) {
		  allPoints->InsertNextPoint( depth[id].ptId, depth[id].z, 0 );
	  }

	  vtkSmartPointer<vtkPolyData> tmpdata = vtkSmartPointer<vtkPolyData>::New();
	  tmpdata->SetPoints( allPoints );
	  ret = this->Controller->Send ( tmpdata, 0, TAG_SENT_DEPTH );
	  cout << "ret4 = " << ret << "\n";
	  */

	  // then send the locally sorted depth values
	  /*
	  int* iarray = new int[numPoints];
	  float* farray = new float[numPoints];

	  for (vtkIdType id = 0; id < numPoints; id ++) {
		  iarray[id] = depth[id].ptId;
		  farray[id] = depth[id].z;
	  }

	  ret = this->Controller->Send(
			  iarray,
			  10,
			  0,
			  TAG_SENT_DEPTH);
	  */

	  int ret = this->Controller->GetCommunicator()->SendVoidArray(
			  (char*)(depth),
			  numPoints*sizeof(vtkSortValues),
			  VTK_CHAR,
			  0,
			  TAG_SENT_DEPTH
		  );
	  //cout << "ret4 = " << ret << "\n";

	  if ( !ret ) {
		  cout << "Rank #" << myId << ": sending locally sorted depth values failed.\n";
	  }

	  //cout << "Process " << myId << " sending done.\n";
	  /*
	  ret = this->Controller->GetCommunicator()->BroadcastVoidArray(
			  (char*)tdepth,
			  tnumPoints*sizeof(vtkSortValues),
			  VTK_CHAR,
			  0);
	  */

	  this->Controller->GetCommunicator()->BroadcastVoidArray(
			  (int*)lut,
			  tnumPoints,
			  VTK_INT,
			  0);

	  /*
	  // receive globally sorted depth values
	  //vtkSortValues* odepth = new vtkSortValues[numPoints];
	  ret = this->Controller->GetCommunicator()->ReceiveVoidArray(
			  (char*)(depth),
			  numPoints*sizeof(vtkSortValues),
			  VTK_CHAR,
			  0,
			  TAG_SENT_DEPTH_SORTED
		  );
	  //cout << "ret4 = " << ret << "\n";
		*/

	  if ( !ret ) {
		  cout << "Rank #" << myId << ": receiving globally sorted depth values failed.\n";
	  }

	  /*
	  //std::copy( odepth, odepth + numPoints, depth );
	  for (vtkIdType id = 0; id < numPoints; id ++ ) {
		  depth[id].ptId = odepth[id].ptId;
	  }
	  */
  }

  this->Controller->Barrier();

  delete [] allNumPoints;

  // Generate sorted output
  if ( this->SortScalars )
    {
		if ( sortScalars == NULL ) {
			sortScalars = vtkUnsignedIntArray::New();
			sortScalars->SetName("sort scalars");
			sortScalars->SetNumberOfTuples(numPoints);
		}

		scalars = sortScalars->GetPointer(0);
		for ( ptId=0; ptId < numPoints; ptId++ ) {
			scalars[ ptId ] = lut[ ptId + idoffset ];
			//scalars[ ptId ] = lut[ depth[ptId].ptId ];
			/*
			int newid = -1;
			for (int id = 0; id < tnumPoints; id ++) {
				if ( tdepth[id].ptId == ptId + idoffset ) {
					newid = id;
					break;
				}
			}

			if ( newid != -1 ) {
				scalars[ptId] = newid;
			}
			*/
		}

		if ( input->GetPointData()->GetArray("sort scalars") ) {
			input->GetPointData()->RemoveArray("sort scalars");
		}
		int idx = input->GetPointData()->AddArray(sortScalars);
		input->GetPointData()->SetActiveAttribute(idx, vtkDataSetAttributes::SCALARS);
		//input->GetPointData()->SetScalars( sortScalars );
		//cout << "input's scalar changed.\n";
    }

  delete [] lut;

  return 1;

  /*
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

  // Points are left alone
  output->SetPoints(input->GetPoints());
  output->GetPointData()->PassData(input->GetPointData());

  this->UpdateProgress(0.90);
  */

  if ( this->SortScalars )
    {
	/*
    int idx = output->GetCellData()->AddArray(sortScalars);
    output->GetCellData()->SetActiveAttribute(idx, vtkDataSetAttributes::SCALARS);
	*/
	//int idx = output->GetPointData()->AddArray(sortScalars);
    //output->GetPointData()->SetActiveAttribute(idx, vtkDataSetAttributes::SCALARS);
	
	//output->GetPointData()->SetScalars( sortScalars );
    //sortScalars->Delete();
    }

  // Clean up and get out    
  /*
  tmpInput->Delete();
  delete [] depth;
  cell->Delete();
  output->Squeeze();
  */

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

void vtkDepthSortPoints::SetController(vtkMultiProcessController *controller)
{
  //vtkSetObjectBodyMacro(Controller,vtkMultiProcessController,controller)
  this->Controller = controller;
}

