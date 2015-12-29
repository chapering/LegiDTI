// ----------------------------------------------------------------------------
// vtkLegiRenderManager.h : an extension to vtkCompositeRenderManager
//	for customizing camera update in all satellite renderers in their corresponding
//	children processes within a global parallel rendering control
//
// Creation : Jan. 19th 2012
// revision : 
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
//
#ifndef __vtkLegiRenderManager_h
#define __vtkLegiRenderManager_h

#include "vtkCompositeRenderManager.h"

class vtkCompositer;
class vtkFloatArray;

class VTK_PARALLEL_EXPORT vtkLegiRenderManager : public vtkCompositeRenderManager
{
public:
  vtkTypeMacro(vtkLegiRenderManager, vtkCompositeRenderManager);
  static vtkLegiRenderManager *New();
  virtual void PrintSelf(ostream &os, vtkIndent indent);

  void GenericUpdateCamera();

  void WriteFullImage();

protected:
  vtkLegiRenderManager();
  ~vtkLegiRenderManager();

  virtual void PreRenderProcessing();
  virtual void PostRenderProcessing();

  virtual void UpdateCamera();

  virtual void SatelliteUpdateCamera();

private:
  vtkLegiRenderManager(const vtkLegiRenderManager &);//Not implemented
  void operator=(const vtkLegiRenderManager &);  //Not implemented
};

#endif //__vtkLegiRenderManager_h
