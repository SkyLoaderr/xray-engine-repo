#ifndef MXGL_INCLUDED // -*- C++ -*-
#define MXGL_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxGL

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxGL.h,v 1.12.2.1 2004/07/01 18:57:12 garland Exp $

 ************************************************************************/

#include <gfx/gl.h>

/*************************************************************************
 *
 * Yes, here it is.  A bunch of nice overloaded routines to make it
 * easier on the fingers to write OpenGL drawing code.
 *
 * The general stuff provided here is:
 *
 *      - glV
 *      - glN
 *      - glC
 *
 *************************************************************************/

inline void glV(short x, short y)   { glVertex2s(x,y); }
inline void glV(int x, int y)       { glVertex2i(x,y); }
inline void glV(float x, float y)   { glVertex2f(x,y); }
inline void glV(double x, double y) { glVertex2d(x,y); }

inline void glV(short x, short y, short z)    { glVertex3s(x,y,z); }
inline void glV(int x, int y, int z)          { glVertex3i(x,y,z); }
inline void glV(float x, float y, float z)    { glVertex3f(x,y,z); }
inline void glV(double x, double y, double z) { glVertex3d(x,y,z); }

inline void glV(short x, short y, short z, short w ) { glVertex4s(x,y,z,w); }
inline void glV(int x, int y, int z, int w)          { glVertex4i(x,y,z,w); }
inline void glV(float x, float y, float z, float w)  { glVertex4f(x,y,z,w); }
inline void glV(double x,double y,double z,double w) { glVertex4d(x,y,z,w); }

inline void glV2(const GLshort *v)  { glVertex2sv(v); }
inline void glV2(const GLint *v)    { glVertex2iv(v); }
inline void glV2(const GLfloat *v)  { glVertex2fv(v); }
inline void glV2(const GLdouble *v) { glVertex2dv(v); }

inline void glV3(const GLshort *v)  { glVertex3sv(v); }
inline void glV3(const GLint *v)    { glVertex3iv(v); }
inline void glV3(const GLfloat *v)  { glVertex3fv(v); }
inline void glV3(const GLdouble *v) { glVertex3dv(v); }

inline void glV4(const GLshort *v)  { glVertex4sv(v); }
inline void glV4(const GLint *v)    { glVertex4iv(v); }
inline void glV4(const GLfloat *v)  { glVertex4fv(v); }
inline void glV4(const GLdouble *v) { glVertex4dv(v); }

////////////////////////////////////

inline void glN(short x, short y, short z)    { glNormal3s(x,y,z); }
inline void glN(int x, int y, int z)          { glNormal3i(x,y,z); }
inline void glN(float x, float y, float z)    { glNormal3f(x,y,z); }
inline void glN(double x, double y, double z) { glNormal3d(x,y,z); }

inline void glN(const GLshort *v)  { glNormal3sv(v); }
inline void glN(const GLint *v)    { glNormal3iv(v); }
inline void glN(const GLfloat *v)  { glNormal3fv(v); }
inline void glN(const GLdouble *v) { glNormal3dv(v); }

////////////////////////////////////

inline void glC(short x, short y, short z)    { glColor3s(x,y,z); }
inline void glC(int x, int y, int z)          { glColor3i(x,y,z); }
inline void glC(float x, float y, float z)    { glColor3f(x,y,z); }
inline void glC(double x, double y, double z) { glColor3d(x,y,z); }

inline void glC(short x,short y,short z,short w)     {glColor4s(x,y,z,w);}
inline void glC(int x, int y, int z, int w)          {glColor4i(x,y,z,w);}
inline void glC(float x,float y,float z,float w)     {glColor4f(x,y,z,w);}
inline void glC(double x,double y,double z,double w) {glColor4d(x,y,z,w);}

inline void glC(float *v)   { glColor3fv(v); }
inline void glC(double *v)  { glColor3dv(v); }
inline void glC4(float *v)  { glColor4fv(v); }
inline void glC4(double *v) { glColor4dv(v); }

////////////////////////////////////

inline void glLoadMatrix(const double *m) { glLoadMatrixd(m); }
inline void glLoadMatrix(const float *m)  { glLoadMatrixf(m); }

inline void glGetMatrix(double *m, GLenum src=GL_PROJECTION_MATRIX)
{ glGetDoublev(src, m); }
inline void glGetMatrix(float *m, GLenum src=GL_PROJECTION_MATRIX)
{ glGetFloatv(src, m); }

/*************************************************************************
 *
 * Here's some more useful stuff.
 *
 *************************************************************************/

inline void glOffsetForMesh(float factor=1.0, float bias=1e-6)
{
#if defined(MIX_HAVE_POLYOFFSET) || defined(WIN32)
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(factor, bias);
#elif defined(MIX_HAVE_POLYOFFSET_EXT)
    glEnable(GL_POLYGON_OFFSET_EXT);
    glPolygonOffsetEXT(factor, bias);
#endif
}


inline void glNoOffset()
{
#if defined(MIX_HAVE_POLYOFFSET) || defined(WIN32)
    glDisable(GL_POLYGON_OFFSET_FILL);
#elif defined(MIX_HAVE_POLYOFFSET_EXT)
    glDisable(GL_POLYGON_OFFSET_EXT);
#endif
}

inline void glGetViewport(int *x, int *y, int *w, int *h)
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    if( x ) *x = viewport[0];
    if( y ) *y = viewport[1];
    if( w ) *w = viewport[2];
    if( h ) *h = viewport[3];
}

inline void glUnproject(int win_x, int win_y, int win_z,
                        double *x, double *y, double *z)
{
    GLdouble modelMatrix[16];
    GLdouble projMatrix[16];
    GLint viewport[4];

    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);

    gluUnProject(win_x,win_y,win_z,
                 modelMatrix, projMatrix, viewport,
                 x, y, z);
}

inline int glGetRenderMode()
{
    GLint mode; glGetIntegerv(GL_RENDER_MODE, &mode); return mode;
}

/*************************************************************************
 *
 * Debugging and diagnostic tools
 *
 *************************************************************************/

extern void mxgl_report_stack_depth();
extern void mxgl_check_errors(const char *msg=NULL);

// MXGL_INCLUDED
#endif
