#include "stdafx.h"
#pragma hdrstop

#include "cl_RAPID.h"

#define myfabs(x) fabsf(x)

__forceinline float fmax(float a, float b, float c)
{
  float t = a;
  if (b > t) t = b;
  if (c > t) t = c;
  return t;
}

__forceinline float fmin(float a, float b, float c)
{
  float t = a;
  if (b < t) t = b;
  if (c < t) t = c;
  return t;
}


int project6(	Fvector& ax,
				Fvector& p1, Fvector& p2, Fvector& p3,
				Fvector& q1, Fvector& q2, Fvector& q3)
{
  float P1 = ax.dotproduct(p1);
  float P2 = ax.dotproduct(p2);
  float P3 = ax.dotproduct(p3);
  float Q1 = ax.dotproduct(q1);
  float Q2 = ax.dotproduct(q2);
  float Q3 = ax.dotproduct(q3);

  float mx1 = fmax(P1, P2, P3);
  float mn1 = fmin(P1, P2, P3);
  float mx2 = fmax(Q1, Q2, Q3);
  float mn2 = fmin(Q1, Q2, Q3);

  if (mn1 > mx2) return 0;
  if (mn2 > mx1) return 0;
  return 1;
}


// very robust triangle intersection test
// uses no divisions
// works on coplanar triangles

int XRCollide::tri_contact (Fvector& P1, Fvector& P2, Fvector& P3,
					        Fvector& Q1, Fvector& Q2, Fvector& Q3)
{

  /*
     One triangle is (p1,p2,p3).  Other is (q1,q2,q3).
     Edges are (e1,e2,e3) and (f1,f2,f3).
     Normals are n1 and m1
     Outwards are (g1,g2,g3) and (h1,h2,h3).

     We assume that the triangle vertices are in the same coordinate system.

     First thing we do is establish a new c.s. so that p1 is at (0,0,0).

     */

  Fvector p1, p2, p3;
  Fvector q1, q2, q3;
  Fvector e1, e2, e3;
  Fvector f1, f2, f3;
  Fvector g1, g2, g3;
  Fvector h1, h2, h3;
  Fvector n1, m1;
  Fvector z;

  Fvector ef11, ef12, ef13;
  Fvector ef21, ef22, ef23;
  Fvector ef31, ef32, ef33;

  z.set(0.f,0.f,0.f);

  p1.m[0] = P1.m[0] - P1.m[0];  p1.m[1] = P1.m[1] - P1.m[1];  p1.m[2] = P1.m[2] - P1.m[2];
  p2.m[0] = P2.m[0] - P1.m[0];  p2.m[1] = P2.m[1] - P1.m[1];  p2.m[2] = P2.m[2] - P1.m[2];
  p3.m[0] = P3.m[0] - P1.m[0];  p3.m[1] = P3.m[1] - P1.m[1];  p3.m[2] = P3.m[2] - P1.m[2];

  q1.m[0] = Q1.m[0] - P1.m[0];  q1.m[1] = Q1.m[1] - P1.m[1];  q1.m[2] = Q1.m[2] - P1.m[2];
  q2.m[0] = Q2.m[0] - P1.m[0];  q2.m[1] = Q2.m[1] - P1.m[1];  q2.m[2] = Q2.m[2] - P1.m[2];
  q3.m[0] = Q3.m[0] - P1.m[0];  q3.m[1] = Q3.m[1] - P1.m[1];  q3.m[2] = Q3.m[2] - P1.m[2];

  e1.m[0] = p2.m[0] - p1.m[0];  e1.m[1] = p2.m[1] - p1.m[1];  e1.m[2] = p2.m[2] - p1.m[2];
  e2.m[0] = p3.m[0] - p2.m[0];  e2.m[1] = p3.m[1] - p2.m[1];  e2.m[2] = p3.m[2] - p2.m[2];
  e3.m[0] = p1.m[0] - p3.m[0];  e3.m[1] = p1.m[1] - p3.m[1];  e3.m[2] = p1.m[2] - p3.m[2];

  f1.m[0] = q2.m[0] - q1.m[0];  f1.m[1] = q2.m[1] - q1.m[1];  f1.m[2] = q2.m[2] - q1.m[2];
  f2.m[0] = q3.m[0] - q2.m[0];  f2.m[1] = q3.m[1] - q2.m[1];  f2.m[2] = q3.m[2] - q2.m[2];
  f3.m[0] = q1.m[0] - q3.m[0];  f3.m[1] = q1.m[1] - q3.m[1];  f3.m[2] = q1.m[2] - q3.m[2];

  n1.crossproduct(e1, e2);
  m1.crossproduct(f1, f2);

  g1.crossproduct(e1, n1);
  g2.crossproduct(e2, n1);
  g3.crossproduct(e3, n1);
  h1.crossproduct(f1, m1);
  h2.crossproduct(f2, m1);
  h3.crossproduct(f3, m1);

  ef11.crossproduct(e1, f1);
  ef12.crossproduct(e1, f2);
  ef13.crossproduct(e1, f3);
  ef21.crossproduct(e2, f1);
  ef22.crossproduct(e2, f2);
  ef23.crossproduct(e2, f3);
  ef31.crossproduct(e3, f1);
  ef32.crossproduct(e3, f2);
  ef33.crossproduct(e3, f3);

  // now begin the series of tests

  if (!project6(n1, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(m1, p1, p2, p3, q1, q2, q3)) return 0;

  if (!project6(ef11, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(ef12, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(ef13, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(ef21, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(ef22, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(ef23, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(ef31, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(ef32, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(ef33, p1, p2, p3, q1, q2, q3)) return 0;

  if (!project6(g1, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(g2, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(g3, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(h1, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(h2, p1, p2, p3, q1, q2, q3)) return 0;
  if (!project6(h3, p1, p2, p3, q1, q2, q3)) return 0;

  return 1;
}



/*

int
obb_disjoint(float B[3][3], float T[3], float a[3], float b[3]);

This is a test between two boxes, box A and box B.  It is assumed that
the coordinate system is aligned and centered on box A.  The 3x3
matrix B specifies box B's orientation with respect to box A.
Specifically, the columns of B are the basis vectors (axis vectors) of
box B.  The center of box B is located at the vector T.  The
dimensions of box B are given in the array b.  The orientation and
placement of box A, in this coordinate system, are the identity matrix
and zero vector, respectively, so they need not be specified.  The
dimensions of box A are given in array a.

This test operates in two modes, depending on how the library is
compiled.  It indicates whether the two boxes are overlapping, by
returning a boolean.

The second version of the routine will return a conservative bounds on
the distance between the polygon sets which the boxes enclose.  It is
used when RAPID is being used to estimate the distance between two
models.

*/


int XRCollide::obb_disjoint(Fmatrix33& B, Fvector& T, Fvector& a, Fvector& b)
{
  register float t, s;
  register int r;
  Fmatrix33 Bf;
  const float reps = 0.000001f;

  // Bf = fabs(B)
  Bf.m[0][0] = myfabs(B.m[0][0]);  Bf.m[0][0] += reps;
  Bf.m[0][1] = myfabs(B.m[0][1]);  Bf.m[0][1] += reps;
  Bf.m[0][2] = myfabs(B.m[0][2]);  Bf.m[0][2] += reps;
  Bf.m[1][0] = myfabs(B.m[1][0]);  Bf.m[1][0] += reps;
  Bf.m[1][1] = myfabs(B.m[1][1]);  Bf.m[1][1] += reps;
  Bf.m[1][2] = myfabs(B.m[1][2]);  Bf.m[1][2] += reps;
  Bf.m[2][0] = myfabs(B.m[2][0]);  Bf.m[2][0] += reps;
  Bf.m[2][1] = myfabs(B.m[2][1]);  Bf.m[2][1] += reps;
  Bf.m[2][2] = myfabs(B.m[2][2]);  Bf.m[2][2] += reps;

  // if any of these tests are one-sided, then the polyhedra are disjoint
  r = 1;

  // A1 x A2 = A0
  t = myfabs(T.x);

  r &= (t <=
	  (a.x + b.x * Bf.m[0][0] + b.y * Bf.m[0][1] + b.z * Bf.m[0][2]));
  if (!r) return 1;

  // B1 x B2 = B0
  s = T.x*B.m[0][0] + T.y*B.m[1][0] + T.z*B.m[2][0];
  t = myfabs(s);

  r &= ( t <=
	  (b.x + a.x * Bf.m[0][0] + a.y * Bf.m[1][0] + a.z * Bf.m[2][0]));
  if (!r) return 2;

  // A2 x A0 = A1
  t = myfabs(T.y);

  r &= ( t <=
	  (a.y + b.x * Bf.m[1][0] + b.y * Bf.m[1][1] + b.z * Bf.m[1][2]));
  if (!r) return 3;

  // A0 x A1 = A2
  t = myfabs(T.z);

  r &= ( t <=
	  (a.z + b.x * Bf.m[2][0] + b.y * Bf.m[2][1] + b.z * Bf.m[2][2]));
  if (!r) return 4;

  // B2 x B0 = B1
  s = T.x*B.m[0][1] + T.y*B.m[1][1] + T.z*B.m[2][1];
  t = myfabs(s);

  r &= ( t <=
	  (b.y + a.x * Bf.m[0][1] + a.y * Bf.m[1][1] + a.z * Bf.m[2][1]));
  if (!r) return 5;

  // B0 x B1 = B2
  s = T.x*B.m[0][2] + T.y*B.m[1][2] + T.z*B.m[2][2];
  t = myfabs(s);

  r &= ( t <=
	  (b.z + a.x * Bf.m[0][2] + a.y * Bf.m[1][2] + a.z * Bf.m[2][2]));
  if (!r) return 6;

  // A0 x B0
  s = T.z * B.m[1][0] - T.y * B.m[2][0];
  t = myfabs(s);

  r &= ( t <=
	(a.y * Bf.m[2][0] + a.z * Bf.m[1][0] +
	 b.y * Bf.m[0][2] + b.z * Bf.m[0][1]));
  if (!r) return 7;

  // A0 x B1
  s = T.z * B.m[1][1] - T.y * B.m[2][1];
  t = myfabs(s);

  r &= ( t <=
	(a.y * Bf.m[2][1] + a.z * Bf.m[1][1] +
	 b.x * Bf.m[0][2] + b.z * Bf.m[0][0]));
  if (!r) return 8;

  // A0 x B2
  s = T.z * B.m[1][2] - T.y * B.m[2][2];
  t = myfabs(s);

  r &= ( t <=
	  (a.y * Bf.m[2][2] + a.z * Bf.m[1][2] +
	   b.x * Bf.m[0][1] + b.y * Bf.m[0][0]));
  if (!r) return 9;

  // A1 x B0
  s = T.x * B.m[2][0] - T.z * B.m[0][0];
  t = myfabs(s);

  r &= ( t <=
	  (a.x * Bf.m[2][0] + a.z * Bf.m[0][0] +
	   b.y * Bf.m[1][2] + b.z * Bf.m[1][1]));
  if (!r) return 10;

  // A1 x B1
  s = T.x * B.m[2][1] - T.z * B.m[0][1];
  t = myfabs(s);

  r &= ( t <=
	  (a.x * Bf.m[2][1] + a.z * Bf.m[0][1] +
	   b.x * Bf.m[1][2] + b.z * Bf.m[1][0]));
  if (!r) return 11;

  // A1 x B2
  s = T.x * B.m[2][2] - T.z * B.m[0][2];
  t = myfabs(s);

  r &= (t <=
	  (a.x * Bf.m[2][2] + a.z * Bf.m[0][2] +
	   b.x * Bf.m[1][1] + b.y * Bf.m[1][0]));
  if (!r) return 12;

  // A2 x B0
  s = T.y * B.m[0][0] - T.x * B.m[1][0];
  t = myfabs(s);

  r &= (t <=
	  (a.x * Bf.m[1][0] + a.y * Bf.m[0][0] +
	   b.y * Bf.m[2][2] + b.z * Bf.m[2][1]));
  if (!r) return 13;

  // A2 x B1
  s = T.y * B.m[0][1] - T.x * B.m[1][1];
  t = myfabs(s);

  r &= ( t <=
	  (a.x * Bf.m[1][1] + a.y * Bf.m[0][1] +
	   b.x * Bf.m[2][2] + b.z * Bf.m[2][0]));
  if (!r) return 14;

  // A2 x B2
  s = T.y * B.m[0][2] - T.x * B.m[1][2];
  t = myfabs(s);

  r &= ( t <=
	  (a.x * Bf.m[1][2] + a.y * Bf.m[0][2] +
	   b.x * Bf.m[2][1] + b.y * Bf.m[2][0]));
  if (!r) return 15;

  return 0;  // should equal 0
}

