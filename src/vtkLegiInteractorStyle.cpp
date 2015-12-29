// ----------------------------------------------------------------------------
// vtkLegiInteractorStyle.cpp : an extension to vtkLegiInteractorStyle
//	for customizing the option of turning zooming/panning on or off 
//
// Creation : Dec. 13th 2011
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#include "vtkLegiInteractorStyle.h"

#include "vtkCamera.h"
#include "vtkCallbackCommand.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"

vtkStandardNewMacro(vtkLegiInteractorStyle);

//----------------------------------------------------------------------------
vtkLegiInteractorStyle::vtkLegiInteractorStyle() 
{
}

//----------------------------------------------------------------------------
vtkLegiInteractorStyle::~vtkLegiInteractorStyle() 
{
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::OnMiddleButtonDown() 
{
	//this->Superclass::OnMiddleButtonDown();
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::OnMiddleButtonUp()
{
	//this->Superclass::OnMiddleButtonUp();
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::OnRightButtonDown() 
{
	/*
	this->Superclass::OnRightButtonDown();
	*/
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::OnRightButtonUp()
{
	/*
	this->Superclass::OnRightButtonUp();
	this->GetInteractor()->GetRenderWindow()->PrintSelf(cout, vtkIndent());
	*/
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::OnMouseWheelForward() 
{
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::OnMouseWheelBackward()
{
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::Rotate()
{
	this->Superclass::Rotate();
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::Spin()
{
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::Pan()
{
	this->Superclass::Pan();
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::Dolly()
{
	/* zooming in/out actually */
	//this->Superclass::Dolly();
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Zooming/panning disabled." << "\n";
}

