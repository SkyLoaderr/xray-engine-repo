#include "stdafx.h"
#include <ode/common.h>
#include <ode/geom.h>
#include <ode/rotation.h>
#include <ode/odemath.h>
#include <ode/memory.h>
#include <ode/misc.h>
#include <ode/objects.h>
#include <ode/matrix.h>

//#include <ode/src/objects.h>
//#include "array.h"
#include <ode/src/geom_internal.h>
#include "dCylinder.h"
// given a pointer `p' to a dContactGeom, return the dContactGeom at
// p + skip bytes.

struct dxCylinder {	// cylinder
  dReal radius,lz;	// radius, length along y axis //
};

struct dxSphere {
  dReal radius;		// sphere radius
};

struct dxBox {
  dVector3 side;	// side lengths (x,y,z)
};

int dCylinderClass = -1;


#define CONTACT(p,skip) ((dContactGeom*) (((char*)p) + (skip)))

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////circleIntersection//////////////////////////////////////////////////
//this does following:
//takes two circles as normals to planes n1,n2, center points cp1,cp2,and radiuses r1,r2
//finds line on which circles' planes intersect
//finds four points O1,O2 - intersection between the line and sphere with center cp1 radius r1
//					O3,O4 - intersection between the line and sphere with center cp2 radius r2
//returns false if there is no intersection
//computes distances O1-O3, O1-O4, O2-O3, O2-O4
//in "point" returns mean point between intersection points with smallest distance
/////////////////////////////////////////////////////////////////////////////////////////////////
inline bool circleIntersection(const dReal* n1,const dReal* cp1,dReal r1,const dReal* n2,const dReal* cp2,dReal r2,dVector3 point){
dReal c1=dDOT14(cp1,n1);
dReal c2=dDOT14(cp2,n2);
dReal cos=dDOT44(n1,n2);
dReal cos_2=cos*cos;
dReal sin_2=1-cos_2;
dReal p1=(c1-c2*cos)/sin_2;
dReal p2=(c2-c1*cos)/sin_2;
dVector3 lp={p1*n1[0]+p2*n2[0],p1*n1[4]+p2*n2[4],p1*n1[8]+p2*n2[8]};
dVector3 n;
dCROSS144(n,=,n1,n2);
dVector3 LC1={lp[0]-cp1[0],lp[1]-cp1[1],lp[2]-cp1[2]};
dVector3 LC2={lp[0]-cp2[0],lp[1]-cp2[1],lp[2]-cp2[2]};
dReal A,B,C,B_A,B_A_2,D;
dReal t1,t2,t3,t4;
A=dDOT(n,n);
B=dDOT(LC1,n);
C=dDOT(LC1,LC1)-r1*r1;
B_A=B/A;
B_A_2=B_A*B_A;
D=B_A_2-C;
if(D<0.f){	//somewhat strange solution 
			//- it is needed to set some 
			//axis to sepparate cylinders
			//when their edges approach
	t1=-B_A+sqrtf(-D);
	t2=-B_A-sqrtf(-D);
//	return false;
	}
else{
t1=-B_A-sqrtf(D);
t2=-B_A+sqrtf(D);
}
B=dDOT(LC2,n);
C=dDOT(LC2,LC2)-r2*r2;
B_A=B/A;
B_A_2=B_A*B_A;
D=B_A_2-C;

if(D<0.f) {
	t3=-B_A+sqrtf(-D);
	t4=-B_A-sqrtf(-D);
//	return false;
	}
else{
t3=-B_A-sqrtf(D);
t4=-B_A+sqrtf(D);
}
dVector3 O1={lp[0]+n[0]*t1,lp[1]+n[1]*t1,lp[2]+n[2]*t1};
dVector3 O2={lp[0]+n[0]*t2,lp[1]+n[1]*t2,lp[2]+n[2]*t2};

dVector3 O3={lp[0]+n[0]*t3,lp[1]+n[1]*t3,lp[2]+n[2]*t3};
dVector3 O4={lp[0]+n[0]*t4,lp[1]+n[1]*t4,lp[2]+n[2]*t4};

dVector3 L1_3={O3[0]-O1[0],O3[1]-O1[1],O3[2]-O1[2]};
dVector3 L1_4={O4[0]-O1[0],O4[1]-O1[1],O4[2]-O1[2]};

dVector3 L2_3={O3[0]-O2[0],O3[1]-O2[1],O3[2]-O2[2]};
dVector3 L2_4={O4[0]-O2[0],O4[1]-O2[1],O4[2]-O2[2]};


dReal l1_3=dDOT(L1_3,L1_3);
dReal l1_4=dDOT(L1_4,L1_4);

dReal l2_3=dDOT(L2_3,L2_3);
dReal l2_4=dDOT(L2_4,L2_4);


if (l1_3<l1_4)
	if(l2_3<l2_4)
		if(l1_3<l2_3)
			{
			//l1_3;
			point[0]=0.5f*(O1[0]+O3[0]);
			point[1]=0.5f*(O1[1]+O3[1]);
			point[2]=0.5f*(O1[2]+O3[2]);
			}
		else{
			//l2_3;
			point[0]=0.5f*(O2[0]+O3[0]);
			point[1]=0.5f*(O2[1]+O3[1]);
			point[2]=0.5f*(O2[2]+O3[2]);
			}
	else
		if(l1_3<l2_4)
			{
			//l1_3;
			point[0]=0.5f*(O1[0]+O3[0]);
			point[1]=0.5f*(O1[1]+O3[1]);
			point[2]=0.5f*(O1[2]+O3[2]);
			}
		else{
			//l2_4;
			point[0]=0.5f*(O2[0]+O4[0]);
			point[1]=0.5f*(O2[1]+O4[1]);
			point[2]=0.5f*(O2[2]+O4[2]);
			}

else
	if(l2_3<l2_4)
		if(l1_4<l2_3)
			{
			//l1_4;
			point[0]=0.5f*(O1[0]+O4[0]);
			point[1]=0.5f*(O1[1]+O4[1]);
			point[2]=0.5f*(O1[2]+O4[2]);
			}
		else{
			//l2_3;
			point[0]=0.5f*(O2[0]+O3[0]);
			point[1]=0.5f*(O2[1]+O3[1]);
			point[2]=0.5f*(O2[2]+O3[2]);
			}
	else
		if(l1_4<l2_4)
			{
			//l1_4;
			point[0]=0.5f*(O1[0]+O4[0]);
			point[1]=0.5f*(O1[1]+O4[1]);
			point[2]=0.5f*(O1[2]+O4[2]);
			}
		else{
			//l2_4;
			point[0]=0.5f*(O2[0]+O4[0]);
			point[1]=0.5f*(O2[1]+O4[1]);
			point[2]=0.5f*(O2[2]+O4[2]);
			}

return true;
}




static void lineClosestApproach (const dVector3 pa, const dVector3 ua,
				 const dVector3 pb, const dVector3 ub,
				 dReal *alpha, dReal *beta)
{
  dVector3 p;
  p[0] = pb[0] - pa[0];
  p[1] = pb[1] - pa[1];
  p[2] = pb[2] - pa[2];
  dReal uaub = dDOT(ua,ub);
  dReal q1 =  dDOT(ua,p);
  dReal q2 = -dDOT(ub,p);
  dReal d = 1-uaub*uaub;
  if (d <= 0) {
    // @@@ this needs to be made more robust
    *alpha = 0;
    *beta  = 0;
  }
  else {
    d = dRecip(d);
    *alpha = (q1 + uaub*q2)*d;
    *beta  = (uaub*q1 + q2)*d;
  }
}


// @@@ some stuff to optimize here, reuse code in contact point calculations.

extern "C" int dCylBox (const dVector3 p1, const dMatrix3 R1,
			const dReal radius,const dReal lz, const dVector3 p2,
			const dMatrix3 R2, const dVector3 side2,
			dVector3 normal, dReal *depth, int *code,
			int maxc, dContactGeom *contact, int skip)
{
  dVector3 p,pp,normalC;
  const dReal *normalR = 0;
  dReal B1,B2,B3,R11,R12,R13,R21,R22,R23,R31,R32,R33,
    Q11,Q12,Q13,Q21,Q22,Q23,Q31,Q32,Q33,s,s2,l,sQ21,sQ22,sQ23;
  int i,invert_normal;

  // get vector from centers of box 1 to box 2, relative to box 1
  p[0] = p2[0] - p1[0];
  p[1] = p2[1] - p1[1];
  p[2] = p2[2] - p1[2];
  dMULTIPLY1_331 (pp,R1,p);		// get pp = p relative to body 1

  // get side lengths / 2
  //A1 =radius; A2 = lz*REAL(0.5); A3 = radius;
  dReal hlz=lz/2.f;
  B1 = side2[0]*REAL(0.5); B2 = side2[1]*REAL(0.5); B3 = side2[2]*REAL(0.5);

  // Rij is R1'*R2, i.e. the relative rotation between R1 and R2
  R11 = dDOT44(R1+0,R2+0); R12 = dDOT44(R1+0,R2+1); R13 = dDOT44(R1+0,R2+2);
  R21 = dDOT44(R1+1,R2+0); R22 = dDOT44(R1+1,R2+1); R23 = dDOT44(R1+1,R2+2);
  R31 = dDOT44(R1+2,R2+0); R32 = dDOT44(R1+2,R2+1); R33 = dDOT44(R1+2,R2+2);

  Q11 = dFabs(R11); Q12 = dFabs(R12); Q13 = dFabs(R13);
  Q21 = dFabs(R21); Q22 = dFabs(R22); Q23 = dFabs(R23);
  Q31 = dFabs(R31); Q32 = dFabs(R32); Q33 = dFabs(R33);

  
  //   * see if the axis separates the box with cylinder. if so, return 0.
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

  // separating axis = cylinder ax u2
 //used when a box vertex touches a flat face of the cylinder
  TEST (pp[1],(hlz + B1*Q21 + B2*Q22 + B3*Q23),R1+1,0);


  // separating axis = box axis v1,v2,v3
  //used when cylinder edge touches box face
  //there is two ways to compute sQ: sQ21=sqrtf(1.f-Q21*Q21); or sQ21=sqrtf(Q23*Q23+Q22*Q22); 
  //if we did not need Q23 and Q22 the first way might be used to quiken the routine but then it need to 
  //check if Q21<=1.f, becouse it may slightly exeed 1.f.

 
  sQ21=sqrtf(Q23*Q23+Q22*Q22);
  TEST (dDOT41(R2+0,p),(radius*sQ21 + hlz*Q21 + B1),R2+0,1);

  sQ22=sqrtf(Q23*Q23+Q21*Q21);
  TEST (dDOT41(R2+1,p),(radius*sQ22 + hlz*Q22 + B2),R2+1,2);

  sQ23=sqrtf(Q22*Q22+Q21*Q21);
  TEST (dDOT41(R2+2,p),(radius*sQ23 + hlz*Q23 + B3),R2+2,3);

 
#undef TEST
#define TEST(expr1,expr2,n1,n2,n3,cc) \
  s2 = dFabs(expr1) - (expr2); \
  if (s2 > 0) return 0; \
  if (s2 > s) { \
      s = s2; \
	  normalR = 0; \
      normalC[0] = (n1); normalC[1] = (n2); normalC[2] = (n3); \
      invert_normal = ((expr1) < 0); \
      *code = (cc); \
    } 
 


// separating axis is a normal to the cylinder axis passing across the nearest box vertex
//used when a box vertex touches the lateral surface of the cylinder

dReal proj,boxProj,cos,sin,cos1,cos3;
dVector3 tAx,Ax,pb;
{
//making Ax which is perpendicular to cyl ax to box position//
proj=dDOT14(p2,R1+1)-dDOT14(p1,R1+1);

Ax[0]=p2[0]-p1[0]-R1[1]*proj;
Ax[1]=p2[1]-p1[1]-R1[5]*proj;
Ax[2]=p2[2]-p1[2]-R1[9]*proj;
dNormalize3(Ax);
//using Ax find box vertex which is nearest to the cylinder axis
	dReal sign;
    
    for (i=0; i<3; i++) pb[i] = p2[i];
    sign = (dDOT14(Ax,R2+0) > 0) ? REAL(-1.0) : REAL(1.0);
    for (i=0; i<3; i++) pb[i] += sign * B1 * R2[i*4];
    sign = (dDOT14(Ax,R2+1) > 0) ? REAL(-1.0) : REAL(1.0);
    for (i=0; i<3; i++) pb[i] += sign * B2 * R2[i*4+1];
    sign = (dDOT14(Ax,R2+2) > 0) ? REAL(-1.0) : REAL(1.0);
    for (i=0; i<3; i++) pb[i] += sign * B3 * R2[i*4+2];

//building axis which is normal to cylinder ax to the nearest box vertex
proj=dDOT14(pb,R1+1)-dDOT14(p1,R1+1);

Ax[0]=pb[0]-p1[0]-R1[1]*proj;
Ax[1]=pb[1]-p1[1]-R1[5]*proj;
Ax[2]=pb[2]-p1[2]-R1[9]*proj;
dNormalize3(Ax);
}

boxProj=dFabs(dDOT14(Ax,R2+0)*B1)+
		dFabs(dDOT14(Ax,R2+1)*B2)+
		dFabs(dDOT14(Ax,R2+2)*B3);

TEST(p[0]*Ax[0]+p[1]*Ax[1]+p[2]*Ax[2],(radius+boxProj),Ax[0],Ax[1],Ax[2],4);


//next three test used to handle collisions between cylinder circles and box ages
proj=dDOT14(p1,R2+0)-dDOT14(p2,R2+0);

tAx[0]=-p1[0]+p2[0]+R2[0]*proj;
tAx[1]=-p1[1]+p2[1]+R2[4]*proj;
tAx[2]=-p1[2]+p2[2]+R2[8]*proj;
dNormalize3(tAx);

//now tAx is normal to first ax of the box to cylinder center
//making perpendicular to tAx lying in the plane which is normal to the cylinder axis
//it is tangent in the point where projection of tAx on cylinder's ring intersect edge circle

cos=dDOT14(tAx,R1+0);
sin=dDOT14(tAx,R1+2);
tAx[0]=R1[2]*cos-R1[0]*sin;
tAx[1]=R1[6]*cos-R1[4]*sin;
tAx[2]=R1[10]*cos-R1[8]*sin;


//use cross between tAx and first ax of the box as separating axix 

dCROSS114(Ax,=,tAx,R2+0);
dNormalize3(Ax);

boxProj=dFabs(dDOT14(Ax,R2+1)*B2)+
		dFabs(dDOT14(Ax,R2+0)*B1)+
		dFabs(dDOT14(Ax,R2+2)*B3);

  cos=dFabs(dDOT14(Ax,R1+1));
  cos1=dDOT14(Ax,R1+0);
  cos3=dDOT14(Ax,R1+2);
  sin=sqrtf(cos1*cos1+cos3*cos3);

TEST(p[0]*Ax[0]+p[1]*Ax[1]+p[2]*Ax[2],(sin*radius+cos*hlz+boxProj),Ax[0],Ax[1],Ax[2],5);


//same thing with the second axis of the box
proj=dDOT14(p1,R2+1)-dDOT14(p2,R2+1);

tAx[0]=-p1[0]+p2[0]+R2[1]*proj;
tAx[1]=-p1[1]+p2[1]+R2[5]*proj;
tAx[2]=-p1[2]+p2[2]+R2[9]*proj;
dNormalize3(tAx);


cos=dDOT14(tAx,R1+0);
sin=dDOT14(tAx,R1+2);
tAx[0]=R1[2]*cos-R1[0]*sin;
tAx[1]=R1[6]*cos-R1[4]*sin;
tAx[2]=R1[10]*cos-R1[8]*sin;

dCROSS114(Ax,=,tAx,R2+1);
dNormalize3(Ax);

boxProj=dFabs(dDOT14(Ax,R2+0)*B1)+
		dFabs(dDOT14(Ax,R2+1)*B2)+
		dFabs(dDOT14(Ax,R2+2)*B3);

  cos=dFabs(dDOT14(Ax,R1+1));
  cos1=dDOT14(Ax,R1+0);
  cos3=dDOT14(Ax,R1+2);
  sin=sqrtf(cos1*cos1+cos3*cos3);
TEST(p[0]*Ax[0]+p[1]*Ax[1]+p[2]*Ax[2],(sin*radius+cos*hlz+boxProj),Ax[0],Ax[1],Ax[2],6);

//same thing with the third axis of the box
proj=dDOT14(p1,R2+2)-dDOT14(p2,R2+2);

Ax[0]=-p1[0]+p2[0]+R2[2]*proj;
Ax[1]=-p1[1]+p2[1]+R2[6]*proj;
Ax[2]=-p1[2]+p2[2]+R2[10]*proj;
dNormalize3(tAx);

cos=dDOT14(tAx,R1+0);
sin=dDOT14(tAx,R1+2);
tAx[0]=R1[2]*cos-R1[0]*sin;
tAx[1]=R1[6]*cos-R1[4]*sin;
tAx[2]=R1[10]*cos-R1[8]*sin;

dCROSS114(Ax,=,tAx,R2+2);
dNormalize3(Ax);
boxProj=dFabs(dDOT14(Ax,R2+1)*B2)+
		dFabs(dDOT14(Ax,R2+2)*B3)+
		dFabs(dDOT14(Ax,R2+0)*B1);

  cos=dFabs(dDOT14(Ax,R1+1));
  cos1=dDOT14(Ax,R1+0);
  cos3=dDOT14(Ax,R1+2);
  sin=sqrtf(cos1*cos1+cos3*cos3);
TEST(p[0]*Ax[0]+p[1]*Ax[1]+p[2]*Ax[2],(sin*radius+cos*hlz+boxProj),Ax[0],Ax[1],Ax[2],7);


#undef TEST

// note: cross product axes need to be scaled when s is computed.
// normal (n1,n2,n3) is relative to box 1.

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

//crosses between cylinder axis and box axes
  // separating axis = u2 x (v1,v2,v3)
  TEST(pp[0]*R31-pp[2]*R11,(radius+B2*Q23+B3*Q22),R31,0,-R11,8);
  TEST(pp[0]*R32-pp[2]*R12,(radius+B1*Q23+B3*Q21),R32,0,-R12,9);
  TEST(pp[0]*R33-pp[2]*R13,(radius+B1*Q22+B2*Q21),R33,0,-R13,10);


#undef TEST

  // if we get to this point, the boxes interpenetrate. compute the normal
  // in global coordinates.
  if (normalR) {
    normal[0] = normalR[0];
    normal[1] = normalR[4];
    normal[2] = normalR[8];
  }
  else {
	  if(*code>7) dMULTIPLY0_331 (normal,R1,normalC);
	  else {normal[0] =normalC[0];normal[1] = normalC[1];normal[2] = normalC[2];}
  }
  if (invert_normal) {
    normal[0] = -normal[0];
    normal[1] = -normal[1];
    normal[2] = -normal[2];
  }
  *depth = -s;

  // compute contact point(s)

  if (*code > 7) {
 //find point on the cylinder pa deepest along normal
    dVector3 pa;
    dReal sign, cos1,cos3,factor;


    for (i=0; i<3; i++) pa[i] = p1[i];

  	cos1 = dDOT14(normal,R1+0);
	cos3 = dDOT14(normal,R1+2) ;
	factor=sqrtf(cos1*cos1+cos3*cos3);

	cos1/=factor;
	cos3/=factor;
	
    for (i=0; i<3; i++) pa[i] += cos1 * radius * R1[i*4];

    sign = (dDOT14(normal,R1+1) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) pa[i] += sign * hlz * R1[i*4+1];

  
    for (i=0; i<3; i++) pa[i] += cos3 * radius * R1[i*4+2];

    // find vertex of the box  deepest along normal 
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
    for (i=0; i<3; i++) ua[i] = R1[1 + i*4];
    for (i=0; i<3; i++) ub[i] = R2[*code-8 + i*4];

    lineClosestApproach (pa,ua,pb,ub,&alpha,&beta);
    for (i=0; i<3; i++) pa[i] += ua[i]*alpha;
    for (i=0; i<3; i++) pb[i] += ub[i]*beta;

    for (i=0; i<3; i++) contact[0].pos[i] = REAL(0.5)*(pa[i]+pb[i]);
    contact[0].depth = *depth;
    return 1;
  }


  	if(*code==4){
		for (i=0; i<3; i++) contact[0].pos[i] = pb[i];
		contact[0].depth = *depth;
		return 1;
				}
  

  dVector3 vertex;
  if (*code == 0) {
   
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
   
    dReal sign,cos1,cos3,factor;
    for (i=0; i<3; i++) vertex[i] = p1[i];
    cos1 = dDOT14(normal,R1+0) ;
	cos3 = dDOT14(normal,R1+2);
	factor=sqrtf(cos1*cos1+cos3*cos3);
	factor= factor ? factor : 1.f;
	cos1/=factor;
	cos3/=factor;
    for (i=0; i<3; i++) vertex[i] += cos1 * radius * R1[i*4];

    sign = (dDOT14(normal,R1+1) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) vertex[i] += sign * hlz * R1[i*4+1];
   
    for (i=0; i<3; i++) vertex[i] += cos3 * radius * R1[i*4+2];
  }
  for (i=0; i<3; i++) contact[0].pos[i] = vertex[i];
  contact[0].depth = *depth;
  return 1;
}

//****************************************************************************

extern "C" int dCylCyl (const dVector3 p1, const dMatrix3 R1,
			const dReal radius1,const dReal lz1, const dVector3 p2,
			const dMatrix3 R2, const dReal radius2,const dReal lz2,
			dVector3 normal, dReal *depth, int *code,
			int maxc, dContactGeom *contact, int skip)
{
  dVector3 p,pp1,pp2,normalC;
  const dReal *normalR = 0;
  dReal hlz1,hlz2,s,s2;
  int i,invert_normal;

  // get vector from centers of box 1 to box 2, relative to box 1
  p[0] = p2[0] - p1[0];
  p[1] = p2[1] - p1[1];
  p[2] = p2[2] - p1[2];
  dMULTIPLY1_331 (pp1,R1,p);		// get pp1 = p relative to body 1
  dMULTIPLY1_331 (pp2,R2,p);
  // get side lengths / 2
  hlz1 = lz1*REAL(0.5);
  hlz2 = lz2*REAL(0.5); 

 dReal proj,cos,sin,cos1,cos3;



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

  cos=dFabs(dDOT44(R1+1,R2+1));
  sin=sqrtf(1.f-(cos>1.f ? 1.f : cos));

  TEST (pp1[1],(hlz1 + radius2*sin + hlz2*cos ),R1+1,0);//pp

  TEST (pp2[1],(radius1*sin + hlz1*cos + hlz2),R2+1,1);



  // note: cross product axes need to be scaled when s is computed.
 
#undef TEST
#define TEST(expr1,expr2,n1,n2,n3,cc) \
  s2 = dFabs(expr1) - (expr2); \
  if (s2 > 0) return 0; \
  if (s2 > s) { \
      s = s2; \
	  normalR = 0; \
      normalC[0] = (n1); normalC[1] = (n2); normalC[2] = (n3); \
      invert_normal = ((expr1) < 0); \
      *code = (cc); \
    } 
 

dVector3 tAx,Ax,pa,pb;

//cross between cylinders' axes
dCROSS144(Ax,=,R1+1,R2+1);
dNormalize3(Ax);
TEST(p[0]*Ax[0]+p[1]*Ax[1]+p[2]*Ax[2],radius1+radius2,Ax[0],Ax[1],Ax[2],6);


{
 
    dReal sign, factor;

	//making ax which is perpendicular to cyl1 ax passing across cyl2 position//
		//(project p on cyl1 flat surface )
    for (i=0; i<3; i++) pb[i] = p2[i];
 	//cos1 = dDOT14(p,R1+0);
	//cos3 = dDOT14(p,R1+2) ;
	tAx[0]=pp1[0]*R1[0]+pp1[2]*R1[2];
	tAx[1]=pp1[0]*R1[4]+pp1[2]*R1[6];
	tAx[2]=pp1[0]*R1[8]+pp1[2]*R1[10];
	dNormalize3(tAx);

//find deepest point pb of cyl2 on opposite direction of tAx
 	cos1 = dDOT14(tAx,R2+0);
	cos3 = dDOT14(tAx,R2+2) ;
	factor=sqrtf(cos1*cos1+cos3*cos3);
	cos1/=factor;
	cos3/=factor;
    for (i=0; i<3; i++) pb[i] -= cos1 * radius2 * R2[i*4];

    sign = (dDOT14(tAx,R2+1) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) pb[i] -= sign * hlz2 * R2[i*4+1];

    for (i=0; i<3; i++) pb[i] -= cos3 * radius2 * R2[i*4+2];

//making perpendicular to cyl1 ax passing across pb
	proj=dDOT14(pb,R1+1)-dDOT14(p1,R1+1);

	Ax[0]=pb[0]-p1[0]-R1[1]*proj;
	Ax[1]=pb[1]-p1[1]-R1[5]*proj;
	Ax[2]=pb[2]-p1[2]-R1[9]*proj;

}

dNormalize3(Ax);


  cos=dFabs(dDOT14(Ax,R2+1));
  cos1=dDOT14(Ax,R2+0);
  cos3=dDOT14(Ax,R2+2);
  sin=sqrtf(cos1*cos1+cos3*cos3);

TEST(p[0]*Ax[0]+p[1]*Ax[1]+p[2]*Ax[2],radius1+cos*hlz2+sin*radius2,Ax[0],Ax[1],Ax[2],3);



{
   
   dReal sign, factor;
   	
    for (i=0; i<3; i++) pa[i] = p1[i];

 	//making ax which is perpendicular to cyl2 ax passing across cyl1 position//
	//(project p on cyl2 flat surface )
 	//cos1 = dDOT14(p,R2+0);
	//cos3 = dDOT14(p,R2+2) ;
	tAx[0]=pp2[0]*R2[0]+pp2[2]*R2[2];
	tAx[1]=pp2[0]*R2[4]+pp2[2]*R2[6];
	tAx[2]=pp2[0]*R2[8]+pp2[2]*R2[10];
	dNormalize3(tAx);

 	cos1 = dDOT14(tAx,R1+0);
	cos3 = dDOT14(tAx,R1+2) ;
	factor=sqrtf(cos1*cos1+cos3*cos3);
	cos1/=factor;
	cos3/=factor;

//find deepest point pa of cyl2 on direction of tAx
    for (i=0; i<3; i++) pa[i] += cos1 * radius1 * R1[i*4];

    sign = (dDOT14(tAx,R1+1) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) pa[i] += sign * hlz1 * R1[i*4+1];

  
    for (i=0; i<3; i++) pa[i] += cos3 * radius1 * R1[i*4+2];

	proj=dDOT14(pa,R2+1)-dDOT14(p2,R2+1);

	Ax[0]=pa[0]-p2[0]-R2[1]*proj;
	Ax[1]=pa[1]-p2[1]-R2[5]*proj;
	Ax[2]=pa[2]-p2[2]-R2[9]*proj;

}
dNormalize3(Ax);



  cos=dFabs(dDOT14(Ax,R1+1));
  cos1=dDOT14(Ax,R1+0);
  cos3=dDOT14(Ax,R1+2);
  sin=sqrtf(cos1*cos1+cos3*cos3);

TEST(p[0]*Ax[0]+p[1]*Ax[1]+p[2]*Ax[2],radius2+cos*hlz1+sin*radius1,Ax[0],Ax[1],Ax[2],4);


////test circl

//@ this needed to set right normal when cylinders edges intersect
//@ the most precise axis for this test may be found as a line between nearest points of two
//@ circles. But it needs comparatively a lot of computation.
//@ I use a trick which lets not to solve quadric equation. 
//@ In the case when cylinder eidges touches the test below rather accurate.
//@ I still not sure about problems with sepparation but they have not been revealed during testing.
dVector3 point;
{
 dVector3 ca,cb; 
 dReal sign;
 for (i=0; i<3; i++) ca[i] = p1[i];
 for (i=0; i<3; i++) cb[i] = p2[i];
//find two nearest flat rings
 sign = (pp1[1] > 0) ? REAL(1.0) : REAL(-1.0);
 for (i=0; i<3; i++) ca[i] += sign * hlz1 * R1[i*4+1];

 sign = (pp2[1] > 0) ? REAL(1.0) : REAL(-1.0);
 for (i=0; i<3; i++) cb[i] -= sign * hlz2 * R2[i*4+1];

 dVector3 tAx,tAx1;
	circleIntersection(R1+1,ca,radius1,R2+1,cb,radius2,point);

	Ax[0]=point[0]-ca[0];
	Ax[1]=point[1]-ca[1];
	Ax[2]=point[2]-ca[2];

  	cos1 = dDOT14(Ax,R1+0);
	cos3 = dDOT14(Ax,R1+2) ;

	tAx[0]=cos3*R1[0]-cos1*R1[2];
	tAx[1]=cos3*R1[4]-cos1*R1[6];
	tAx[2]=cos3*R1[8]-cos1*R1[10];

	Ax[0]=point[0]-cb[0];
	Ax[1]=point[1]-cb[1];
	Ax[2]=point[2]-cb[2];


 	cos1 = dDOT14(Ax,R2+0);
	cos3 = dDOT14(Ax,R2+2) ;

	tAx1[0]=cos3*R2[0]-cos1*R2[2];
	tAx1[1]=cos3*R2[4]-cos1*R2[6];
	tAx1[2]=cos3*R2[8]-cos1*R2[10];
	dCROSS(Ax,=,tAx,tAx1);
	

 

dNormalize3(Ax);
dReal cyl1Pr,cyl2Pr;

 cos=dFabs(dDOT14(Ax,R1+1));
 cos1=dDOT14(Ax,R1+0);
 cos3=dDOT14(Ax,R1+2);
 sin=sqrtf(cos1*cos1+cos3*cos3);
 cyl1Pr=cos*hlz1+sin*radius1;

 cos=dFabs(dDOT14(Ax,R2+1));
 cos1=dDOT14(Ax,R2+0);
 cos3=dDOT14(Ax,R2+2);
 sin=sqrtf(cos1*cos1+cos3*cos3);
 cyl2Pr=cos*hlz2+sin*radius2;
TEST(p[0]*Ax[0]+p[1]*Ax[1]+p[2]*Ax[2],cyl1Pr+cyl2Pr,Ax[0],Ax[1],Ax[2],5);


}


#undef TEST



  // if we get to this point, the cylinders interpenetrate. compute the normal
  // in global coordinates.
  if (normalR) {
    normal[0] = normalR[0];
    normal[1] = normalR[4];
    normal[2] = normalR[8];
  }
  else {
		normal[0] =normalC[0];normal[1] = normalC[1];normal[2] = normalC[2];
		}
  if (invert_normal) {
    normal[0] = -normal[0];
    normal[1] = -normal[1];
    normal[2] = -normal[2];
  }

  *depth = -s;

  // compute contact point(s)

	if(*code==3){
		for (i=0; i<3; i++) contact[0].pos[i] = pb[i];
		contact[0].depth = *depth;
		return 1;
				}

	if(*code==4){
		for (i=0; i<3; i++) contact[0].pos[i] = pa[i];
		contact[0].depth = *depth;
		return 1;
				}

	if(*code==5){
		for (i=0; i<3; i++) contact[0].pos[i] = point[i];
		contact[0].depth = *depth;
		return 1;
				}

if (*code == 6) {
	    dVector3 pa;
    dReal sign, cos1,cos3,factor;


    for (i=0; i<3; i++) pa[i] = p1[i];

  	cos1 = dDOT14(normal,R1+0);
	cos3 = dDOT14(normal,R1+2) ;
	factor=sqrtf(cos1*cos1+cos3*cos3);

	cos1/=factor;
	cos3/=factor;
	
    for (i=0; i<3; i++) pa[i] += cos1 * radius1 * R1[i*4];

    sign = (dDOT14(normal,R1+1) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) pa[i] += sign * hlz1 * R1[i*4+1];

  
    for (i=0; i<3; i++) pa[i] += cos3 * radius1 * R1[i*4+2];

    // find a point pb on the intersecting edge of cylinder 2
    dVector3 pb;
    for (i=0; i<3; i++) pb[i] = p2[i];
 	cos1 = dDOT14(normal,R2+0);
	cos3 = dDOT14(normal,R2+2) ;
	factor=sqrtf(cos1*cos1+cos3*cos3);

	cos1/=factor;
	cos3/=factor;
	
    for (i=0; i<3; i++) pb[i] -= cos1 * radius2 * R2[i*4];

    sign = (dDOT14(normal,R2+1) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) pb[i] -= sign * hlz2 * R2[i*4+1];

  
    for (i=0; i<3; i++) pb[i] -= cos3 * radius2 * R2[i*4+2];

	
	dReal alpha,beta;
	dVector3 ua,ub;
	for (i=0; i<3; i++) ua[i] = R1[1 + i*4];
	for (i=0; i<3; i++) ub[i] = R2[1 + i*4];
	lineClosestApproach (pa,ua,pb,ub,&alpha,&beta);
	for (i=0; i<3; i++) pa[i] += ua[i]*alpha;
	for (i=0; i<3; i++) pb[i] += ub[i]*beta;

    for (i=0; i<3; i++) contact[0].pos[i] = REAL(0.5)*(pa[i]+pb[i]);
    contact[0].depth = *depth;
    return 1;
  }

  // okay, we have a face-something intersection (because the separating
  // axis is perpendicular to a face).

  // @@@ temporary: make deepest point on the "other" cylinder the contact point.
  // @@@ this kind of works, but we need multiple contact points for stability,
  // @@@ especially for face-face contact.

  dVector3 vertex;
  if (*code == 0) {
    // flat face from cylinder 1 touches a edge/face from cylinder 2.
    dReal sign,cos1,cos3,factor;
    for (i=0; i<3; i++) vertex[i] = p2[i];
    cos1 = dDOT14(normal,R2+0) ;
	cos3 = dDOT14(normal,R2+2);
	factor=sqrtf(cos1*cos1+cos3*cos3);

	cos1/=factor;
	cos3/=factor;
    for (i=0; i<3; i++) vertex[i] -= cos1 * radius2 * R2[i*4];

    sign = (dDOT14(normal,R1+1) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) vertex[i] -= sign * hlz2 * R2[i*4+1];
   
    for (i=0; i<3; i++) vertex[i] -= cos3 * radius2 * R2[i*4+2];
  }
  else {
     // flat face from cylinder 2 touches a edge/face from cylinder 1.
    dReal sign,cos1,cos3,factor;
    for (i=0; i<3; i++) vertex[i] = p1[i];
    cos1 = dDOT14(normal,R1+0) ;
	cos3 = dDOT14(normal,R1+2);
	factor=sqrtf(cos1*cos1+cos3*cos3);

	cos1/=factor;
	cos3/=factor;
    for (i=0; i<3; i++) vertex[i] += cos1 * radius1 * R1[i*4];

    sign = (dDOT14(normal,R1+1) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) vertex[i] += sign * hlz1 * R1[i*4+1];
   
    for (i=0; i<3; i++) vertex[i] += cos3 * radius1 * R1[i*4+2];
  }
  for (i=0; i<3; i++) contact[0].pos[i] = vertex[i];
  contact[0].depth = *depth;
  return 1;
}

//****************************************************************************


int dCollideCylS (const dxGeom *o1, const dxGeom *o2, int flags,
		dContactGeom *contact, int skip)
{
 

  dIASSERT (skip >= (int)sizeof(dContactGeom));
  dIASSERT (o2->_class->num == dSphereClass);
  dIASSERT (o1->_class->num == dCylinderClass);
  dxSphere *sphere = (dxSphere*) CLASSDATA(o2);
  dxCylinder *cyl = (dxCylinder*) CLASSDATA(o1);

  const dReal* p1=o1->pos;
  const dReal* p2=o2->pos;
  const dReal* R=o1->R;
  dVector3 p,normalC,normal;
  const dReal *normalR = 0;
  dReal cylRadius=cyl->radius;
  dReal sphereRadius=sphere->radius;
  dReal hl=cyl->lz/2.f;
  int i,invert_normal;

  // get vector from centers of cyl to shere
  p[0] = p2[0] - p1[0];
  p[1] = p2[1] - p1[1];
  p[2] = p2[2] - p1[2];
 
dReal s,s2;
unsigned char code;
#define TEST(expr1,expr2,norm,cc) \
  s2 = dFabs(expr1) - (expr2); \
  if (s2 > 0) return 0; \
  if (s2 > s) { \
    s = s2; \
    normalR = norm; \
    invert_normal = ((expr1) < 0); \
    code = (cc); \
  }

  s = -dInfinity;
  invert_normal = 0;
  code = 0;

  // separating axis cyl ax 

  TEST (dDOT14(p,R+1),sphereRadius+hl,R+1,2);
  // note: cross product axes need to be scaled when s is computed.
  // normal (n1,n2,n3) is relative to 
#undef TEST
#define TEST(expr1,expr2,n1,n2,n3,cc) \
  s2 = dFabs(expr1) - (expr2); \
  if (s2 > 0) return 0; \
  if (s2 > s) { \
      s = s2; \
	  normalR = 0; \
      normalC[0] = (n1); normalC[1] = (n2); normalC[2] = (n3); \
      invert_normal = ((expr1) < 0); \
      code = (cc); \
    } 
 
//making ax which is perpendicular to cyl1 ax to sphere center//
 
dReal proj,cos,sin,cos1,cos3;
dVector3 Ax;
	proj=dDOT14(p2,R+1)-dDOT14(p1,R+1);

	Ax[0]=p2[0]-p1[0]-R[1]*proj;
	Ax[1]=p2[1]-p1[1]-R[5]*proj;
	Ax[2]=p2[2]-p1[2]-R[9]*proj;
dNormalize3(Ax);
TEST(dDOT(p,Ax),sphereRadius+cylRadius,Ax[0],Ax[1],Ax[2],9);


Ax[0]=p[0];
Ax[1]=p[1];
Ax[2]=p[2];
dNormalize3(Ax);

	dVector3 pa;
    dReal sign, factor;
    for (i=0; i<3; i++) pa[i] = p1[i];

  	cos1 = dDOT14(Ax,R+0);
	cos3 = dDOT14(Ax,R+2) ;
	factor=sqrtf(cos1*cos1+cos3*cos3);
	cos1/=factor;
	cos3/=factor;
    for (i=0; i<3; i++) pa[i] += cos1 * cylRadius * R[i*4];
    sign = (dDOT14(normal,R+1) > 0) ? REAL(1.0) : REAL(-1.0);
    for (i=0; i<3; i++) pa[i] += sign * hl * R[i*4+1];
    for (i=0; i<3; i++) pa[i] += cos3 * cylRadius  * R[i*4+2];

Ax[0]=p2[0]-pa[0];
Ax[1]=p2[1]-pa[1];
Ax[2]=p2[2]-pa[2];
dNormalize3(Ax);

 cos=dFabs(dDOT14(Ax,R+1));
 cos1=dDOT14(Ax,R+0);
 cos3=dDOT14(Ax,R+2);
 sin=sqrtf(cos1*cos1+cos3*cos3);
TEST(dDOT(p,Ax),sphereRadius+cylRadius*sin+hl*cos,Ax[0],Ax[1],Ax[2],14);


#undef TEST

  if (normalR) {
    normal[0] = normalR[0];
    normal[1] = normalR[4];
    normal[2] = normalR[8];
  }
  else {

	normal[0] = normalC[0];
	normal[1] = normalC[1];
	normal[2] = normalC[2];
		}
  if (invert_normal) {
    normal[0] = -normal[0];
    normal[1] = -normal[1];
    normal[2] = -normal[2];
  }
   // compute contact point(s)
contact->depth=-s;
contact->normal[0]=-normal[0];
contact->normal[1]=-normal[1];
contact->normal[2]=-normal[2];
contact->g1=const_cast<dxGeom*> (o1);
contact->g2=const_cast<dxGeom*> (o2);
contact->pos[0]=p2[0]-normal[0]*sphereRadius;
contact->pos[1]=p2[1]-normal[1]*sphereRadius;
contact->pos[2]=p2[2]-normal[2]*sphereRadius;
return 1;
}



int dCollideCylB (const dxGeom *o1, const dxGeom *o2, int flags,
		dContactGeom *contact, int skip)
{
  dVector3 normal;
  dReal depth;
  int code;
  dxBox *b = (dxBox*) CLASSDATA(o2);
  dxCylinder *c = (dxCylinder*) CLASSDATA(o1);
  int num = dCylBox (o1->pos,o1->R,c->radius,c->lz, o2->pos,o2->R,b->side,
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

int dCollideCylCyl (const dxGeom *o1, const dxGeom *o2, int flags,
		dContactGeom *contact, int skip)
{
  dVector3 normal;
  dReal depth;
  int code;
  dxCylinder *c1 = (dxCylinder*) CLASSDATA(o1);
  dxCylinder *c2 = (dxCylinder*) CLASSDATA(o2);

  int num = dCylCyl (o1->pos,o1->R,c1->radius,c1->lz, o2->pos,o2->R,c2->radius,c2->lz,
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

struct dxPlane {
  dReal p[4];
};


int dCollideCylPlane 
	(
	const dxGeom *o1, const dxGeom *o2, int flags,
			  dContactGeom *contact, int skip){
  dIASSERT (skip >= (int)sizeof(dContactGeom));
  dIASSERT (o1->_class->num == dCylinderClass);
  dIASSERT (o2->_class->num == dPlaneClass);
  contact->g1 = const_cast<dxGeom*> (o1);
  contact->g2 = const_cast<dxGeom*> (o2);
  dxCylinder *cylinder = (dxCylinder*) CLASSDATA(o1);
  dxPlane *plane = (dxPlane*) CLASSDATA(o2);
  unsigned int ret = 0;

 const dReal radius=cylinder->radius;
 const dReal hlz=cylinder->lz/2.f;
 const dReal *R = o1->R;// rotation of cylinder
 const dReal* p=o1->pos;
 const dReal *n = plane->p;		// normal vector
 const dReal pp=plane->p[3];

 dReal cos1,sin1;
  cos1=dFabs(dDOT14(n,R+1));

cos1=cos1<REAL(1.) ? cos1 : REAL(1.); //cos1 may slightly exeed 1.f
sin1=sqrtf(REAL(1.)-cos1*cos1);
//////////////////////////////

dReal sidePr=cos1*hlz+sin1*radius;

dReal dist=-pp+dDOT(n,p);
dReal outDepth=sidePr-dist;

if(outDepth<0.f) return 0;

dVector3 pos;


/////////////////////////////////////////// from geom.cpp dCollideBP
  dReal Q1 = dDOT14(n,R+0);
  dReal Q2 = dDOT14(n,R+1);
  dReal Q3 = dDOT14(n,R+2);
  dReal factor =sqrtf(Q1*Q1+Q3*Q3);
  factor= factor ? factor :1.f;
  dReal A1 = radius *		Q1/factor;
  dReal A2 = hlz*Q2;
  dReal A3 = radius *		Q3/factor;

  pos[0]=p[0];
  pos[1]=p[1];
  pos[2]=p[2];

  pos[0]-= A1*R[0];
  pos[1]-= A1*R[4];
  pos[2]-= A1*R[8];

  pos[0]-= A3*R[2];
  pos[1]-= A3*R[6];
  pos[2]-= A3*R[10];

  pos[0]-= A2>0 ? hlz*R[1]:-hlz*R[1];
  pos[1]-= A2>0 ? hlz*R[5]:-hlz*R[5];
  pos[2]-= A2>0 ? hlz*R[9]:-hlz*R[9];
  
 

  contact->pos[0] = pos[0];
  contact->pos[1] = pos[1];
  contact->pos[2] = pos[2];
   contact->depth = outDepth;
  ret=1;
 
if(dFabs(Q2)>M_SQRT1_2){

  CONTACT(contact,ret*skip)->pos[0]=pos[0]+2.f*A1*R[0];
  CONTACT(contact,ret*skip)->pos[1]=pos[1]+2.f*A1*R[4];
  CONTACT(contact,ret*skip)->pos[2]=pos[2]+2.f*A1*R[8];
  CONTACT(contact,ret*skip)->depth=outDepth-dFabs(Q1*2.f*A1);

  if(CONTACT(contact,ret*skip)->depth>0.f)
  ret++;
  
  
  CONTACT(contact,ret*skip)->pos[0]=pos[0]+2.f*A3*R[2];
  CONTACT(contact,ret*skip)->pos[1]=pos[1]+2.f*A3*R[6];
  CONTACT(contact,ret*skip)->pos[2]=pos[2]+2.f*A3*R[10];
  CONTACT(contact,ret*skip)->depth=outDepth-dFabs(Q3*2.f*A3);

  if(CONTACT(contact,ret*skip)->depth>0.f) ret++;
} else {

  CONTACT(contact,ret*skip)->pos[0]=pos[0]+2.f*(A2>0 ? hlz*R[1]:-hlz*R[1]);
  CONTACT(contact,ret*skip)->pos[1]=pos[1]+2.f*(A2>0 ? hlz*R[5]:-hlz*R[5]);
  CONTACT(contact,ret*skip)->pos[2]=pos[2]+2.f*(A2>0 ? hlz*R[9]:-hlz*R[9]);
  CONTACT(contact,ret*skip)->depth=outDepth-dFabs(Q2*2.f*A2);

  if(CONTACT(contact,ret*skip)->depth>0.f) ret++;
}



 for (unsigned int i=0; i<ret; i++) {
    CONTACT(contact,i*skip)->g1 = const_cast<dxGeom*> (o1);
    CONTACT(contact,i*skip)->g2 = const_cast<dxGeom*> (o2);
	CONTACT(contact,i*skip)->normal[0] =n[0];
	CONTACT(contact,i*skip)->normal[1] =n[1];
	CONTACT(contact,i*skip)->normal[2] =n[2];
  }
  return ret;  
}

static  dColliderFn * FUNCCALL dCylinderColliderFn (int num)
{
  if (num == dBoxClass) return (dColliderFn *) &dCollideCylB;
  if (num == dSphereClass) return (dColliderFn *) &dCollideCylS;
  if (num == dCylinderClass) return (dColliderFn *) &dCollideCylCyl;
  if (num == dPlaneClass) return (dColliderFn *) &dCollideCylPlane;
  return 0;
}


static  void FUNCCALL dCylinderAABB (dxGeom *geom, dReal aabb[6])
{
  dxCylinder *b = (dxCylinder*) CLASSDATA(geom);
  dReal xrange =  dFabs (geom->R[0] * b->radius) +
    REAL(0.5) *dFabs (geom->R[1] * b->lz) + dFabs (geom->R[2] * b->radius);

  dReal yrange = dFabs (geom->R[4] * b->radius) +
    REAL(0.5) * dFabs (geom->R[5] * b->lz) + dFabs (geom->R[6] * b->radius);

  dReal zrange =  dFabs (geom->R[8] * b->radius) +
    REAL(0.5) *dFabs (geom->R[9] * b->lz) + dFabs (geom->R[10] * b->radius);

  aabb[0] = geom->pos[0] - xrange;
  aabb[1] = geom->pos[0] + xrange;
  aabb[2] = geom->pos[1] - yrange;
  aabb[3] = geom->pos[1] + yrange;
  aabb[4] = geom->pos[2] - zrange;
  aabb[5] = geom->pos[2] + zrange;
}

dxGeom *dCreateCylinder (dSpaceID space, dReal r, dReal lz)
{
  dAASSERT (r > 0 && lz > 0);
  if (dCylinderClass == -1) {
    dGeomClass c;
    c.bytes = sizeof (dxCylinder);
    c.collider = &dCylinderColliderFn;
    c.aabb = &dCylinderAABB;
    c.aabb_test = 0;
    c.dtor = 0;
    dCylinderClass = dCreateGeomClass (&c);
  }

  dxGeom *g = dCreateGeom (dCylinderClass);
  if (space) dSpaceAdd (space,g);
  dxCylinder *c = (dxCylinder*) CLASSDATA(g);
  c->radius = r;
  c->lz = lz;
  return g;
}



void dGeomCylinderSetParams (dGeomID g, dReal radius, dReal length)
{
  dUASSERT (g && g->_class->num == dCCylinderClass,"argument not a ccylinder");
  dAASSERT (radius > 0 && length > 0);
  dxCylinder *c = (dxCylinder*) CLASSDATA(g);
  c->radius = radius;
  c->lz = length;
}



void dGeomCylinderGetParams (dGeomID g, dReal *radius, dReal *length)
{
  dUASSERT (g && g->_class->num == dCylinderClass ,"argument not a cylinder");
  dxCylinder *c = (dxCylinder*) CLASSDATA(g);
  *radius = c->radius;
  *length = c->lz;
}

/*
void dMassSetCylinder (dMass *m, dReal density,
		  dReal radius, dReal length)
{
  dAASSERT (m);
  dMassSetZero (m);
  dReal M = length*M_PI*radius*radius*density;
  m->mass = M;
  m->_I(0,0) = M/REAL(4.0) * (ly*ly + lz*lz);
  m->_I(1,1) = M/REAL(12.0) * (lx*lx + lz*lz);
  m->_I(2,2) = M/REAL(4.0) * (lx*lx + ly*ly);

# ifndef dNODEBUG
  checkMass (m);
# endif
}
*/