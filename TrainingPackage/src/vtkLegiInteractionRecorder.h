// ----------------------------------------------------------------------------
// vtkLegiInteractionRecorder.h : extension to vtkLegiInteractionRecorder for 
//		logging interaction in the format that contains time stamp with support
//		of serializing into ostream rather than a common file
//
// Creation : Dec. 12th 2011
// Revisions:
//	Dec. 17th fix the bug that causes collection of unwanted event logs
//			; and apply the format of logs on rotation related events that was used
//			in previous studies in order to reuse existing log analyzer
//
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------

#ifndef __vtkLegiInteractionRecorder_h
#define __vtkLegiInteractionRecorder_h

#include <vtkInteractorEventRecorder.h>
#include "point.h"

// The superclass that all commands should be subclasses of
class VTK_RENDERING_EXPORT vtkLegiInteractionRecorder : public vtkInteractorEventRecorder
{
public:
  static vtkLegiInteractionRecorder *New();
  vtkTypeMacro(vtkLegiInteractionRecorder,vtkInteractorEventRecorder);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetOutputStream(ostream* os) { OutputStream = os; }

  bool	m_bLeftButtonPressed;
  point_t	m_lastpos;

  vtkSetMacro(StudyLogStyle, int);
  vtkGetMacro(StudyLogStyle, int);
  vtkBooleanMacro(StudyLogStyle, int);

protected:
  vtkLegiInteractionRecorder();
  ~vtkLegiInteractionRecorder();

  //methods for processing events
  static void ProcessEvents(vtkObject* object, unsigned long event,
                            void* clientdata, void* calldata);

  void WriteEvent(const char* event, int pos[2], int ctrlKey,
                          int shiftKey, int keyCode, int repeatCount,
                          char* keySym);
  
private:
  vtkLegiInteractionRecorder(const vtkLegiInteractionRecorder&);  // Not implemented.
  void operator=(const vtkLegiInteractionRecorder&);  // Not implemented.
  
  int		StudyLogStyle;
};

#endif /* __vtkLegiInteractionRecorder_h */
 
/* sts=8 ts=8 sw=80 tw=8 */

