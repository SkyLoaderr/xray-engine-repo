/************************************************************************

  Frame-to-frame distance functions.

  Since the code for computing frame-to-frame distances is reasonably
  large, and not always necessary, I've placed it here in a separate file.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxFitFrame-2.cxx,v 1.2 1998/10/26 21:08:52 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxFitFrame.h"
#include "MxVector.h"

void MxFitFrame::compute_distance_bounds(const MxFitFrame& F,
					 double *min, double *max) const
{
    Vec3 K1[8], K2[8];
    uint i,j;

    worldspace_corners(K1);
    F.worldspace_corners(K2);

    // Compute maximum possible distance -- No two points are
    // further apart than this.
    //
    *max = 0.0;
    for(i=0; i<8; i++)  for(j=0; j<8; j++)
    {
	double d2 = norm2(K1[i] - K2[j]);
	if( d2 > *max )  *max = d2;
    }

    // Compute the minimum possible distance -- No two points
    // are closer together than this.
    //
    if( check_intersection(F) )
	*min = 0.0;
    else
    {
	*min = HUGE;

	for(i=0; i<8; i++)
	{
	    double d_1 = F.compute_closest_sqrdistance(K1[i]);
	    double d_2 = compute_closest_sqrdistance(K2[i]);

	    if( d_1 < *min ) *min = d_1;
	    if( d_2 < *min ) *min = d_2;
	}
    }
}

/************************************************************************
 *
 * This code for intersection testing was adapted from code written by
 * David Eberly <eberly@cs.unc.edu>.  His original code is available at
 * http://www.cs.unc.edu/~eberly/graphics.htm and was distributed
 * under the following license:
 *
 *      Permission is granted to use, copy, and distribute at no
 *      charge the MAGIC library code. Permission is also granted to
 *      the user to modify the code. However, if the modified code
 *      is distributed, then all such modifications must be
 *      documented to indicate that they are not part of the
 *      original package. The original code comes with absolutely no
 *      warranty and no guarantee is made that the code is bug-free.
 *
 * My changes to the original code:
 *
 *    - Added new external interface to underlying code
 *    - Some simple reformatting
 *    - Use MixKit vector classes
 *    - C++ conversion
 *
 ************************************************************************/

typedef Vec3 Vector;

/* oriented bounding boxes without velocity */
struct OBBox0 {
    Vector C;     /* center */
    Vector B[3];  /* orthonormal basis */
    Vector E;     /* extent along basis vectors */
};

static
double Dot(const Vector& P, const Vector& Q)
	{ return P[0]*Q[0]+P[1]*Q[1]+P[2]*Q[2]; }

#define Dot(P,Q) ((P)*(Q))

static
bool TestIntersection0(const OBBox0* b0, const OBBox0* b1)
{
    double d0, d1, d2;
    double r0, r1, r2;
    double C[3][3];
    
    Vector D(b1->C[0]-b0->C[0], b1->C[1]-b0->C[1], b1->C[2]-b0->C[2]);
    
    /* L = A0 */
    C[0][0] = Dot(b0->B[0],b1->B[0]);
    C[0][1] = Dot(b0->B[0],b1->B[1]);
    C[0][2] = Dot(b0->B[0],b1->B[2]);
    d0 = Dot(D,b0->B[0]);
    r0 = fabs(b0->E[0]);
    r1 = fabs(b1->E[0]*C[0][0])+fabs(b1->E[1]*C[0][1])+fabs(b1->E[2]*C[0][2]);
    r2 = fabs(d0);
    if ( r2 > r0+r1 )
        return false;

    /* L = A1 */
    C[1][0] = Dot(b0->B[1],b1->B[0]);
    C[1][1] = Dot(b0->B[1],b1->B[1]);
    C[1][2] = Dot(b0->B[1],b1->B[2]);
    d1 = Dot(D,b0->B[1]);
    r0 = fabs(b0->E[1]);
    r1 = fabs(b1->E[0]*C[1][0])+fabs(b1->E[1]*C[1][1])+fabs(b1->E[2]*C[1][2]);
    r2 = fabs(d1);
    if ( r2 > r0+r1 )
        return false;

    /* L = A2 */
    C[2][0] = Dot(b0->B[2],b1->B[0]);
    C[2][1] = Dot(b0->B[2],b1->B[1]);
    C[2][2] = Dot(b0->B[2],b1->B[2]);
    d2 = Dot(D,b0->B[2]);
    r0 = fabs(b0->E[2]);
    r1 = fabs(b1->E[0]*C[2][0])+fabs(b1->E[1]*C[2][1])+fabs(b1->E[2]*C[2][2]);
    r2 = fabs(d2);
    if ( r2 > r0+r1 )
        return false;

    /* L = B0 */
    r0 = fabs(b0->E[0]*C[0][0])+fabs(b0->E[1]*C[1][0])+fabs(b0->E[2]*C[2][0]);
    r1 = fabs(b1->E[0]);
    r2 = fabs(Dot(D,b1->B[0]));
    if ( r2 > r0+r1 )
	return false;

    /* L = B1 */
    r0 = fabs(b0->E[0]*C[0][1])+fabs(b0->E[1]*C[1][1])+fabs(b0->E[2]*C[2][1]);
    r1 = fabs(b1->E[1]);
    r2 = fabs(Dot(D,b1->B[1]));
    if ( r2 > r0+r1 )
	return false;

    /* L = B2 */
    r0 = fabs(b0->E[0]*C[0][2])+fabs(b0->E[1]*C[1][2])+fabs(b0->E[2]*C[2][2]);
    r1 = fabs(b1->E[2]);
    r2 = fabs(Dot(D,b1->B[2]));
    if ( r2 > r0+r1 )
	return false;

    /* L = A0xB0 */
    r0 = fabs(b0->E[1]*C[2][0])+fabs(b0->E[2]*C[1][0]);
    r1 = fabs(b1->E[1]*C[0][2])+fabs(b1->E[2]*C[0][1]);
    r2 = fabs(d2*C[1][0]-d1*C[2][0]);
    if ( r2 > r0+r1 )
	return false;

    /* L = A0xB1 */
    r0 = fabs(b0->E[1]*C[2][1])+fabs(b0->E[2]*C[1][1]);
    r1 = fabs(b1->E[0]*C[0][2])+fabs(b1->E[2]*C[0][0]);
    r2 = fabs(d2*C[1][1]-d1*C[2][1]);
    if ( r2 > r0+r1 )
	return false;

    /* L = A0xB2 */
    r0 = fabs(b0->E[1]*C[2][2])+fabs(b0->E[2]*C[1][2]);
    r1 = fabs(b1->E[0]*C[0][1])+fabs(b1->E[1]*C[0][0]);
    r2 = fabs(d2*C[1][2]-d1*C[2][2]);
    if ( r2 > r0+r1 )
	return false;

    /* L = A1xB0 */
    r0 = fabs(b0->E[0]*C[2][0])+fabs(b0->E[2]*C[0][0]);
    r1 = fabs(b1->E[1]*C[1][2])+fabs(b1->E[2]*C[1][1]);
    r2 = fabs(d0*C[2][0]-d2*C[0][0]);
    if ( r2 > r0+r1 )
        return false;

    /* L = A1xB1 */
    r0 = fabs(b0->E[0]*C[2][1])+fabs(b0->E[2]*C[0][1]);
    r1 = fabs(b1->E[0]*C[1][2])+fabs(b1->E[2]*C[1][0]);
    r2 = fabs(d0*C[2][1]-d2*C[0][1]);
    if ( r2 > r0+r1 )
	return false;

    /* L = A1xB2 */
    r0 = fabs(b0->E[0]*C[2][2])+fabs(b0->E[2]*C[0][2]);
    r1 = fabs(b1->E[0]*C[1][1])+fabs(b1->E[1]*C[1][0]);
    r2 = fabs(d0*C[2][2]-d2*C[0][2]);
    if ( r2 > r0+r1 )
	return false;

    /* L = A2xB0 */
    r0 = fabs(b0->E[0]*C[1][0])+fabs(b0->E[1]*C[0][0]);
    r1 = fabs(b1->E[1]*C[2][2])+fabs(b1->E[2]*C[2][1]);
    r2 = fabs(d1*C[0][0]-d0*C[1][0]);
    if ( r2 > r0+r1 )
	return false;

    /* L = A2xB1 */
    r0 = fabs(b0->E[0]*C[1][1])+fabs(b0->E[1]*C[0][1]);
    r1 = fabs(b1->E[0]*C[2][2])+fabs(b1->E[2]*C[2][0]);
    r2 = fabs(d1*C[0][1]-d0*C[1][1]);
    if ( r2 > r0+r1 )
        return false;

    /* L = A2xB2 */
    r0 = fabs(b0->E[0]*C[1][2])+fabs(b0->E[1]*C[0][2]);
    r1 = fabs(b1->E[0]*C[2][1])+fabs(b1->E[1]*C[2][0]);
    r2 = fabs(d1*C[0][2]-d0*C[1][2]);
    if ( r2 > r0+r1 )
	return false;

    return true;
}

static
void frame_to_obb(const MxFitFrame& F, OBBox0 b0)
{
    // Must reparameterize cluster bounding box so that it extends equally
    // in all directions from the center

    Vec3 avg;
    Vec3& diff = b0.E;
    Vec3& ctr = b0.C;

    // avg = (max + min)/2
    mxv_add(avg, F.max(), F.min(), 3); mxv_scale(avg, 0.5, 3);
    // diff = (max - min)/2
    mxv_sub(diff, F.max(), F.min(), 3); mxv_scale(diff, 0.5, 3);

    F.from_frame(avg, ctr);

    mxv_set(b0.B[0], F.axis(0), 3);
    mxv_set(b0.B[1], F.axis(1), 3);
    mxv_set(b0.B[2], F.axis(2), 3);
}

bool MxFitFrame::check_intersection(const MxFitFrame& c1) const
{
    OBBox0 b0, b1;

    frame_to_obb(*this, b0);
    frame_to_obb(c1, b1);

    return TestIntersection0(&b0, &b1);
}
