#ifndef MXFRAME_INCLUDED // -*- C++ -*-
#define MXFRAME_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxFrame

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxFrame.h,v 1.7 1998/10/26 21:08:54 garland Exp $

 ************************************************************************/

#define MXFRAME_UAXIS  0
#define MXFRAME_VAXIS  1
#define MXFRAME_NORMAL 2

class MxFrame
{
private:
    double v0[3];		// origin of local frame
    double axes[3][3];		// unit axis vectors of local frame
    double d0;			// together with axis(2), defines base plane

public:
    MxFrame();
    MxFrame(double *c);
    MxFrame(double *c, double *u0, double *u1, double *u2);

    MxFrame(const MxFrame& f) { *this = f; }
    MxFrame& operator=(const MxFrame& f);

    double       *origin()       { return v0; }
    const double *origin() const { return v0; }
    double       *axis(uint i)       { AssertBound(i<3); return axes[i]; }
    const double *axis(uint i) const { AssertBound(i<3); return axes[i]; }
    double plane_offset() const { return d0; }
    void plane_offset(double d) { d0 = d; }

    double *to_frame(const double *, double *) const;
    double *from_frame(const double *, double *) const;
    double *compute_xform_toframe(double *) const;
    double *compute_xform_fromframe(double *) const;

    double dist_to_plane(const double *) const;

    void align_axis(uint i, const double *v);
};

// MXFRAME_INCLUDED
#endif
