#ifndef GFXBASEBALL_INCLUDED // -*- C++ -*-
#define GFXBASEBALL_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Common base class for ball-based rotators (e.g., Trackball & Arcball).
  
  $Id: baseball.h,v 1.2 2003/08/13 15:40:32 garland Exp $

 ************************************************************************/

#include "quat.h"

class Baseball
{
public:
    Vec3 ctr;			// Describes bounding sphere of object
    double radius;		//

    Quat curquat;		// Current rotation of object
    Vec3 trans;			// Current translation of object

public:
    Baseball();

    // Required initialization method
    template<class T>
    void bounding_sphere(const TVec3<T>& v, T r) { ctr=v; radius=r; }

    // Standard event interface provide by all Ball controllers
    virtual void update_animation() = 0;
    virtual bool mouse_down(int *where, int which) = 0;
    virtual bool mouse_up(int *where, int which) = 0;
    virtual bool mouse_drag(int *where, int *last, int which) = 0;

    // Interface for use during drawing to apply appropriate transformation
    virtual void apply_transform();
    virtual void unapply_transform();

    // Interface for reading/writing transform
    virtual void write(std::ostream&);
    virtual void read(std::istream&);
};

// GFXBASEBALL_INCLUDED
#endif
