//============================================================================
// opengl.hpp
//============================================================================


//----------------------------------------------------------------------------
// OPENGL INCLUDE FILES (be sure to specify the correct paths)
//----------------------------------------------------------------------------
#ifndef XWINDOWS
  #include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>


//----------------------------------------------------------------------------
// CALLBACK FUNCTION PROTOTYPES (Display, Reshape, User Input)
//----------------------------------------------------------------------------
void myDisplay(void);
void myReshape(int w, int h);

void KeyboardHandler(int Key, int x, int y);
void SpecialKeyInputHandler(int Key, int x, int y);
void MouseInputHandler(int button, int state, int x, int y);
void MouseMoveHandler(int x, int y);
void MenuHandler(int value);


//----------------------------------------------------------------------------
// OPENGL INITIALIZATION FUNCTION PROTOTYPES
//----------------------------------------------------------------------------
void GoOpenGL(float minx=-1, float miny=-1, float minz=-1, 
              float maxx=1, float maxy=1, float maxz=1);
void StartPlayBack(void);

