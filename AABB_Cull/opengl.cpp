//============================================================================
// opengl.cpp
//============================================================================

#include "stdafx.h"
#include "opengl.hpp"
#include "camera.hpp"


//----------------------------------------------------------------------------
// EXTERNS
//----------------------------------------------------------------------------
void DrawScene();


//----------------------------------------------------------------------------
// GLOBALS (CAMERA, MODE, MOD KEYS, OLD MOUSE POS, MODEL EXTENTS, UP VECTOR)
//----------------------------------------------------------------------------
Camera Cam;
int OldMouseX, OldMouseY, LeftButtonDown=0, RightButtonDown=0;
int Mode=0;
Vect3d ModelCenter(0,0,0);
float TRANS=0.003;  // TRANSLATION SENSITIVITY


//----------------------------------------------------------------------------
// RECORDING AND PLAYBACK VARIABLES
//----------------------------------------------------------------------------
FILE* FilePtr;
int RecordOn=0;
#define RECORD(M) if(RecordOn){ int i; for(i=0;i<16;i++) fprintf(FilePtr,"%f ",M[i]); }


//----------------------------------------------------------------------------
// OpenGL DISPLAY routine.
//----------------------------------------------------------------------------
void myDisplay(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  GLfloat M[16];
  glLoadMatrixf( Cam.GetViewMatrix(M) );
  RECORD(M);

  DrawScene();

  glutSwapBuffers();
}


//----------------------------------------------------------------------------
// OpenGL RESHAPE routine (NOTE: resets to original VIEWING position).
//----------------------------------------------------------------------------
void myReshape(int w, int h)
{
  glViewport(0, 0, w, h);  // LAST STAGE IN THE PIPE (AFTER PROJ AND MODELVIEW)

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, (GLfloat)w/(GLfloat)h, 0.5, 5000);

  Cam.Perspective(40, (GLfloat)w/(GLfloat)h, 1, 5000);
}


//----------------------------------------------------------------------------
// OpenGL GLUT INPUT HANDLING routines.
//----------------------------------------------------------------------------
void KeyBoardHandler(unsigned char Key, int x, int y)
{
  switch(Key)
  {
    case 'z': Mode=0; break;
    case 'x': Mode=1; break;
    case 'c': Mode=2; break;
    case 'v': Mode=3; break;
    case ' ': if (LeftButtonDown)
                if (RightButtonDown) RightButtonDown=0; else RightButtonDown=1; 
              break;
  };
}

void SpecialKeyInputHandler(int Key, int x, int y)
{
  switch(Key)
  {
    case GLUT_KEY_UP: Cam.Move(Vect3d(0,0,-1)); break;
    case GLUT_KEY_DOWN: Cam.Move(Vect3d(0,0,1)); break;
    case GLUT_KEY_LEFT: Cam.Yaw(0.04); break;
    case GLUT_KEY_RIGHT: Cam.Yaw(-0.04); break;
  };
  glutPostRedisplay();
}

void MouseInputHandler(int button, int state, int x, int y)
{
  if (button==GLUT_LEFT_BUTTON)
  {
    if (state==GLUT_DOWN)
    { 
      OldMouseX=x; OldMouseY=y; LeftButtonDown=1; 
      if (glutGetModifiers()==GLUT_ACTIVE_SHIFT) // SHIFT ACTS AS RIGHT-BUTTON
         RightButtonDown=1;
      glutDetachMenu(GLUT_RIGHT_BUTTON);
	 }
    else // GLUT_UP
    { 
      LeftButtonDown=RightButtonDown=0;
      glutAttachMenu(GLUT_RIGHT_BUTTON);
	 }
  }
  else if (button==GLUT_RIGHT_BUTTON)
  {
    if (state==GLUT_DOWN) RightButtonDown=1;
    else RightButtonDown=0;
  }
}

void MouseMoveHandler(int x, int y)  // ONLY CALLED WHEN MOUSE IS "DRAGGED"
{
  if (LeftButtonDown)
  {
    float RelX=x-OldMouseX, RelY=y-OldMouseY;
    OldMouseX=x; OldMouseY=y;
    if (Mode==2) // (MOVE IN XY OR XZ PLANE)
    {
		if (RightButtonDown) { float TransX=RelX*TRANS, TransZ=RelY*TRANS;
									  Cam.Move(Vect3d(-TransX,0,-TransZ)); }
		else { float TransX=RelX*TRANS, TransY=RelY*TRANS;
				 Cam.Move(Vect3d(-TransX,TransY,0)); }
    }
    else if (Mode==1) // DRIVE MODE
    {
		if (RightButtonDown) { float RotZ = RelY*0.003; Cam.Roll(RotZ); }
		else { float TransZ=RelY*TRANS, RotY=RelX*0.003;
				 Cam.Move(Vect3d(0,0,TransZ)); Cam.Yaw(-RotY); }
	 }
	 else if (Mode==3) // LOOK MODE
	 {
		if (RightButtonDown) { float RotZ=RelY*0.003; Cam.Roll(RotZ); }
		else { float RotX=RelY*0.003, RotY=RelX*0.003;
				 Cam.Pitch(RotX); Cam.Yaw(-RotY); }
	 }
	 else // TRACKBALL (MODE=0)
	 {
		if (RightButtonDown) { float RotZ=RelY*0.005;
                           Cam.TrackBallRotate(ModelCenter,-RotZ,2); }
		else { float RotX=RelY*0.005, RotY=RelX*0.005;
           Cam.TrackBallRotate(ModelCenter,-RotX,0);
           Cam.TrackBallRotate(ModelCenter,RotY,1); }
    }

    glutPostRedisplay();
  }
}

void MenuHandler(int value)
{
  switch(value)
  {
    case 0: case 1: case 2: case 3:  // TRACKBALL, DRIVE, TRANSLATE, LOOK
      Mode=value; 
      break;
    case 4:  // RECORD
      if (RecordOn) { printf("\nALREADY RECORDING!\n"); }
      else {
      RecordOn=1;
      FilePtr = fopen("record0.dat","w");
		printf("STARTED RECORDING!\n"); }
      break;
    case 5:  // PLAY
      if (RecordOn) { RecordOn=0; fclose(FilePtr); printf("\nDONE RECORDING!\n"); }
      FilePtr = fopen("record0.dat","r");
      if (FilePtr==NULL) { printf("THIS PLAYBACK BANK IS EMPTY!\n"); }
      else { printf("\nSTARTING PLAYBACK!\n"); StartPlayBack(); }
      break;
    case 6:  // STOP
      if (RecordOn) { RecordOn=0; fclose(FilePtr); printf("\nDONE RECORDING!\n"); }
      else { printf("NOT RECORDING!\n"); }
      break;
  };
}


//----------------------------------------------------------------------------
// PLAYBACK A RECORDED "PATH" FILE (SAME AS A DISPLAY BUT CAM IS READ IN)
//----------------------------------------------------------------------------
void StartPlayBack(void)
{
  glMatrixMode(GL_MODELVIEW);
  GLfloat M[16];
  int i, Cond;
  do {
    for(i=0;i<16;i++) Cond=fscanf(FilePtr,"%f",&M[i]); // READ IN 4X4 CAM MATRIX
    if (Cond!=EOF)     // DISPLAY A FRAME (IF NOT EOF)
    {  
      glLoadMatrixf(M);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      DrawScene();
      glutSwapBuffers();
    }
  } while(Cond!=EOF);
  fclose(FilePtr);
  printf("\nPLAYBACK DONE!\n");
}


//----------------------------------------------------------------------------
// OpenGL main event loop routine (inits aux and opengl)
//----------------------------------------------------------------------------
void GoOpenGL(float minx, float miny, float minz,
              float maxx, float maxy, float maxz)
{
  // SET INITIAL VIEW
  Vect3d ModelMin(minx,miny,minz), ModelMax(maxx,maxy,maxz);
  ModelCenter = (ModelMax+ModelMin)*0.5;
  Vect3d ViewPos = ModelCenter + ((ModelMax-ModelCenter)*1.5);
  Cam.LookAt(ViewPos,ModelCenter,Vect3d(0,1,0));

  // SET TRANSLATION SCALES
  Vect3d Dim = ModelMax-ModelMin;
  float MaxDim=Dim.x;
  if (Dim.y>MaxDim) MaxDim=Dim.y;
  if (Dim.z>MaxDim) MaxDim=Dim.z;
  TRANS*=(MaxDim*0.5);

  // OPENGL STUFF
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
  glutInitWindowPosition(50,50);
  glutInitWindowSize(300,300);
  glutCreateWindow("Kenny's FAST Basic Viewer");

  glShadeModel(GL_FLAT);
//  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  glutKeyboardFunc(KeyBoardHandler);
  glutSpecialFunc(SpecialKeyInputHandler);
  glutMouseFunc(MouseInputHandler);
  glutMotionFunc(MouseMoveHandler);

  glutCreateMenu(MenuHandler);
	 glutAddMenuEntry("TrackBall (z)",0);
	 glutAddMenuEntry("Drive (x)",1);
	 glutAddMenuEntry("Translate (c)",2);
	 glutAddMenuEntry("Look (v)",3);
	 glutAddMenuEntry("Record",4);
	 glutAddMenuEntry("Play",5);
	 glutAddMenuEntry("Stop",6);
	 glutAttachMenu(GLUT_RIGHT_BUTTON);

  glutReshapeFunc(myReshape);
  glutDisplayFunc(myDisplay);
  glutMainLoop();
}

