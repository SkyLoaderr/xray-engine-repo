#include "stdafx.h"
#include "opengl.hpp"
#include "camera.hpp"
#include "aabbtree.hpp"
#include "boxvfc.hpp"


extern Camera Cam;  // CAMERA USED IN opengl.cpp
AABBtree* AABBt;    // MADE GLOBAL ONLY FOR opengl.cpp DrawScene CALLBACK


//--------------------------------------------------------------------------
// AABB HANDLING ROUTINES FOR COMPLETE OUT, COMPLETE IN, AND PARTIAL AABBs
//--------------------------------------------------------------------------
void DrawAABB(AABBtree* aabbt)
{
  Vect3d m=aabbt->MinExt, M=aabbt->MaxExt, C=(m+M)*0.5;
  glPushMatrix();
    glTranslatef(C.x,C.y,C.z);
    glScalef(M.x-m.x,M.y-m.y,M.z-m.z);
    glutWireCube(1);
  glPopMatrix();
}

void DrawTrisInAABB(AABBtree* aabbt)
{
  glColor3f(0.3,0.3,0.3);
  glBegin(GL_TRIANGLES);
    int i;
    for (i=0; i<aabbt->NumTris; i++)
    {
      glVertex3fv((float*)&(aabbt->MyTris[i][0]));
      glVertex3fv((float*)&(aabbt->MyTris[i][1]));
      glVertex3fv((float*)&(aabbt->MyTris[i][2]));
    }
  glEnd();
}

void CompleteInAABB(AABBtree* aabbt)
{
  glColor3f(1,0,0);
  DrawAABB(aabbt);
  DrawTrisInAABB(aabbt);
}

void PartialAABB(AABBtree* aabbt)
{
  glColor3f(0,1,0);
  DrawAABB(aabbt);
  DrawTrisInAABB(aabbt);
}

void CompleteOutAABB(AABBtree* aabbt)
{
  glColor3f(0,0,1);
  DrawAABB(aabbt);
}

//--------------------------------------------------------------------------
// THE "STANDARD" AABBtree CULLING TRAVERSAL ROUTINE:
// RECURSIVELY TRAVERSE THE AABBtree AND DETERMINE MINIMAL OVERLAPPING NODE SET
//--------------------------------------------------------------------------
void CullAABBtree(AABBtree* aabbt)
{
  if (aabbt)
  {
    int OVERLAP=BoxOverlapVF(aabbt->MinExt,aabbt->MaxExt);
    if (OVERLAP==COMPLETEIN)
      { CompleteInAABB(aabbt); }
    else if (OVERLAP==PARTIAL)
      { if (aabbt->IsLeaf()) PartialAABB(aabbt);
        else { CullAABBtree(aabbt->N); CullAABBtree(aabbt->P); } }
    else
      { CompleteOutAABB(aabbt); }
  }
}


//--------------------------------------------------------------------------
// Draws an AABBtree to a specified level (if Level<0 draws leaf : default)
//--------------------------------------------------------------------------
void DrawAABBtree(AABBtree* aabbt, int Level=-1)
{
  if (Level==0 || aabbt->IsLeaf()) DrawAABB(aabbt);
  else { DrawAABBtree(aabbt->N,Level-1); DrawAABBtree(aabbt->P,Level-1); };
}


//----------------------------------------------------------------------------
// DRAWS THE CAMERA (THE VIEWING FRUSTUM)
//----------------------------------------------------------------------------
void DrawCam()
{
  Vect3d sPts[4];  // SILHOUETTE POINTS (CORNERS OF WINDOW)
  Vect3d Offset = Cam.Orig-Cam.Z;
  sPts[0] = Cam.X*Cam.wR + Cam.Y*Cam.wT + Offset; // TR
  sPts[1] = Cam.X*Cam.wL + Cam.Y*Cam.wT + Offset; // TL
  sPts[2] = Cam.X*Cam.wL + Cam.Y*Cam.wB + Offset; // BL
  sPts[3] = Cam.X*Cam.wR + Cam.Y*Cam.wB + Offset; // BR

  glColor3f(1,1,0);
  glBegin(GL_LINE_LOOP);
    glVertex3f(Cam.Orig.x,Cam.Orig.y,Cam.Orig.z);
    glVertex3f(sPts[0].x,sPts[0].y,sPts[0].z);
    glVertex3f(sPts[1].x,sPts[1].y,sPts[1].z);
  glEnd();
  glBegin(GL_LINE_LOOP);
    glVertex3f(Cam.Orig.x,Cam.Orig.y,Cam.Orig.z);
    glVertex3f(sPts[1].x,sPts[1].y,sPts[1].z);
    glVertex3f(sPts[2].x,sPts[2].y,sPts[2].z);
  glEnd();
  glBegin(GL_LINE_LOOP);
    glVertex3f(Cam.Orig.x,Cam.Orig.y,Cam.Orig.z);
    glVertex3f(sPts[2].x,sPts[2].y,sPts[2].z);
    glVertex3f(sPts[3].x,sPts[3].y,sPts[3].z);
  glEnd();
  glBegin(GL_LINE_LOOP);
    glVertex3f(Cam.Orig.x,Cam.Orig.y,Cam.Orig.z);
    glVertex3f(sPts[3].x,sPts[3].y,sPts[3].z);
    glVertex3f(sPts[0].x,sPts[0].y,sPts[0].z);
  glEnd();
}


//--------------------------------------------------------------------------
// SCENE DRAWING CALLBACK. Performs the AABBtree VF culling.
//--------------------------------------------------------------------------
void DrawScene()
{
  // UPDATE THE FRUSTUM USING THE CAMERA
  Vect3d Eye=Cam.Orig, ViewDir=-Cam.Z, ViewUp=Cam.Y;
  float Yfov=(float)(atan(Cam.wT)*2.0*57.29578),
        Aspect=(Cam.wR-Cam.wL)/(Cam.wT-Cam.wB);
  SetFrustum(Eye,ViewDir,ViewUp,Yfov,Aspect);

  // CULL THE MODEL AGAINST THE FRUSTUM USING THE AABBtree
  DrawCam();
  CullAABBtree(AABBt);
//  DrawTrisInAABB(AABBt);
}


void main()
{
  // LOAD A TRI MODEL AND CREATE AN AABBtree FOR IT
  AABBt = new AABBtree("../../models/london.tri",0);

  // USING THE TOP-LEVEL AABB FOR THE MODEL BOUNDS FOR OpenGL
  Vect3d m = AABBt->MinExt, M = AABBt->MaxExt;
  GoOpenGL(m.x,m.y,m.z,M.x,M.y,M.z);
}
