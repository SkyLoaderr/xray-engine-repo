#include "stdafx.h"
#pragma hdrstop

#include "cl_intersect.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

bool FrustumPickFace(
	ICullPlane *planes, // four planes !
	Fvector& p0, Fvector& p1, Fvector& p2 )
{
	for( int i=0; i<4; i++)
		if( planes[i].dist_point( p0 )<0 &&
			planes[i].dist_point( p1 )<0 && 
			planes[i].dist_point( p2 )<0    )
				return false;
	return true;
};

bool PickFace(  Fvector& start, Fvector& direction,
                Fvector& p0, Fvector& p1, Fvector& p2,
                float *distance )
{
    Fvector facenormal;

    Fvector t1,t2,N;
    t1.sub(p1,p0);
    t2.sub(p2,p1);
    facenormal.crossproduct(t1,t2);
    facenormal.normalize();

	float clcheck = facenormal.dotproduct( direction );
	if( clcheck != 0.f ){

  		float d = - facenormal.dotproduct( p0 );
  		(*distance) = - ( (d + facenormal.dotproduct(start)) / clcheck );
		if( (*distance) <= 0 )
			return false;

  		Fvector hitpoint;
  		hitpoint.x = start.x + direction.x * (*distance);
  		hitpoint.y = start.y + direction.y * (*distance);
  		hitpoint.z = start.z + direction.z * (*distance);

  		Fvector np_from_p1;
  		np_from_p1.sub( hitpoint, p1 );
  		np_from_p1.normalize_safe();

  		Fvector tridir_10;
  		tridir_10.sub( p0, p1 );
  		tridir_10.normalize_safe();

  		Fvector tstn1;
  		tstn1.crossproduct( tridir_10, facenormal );

  		if( tstn1.dotproduct( np_from_p1 ) >= (-EPS) ){

  			Fvector np_from_p2;
  			np_from_p2.sub( hitpoint, p2 );
  			np_from_p2.normalize_safe();

  			Fvector tridir_21;
  			tridir_21.sub(p1,p2);
  			tridir_21.normalize_safe();

  			Fvector tstn2;
  			tstn2.crossproduct( tridir_21, facenormal );

  			if( tstn2.dotproduct( np_from_p2 ) >= (-EPS) ){

  				Fvector np_from_p0;
  				np_from_p0.sub( hitpoint, p0 );
  				np_from_p0.normalize_safe();

  				Fvector tridir_02;
  				tridir_02.sub(p2,p0);
  				tridir_02.normalize_safe();

  				Fvector tstn0;
  				tstn0.crossproduct( tridir_02, facenormal );

  				if( tstn0.dotproduct( np_from_p0 ) >= (-EPS) ){
  					return true;
  				}
  			}
  		}
	}
	return false;
};

/*
int intersect_triangle_barycentric(
   float orig[3], float dir[3],
   float vert0[3], float vert1[3], float vert2[3],
   float planeq[4], int i1, int i2,
   float *t, float *alpha, float *beta)
{
   float	dot, dot2;
   float	point[2];
   float	u0, v0, u1, v1, u2, v2;

   // is ray parallel to plane?
   dot = dir[0] * planeq[0] + dir[1] * planeq[1] + dir[2] * planeq[2];
   if (dot < EPSILON && dot > -EPSILON)		// or use culling check
      return 0;

   // find distance to plane and intersection point
   dot2 = orig[0] * planeq[0] +
      orig[1] * planeq[1] + orig[2] * planeq[2];
   *t = -(planeq[3] + dot2 ) / dot;
   point[0] = orig[i1] + dir[i1] * *t;
   point[1] = orig[i2] + dir[i2] * *t;

   // begin barycentric intersection algorithm
   u0 = point[0] - vert0[i1];
   v0 = point[1] - vert0[i2];
   u1 = vert1[i1] - vert0[i1];
   u2 = vert2[i1] - vert0[i1];
   v1 = vert1[i2] - vert0[i2];
   v2 = vert2[i2] - vert0[i2];

   // calculate and compare barycentric coordinates
   if (u1 == 0) {		// uncommon case
      *beta = u0 / u2;
      if (*beta < 0 || *beta > 1)
	       return 0;
      *alpha = (v0 - *beta * v2) / v1;
   }
   else {			// common case, used for this analysis
      *beta = (v0 * u1 - u0 * v1) / (v2 * u1 - u2 * v1);
      if (*beta < 0 || *beta > 1)
	       return 0;
      *alpha = (u0 - *beta * u2) / u1;
   }

   if (*alpha < 0 || (*alpha + *beta) > 1.0)
      return 0;

   return 1;
}
*/



