#ifndef MXMANIPULATOR_INCLUDED // -*- C++ -*-
#define MXMANIPULATOR_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxManipulator

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxManipulator.h,v 1.6 1998/10/26 21:09:04 garland Exp $

 ************************************************************************/

#include "MxGeom3D.h"

class MxManipulator
{
public:

    virtual void init(const MxBounds&, float) { }

    bool mouse(int kind, int which, int x, int y)
	{
	    if( kind&0xf == 1 ) return mouse_down(which, x, y);
	    else if( kind&0xf == 2 ) return mouse_up(which, x, y);
	    else return motion(x, y);
	}
    virtual bool mouse_down(int which, int x, int y, int kind=1) = 0;
    virtual bool mouse_up(int which, int x, int y, int kind=2) = 0;
    virtual bool motion(int x, int y) = 0;
    virtual bool animate() { return false; }

    virtual void apply() = 0;
    virtual void unapply() = 0;
    virtual void draw() { }
};

// MXMANIPULATOR_INCLUDED
#endif
