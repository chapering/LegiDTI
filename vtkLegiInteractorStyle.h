// ----------------------------------------------------------------------------
// vtkLegiInteractorStyle.h : an extension to vtkInteractorStyleTrackballCamera
//	for customizing the option of turning zooming/panning on or off 
//
// Creation : Dec. 13th 2011
// revision : 
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#ifndef __vtkLegiInteractorStyle_h
#define __vtkLegiInteractorStyle_h

#include <vtkInteractorStyleTrackballCamera.h>

class vtkPolyDataAlgorithm;
class vtkActor;

class VTK_RENDERING_EXPORT vtkLegiInteractorStyle  : public vtkInteractorStyleTrackballCamera 
{
public:
  static vtkLegiInteractorStyle *New();
  vtkTypeMacro(vtkLegiInteractorStyle,vtkInteractorStyleTrackballCamera);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();
  virtual void OnMiddleButtonDown();
  virtual void OnMiddleButtonUp();
  virtual void OnRightButtonDown();
  virtual void OnRightButtonUp();
  virtual void OnMouseWheelForward();
  virtual void OnMouseWheelBackward();
  virtual void OnMouseMove();

  virtual void Rotate();
  virtual void Spin();
  virtual void Pan();
  virtual void Dolly();

  void updateDataPipeline();

  vtkPolyDataAlgorithm* agent;
  vtkPolyDataAlgorithm* agent2;
  vtkPolyDataAlgorithm* streamtube;
  vtkActor*	actor;

protected:
  vtkLegiInteractorStyle();
  ~vtkLegiInteractorStyle();

  bool m_bLeftButtonDown;
private:
  vtkLegiInteractorStyle(const vtkLegiInteractorStyle&);  // Not implemented.
  void operator=(const vtkLegiInteractorStyle&);  // Not implemented.
};

#endif
