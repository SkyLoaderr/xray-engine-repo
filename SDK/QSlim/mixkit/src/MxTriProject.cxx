/************************************************************************

  Code for projecting points onto the nearest point of a triangle.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxTriProject.cxx,v 1.4 1998/10/26 21:09:33 garland Exp $

 ************************************************************************/

/*
 * The projection code in this file was originally due to Hugues Hoppe
 * and was distributed under the following terms:
 *
 *      Copyright (c) 1992, 1993, 1994, Hugues Hoppe, University of Washington.
 *      Copying, use, and development for non-commercial purposes permitted.
 *                       All rights for commercial use reserved.
 *
 * Anoop Bhattacharjya <anoop@epal.smos.com> made some small changes.
 *
 * I made some small modifications, primarily to fix some C++ const-related
 * issues.  I've also added my own interface to the code.  My intention is
 * to rewrite it at some point, but at the moment, I have other things to do.
 * 
 *
 *      - Michael Garland
 *
 */

#include "stdmix.h"
#include "MxGeom3D.h"

#ifdef DOTP
#undef DOTP
#endif

#define DOTP(a, b) (a[0] * b[0] + a[1] * b[1] + a[2] * b[2])

static double Distance2(double x[3], double *y)
{
    double a, b, c;
	
    a = x[0] - y[0];  b = x[1] - y[1];  c = x[2] - y[2];
    return (a * a + b * b + c * c);
}

static void interp(double *proj, const double *p1, const double *p2,
		   const double *p3, const double *bary)
{
    proj[0] = p1[0] * bary[0] + p2[0] * bary[1] + p3[0] * bary[2];
    proj[1] = p1[1] * bary[0] + p2[1] * bary[1] + p3[1] * bary[2];
    proj[2] = p1[2] * bary[0] + p2[2] * bary[1] + p3[2] * bary[2];
}

static void Projecth(const double *v1, const double *v2, const double *v3,double *bary)
{
    int     i;
    double   vvi[3],vppi[3];
    double   d12sq, don12,d2, mind2, a;
    double   proj[3];
    double   pf[3][3];
    double   ba[3];
    double   cba[3];

    mind2 = 1e30;
    interp(proj,v1,v2,v3,bary);
    pf[0][0] = v1[0]; pf[0][1] = v1[1]; pf[0][2] = v1[2];
    pf[1][0] = v2[0]; pf[1][1] = v2[1]; pf[1][2] = v2[2];
    pf[2][0] = v3[0]; pf[2][1] = v3[1]; pf[2][2] = v3[2];

    ba[0] = bary[0]; ba[1] = bary[1]; ba[2] = bary[2];

    for (i = 0; i < 3; i++){
	if (ba[(i+2)%3] >= 0) continue;
	/* project proj onto segment pf[(i+0)%3]--pf[(i+1)%3]  */
         
	vvi[0] = pf[(i+1) % 3][0] - pf[i][0];
	vvi[1] = pf[(i+1) % 3][1] - pf[i][1];
	vvi[2] = pf[(i+1) % 3][2] - pf[i][2];
         
	vppi[0] = proj[0] - pf[i][0];
	vppi[1] = proj[1] - pf[i][1];
	vppi[2] = proj[2] - pf[i][2];

	d12sq = DOTP(vvi, vvi);
	don12 = DOTP(vvi, vppi);

	if (don12<=0) {
	    d2 = Distance2(pf[i], proj);
	    if (d2 >= mind2) continue;
	    mind2=d2; cba[i]=1; cba[(i+1)%3]=0; cba[(i+2)%3]=0;
	}
	else {
	    if (don12 >= d12sq) {
		d2 = Distance2(pf[(i+1)%3], proj);
		if (d2>=mind2) continue;
		mind2=d2; cba[i]=0; cba[(i+1)%3]=1; cba[(i+2)%3]=0;
	    }
	    else {
		a = don12/d12sq;
		cba[i]=1-a; cba[(i+1)%3]=a; cba[(i+2)%3]=0;
		break;
	    }
	}
    }

    bary[0] = cba[0]; bary[1] = cba[1]; bary[2] = cba[2];
}

static void ProjectPtri(const double *point,  const double *v1, 
			const double *v2,  const double *v3, double *bary)
{
    int    i;
    double  localv2[3], localv3[3], vpp1[3];
    double  v22,v33,v23,v2pp1,v3pp1;
    double  a1,a2,a3,denom;

    for (i = 0; i < 3; i++){
	localv2[i] = v2[i] - v1[i];
	localv3[i] = v3[i] - v1[i];
	vpp1[i] = point[i] - v1[i];
    }
	
    v22   = DOTP(localv2, localv2);
    v33   = DOTP(localv3, localv3);
    v23   = DOTP(localv2, localv3);
    v2pp1 = DOTP(localv2, vpp1);
    v3pp1 = DOTP(localv3, vpp1);
	
    if (!v22) v22=1;        /* recover if v2==0 */
    if (!v33) v33=1;        /* recover if v3==0 */

    denom = ( v33 - v23 * v23 / v22);
    if (!denom) {
	a2 = a3 = 1.0/3.0;    /* recover if v23*v23==v22*v33 */
    }
    else {
	a3=(v3pp1-v23/v22*v2pp1)/denom;
	a2=(v2pp1-a3*v23)/v22;
    }
    a1 = 1 - a2 - a3;

    bary[0] = a1; bary[1] = a2; bary[2] = a3;

    if ((a1 < 0) || (a2 < 0) || (a3 < 0)){
	Projecth(v1,v2,v3,bary);
	return;
    }
}

double triangle_project_point(const Vec3& v0, const Vec3& v1,
			      const Vec3& v2, const Vec3& v, 
			      Vec3 *bary)
{
    Vec3 __bary;

    if( !bary ) bary = &__bary;

    ProjectPtri(v, v0, v1, v2, *bary);

    Vec3 p = (*bary)[X]*v0 + (*bary)[Y]*v1 + (*bary)[Z]*v2;

    Vec3 diff = v - p;

    return diff*diff;
}
