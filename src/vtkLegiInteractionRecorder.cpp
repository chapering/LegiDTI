// ----------------------------------------------------------------------------
// vtkLegiInteractionRecorder.cpp : extension to vtkLegiInteractionRecorder for 
//		logging interaction in the format that contains time stamp with support
//		of serializing into ostream rather than a common file
//
// Creation : Dec. 12th 2011
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#include "vtkLegiInteractionRecorder.h"
#include "vtkCallbackCommand.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindowInteractor.h"

#include "cppmoth.h"

#include <vtksys/ios/sstream>
#include <locale>
#include <vtksys/SystemTools.hxx>

vtkStandardNewMacro(vtkLegiInteractionRecorder);

//----------------------------------------------------------------------------
vtkLegiInteractionRecorder::vtkLegiInteractionRecorder()
	: m_bLeftButtonPressed(false)
{
  this->EventCallbackCommand->SetCallback(vtkLegiInteractionRecorder::ProcessEvents);
  this->EventCallbackCommand->SetPassiveObserver(1); // get events first
}

//----------------------------------------------------------------------------
vtkLegiInteractionRecorder::~vtkLegiInteractionRecorder()
{
	// avoid the special ostream pointer to be destroyed in parent's destructor
	this->OutputStream = NULL;
}

//----------------------------------------------------------------------------
void vtkLegiInteractionRecorder::ProcessEvents(vtkObject* object, 
                                               unsigned long event,
                                               void* clientData, 
                                               void* vtkNotUsed(callData))
{
	vtkLegiInteractionRecorder* self = reinterpret_cast<vtkLegiInteractionRecorder *>( clientData );
	vtkRenderWindowInteractor* rwi = static_cast<vtkRenderWindowInteractor *>( object );

	if ( event != vtkCallbackCommand::LeftButtonPressEvent &&
		event != vtkCallbackCommand::LeftButtonReleaseEvent &&
		event != vtkCallbackCommand::MouseMoveEvent ) {
		return;
	}

	// all events are processed
	if ( self->State == vtkLegiInteractionRecorder::Recording ) {
		switch(event) {
			case vtkCommand::ModifiedEvent: //dont want these
				break;
			case vtkCommand::LeftButtonPressEvent:
				if ( self->FileName != NULL ) { // logging event through ordinary file stream
					break;
				}
				self->m_lastpos.update( rwi->GetEventPosition()[0], rwi->GetEventPosition()[1] );
				self->m_bLeftButtonPressed = true;
				*(MyCout*)self->OutputStream << "rotating by mouse.\n";
				break;
			case vtkCommand::MouseMoveEvent:
				if (!self->StudyLogStyle || self->m_bLeftButtonPressed) {
					self->WriteEvent(vtkCommand::GetStringFromEventId(event),
							rwi->GetEventPosition(), rwi->GetControlKey(), 
							rwi->GetShiftKey(), rwi->GetKeyCode(),
							rwi->GetRepeatCount(), rwi->GetKeySym());
				}
				break;
			case vtkCommand::LeftButtonReleaseEvent:
				if ( self->FileName != NULL ) { // logging event through ordinary file stream
					break;
				}
				self->m_bLeftButtonPressed = false;
				*(MyCout*)self->OutputStream << "rotating finished.\n";
				break;
			default:
				break;
		}
		self->OutputStream->flush();
	}
}

//----------------------------------------------------------------------------
void vtkLegiInteractionRecorder::WriteEvent(const char* event, int pos[2], 
                                            int ctrlKey, int shiftKey, 
                                            int keyCode, int repeatCount,
                                            char* keySym)
{
	if ( FileName != NULL ) { // logging event through ordinary file stream
		this->Superclass::WriteEvent( event, pos, ctrlKey, shiftKey, keyCode, repeatCount, keySym);
		return;
	}

	if ( StudyLogStyle ) {
		double dx = pos[0] - m_lastpos[0], dy = pos[1] - m_lastpos[1], dz = 0.0;
		double a, b, c, d=0;
		double fangle = -1.0;

		a = -dy, b = -dx, c = dz, d=0;
		*(MyCout*)this->OutputStream << "rotating around axis= (" << a << "," << 
			b << "," << c << ") by angle=" << fangle << " by mouse.\n";

		m_lastpos.update( pos[0], pos[1] );
		return;
	}

	*(MyCout*)this->OutputStream << event << " " << pos[0] << " " << pos[1] << " "
                      << ctrlKey << " " << shiftKey << " "
                      << keyCode << " " << repeatCount << " ";
	if ( keySym ) {
		*(MyCout*)this->OutputStream << keySym << "\n";
    }
	else {
		*(MyCout*)this->OutputStream << "0\n";
    }
}

void vtkLegiInteractionRecorder::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);

	if ( FileName == NULL && OutputStream != NULL ) {
		os << indent << " MyCout: mounted in.\n";
	}
	else {
		os << indent << " MyCout: (None).\n";
	}
}

/* sts=8 ts=8 sw=80 tw=8 */
