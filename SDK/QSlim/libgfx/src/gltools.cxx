/************************************************************************

  Handy functions for common OpenGL tasks

  $Id: gltools.cxx,v 1.9 2002/04/02 16:57:53 garland Exp $

 ************************************************************************/


#include <gfx/gfx.h>

#ifdef HAVE_OPENGL

#include <gfx/gltools.h>

#include <cassert>

using namespace std;

GLuint opengl_pick_nil = (~0);
GLuint opengl_pick_zmax = (~0);

void begin_opengl_pick(int *where, double radius, GLuint *buffer, int size)
{
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    glSelectBuffer(size, buffer);
    glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(opengl_pick_nil);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();             // Save the current transformation
    glLoadIdentity();
    gluPickMatrix(where[0], vp[3] - where[1], radius, radius, vp);
}

GLuint complete_opengl_pick(GLuint *buffer)
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();              // get rid of the pick matrix
    glFlush();

    GLint nhits = glRenderMode(GL_RENDER);
    GLuint *hit = NULL;
    GLuint hit_nnames = 0;
    GLuint zmin = opengl_pick_zmax;
    GLuint *ptr = buffer;

    for(int i=0; i<nhits; i++)
    {
        GLuint nnames   = *ptr++;
        GLuint cur_zmin = *ptr++;
        GLuint cur_zmax = *ptr++;

        if( cur_zmin < zmin )
        {
            zmin = cur_zmin;
            hit = ptr;
	    hit_nnames = nnames;
        }
        ptr+=nnames;
    }


    buffer[0] = hit_nnames;
    if( hit )
    {
	for(int k=0; k<hit_nnames; k++)
	    buffer[k+1] = hit[k];

	return *hit;
    }
    else
	return opengl_pick_nil;
}

void report_opengl_stacks()
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

void check_opengl_errors(const char *msg)
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

    if( stack_error )  report_opengl_stacks();
}

void camera_lookat(const Vec3& min, const Vec3& max, double aspect)
{
    Vec3 up(0, 1, 0);
    double fovy = 60.0;

    Vec3 at = (max + min)/2.0;         // look at the center of the bbox
    double radius = norm(max - at);    // radius of a bounding sphere
    double d = 3*radius / tan(fovy * M_PI/180.0);

    Vec3 from = at;
    from[2] += d;

    double znear = d/20;
    double zfar = 10*d;

    glMatrixMode(GL_PROJECTION);
    gluPerspective(fovy, aspect, znear, zfar);

    glMatrixMode(GL_MODELVIEW);
    gluLookAt(from[0], from[1], from[2],
	      at[0], at[1], at[2],
	      up[0], up[1], up[2]);
}

int unproject_pixel(int *pixel, double *world, double z)
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

#endif /* HAVE_OPENGL */
