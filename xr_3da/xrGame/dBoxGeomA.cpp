#include "stdafx.h"
#include <ode/geom.h>
#include <ode/rotation.h>
#include <ode/odemath.h>
#include <ode/memory.h>
#include <ode/misc.h>
#include <ode/objects.h>
#include <ode/matrix.h>
#include <ode/src/geom_internal.h>
#include "dBoxGeomA.h"

struct dxCylinder {	// cylinder
  dReal radius,lz;	// radius, length along y axis //
};

struct dxSphere {
  dReal radius;		// sphere radius
};

struct dxBox {
  dVector3 side;	// side lengths (x,y,z)
};

int dBoxAClass = -1;


#define CONTACT(p,skip) ((dContactGeom*) (((char*)p) + (skip)))

 void lineClosestApproach (const dVector3 pa, const dVector3 ua,
				 const dVector3 pb, const dVector3 ub,
				 dReal *alpha, dReal *beta);




extern "C" int dBoxABoxA (const dVector3 p1, const dMatrix3 R1,
			const dVector3 side1, const dVector3 p2,
			const dMatrix3 R2, const dVector3 side2,
			dVector3 normal, dReal *depth, int *code,
			int maxc, dContactGeom *contact, int skip)
{
  dVector3 p,pp,normalC;
  const dReal *normalR = 0;
  dReal A1,A2,A3,B1,B2,B3,R11,R12,R13,R21,R22,R23,R31,R32,R33,
    Q11,Q12,Q13,Q21,Q22,Q23,Q31,Q32,Q33,s,s2,l;
  int i,invert_normal;

  // get vector from centers of box 1 to box 2, relative to box 1
  p[0] = p2[0] - p1[0];
  p[1] = p2[1] - p1[1];
  p[2] = p2[2] - p1[2];
  dMULTIPLY1_331 (pp,R1,p);		// get pp = p relative to body 1

  // get side lengths / 2
  A1 = side1[0]*REAL(0.5); A2 = side1[1]*REAL(0.5); A3 = side1[2]*REAL(0.5);
  B1 = side2[0]*REAL(0.5); B2 = side2[1]*REAL(0.5); B3 = side2[2]*REAL(0.5);

  // Rij is R1'*R2, i.e. the relative rotation between R1 and R2
  R11 = dDOT44(R1+0,R2+0); R12 = dDOT44(R1+0,R2+1); R13 = dDOT44(R1+0,R2+2);
  R21 = dDOT44(R1+1,R2+0); R22 = dDOT44(R1+1,R2+1); R23 = dDOT44(R1+1,R2+2);
  R31 = dDOT44(R1+2,R2+0); R32 = dDOT44(R1+2,R2+1); R33 = dDOT44(R1+2,R2+2);

  Q11 = dFabs(R11); Q12 = dFabs(R12); Q13 = dFabs(R13);
  Q21 = dFabs(R21); Q22 = dFabs(R22); Q23 = dFabs(R23);
  Q31 = dFabs(R31); Q32 = dFabs(R32); Q33 = dFabs(R33);

  // for all 15 possible separating axes:
  //   * see if the axis separates the boxes. if so, return 0.
  //   * find the depth of the penetration along the separating axis (s2)
  //   * if this is the largest depth so far, record it.
  // the normal vector will be set to the separating axis with the smallest
  // depth. note: normalR is set to point to a column of R1 or R2 if that is
  // the smallest depth normal so far. otherwise normalR is 0 and normalC is
  // set to a vector relative to body 1. invert_normal is 1 if the sign of
  // the normal should be flipped.

#define TEST(expr1,expr2,norm,cc) \
  s2 = dFabs(expr1) - (expr2); \
  if (s2 > 0) return 0; \
  if (s2 > s) { \
    s = s2; \
    normalR = norm; \
    invert_normal = ((expr1) < 0); \
    *code = (cc); \
  }

  s = -dInfinity;
  invert_normal = 0;
  *code = 0;

  // separating axis = u1,u2,u3
  TEST (pp[0],(A1 + B1*Q11 + B2*Q12 + B3*Q13),R1+0,1);
  TEST (pp[1],(A2 + B1*Q21 + B2*Q22 + B3*Q23),R1+1,2);
  TEST (pp[2],(A3 + B1*Q31 + B2*Q32 + B3*Q33),R1+2,3);

  // separating axis = v1,v2,v3
  TEST (dDOT41(R2+0,p),(A1*Q11 + A2*Q21 + A3*Q31 + B1),R2+0,4);
  TEST (dDOT41(R2+1,p),(A1*Q12 + A2*Q22 + A3*Q32 + B2),R2+1,5);
  TEST (dDOT41(R2+2,p),(A1*Q13 + A2*Q23 + A3*Q33 + B3),R2+2,6);

  // note: cross product axes need to be scaled when s is computed.
  // normal (n1,n2,n3) is relative to box 1.
#undef TEST
#define TEST(expr1,expr2,n1,n2,n3,cc) \
  s2 = dFabs(expr1) - (expr2); \
  if (s2 > 0) return 0; \
  l = dSqrt ((n1)*(n1) + (n2)*(n2) + (n3)*(n3)); \
  if (l > 0) { \
    s2 /= l; \
    if (s2 > s) { \
      s = s2; \
      normalR = 0; \
      normalC[0] = (n1)/l; normalC[1] = (n2)/l; normalC[2] = (n3)/l; \
      invert_normal = ((expr1) < 0); \
      *code = (cc); \
    } \
  }

  // separating axis = u1 x (v1,v2,v3)
  TEST(pp[2]*R21-pp[1]*R31,(A2*Q31+A3*Q21+B2*Q13+B3*Q12),0,-R31,R21,7);
  TEST(pp[2]*R22-pp[1]*R32,(A2*Q32+A3*Q22+B1*Q13+B3*Q11),0,-R32,R22,8);
  TEST(pp[2]*R23-pp[1]*R33,(A2*Q33+A3*Q23+B1*Q12+B2*Q11),0,-R33,R23,9);

  // separating axis = u2 x (v1,v2,v3)
  TEST(pp[0]*R31-pp[2]*R11,(A1*Q31+A3*Q11+B2*Q23+B3*Q22),R31,0,-R11,10);
  TEST(pp[0]*R32-pp[2]*R12,(A1*Q32+A3*Q12+B1*Q23+B3*Q21),R32,0,-R12,11);
  TEST(pp[0]*R33-pp[2]*R13,(A1*Q33+A3*Q13+B1*Q22+B2*Q21),R33,0,-R13,12);

  // separating axis = u3 x (v1,v2,v3)
  TEST(pp[1]*R11-pp[0]*R21,(A1*Q21+A2*Q11+B2*Q33+B3*Q32),-R21,R11,0,13);
  TEST(pp[1]*R12-pp[0]*R22,(A1*Q22+A2*Q12+B1*Q33+B3*Q31),-R22,R12,0,14);
  TEST(pp[1]*R13-pp[0]*R23,(A1*Q23+A2*Q13+B1*Q32+B2*Q31),-R23,R13,0,15);

#undef TEST

  // if we get to this point, the boxes interpenetrate. compute the normal
  // in global coordinates.
  if (normalR) {
    normal[0] = normalR[0];
    normal[1] = normalR[4];
    normal[2] = normalR[8];
  }
  else {
    dMULTIPLY0_331 (normal,R1,normalC);
  }
  if (invert_normal) {
    normal[0] = -normal[0];
    normal[1] = -normal[1];
    normal[2] = -normal[2];
  }
  *depth = -s;

  // compute contact point(s)

  if (*code > 6) {
    // an edge from box 1 touches an edge from box 2.
    // find a point pa on the intersecting edge of box 1
    dVector3 pa;
    dReal sign;
    for (i=0; i<3; i++) pa[i] = p1[i];
    sign = (dDOT14(normal,R1+0) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) pa[i] += sign * A1 * R1[i*4];
    sign = (dDOT14(normal,R1+1) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) pa[i] += sign * A2 * R1[i*4+1];
    sign = (dDOT14(normal,R1+2) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) pa[i] += sign * A3 * R1[i*4+2];

    // find a point pb on the intersecting edge of box 2
    dVector3 pb;
    for (i=0; i<3; i++) pb[i] = p2[i];
    sign = (dDOT14(normal,R2+0) > 0) ? REAL(-1.0) : REAL(1.0);
    for (i=0; i<3; i++) pb[i] += sign * B1 * R2[i*4];
    sign = (dDOT14(normal,R2+1) > 0) ? REAL(-1.0) : REAL(1.0);
    for (i=0; i<3; i++) pb[i] += sign * B2 * R2[i*4+1];
    sign = (dDOT14(normal,R2+2) > 0) ? REAL(-1.0) : REAL(1.0);
    for (i=0; i<3; i++) pb[i] += sign * B3 * R2[i*4+2];

    dReal alpha,beta;
    dVector3 ua,ub;
    for (i=0; i<3; i++) ua[i] = R1[((*code)-7)/3 + i*4];
    for (i=0; i<3; i++) ub[i] = R2[((*code)-7)%3 + i*4];

    lineClosestApproach (pa,ua,pb,ub,&alpha,&beta);
    for (i=0; i<3; i++) pa[i] += ua[i]*alpha;
    for (i=0; i<3; i++) pb[i] += ub[i]*beta;

    for (i=0; i<3; i++) contact[0].pos[i] = REAL(0.5)*(pa[i]+pb[i]);
    contact[0].depth = *depth;
    return 1;
  }

  // okay, we have a face-something intersection (because the separating
  // axis is perpendicular to a face).

  // @@@ temporary: make deepest vertex on the "other" box the contact point.
  // @@@ this kind of works, but we need multiple contact points for stability,
  // @@@ especially for face-face contact.

  dVector3 vertex;
  if (*code <= 3) {
    // face from box 1 touches a vertex/edge/face from box 2.
    dReal sign;
    for (i=0; i<3; i++) vertex[i] = p2[i];
    sign = (dDOT14(normal,R2+0) > 0) ? REAL(-1.0) : REAL(1.0);
    for (i=0; i<3; i++) vertex[i] += sign * B1 * R2[i*4];
    sign = (dDOT14(normal,R2+1) > 0) ? REAL(-1.0) : REAL(1.0);
    for (i=0; i<3; i++) vertex[i] += sign * B2 * R2[i*4+1];
    sign = (dDOT14(normal,R2+2) > 0) ? REAL(-1.0) : REAL(1.0);
    for (i=0; i<3; i++) vertex[i] += sign * B3 * R2[i*4+2];
  }
  else {
    // face from box 2 touches a vertex/edge/face from box 1.
    dReal sign;
    for (i=0; i<3; i++) vertex[i] = p1[i];
    sign = (dDOT14(normal,R1+0) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) vertex[i] += sign * A1 * R1[i*4];
    sign = (dDOT14(normal,R1+1) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) vertex[i] += sign * A2 * R1[i*4+1];
    sign = (dDOT14(normal,R1+2) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) vertex[i] += sign * A3 * R1[i*4+2];
  }
  for (i=0; i<3; i++) contact[0].pos[i] = vertex[i];
  contact[0].depth = *depth;
  return 1;
}



int dCollideBABA (const dxGeom *o1, const dxGeom *o2, int flags,
		dContactGeom *contact, int skip)
{
  dVector3 normal;
  dReal depth;
  int code;
  dxBox *b1 = (dxBox*) CLASSDATA(o1);
  dxBox *b2 = (dxBox*) CLASSDATA(o2);
  int num = dBoxABoxA (o1->pos,o1->R,b1->side, o2->pos,o2->R,b2->side,
		     normal,&depth,&code,flags & NUMC_MASK,contact,skip);
  for (int i=0; i<num; i++) {
    CONTACT(contact,i*skip)->normal[0] = -normal[0];
    CONTACT(contact,i*skip)->normal[1] = -normal[1];
    CONTACT(contact,i*skip)->normal[2] = -normal[2];
    CONTACT(contact,i*skip)->g1 = const_cast<dxGeom*> (o1);
    CONTACT(contact,i*skip)->g2 = const_cast<dxGeom*> (o2);
  }
  return num;
}


static int dBoxBox (const dVector3 p1, const dMatrix3 R1,
			const dVector3 side1, const dVector3 p2,
			const dMatrix3 R2, const dVector3 side2,
			dVector3 normal, dReal *depth, int *code,
			int maxc, dContactGeom *contact, int skip)
{
  dVector3 p,pp,normalC;
  const dReal *normalR = 0;
  dReal A1,A2,A3,B1,B2,B3,R11,R12,R13,R21,R22,R23,R31,R32,R33,
    Q11,Q12,Q13,Q21,Q22,Q23,Q31,Q32,Q33,s,s2,l;
  int i,invert_normal;

  // get vector from centers of box 1 to box 2, relative to box 1
  p[0] = p2[0] - p1[0];
  p[1] = p2[1] - p1[1];
  p[2] = p2[2] - p1[2];
  dMULTIPLY1_331 (pp,R1,p);		// get pp = p relative to body 1

  // get side lengths / 2
  A1 = side1[0]*REAL(0.5); A2 = side1[1]*REAL(0.5); A3 = side1[2]*REAL(0.5);
  B1 = side2[0]*REAL(0.5); B2 = side2[1]*REAL(0.5); B3 = side2[2]*REAL(0.5);

  // Rij is R1'*R2, i.e. the relative rotation between R1 and R2
  R11 = dDOT44(R1+0,R2+0); R12 = dDOT44(R1+0,R2+1); R13 = dDOT44(R1+0,R2+2);
  R21 = dDOT44(R1+1,R2+0); R22 = dDOT44(R1+1,R2+1); R23 = dDOT44(R1+1,R2+2);
  R31 = dDOT44(R1+2,R2+0); R32 = dDOT44(R1+2,R2+1); R33 = dDOT44(R1+2,R2+2);

  Q11 = dFabs(R11); Q12 = dFabs(R12); Q13 = dFabs(R13);
  Q21 = dFabs(R21); Q22 = dFabs(R22); Q23 = dFabs(R23);
  Q31 = dFabs(R31); Q32 = dFabs(R32); Q33 = dFabs(R33);

  // for all 15 possible separating axes:
  //   * see if the axis separates the boxes. if so, return 0.
  //   * find the depth of the penetration along the separating axis (s2)
  //   * if this is the largest depth so far, record it.
  // the normal vector will be set to the separating axis with the smallest
  // depth. note: normalR is set to point to a column of R1 or R2 if that is
  // the smallest depth normal so far. otherwise normalR is 0 and normalC is
  // set to a vector relative to body 1. invert_normal is 1 if the sign of
  // the normal should be flipped.

#define TEST(expr1,expr2,norm,cc) \
  s2 = dFabs(expr1) - (expr2); \
  if (s2 > 0) return 0; \
  if (s2 > s) { \
    s = s2; \
    normalR = norm; \
    invert_normal = ((expr1) < 0); \
    *code = (cc); \
  }

  s = -dInfinity;
  invert_normal = 0;
  *code = 0;

  // separating axis = u1,u2,u3
  TEST (pp[0],(A1 + B1*Q11 + B2*Q12 + B3*Q13),R1+0,1);
  TEST (pp[1],(A2 + B1*Q21 + B2*Q22 + B3*Q23),R1+1,2);
  TEST (pp[2],(A3 + B1*Q31 + B2*Q32 + B3*Q33),R1+2,3);

  // separating axis = v1,v2,v3
  TEST (dDOT41(R2+0,p),(A1*Q11 + A2*Q21 + A3*Q31 + B1),R2+0,4);
  TEST (dDOT41(R2+1,p),(A1*Q12 + A2*Q22 + A3*Q32 + B2),R2+1,5);
  TEST (dDOT41(R2+2,p),(A1*Q13 + A2*Q23 + A3*Q33 + B3),R2+2,6);

  // note: cross product axes need to be scaled when s is computed.
  // normal (n1,n2,n3) is relative to box 1.
#undef TEST
#define TEST(expr1,expr2,n1,n2,n3,cc) \
  s2 = dFabs(expr1) - (expr2); \
  if (s2 > 0) return 0; \
  l = dSqrt ((n1)*(n1) + (n2)*(n2) + (n3)*(n3)); \
  if (l > 0) { \
    s2 /= l; \
    if (s2 > s) { \
      s = s2; \
      normalR = 0; \
      normalC[0] = (n1)/l; normalC[1] = (n2)/l; normalC[2] = (n3)/l; \
      invert_normal = ((expr1) < 0); \
      *code = (cc); \
    } \
  }

  // separating axis = u1 x (v1,v2,v3)
  TEST(pp[2]*R21-pp[1]*R31,(A2*Q31+A3*Q21+B2*Q13+B3*Q12),0,-R31,R21,7);
  TEST(pp[2]*R22-pp[1]*R32,(A2*Q32+A3*Q22+B1*Q13+B3*Q11),0,-R32,R22,8);
  TEST(pp[2]*R23-pp[1]*R33,(A2*Q33+A3*Q23+B1*Q12+B2*Q11),0,-R33,R23,9);

  // separating axis = u2 x (v1,v2,v3)
  TEST(pp[0]*R31-pp[2]*R11,(A1*Q31+A3*Q11+B2*Q23+B3*Q22),R31,0,-R11,10);
  TEST(pp[0]*R32-pp[2]*R12,(A1*Q32+A3*Q12+B1*Q23+B3*Q21),R32,0,-R12,11);
  TEST(pp[0]*R33-pp[2]*R13,(A1*Q33+A3*Q13+B1*Q22+B2*Q21),R33,0,-R13,12);

  // separating axis = u3 x (v1,v2,v3)
  TEST(pp[1]*R11-pp[0]*R21,(A1*Q21+A2*Q11+B2*Q33+B3*Q32),-R21,R11,0,13);
  TEST(pp[1]*R12-pp[0]*R22,(A1*Q22+A2*Q12+B1*Q33+B3*Q31),-R22,R12,0,14);
  TEST(pp[1]*R13-pp[0]*R23,(A1*Q23+A2*Q13+B1*Q32+B2*Q31),-R23,R13,0,15);

#undef TEST

  // if we get to this point, the boxes interpenetrate. compute the normal
  // in global coordinates.
  if (normalR) {
    normal[0] = normalR[0];
    normal[1] = normalR[4];
    normal[2] = normalR[8];
  }
  else {
    dMULTIPLY0_331 (normal,R1,normalC);
  }
  if (invert_normal) {
    normal[0] = -normal[0];
    normal[1] = -normal[1];
    normal[2] = -normal[2];
  }
  *depth = -s;

  // compute contact point(s)

  if (*code > 6) {
    // an edge from box 1 touches an edge from box 2.
    // find a point pa on the intersecting edge of box 1
    dVector3 pa;
    dReal sign;
    for (i=0; i<3; i++) pa[i] = p1[i];
    sign = (dDOT14(normal,R1+0) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) pa[i] += sign * A1 * R1[i*4];
    sign = (dDOT14(normal,R1+1) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) pa[i] += sign * A2 * R1[i*4+1];
    sign = (dDOT14(normal,R1+2) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) pa[i] += sign * A3 * R1[i*4+2];

    // find a point pb on the intersecting edge of box 2
    dVector3 pb;
    for (i=0; i<3; i++) pb[i] = p2[i];
    sign = (dDOT14(normal,R2+0) > 0) ? REAL(-1.0) : REAL(1.0);
    for (i=0; i<3; i++) pb[i] += sign * B1 * R2[i*4];
    sign = (dDOT14(normal,R2+1) > 0) ? REAL(-1.0) : REAL(1.0);
    for (i=0; i<3; i++) pb[i] += sign * B2 * R2[i*4+1];
    sign = (dDOT14(normal,R2+2) > 0) ? REAL(-1.0) : REAL(1.0);
    for (i=0; i<3; i++) pb[i] += sign * B3 * R2[i*4+2];

    dReal alpha,beta;
    dVector3 ua,ub;
    for (i=0; i<3; i++) ua[i] = R1[((*code)-7)/3 + i*4];
    for (i=0; i<3; i++) ub[i] = R2[((*code)-7)%3 + i*4];

    lineClosestApproach (pa,ua,pb,ub,&alpha,&beta);
    for (i=0; i<3; i++) pa[i] += ua[i]*alpha;
    for (i=0; i<3; i++) pb[i] += ub[i]*beta;

    for (i=0; i<3; i++) contact[0].pos[i] = REAL(0.5)*(pa[i]+pb[i]);
    contact[0].depth = *depth;
    return 1;
  }

  // okay, we have a face-something intersection (because the separating
  // axis is perpendicular to a face).

  // @@@ temporary: make deepest vertex on the "other" box the contact point.
  // @@@ this kind of works, but we need multiple contact points for stability,
  // @@@ especially for face-face contact.

  dVector3 vertex;
  if (*code <= 3) {
    // face from box 1 touches a vertex/edge/face from box 2.
    dReal sign;
    for (i=0; i<3; i++) vertex[i] = p2[i];
    sign = (dDOT14(normal,R2+0) > 0) ? REAL(-1.0) : REAL(1.0);
    for (i=0; i<3; i++) vertex[i] += sign * B1 * R2[i*4];
    sign = (dDOT14(normal,R2+1) > 0) ? REAL(-1.0) : REAL(1.0);
    for (i=0; i<3; i++) vertex[i] += sign * B2 * R2[i*4+1];
    sign = (dDOT14(normal,R2+2) > 0) ? REAL(-1.0) : REAL(1.0);
    for (i=0; i<3; i++) vertex[i] += sign * B3 * R2[i*4+2];
  }
  else {
    // face from box 2 touches a vertex/edge/face from box 1.
    dReal sign;
    for (i=0; i<3; i++) vertex[i] = p1[i];
    sign = (dDOT14(normal,R1+0) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) vertex[i] += sign * A1 * R1[i*4];
    sign = (dDOT14(normal,R1+1) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) vertex[i] += sign * A2 * R1[i*4+1];
    sign = (dDOT14(normal,R1+2) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) vertex[i] += sign * A3 * R1[i*4+2];
  }
  for (i=0; i<3; i++) contact[0].pos[i] = vertex[i];
  contact[0].depth = *depth;
  return 1;
}



static int FUNCCALL dCollideBBA (const dxGeom *o1, const dxGeom *o2, int flags,
		dContactGeom *contact, int skip)
{
  dVector3 normal;
  dReal depth;
  int code;
  dxBox *b1 = (dxBox*) CLASSDATA(o1);
  dxBox *b2 = (dxBox*) CLASSDATA(o2);
  dxBox *box;
  dReal* R;
  
  int ret = dBoxBox (o1->pos,o1->R,b1->side, o2->pos,o2->R,b2->side,
		     normal,&depth,&code,flags & NUMC_MASK,contact,skip);
  if(ret==0) return 0;

////////////////////////////////////////////////////////////////////////////////////
  dVector3 p;
  dVector3 n;
  if(code<3){
  p[0] = o2->pos[0];
  p[1] = o2->pos[1];
  p[2] = o2->pos[2];
  box=b2;
  R=o2->R;
  n[0]=-normal[0];
  n[1]=-normal[1];
  n[2]=-normal[2];
  }
  else if(code<6)
  {
  p[0] = o1->pos[0];
  p[1] = o1->pos[1];
  p[2] = o1->pos[2];
  box=b1;
  R=o1->R;
  n[0]=normal[0];
  n[1]=normal[1];
  n[2]=normal[2];
  }
  if(code<6){

  int maxc = flags & NUMC_MASK;
  if (maxc < 1) maxc = 1;
  if (maxc > 3) maxc = 3;	// no more than 3 contacts per box allowed


  dReal Q1 = dDOT14(n,R+0);
  dReal Q2 = dDOT14(n,R+1);
  dReal Q3 = dDOT14(n,R+2);
  dReal A1 = box->side[0] * Q1;
  dReal A2 = box->side[1] * Q2;
  dReal A3 = box->side[2] * Q3;
  dReal B1 = dFabs(A1);
  dReal B2 = dFabs(A2);
  dReal B3 = dFabs(A3);

  // get the second and third contact points by starting from `p' and going
  // along the two sides with the smallest projected length.

#define FOO(i,j,op) \
  CONTACT(contact,i*skip)->pos[0] = p[0] op box->side[j] * R[0+j]; \
  CONTACT(contact,i*skip)->pos[1] = p[1] op box->side[j] * R[4+j]; \
  CONTACT(contact,i*skip)->pos[2] = p[2] op box->side[j] * R[8+j];
#define BAR(ctact,side,sideinc) \
  depth -= B ## sideinc; \
  if (depth < 0) goto done; \
  if (A ## sideinc > 0) { FOO(ctact,side,+) } else { FOO(ctact,side,-) } \
  CONTACT(contact,ctact*skip)->depth = depth; \
  ret++;

 // CONTACT(contact,skip)->normal[0] = n[0];
  //CONTACT(contact,skip)->normal[1] = n[1];
 // CONTACT(contact,skip)->normal[2] = n[2];
 // if (maxc == 3) {
  //  CONTACT(contact,2*skip)->normal[0] = n[0];
 //   CONTACT(contact,2*skip)->normal[1] = n[1];
  //  CONTACT(contact,2*skip)->normal[2] = n[2];
 // }

  if (B1 < B2) {
    if (B3 < B1) goto use_side_3; else {
      BAR(1,0,1);	// use side 1
      if (maxc == 2) goto done;
      if (B2 < B3) goto contact2_2; else goto contact2_3;
    }
  }
  else {
    if (B3 < B2) {
      use_side_3:	// use side 3
      BAR(1,2,3);
      if (maxc == 2) goto done;
      if (B1 < B2) goto contact2_1; else goto contact2_2;
    }
    else {
      BAR(1,1,2);	// use side 2
      if (maxc == 2) goto done;
      if (B1 < B3) goto contact2_1; else goto contact2_3;
    }
  }

  contact2_1: BAR(2,0,1); goto done;
  contact2_2: BAR(2,1,2); goto done;
  contact2_3: BAR(2,2,3); goto done;
#undef FOO
#undef BAR

 done:;
  }
////////////////////////////////////////////////////////////////////////////////////
  for (int i=0; i<ret; i++) {
    CONTACT(contact,i*skip)->normal[0] = -normal[0];
    CONTACT(contact,i*skip)->normal[1] = -normal[1];
    CONTACT(contact,i*skip)->normal[2] = -normal[2];
    CONTACT(contact,i*skip)->g1 = const_cast<dxGeom*> (o1);
    CONTACT(contact,i*skip)->g2 = const_cast<dxGeom*> (o2);
  }
  return ret;
}



static int dCollideSBA (const dxGeom *o2, const dxGeom *o1, int flags,
		dContactGeom *contact, int skip)
{
  // this is easy. get the sphere center `p' relative to the box, and then clip
  // that to the boundary of the box (call that point `q'). if q is on the
  // boundary of the box and |p-q| is <= sphere radius, they touch.
  // if q is inside the box, the sphere is inside the box, so set a contact
  // normal to push the sphere to the closest box edge.

  dVector3 l,t,p,q,r;
  dReal depth;
  int onborder = 0;

  dIASSERT (skip >= (int)sizeof(dContactGeom));
  dIASSERT (o1->_class->num == dSphereClass);
  dIASSERT (o2->_class->num == dBoxAClass);
  dxSphere *sphere = (dxSphere*) CLASSDATA(o1);
  dxBox *box = (dxBox*) CLASSDATA(o2);

  contact->g1 = const_cast<dxGeom*> (o1);
  contact->g2 = const_cast<dxGeom*> (o2);

  p[0] = o1->pos[0] - o2->pos[0];
  p[1] = o1->pos[1] - o2->pos[1];
  p[2] = o1->pos[2] - o2->pos[2];

  l[0] = box->side[0]*REAL(0.5);
  t[0] = dDOT14(p,o2->R);
  if (t[0] < -l[0]) { t[0] = -l[0]; onborder = 1; }
  if (t[0] >  l[0]) { t[0] =  l[0]; onborder = 1; }

  l[1] = box->side[1]*REAL(0.5);
  t[1] = dDOT14(p,o2->R+1);
  if (t[1] < -l[1]) { t[1] = -l[1]; onborder = 1; }
  if (t[1] >  l[1]) { t[1] =  l[1]; onborder = 1; }

  t[2] = dDOT14(p,o2->R+2);
  l[2] = box->side[2]*REAL(0.5);
  if (t[2] < -l[2]) { t[2] = -l[2]; onborder = 1; }
  if (t[2] >  l[2]) { t[2] =  l[2]; onborder = 1; }

  if (!onborder) {
    // sphere center inside box. find largest `t' value
    dReal max = dFabs(t[0]);
    int maxi = 0;
    for (int i=1; i<3; i++) {
      dReal tt = dFabs(t[i]);
      if (tt > max) {
	max = tt;
	maxi = i;
      }
    }
    // contact position = sphere center
    contact->pos[0] = o1->pos[0];
    contact->pos[1] = o1->pos[1];
    contact->pos[2] = o1->pos[2];
    // contact normal aligned with box edge along largest `t' value
    dVector3 tmp;
    tmp[0] = 0;
    tmp[1] = 0;
    tmp[2] = 0;
    tmp[maxi] = (t[maxi] > 0) ? REAL(1.0) : REAL(-1.0);
    dMULTIPLY0_331 (contact->normal,o2->R,tmp);
    // contact depth = distance to wall along normal plus radius
    contact->depth = l[maxi] - max + sphere->radius;
    return 1;
  }

  t[3] = 0;			//@@@ hmmm
  dMULTIPLY0_331 (q,o2->R,t);
  r[0] = p[0] - q[0];
  r[1] = p[1] - q[1];
  r[2] = p[2] - q[2];
  depth = sphere->radius - dSqrt(dDOT(r,r));
  if (depth < 0) return 0;
  contact->pos[0] = q[0] + o2->pos[0];
  contact->pos[1] = q[1] + o2->pos[1];
  contact->pos[2] = q[2] + o2->pos[2];
  contact->normal[0] = r[0];
  contact->normal[1] = r[1];
  contact->normal[2] = r[2];
  dNormalize3 (contact->normal);
  contact->depth = depth;
  return 1;
}



  static dColliderFn * FUNCCALL dBoxAColliderFn (int num)
{
  if (num == dBoxClass) return (dColliderFn *) &dCollideBBA;
  if (num == dBoxAClass) return (dColliderFn *) &dCollideBABA;
  if (num == dSphereClass) return (dColliderFn *) &dCollideSBA;
  return 0;
}

static void FUNCCALL dBoxAABB (dxGeom *geom, dReal aabb[6])
{
  dxBox *b = (dxBox*) CLASSDATA(geom);
  dReal xrange = REAL(0.5) * (dFabs (geom->R[0] * b->side[0]) +
    dFabs (geom->R[1] * b->side[1]) + dFabs (geom->R[2] * b->side[2]));
  dReal yrange = REAL(0.5) * (dFabs (geom->R[4] * b->side[0]) +
    dFabs (geom->R[5] * b->side[1]) + dFabs (geom->R[6] * b->side[2]));
  dReal zrange = REAL(0.5) * (dFabs (geom->R[8] * b->side[0]) +
    dFabs (geom->R[9] * b->side[1]) + dFabs (geom->R[10] * b->side[2]));
  aabb[0] = geom->pos[0] - xrange;
  aabb[1] = geom->pos[0] + xrange;
  aabb[2] = geom->pos[1] - yrange;
  aabb[3] = geom->pos[1] + yrange;
  aabb[4] = geom->pos[2] - zrange;
  aabb[5] = geom->pos[2] + zrange;
}

dxGeom *dCreateBoxA (dSpaceID space, dReal lx, dReal ly, dReal lz)
{
  dAASSERT (lx > 0 && ly > 0 && lz > 0);
  if (dBoxClass == -1) {
    dGeomClass c;
    c.bytes = sizeof (dxBox);
    c.collider = &dBoxAColliderFn;
    c.aabb = &dBoxAABB;
    c.aabb_test = 0;
    c.dtor = 0;
    dBoxClass = dCreateGeomClass (&c);
  }

  dxGeom *g = dCreateGeom (dBoxAClass);
  if (space) dSpaceAdd (space,g);
  dxBox *b = (dxBox*) CLASSDATA(g);
  b->side[0] = lx;
  b->side[1] = ly;
  b->side[2] = lz;
  return g;
}

