/************************************************************************

  MxFrame

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxFrame.cxx,v 1.6 1998/10/26 21:08:53 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxVector.h"
#include "MxFrame.h"


MxFrame::MxFrame()
{
    mxv_set(v0, 0.0, 3);
    mxv_basis(axes[0], 0, 3);
    mxv_basis(axes[1], 1, 3);
    mxv_basis(axes[2], 2, 3);

    d0 = 0.0;
}

MxFrame::MxFrame(double *c)
{
    mxv_set(v0, c, 3);
    mxv_basis(axes[0], 0, 3);
    mxv_basis(axes[1], 1, 3);
    mxv_basis(axes[2], 2, 3);

    d0 = -mxv_dot(axes[2], v0, 3);
}

MxFrame::MxFrame(double *c, double *u0, double *u1, double *u2)
{
    mxv_set(v0, c, 3);
    mxv_set(axes[0], u0, 3);
    mxv_set(axes[1], u1, 3);
    mxv_set(axes[2], u2, 3);

    d0 = -mxv_dot(axes[2], v0, 3);
}

MxFrame& MxFrame::operator=(const MxFrame& f)
{
    mxv_set(v0, f.v0, 3);
    mxv_set(axes[0], f.axes[0], 3);
    mxv_set(axes[1], f.axes[1], 3);
    mxv_set(axes[2], f.axes[2], 3);
    d0 = f.d0;

    return *this;
}

double *MxFrame::to_frame(const double *v, double *u) const
{
    double delta[3];
    mxv_sub(delta, v, v0, 3);

    u[0] = mxv_dot(delta, axes[0], 3);
    u[1] = mxv_dot(delta, axes[1], 3);
    u[2] = mxv_dot(delta, axes[2], 3);

    return u;
}

double *MxFrame::from_frame(const double *u, double *v) const
{
    double t[3];

    mxv_set(v, v0, 3);

    mxv_scale(t, axes[0], u[0], 3);  mxv_addinto(v, t, 3);
    mxv_scale(t, axes[1], u[1], 3);  mxv_addinto(v, t, 3);
    mxv_scale(t, axes[2], u[2], 3);  mxv_addinto(v, t, 3);

    return v;
}

double MxFrame::dist_to_plane(const double *v) const
{
    return mxv_dot(v, axes[2], 3) + d0;
}

void MxFrame::align_axis(uint i, const double *v)
{
    AssertBound( i<3 );

    if( mxv_dot(axes[i], v, 3) < 0.0 )
	mxv_neg(axes[i], 3);
}

double *MxFrame::compute_xform_toframe(double *R) const
{
    // Assignments are in row order.  The rows are:
    //      [ e_1   | 0 ]
    //      [ e_2   | 0 ]
    //      [ e_3   | 0 ]
    //      [ v0    | 1 ]

    R[0] =axes[0][0];  R[1] =axes[0][1];  R[2] =axes[0][2];  R[3] =0;
    R[4] =axes[1][0];  R[5] =axes[1][1];  R[6] =axes[1][2];  R[7] =0;
    R[8] =axes[2][0];  R[9] =axes[2][1];  R[10]=axes[2][2];  R[11]=0;
    R[12]=v0[0];       R[13]=v0[1];       R[14]=v0[2];       R[15]=1;

    return R;
}

double *MxFrame::compute_xform_fromframe(double *R) const
{
    // Assignments are in row order.  The columns are:
    //      [                   ]
    //      [ e_1  e_2  e_3  v0 ]
    //      [                   ]
    //      [  0    0    0    1 ]

    R[0] =axes[0][0];  R[1] =axes[1][0];  R[2] =axes[2][0];  R[3] =v0[0];
    R[4] =axes[0][1];  R[5] =axes[1][1];  R[6] =axes[2][1];  R[7] =v0[1];
    R[8] =axes[0][2];  R[9] =axes[1][2];  R[10]=axes[2][2];  R[11]=v0[2];
    R[12]=0;           R[13]=0;           R[14]=0;           R[15]=1;

    return R;
}
