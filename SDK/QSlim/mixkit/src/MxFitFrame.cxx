/************************************************************************

  MxFitFrame

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxFitFrame.cxx,v 1.9.2.1 2002/01/31 18:38:37 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxFitFrame.h"
#include "MxVector.h"
#include "MxGeom3D.h"

MxFitFrame::MxFitFrame()
{
    reset_bounds();
    mxv_set(normal_accum, 0.0, 3);
    mxv_set(avg_normal, 0.0, 3);
}

MxFitFrame::MxFitFrame(double *c, double *u0, double *u1, double *u2)
    : MxFrame(c, u0, u1, u2)
{
    reset_bounds();
    mxv_set(normal_accum, u2, 3);
    mxv_set(avg_normal, u2, 3);
}

MxFitFrame::MxFitFrame(const MxQuadric3& Q_fit, uint nverts)
{
    reset_bounds();
    mxv_set(normal_accum, 0.0, 3);
    mxv_set(avg_normal, 0.0, 3);
    if( !compute_frame(Q_fit, nverts) )
	fatal_error("MxFitFrame -- unable to construct frame from quadric.");
}

bool MxFitFrame::compute_frame(const MxQuadric3& Q_fit, uint _nverts)
{
    Mat3 A = Q_fit.tensor();
    Vec3 v = Q_fit.vector();
    double k = Q_fit.offset();
    double nverts = (double)_nverts;

    Mat3 CV = A - Mat3::outer_product(v)/nverts;
    if( !jacobi(CV, axis_evals, axis(0)) )
	return false;

    // ASSUME: jacobi has unitized the eigenvectors for us

    align_axis(MXFRAME_NORMAL, avg_normal);

    mxv_invscale(origin(), v, nverts, 3);
    plane_offset( -mxv_dot(axis(MXFRAME_NORMAL), origin(), 3) );

    return true;
}

bool MxFitFrame::compute_frame(const float *p0,const float *p1,const float *p2)
{
    Vec3 v0(p0), v1(p1), v2(p2);

    // Set the origin to the barycenter of the triangle
    mxv_add(origin(), v0, v1, 3);
    mxv_addinto(origin(), v2, 3);
    mxv_invscale(origin(), 3.0, 3);

    // Compute the 3 edge vectors
    double e[3][3], l[3];
    mxv_sub(e[0], v1, v0, 3);
    mxv_sub(e[1], v2, v1, 3);
    mxv_sub(e[2], v0, v2, 3);

    l[0] = mxv_unitize(e[0], 3);
    l[1] = mxv_unitize(e[1], 3);
    l[2] = mxv_unitize(e[2], 3);

    // Set frame normal to the face normal.  The cross product is
    // reversed because e[2] points in the opposite direction.
    mxv_cross3(axis(MXFRAME_NORMAL), e[2], e[0]);
    mxv_set(normal_accum, axis(MXFRAME_NORMAL), 3);
    mxv_set(avg_normal, axis(MXFRAME_NORMAL), 3);
    //
    // and compute the relevant plane offset constant
    plane_offset( -mxv_dot(axis(MXFRAME_NORMAL), origin(), 3) );

    // Choose the longest edge as the first tangent axis
    if( l[0]>l[1] && l[0]>l[2] )       mxv_set(axis(MXFRAME_UAXIS), e[0], 3);
    else if( l[1]>l[0] && l[1]>l[2] )  mxv_set(axis(MXFRAME_UAXIS), e[1], 3);
    else                               mxv_set(axis(MXFRAME_UAXIS), e[2], 3);

    // Compute the other tangent axis
    mxv_unitize(mxv_cross3(axis(MXFRAME_VAXIS),
			   axis(MXFRAME_NORMAL), axis(MXFRAME_UAXIS)), 3);

    // Finally, compute the bounding box
    reset_bounds();
    accumulate_bounds(v0);
    accumulate_bounds(v1);
    accumulate_bounds(v2);

    return true;
}


void MxFitFrame::reset_bounds()
{
    mxv_set(vmin, HUGE, 3);
    mxv_set(vmax, -HUGE, 3);
    mxv_set(axis_evals, 0.0, 3);
}

void MxFitFrame::accumulate_bounds(const double *v, uint npoint)
{
    while( npoint )
    {
	double u[3];
	to_frame(v, u);

        if( u[X] < vmin[X] ) vmin[X] = u[X];
        if( u[Y] < vmin[Y] ) vmin[Y] = u[Y];
        if( u[Z] < vmin[Z] ) vmin[Z] = u[Z];
        if( u[X] > vmax[X] ) vmax[X] = u[X];
        if( u[Y] > vmax[Y] ) vmax[Y] = u[Y];
        if( u[Z] > vmax[Z] ) vmax[Z] = u[Z];

	v += 3;
	npoint--;
    }
}

void MxFitFrame::accumulate_bounds(const float *v, uint npoint)
{
    while( npoint )
    {
	double u[3], v2[3];
	v2[0]=v[0];  v2[1]=v[1];  v2[2]=v[2];
	to_frame(v2, u);

        if( u[X] < vmin[X] ) vmin[X] = u[X];
        if( u[Y] < vmin[Y] ) vmin[Y] = u[Y];
        if( u[Z] < vmin[Z] ) vmin[Z] = u[Z];
        if( u[X] > vmax[X] ) vmax[X] = u[X];
        if( u[Y] > vmax[Y] ) vmax[Y] = u[Y];
        if( u[Z] > vmax[Z] ) vmax[Z] = u[Z];

	v += 3;
	npoint--;
    }
}

void MxFitFrame::worldspace_corners(Vec3 *corners) const
{
    mx3d_box_corners(Vec3(vmin), Vec3(vmax), corners);

    for(uint i=0; i<8; i++)
    {
	double tmp[3];
	from_frame(corners[i], tmp);
	corners[i] = Vec3(tmp);
    }
}

void MxFitFrame::accumulate_bounds(const MxFitFrame& F)
{
    Vec3 corners[8];
    F.worldspace_corners(corners);
    accumulate_bounds(corners[0], 8);
}

void MxFitFrame::set_normal(const double *n)
{
    mxv_set(normal_accum, n, 3);
    mxv_set(avg_normal, n, 3);
    mxv_unitize(avg_normal, 3);
}

void MxFitFrame::clear_normal()
{
    mxv_set(normal_accum, 0.0, 3);
    mxv_set(avg_normal, 0.0, 3);
}

void MxFitFrame::add_normal(const double *n)
{
    mxv_addinto(normal_accum, n, 3);
}

void MxFitFrame::finalize_normal()
{
    mxv_set(avg_normal, normal_accum, 3);
    mxv_unitize(avg_normal, 3);
}

double MxFitFrame::compute_closest_distance(const double *v, double *w) const
{
    return sqrt(compute_closest_sqrdistance(v, w));
}

static inline void CLIP(double& x, double min, double max)
	{if( x<min ) x=min; else if( x>max ) x=max;}

double MxFitFrame::compute_closest_sqrdistance(const double *v,double *w) const
{
    double u[3], clipped[3];

    to_frame(v, u);
    mxv_set(clipped, u, 3);

    CLIP(clipped[X], vmin[X], vmax[X]);
    CLIP(clipped[Y], vmin[Y], vmax[Y]);
    CLIP(clipped[Z], vmin[Z], vmax[Z]);

    if( w ) from_frame(clipped, w);

    return mxv_L2(u, clipped, 3);
}


double MxFitFrame::compute_max_distance(const double *v) const
{
    return sqrt(compute_max_sqrdistance(v));
}

double MxFitFrame::compute_max_sqrdistance(const double *v) const
{
    double u[3];
    to_frame(v, u);

    double dx0 = u[0] - vmin[0];  dx0 *= dx0;
    double dx1 = u[0] - vmax[0];  dx1 *= dx1;

    double dy0 = u[1] - vmin[1];  dy0 *= dy0;
    double dy1 = u[1] - vmax[1];  dy1 *= dy1;

    double dz0 = u[2] - vmin[2];  dz0 *= dz0;
    double dz1 = u[2] - vmax[2];  dz1 *= dz1;

    return MAX(dx0,dx1) + MAX(dy0, dy1) + MAX(dz0, dz1);
}
