// ----------------------------------------------------------------------------
// vtkLegiProperty.cpp : extension to vtkLegiProperty for 
//		"fixing" the bug that causes the disappearance of vtkScalarBarActor in
//		the presence of another actor with the property having the 
//		FrontfaceCullingOn
//
// Creation : Dec. 14th 2011
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#include "vtkOpenGLRenderer.h"
#include "vtkLegiProperty.h"
#ifndef VTK_IMPLEMENT_MESA_CXX
# include "vtkOpenGL.h"
#endif

#include "vtkObjectFactory.h"
#include "vtkgl.h" // vtkgl namespace
#include <assert.h>

#ifndef VTK_IMPLEMENT_MESA_CXX
vtkStandardNewMacro(vtkLegiProperty);
#endif

vtkLegiProperty::vtkLegiProperty()
{
}

vtkLegiProperty::~vtkLegiProperty()
{
}

// ----------------------------------------------------------------------------
// Implement base class method.
void vtkLegiProperty::Render(vtkActor *anActor,
                               vtkRenderer *ren)
{

  //------------------------------ to fix the "vtkScalarBarActor disappearing" bug -------------
	/*
    static int i=0;
	if (i<=1) {
		cout << "--- saved here in vtkLegiProperty ---\n";
		glPushAttrib(GL_ALL_ATTRIB_BITS);
	}
	i++;
	*/
	//glPushClientAttrib(GL_ALL_CLIENT_ATTRIB_BITS);

	this->Superclass::Render(anActor, ren);

	//glPopClientAttrib();
	//glPopAttrib();
  //------------------------------ to fix the "vtkScalarBarActor disappearing" bug -------------
}

//----------------------------------------------------------------------------
void vtkLegiProperty::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  
  os << indent << "Disappearing vtkScalarBarActor bug fixed.\n";
}

