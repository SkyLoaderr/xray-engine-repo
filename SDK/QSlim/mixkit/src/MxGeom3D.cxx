/************************************************************************

  Handy 3D geometrical primitives

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxGeom3D.cxx,v 1.13 2000/12/14 17:47:43 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxGeom3D.h"

void mx3d_box_corners(const Vec3& min, const Vec3& max, Vec3 *v)
{
    v[0] = min;
    v[1] = Vec3(min[X], max[Y], min[Z]);
    v[2] = Vec3(max[X], min[Y], min[Z]);
    v[3] = Vec3(max[X], max[Y], min[Z]);
    v[4] = Vec3(min[X], min[Y], max[Z]);
    v[5] = Vec3(min[X], max[Y], max[Z]);
    v[6] = Vec3(max[X], min[Y], max[Z]);
    v[7] = max;
}

////////////////////////////////////////////////////////////////////////
//
// Basic bounding volumes.
// This has been imported pretty much directly from GFX
//

void MxBounds::reset()
{
    center[X] = center[Y] = center[Z] = 0.0;
    radius = 0.0;

    points = 0;
    is_initialized = false;
}

void MxBounds::add_point(const double *v, bool will_update)
{
    if( !is_initialized )
    {
	min[X] = max[X] = v[X];
	min[Y] = max[Y] = v[Y];
	min[Z] = max[Z] = v[Z];
	is_initialized = true;
    }
    else
    {
	if( v[X] < min[X] ) min[X] = v[X];
	if( v[Y] < min[Y] ) min[Y] = v[Y];
	if( v[Z] < min[Z] ) min[Z] = v[Z];

	if( v[X] > max[X] ) max[X] = v[X];
	if( v[Y] > max[Y] ) max[Y] = v[Y];
	if( v[Z] > max[Z] ) max[Z] = v[Z];
    }

    if( will_update )
    {
	center += Vec3(v);
	points++;
    }
}

void MxBounds::add_point(const float *v, bool will_update)
{
    if( !is_initialized )
    {
	min[X] = max[X] = v[X];
	min[Y] = max[Y] = v[Y];
	min[Z] = max[Z] = v[Z];
	is_initialized = true;
    }
    else
    {
	if( v[X] < min[X] ) min[X] = v[X];
	if( v[Y] < min[Y] ) min[Y] = v[Y];
	if( v[Z] < min[Z] ) min[Z] = v[Z];

	if( v[X] > max[X] ) max[X] = v[X];
	if( v[Y] > max[Y] ) max[Y] = v[Y];
	if( v[Z] > max[Z] ) max[Z] = v[Z];
    }

    if( will_update )
    {
	center += Vec3(v);
	points++;
    }
}

void MxBounds::complete()
{
    center /= (double)points;

    Vec3 R1 = max-center;
    Vec3 R2 = min-center;

    radius = MAX(norm(R1), norm(R2));
}

void MxBounds::merge(const MxBounds& b)
{
    add_point(b.min, false);
    add_point(b.max, false);
    points += b.points;

    Vec3 dC = b.center - center;
    double dist = norm(dC);

    if( dist + b.radius > radius )
    {
	// New sphere does not lie within old sphere
	center += b.center;
	center /= 2;

	dist /= 2;
	radius = MAX(dist+radius, dist+b.radius);
    }
}
