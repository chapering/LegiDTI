// ----------------------------------------------------------------------------
// vtkTubeHaloMapper.h : extension to vtkOpenGLPolyDataMapper for making halos
//
// Creation : Nov. 17th 2011
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------

#ifndef __vtkTubeHaloMapper_h
#define __vtkTubeHaloMapper_h

#include "vtkOpenGLPolyDataMapper.h"

#include "vtkOpenGL.h" // Needed for GLenum

class vtkCellArray;
class vtkPoints;
class vtkProperty;
class vtkRenderWindow;
class vtkOpenGLRenderer;
class vtkOpenGLTexture;

class VTK_RENDERING_EXPORT vtkTubeHaloMapper: public vtkOpenGLPolyDataMapper 
{
public:
  static vtkTubeHaloMapper* New();
  vtkTypeMacro(vtkTubeHaloMapper, vtkOpenGLPolyDataMapper);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Implement superclass render method.
  virtual void RenderPiece(vtkRenderer *ren, vtkActor *a);

  void ReleaseGraphicsResources(vtkWindow *win);

  // Description:
  // Draw method for OpenGL.
  virtual int Draw(vtkRenderer *ren, vtkActor *a);
  
protected:
  vtkTubeHaloMapper();
  ~vtkTubeHaloMapper();

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
    
private:
  vtkTubeHaloMapper(const vtkTubeHaloMapper&);  // Not implemented.
  void operator=(const vtkTubeHaloMapper&);  // Not implemented.
};

#endif
