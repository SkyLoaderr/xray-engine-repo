/************************************************************************

  MxGLDebug

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxGLDebug.cxx,v 1.3 1998/10/26 21:08:55 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxGL.h"

void mxgl_report_stack_depth()
{
    GLint depth;

    glGetIntegerv(GL_PROJECTION_STACK_DEPTH, &depth);
    cerr << "   Projection stack depth = " << depth;
    glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &depth);
    cerr << " (" << depth << " max)" << endl;

    glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &depth);
    cerr << "   ModelView stack depth = " << depth;
    glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &depth);
    cerr << " (" << depth << " max)" << endl;

    glGetIntegerv(GL_TEXTURE_STACK_DEPTH, &depth);
    cerr << "   Texture stack depth = " << depth;
    glGetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH, &depth);
    cerr << " (" << depth << " max)" << endl;
}


void mxgl_check_errors(const char *msg)
{
    bool stack_error = false;

    for(GLenum err=glGetError(); err!=GL_NO_ERROR; err=glGetError())
    {
	cerr << "GL ERROR ";
	if( msg ) cerr << msg;
	cerr << ": " << (const char *)gluErrorString(err) << endl;

	if( err==GL_STACK_OVERFLOW || err==GL_STACK_UNDERFLOW )
	    stack_error = true;
    }

    if( stack_error )  mxgl_report_stack_depth();
}
