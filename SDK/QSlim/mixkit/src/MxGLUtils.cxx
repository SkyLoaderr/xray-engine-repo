/************************************************************************

  MxGLUtils

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxGLUtils.cxx,v 1.7 2000/02/24 23:07:02 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxGLUtils.h"

GLenum mxgl_matrix_for_mode(GLenum mode)
{
    switch( mode )
    {
    case GL_MODELVIEW:  return GL_MODELVIEW_MATRIX;
    case GL_PROJECTION: return GL_PROJECTION_MATRIX;
    case GL_TEXTURE:    return GL_TEXTURE_MATRIX;
    }

    return GL_FALSE;
}

int mxgl_unproject_pixel(int *pixel, double *world, double z)
{
    GLdouble modelMatrix[16];
    GLdouble projMatrix[16];
    GLint viewport[4];

    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);

    // Notice that we have to correct the y pixel coordinate.  GL
    // assigns the origin to the lower left corner, while FLTK assigns
    // the origin to the upper left corner.
    return gluUnProject(pixel[0], viewport[3]-pixel[1], z,
			modelMatrix, projMatrix, viewport,
			world, world+1, world+2);
}

const unsigned int MXGL_PICK_NIL=(~0);
const unsigned int MXGL_PICK_ZMAX=(~0);

void mxgl_begin_pick(int *where, double radius, GLuint *buffer, int size)
{
    GLint vp[4];

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();		// Save the current transformation
    glLoadIdentity();
    glGetIntegerv(GL_VIEWPORT, vp);
    glSelectBuffer(size, buffer);
    glRenderMode(GL_SELECT);

    gluPickMatrix(where[0], vp[3] - where[1], radius, radius, vp);
    glInitNames();
    glPushName(MXGL_PICK_NIL);
}


unsigned int mxgl_complete_pick(GLuint *buffer)
{
    glFlush();

    GLint nhits = glRenderMode(GL_RENDER);
    GLuint hit = MXGL_PICK_NIL;
    GLuint zmin = MXGL_PICK_ZMAX;
    GLuint *ptr = buffer;

    for(uint i=0; i<nhits; i++)
    {
	GLuint nnames = *ptr++;
	assert( nnames==1 ); // assume name stack is always 1 deep

	GLuint cur_zmin = *ptr++;
	GLuint cur_zmax = *ptr++;

	if( cur_zmin < zmin )
	{
	    zmin = cur_zmin;
	    hit = *ptr;
	}
	ptr++;
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();		// get rid of the pick matrix

    return hit;
}
