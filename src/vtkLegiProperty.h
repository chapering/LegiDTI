// ----------------------------------------------------------------------------
// vtkLegiProperty.h : extension to vtkOpenGLProperty for 
//		"fixing" the bug that causes the disappearance of vtkScalarBarActor in
//		the presence of another actor with the property having the 
//		FrontfaceCullingOn
//
// Creation : Dec. 14th 2011
// Revisions:
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#ifndef __vtkLegiProperty_h
#define __vtkLegiProperty_h

#include "vtkOpenGLProperty.h"

class VTK_RENDERING_EXPORT vtkLegiProperty : public vtkOpenGLProperty
{
public:
  static vtkLegiProperty *New();
  vtkTypeMacro(vtkLegiProperty,vtkOpenGLProperty);

  // Description:
  // Implement base class method.
  void Render(vtkActor *a, vtkRenderer *ren);

  void PrintSelf(ostream& os, vtkIndent indent);
protected:
  vtkLegiProperty();
  ~vtkLegiProperty();

private:
  vtkLegiProperty(const vtkLegiProperty&);  // Not implemented.
  void operator=(const vtkLegiProperty&);  // Not implemented.
};

#endif
