#ifndef GFXARCBALL_INCLUDED // -*- C++ -*-
#define GFXARCBALL_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Arcball rotation control.
  
  $Id: arcball.h,v 1.7 2003/08/13 15:40:31 garland Exp $

 ************************************************************************/

#include "baseball.h"

class Arcball : public Baseball
{
private:
    Vec2 ball_ctr;
    double ball_radius;

    Quat q_now, q_down, q_drag;	// Quaternions describing rotation
    Vec3 v_from, v_to;		//

    bool is_dragging;

protected:
    Vec3 proj_to_sphere(const Vec2&);
    void update();


public:
    Arcball();

    virtual void update_animation();
    virtual bool mouse_down(int *where, int which);
    virtual bool mouse_up(int *where, int which);
    virtual bool mouse_drag(int *where, int *last, int which);

    virtual void apply_transform();
    virtual void get_transform(Vec3 & c, Vec3 &t, Quat & q);
    virtual void set_transform(const Vec3 & c, const Vec3 & t, const Quat & q); 

    virtual void write(std::ostream&);
    virtual void read(std::istream&);
};



// GFXARCBALL_INCLUDED
#endif
