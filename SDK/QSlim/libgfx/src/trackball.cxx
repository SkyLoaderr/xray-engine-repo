/************************************************************************

  Virtual Trackball for manipulating objects on the screen.

  This code is based on the virtual trackball mechanism developed by
  Gavin Bell of Silicon Graphics.

  $Id: trackball.cxx,v 1.7 2002/03/12 17:52:14 garland Exp $

 ************************************************************************/

#include <gfx/gfx.h>
#include <gfx/gl.h>
#include <gfx/trackball.h>

static
float proj_to_sphere(float r, float x, float y)
{
    // This is a magic number taken from the SGI trackball.
    //
    const float MAGIC_RLIMIT = 0.70710678118654752440f;

    // Project (x,y) onto a sphere of radius r or a hyperbolic sheet
    // if the point is far enough away from the origin.
    //
    float d = sqrt(x*x + y*y);
    if (d < r * MAGIC_RLIMIT )
    {
	// Inside sphere
        return sqrt(r*r - d*d);
    }
    else
    {
	// On hyperbola
	float t = r * MAGIC_RLIMIT;
        return t*t / d;
    }
}

//
// This is the core routine that actually simulates the action of the
// trackball.  It projects the given coordinates onto the trackball
// surface, and computes an appropriate rotation.
//
// NOTE: This uses the combined spherical/hyperbolic trackball surface
// used in the SGI trackball.
//
void trackball(Quat& q, float p1x, float p1y, float p2x, float p2y)
{
    if (p1x == p2x && p1y == p2y)
    {
	q = Quat::ident();
        return;
    }

    // This is a magic number taken from the SGI trackball source.
    // Here is the original explanation of this constant:
    //
    //     This size should really be based on the distance from the
    //     center of rotation to the point on the object underneath
    //     the mouse.  That point would then track the mouse as
    //     closely as possible.  This is a simple example, though, so
    //     that is left as an Exercise for the Programmer.
    //
    const float TRACKBALLSIZE = 0.8f;

    // Project the window coordinates onto the trackball surface.
    //
    Vec3 p1(p1x,p1y,proj_to_sphere(TRACKBALLSIZE,p1x,p1y));
    Vec3 p2(p2x,p2y,proj_to_sphere(TRACKBALLSIZE,p2x,p2y));

    // And how much do we rotate?
    //
    float t = norm(p1-p2) / (2.0*TRACKBALLSIZE);
    if (t >  1.0) t =  1.0;
    if (t < -1.0) t = -1.0;

    // Convert this axis/angle rotation to a quaternion.
    //
    q = axis_to_quat(p2^p1, 2.0*asin(t));
}

static
void add_quats(const Quat& q1, const Quat& q2, Quat& dest)
{
    dest = q2*q1;
    unitize(dest);
}

////////////////////////////////////////////////////////////////////////
//
// Encapsulate the standard interface of the trackball to simplify
// its use in most applications.
//

Trackball::Trackball()
{
}

void Trackball::update_animation()
{
    add_quats(lastquat, curquat, curquat);
}

bool Trackball::mouse_up(int *where, int which)
{
    return false;
}

bool Trackball::mouse_down(int *where, int which)
{
    if( which==1 )
	lastquat = Quat::ident();

    return false;
}

bool Trackball::mouse_drag(int *where, int *last, int which)
{
    float vp[4];
    glGetFloatv(GL_VIEWPORT, vp);
    float W=vp[2], H=vp[3];

    float diam = 2*radius;

    if( which==1 )
    {
	trackball(lastquat,
		  (2.0 * last[0] - W)/W,
		  (H - 2.0 * last[1])/H,
		  (2.0 * where[0] - W)/W,
		  (H - 2.0 * where[1])/H);
	add_quats(lastquat, curquat, curquat);
    }
    else if( which==2 )
    {
	trans[0] += diam * (where[0] - last[0]) / W;
	trans[1] += diam * (last[1] - where[1]) / H;
    }
    else if( which==3 )
    {
	trans[2] += 0.02*diam*(where[1] - last[1]);
    }
    else
	return false;

    return true;
}
