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
#include "vtkPolyDataAlgorithm.h"
#include "vtkActor.h"
#include "vtkMapper.h"
#include "QVTKWidget.h"
#include "vtkTubeFilter.h"

vtkStandardNewMacro(vtkLegiInteractorStyle);

//----------------------------------------------------------------------------
vtkLegiInteractorStyle::vtkLegiInteractorStyle() 
{
	m_bLeftButtonDown = false;
	agent = NULL;
	agent2 = NULL;
	streamtube = NULL;
	actor = NULL;
}

//----------------------------------------------------------------------------
vtkLegiInteractorStyle::~vtkLegiInteractorStyle() 
{
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::OnMiddleButtonDown() 
{
	this->Superclass::OnMiddleButtonDown();
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::OnMiddleButtonUp()
{
	this->Superclass::OnMiddleButtonUp();
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::OnLeftButtonDown() 
{
	this->Superclass::OnLeftButtonDown();
	m_bLeftButtonDown = true;
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::OnLeftButtonUp()
{
	this->Superclass::OnLeftButtonUp();

	m_bLeftButtonDown = false;
	return;

	updateDataPipeline();
	this->GetInteractor()->GetRenderWindow()->Render();
	return;

	if ( agent != NULL && agent->GetTotalNumberOfInputConnections() >= 1) 
	{
		agent->Update();
		if ( actor ) {
			actor->GetMapper()->GetInput()->Modified();
			//cout << "mapper updated.\n";
			this->GetInteractor()->GetRenderWindow()->Render();
		}
		//cout << "update agent.\n";
	}
	//else cout << "agent is still NULL!\n";

	/*
	this->GetInteractor()->GetRenderWindow()->PrintSelf(cout, vtkIndent());
	*/
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::OnRightButtonDown() 
{
	this->Superclass::OnRightButtonDown();
	/*
	*/
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::OnRightButtonUp()
{
	this->Superclass::OnRightButtonUp();
	/*
	this->GetInteractor()->GetRenderWindow()->PrintSelf(cout, vtkIndent());
	*/
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::OnMouseWheelForward() 
{
	this->Superclass::OnMouseWheelForward();
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::OnMouseWheelBackward()
{
	this->Superclass::OnMouseWheelBackward();
}

void vtkLegiInteractorStyle::OnMouseMove()
{
	this->Superclass::OnMouseMove();

	/*
	static int i = 0;
	if ( ++i % 2 != 0 ) {
		return;
	}
	*/

	bool updateActor = false;

	if ( m_bLeftButtonDown ) {
		if ( agent2 != NULL && agent2->GetInput() != NULL && vtkTubeFilter::SafeDownCast(streamtube)->GetVaryRadius() > 0) 
		//if ( agent2 != NULL && streamtube->GetInput() != NULL ) 
		{
			agent2->Update();
			streamtube->GetInput()->Modified();
			updateActor = false;
		}

		if ( agent != NULL && agent->GetTotalNumberOfInputConnections() >= 1) 
		{
			agent->Update();
			updateActor = true;
		}

		if ( updateActor && actor ) {
			actor->GetMapper()->GetInput()->Modified();
			//cout << "mapper updated.\n";
		}
		//cout << "update agent.\n";

		//else cout << "agent is still NULL!\n";
	}
}

void vtkLegiInteractorStyle::updateDataPipeline()
{
	//if ( m_bLeftButtonDown ) 
	{
		if ( agent2 != NULL && agent2->GetInput() != NULL && vtkTubeFilter::SafeDownCast(streamtube)->GetVaryRadius() > 0) 
		{
			agent2->Update();
			streamtube->GetInput()->Modified();
		}

		if ( agent != NULL && agent->GetTotalNumberOfInputConnections() >= 1) 
		{
			agent->Update();
			if ( actor ) {
				actor->GetMapper()->GetInput()->Modified();
			}
		}
	}
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::Rotate()
{
	this->Superclass::Rotate();
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::Spin()
{
	this->Superclass::Spin();
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
	this->Superclass::Dolly();
}

//----------------------------------------------------------------------------
void vtkLegiInteractorStyle::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Zooming/panning disabled." << "\n";
}

