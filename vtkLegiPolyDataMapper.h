// ----------------------------------------------------------------------------
// vtkLegiPolyDataMapper.h : extension to vtkOpenGLPolyDataMapper for 
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
#ifndef __vtkLegiPolyDataMapper_h
#define __vtkLegiPolyDataMapper_h

#include "vtkOpenGLPolyDataMapper.h"

#include "vtkOpenGL.h" // Needed for GLenum

class vtkCellArray;
class vtkPoints;
class vtkProperty;
class vtkRenderWindow;
class vtkOpenGLRenderer;
class vtkOpenGLTexture;

class VTK_RENDERING_EXPORT vtkLegiPolyDataMapper : public vtkOpenGLPolyDataMapper
{
public:
  static vtkLegiPolyDataMapper *New();
  vtkTypeMacro(vtkLegiPolyDataMapper,vtkOpenGLPolyDataMapper);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Implement superclass render method.
  virtual void RenderPiece(vtkRenderer *ren, vtkActor *a);

  // Description:
  // Release any graphics resources that are being consumed by this mapper.
  // The parameter window could be used to determine which graphic
  // resources to release.
  void ReleaseGraphicsResources(vtkWindow *);

  // Description:
  // Draw method for OpenGL.
  virtual int Draw(vtkRenderer *ren, vtkActor *a);
  
protected:
  vtkLegiPolyDataMapper();
  ~vtkLegiPolyDataMapper();

  void DrawPoints(int idx,
                  vtkPoints *p, 
                  vtkDataArray *n,
                  vtkUnsignedCharArray *c,
                  vtkDataArray *t,
                  vtkIdType &cellNum,
                  int &noAbort,
                  vtkCellArray *ca,
                  vtkRenderer *ren);
  
  void DrawLines(int idx,
                 vtkPoints *p, 
                 vtkDataArray *n,
                 vtkUnsignedCharArray *c,
                 vtkDataArray *t,
                 vtkIdType &cellNum,
                 int &noAbort,
                 vtkCellArray *ca,
                 vtkRenderer *ren);

  void DrawPolygons(int idx,
                    vtkPoints *p, 
                    vtkDataArray *n,
                    vtkUnsignedCharArray *c,
                    vtkDataArray *t,
                    vtkIdType &cellNum,
                    int &noAbort,
                    GLenum rep,
                    vtkCellArray *ca,
                    vtkRenderer *ren);

  void DrawTStrips(int idx,
                   vtkPoints *p, 
                   vtkDataArray *n,
                   vtkUnsignedCharArray *c,
                   vtkDataArray *t,
                   vtkIdType &cellNum,
                   int &noAbort,
                   GLenum rep,
                   vtkCellArray *ca,
                   vtkRenderer *ren);
    
  vtkIdType TotalCells;
  int ListId;
  vtkOpenGLTexture* InternalColorTexture;

private:
  vtkLegiPolyDataMapper(const vtkLegiPolyDataMapper&);  // Not implemented.
  void operator=(const vtkLegiPolyDataMapper&);  // Not implemented.
};

#endif
