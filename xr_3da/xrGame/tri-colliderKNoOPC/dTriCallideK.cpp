#include "stdafx.h"

#include <include/ode/common.h>
#include <include/ode/geom.h>
#include <include/ode/rotation.h>
#include <include/ode/odemath.h>
#include <ode/src/geom_internal.h>

#include "dTriCollideK.h"

//#define CONTACT(p,skip) ((dContactGeom*) (((char*)p) + (skip)))
#define CONTACT(Ptr, Stride) ((dContactGeom*) (((byte*)Ptr) + (Stride)))


extern "C" int dTriBox (
						const dReal* v0,const dReal* v1,const dReal* v2,
						const dxGeom *o1, const dxGeom *o2,
						int flags, dContactGeom *contact, int skip
						

						)
{

 // dIASSERT (skip >= (int)sizeof(dContactGeom));
 // dIASSERT (o1->_class->num == dBoxClass);
  

  dxBox *box = (dxBox*) CLASSDATA(o1);
  
  const dReal *R = o1->R;
  const dReal* p=o1->pos;
  const dVector3 side={box->side[0],box->side[1],box->side[2],-1};

    // find number of contacts requested
  int maxc = flags & NUMC_MASK;
  if (maxc < 1) maxc = 1;
  if (maxc > 3) maxc = 3;	// no more than 3 contacts per box allowed


  dVector3 triAx;
  dVector3 triSideAx0={v1[0]-v0[0],v1[1]-v0[1],v1[2]-v0[2]};
  dVector3 triSideAx1={v2[0]-v1[0],v2[1]-v1[1],v2[2]-v1[2]};
  dVector3 triSideAx2={v0[0]-v2[0],v0[1]-v2[1],v0[2]-v2[2]};
  dCROSS(triAx,=,triSideAx0,triSideAx1);
  int code=0;
  dReal outDepth, outDist;

  //sepparation along tri plane normal;
dNormalize3(triAx);


 dReal sidePr=
	dFabs(dDOT14(triAx,R+0)*side[0])+
	dFabs(dDOT14(triAx,R+1)*side[1])+
	dFabs(dDOT14(triAx,R+2)*side[2]);

dReal dist=dDOT(triAx,v0)-dDOT(triAx,p);
dReal depth=sidePr/2.f-dFabs(dist);
outDepth=depth;
outDist=dist;
code=0;
if(depth<0.f) return 0;



dReal depth0,depth1,depth2;

//sepparation along tri sides

//side ax0
dNormalize3(triSideAx0);
sidePr=
	dFabs(dDOT14(triSideAx0,R+0)*side[0])+
	dFabs(dDOT14(triSideAx0,R+1)*side[1])+
	dFabs(dDOT14(triSideAx0,R+2)*side[2]);

dReal dist0=dDOT(v0,triSideAx0)-dDOT(p,triSideAx0);
dReal dist1=dDOT(v1,triSideAx0)-dDOT(p,triSideAx0);
dReal dist2=dDOT(v2,triSideAx0)-dDOT(p,triSideAx0);

if(
   (dist0>0.f&&dist1>0.f&&dist2>0.f)
   ||
   (dist0<0.f&&dist1<0.f&&dist2<0.f)
   )
{
depth0=sidePr/2.f-dFabs(dist0);
depth1=sidePr/2.f-dFabs(dist1);
depth2=sidePr/2.f-dFabs(dist2);

if(depth0>depth1) 
		if(depth0>depth2) 	
			if(depth0>0.f){
				if(depth0<outDepth) 
					{
					outDepth=depth0;
					outDist=dist0;
					code=1;
					}
				}
			else return 0;
		else
			if(depth2>0.f){
				if(depth2<outDepth) 
					{
					outDepth=depth2;
					outDist=dist2;
					code=3;
					}
			}
			else return 0;
else
		if(depth1>depth2)
			if(depth1>0.f){
				if(depth1<outDepth) 
					{
					outDepth=depth1;
					outDist=dist1;
					code=2;
					}
			}
			else return 0;

		else
			if(depth2>0.f){
				if(depth2<outDepth) 
					{
					outDepth=depth2;
					outDist=dist2;
					code=3;
					}
			}
			else return 0;
			

}





//side ax1
dNormalize3(triSideAx1);
sidePr=
	dFabs(dDOT14(triSideAx1,R+0)*side[0])+
	dFabs(dDOT14(triSideAx1,R+1)*side[1])+
	dFabs(dDOT14(triSideAx1,R+2)*side[2]);

dist0=dDOT(v0,triSideAx1)-dDOT(p,triSideAx1);
dist1=dDOT(v1,triSideAx1)-dDOT(p,triSideAx1);
dist2=dDOT(v2,triSideAx1)-dDOT(p,triSideAx1);
if(
   (dist0>0.f&&dist1>0.f&&dist2>0.f)
   ||
   (dist0<0.f&&dist1<0.f&&dist2<0.f)
   )
{
depth0=sidePr/2.f-dFabs(dist0);
depth1=sidePr/2.f-dFabs(dist1);
depth2=sidePr/2.f-dFabs(dist2);

if(depth0>depth1) 
		if(depth0>depth2) 	
			if(depth0>0.f){
				if(depth0<outDepth) 
					{
					outDepth=depth0;
					outDist=dist0;
					code=4;
					}
				}
			else return 0;
		else
			if(depth2>0.f){
				if(depth2<outDepth) 
					{
					outDepth=depth2;
					outDist=dist2;
					code=6;
					}
			}
			else return 0;
else
		if(depth1>depth2)
			if(depth1>0.f){
				if(depth1<outDepth) 
					{
					outDepth=depth1;
					outDist=dist1;
					code=5;
					}
			}
			else return 0;

		else
			if(depth2>0.f){
				if(depth2<outDepth) 
					{
					outDepth=depth2;
					outDist=dist2;
					code=6;
					}
			}
			else return 0;
			

}

//side ax2
dNormalize3(triSideAx2);
sidePr=
	dFabs(dDOT14(triSideAx2,R+0)*side[0])+
	dFabs(dDOT14(triSideAx2,R+1)*side[1])+
	dFabs(dDOT14(triSideAx2,R+2)*side[2]);

dist0=dDOT(v0,triSideAx2)-dDOT(p,triSideAx2);
dist1=dDOT(v1,triSideAx2)-dDOT(p,triSideAx2);
dist2=dDOT(v2,triSideAx2)-dDOT(p,triSideAx2);

if(
   (dist0>0.f&&dist1>0.f&&dist2>0.f)
   ||
   (dist0<0.f&&dist1<0.f&&dist2<0.f)
   )
{
depth0=sidePr/2.f-dFabs(dist0);
depth1=sidePr/2.f-dFabs(dist1);
depth2=sidePr/2.f-dFabs(dist2);

if(depth0>depth1) 
		if(depth0>depth2) 	
			if(depth0>0.f){
				if(depth0<outDepth) 
					{
					outDepth=depth0;
					outDist=dist0;
					code=7;
					}
				}
			else return 0;
		else
			if(depth2>0.f){
				if(depth2<outDepth) 
					{
					outDepth=depth2;
					outDist=dist2;
					code=9;
					}
			}
			else return 0;
else
		if(depth1>depth2)
			if(depth1>0.f){
				if(depth1<outDepth) 
					{
					outDepth=depth1;
					outDist=dist1;
					code=8;
					}
			}
			else return 0;

		else
			if(depth2>0.f){
				if(depth2<outDepth) 
					{
					outDepth=depth2;
					outDist=dist2;
					code=9;
					}
			}
			else return 0;
			

}

//sepparation along box axes
//box ax0
dist0=dDOT14(v0,R+0)-dDOT14(p,R+0);
dist1=dDOT14(v1,R+0)-dDOT14(p,R+0);
dist2=dDOT14(v2,R+0)-dDOT14(p,R+0);

if(
   (dist0>0.f&&dist1>0.f&&dist2>0.f)
   ||
   (dist0<0.f&&dist1<0.f&&dist2<0.f)
   )
{
depth0=side[0]/2.f-dFabs(dist0);
depth1=side[0]/2.f-dFabs(dist1);
depth2=side[0]/2.f-dFabs(dist2);

if(depth0>depth1) 
		if(depth0>depth2) 	
			if(depth0>0.f){
				if(depth0<outDepth) 
					{
					outDepth=depth0;
					outDist=dist0;
					code=10;
					}
				}
			else return 0;
		else
			if(depth2>0.f){
				if(depth2<outDepth) 
					{
					outDepth=depth2;
					outDist=dist2;
					code=12;
					}
			}
			else return 0;
else
		if(depth1>depth2)
			if(depth1>0.f){
				if(depth1<outDepth) 
					{
					outDepth=depth1;
					outDist=dist1;
					code=11;
					}
			}
			else return 0;

		else
			if(depth2>0.f){
				if(depth2<outDepth) 
					{
					outDepth=depth2;
					outDist=dist2;
					code=12;
					}
			}
			else return 0;
			

}

//box ax1
dist0=dDOT14(v0,R+1)-dDOT14(p,R+1);
dist1=dDOT14(v1,R+1)-dDOT14(p,R+1);
dist2=dDOT14(v2,R+1)-dDOT14(p,R+1);
if(
   (dist0>0.f&&dist1>0.f&&dist2>0.f)
   ||
   (dist0<0.f&&dist1<0.f&&dist2<0.f)
   )
{
depth0=side[1]/2.f-dFabs(dist0);
depth1=side[1]/2.f-dFabs(dist1);
depth2=side[1]/2.f-dFabs(dist2);

if(depth0>depth1) 
		if(depth0>depth2) 	
			if(depth0>0.f){
				if(depth0<outDepth) 
					{
					outDepth=depth0;
					outDist=dist0;
					code=13;
					}
				}
			else return 0;
		else
			if(depth2>0.f){
				if(depth2<outDepth) 
					{
					outDepth=depth2;
					outDist=dist2;
					code=15;
					}
			}
			else return 0;
else
		if(depth1>depth2)
			if(depth1>0.f){
				if(depth1<outDepth) 
					{
					outDepth=depth1;
					outDist=dist1;
					code=14;
					}
			}
			else return 0;

		else
			if(depth2>0.f){
				if(depth2<outDepth) 
					{
					outDepth=depth2;
					outDist=dist2;
					code=15;
					}
			}
			else return 0;
			

}


// box ax2
dist0=dDOT14(v0,R+2)-dDOT14(p,R+2);
dist1=dDOT14(v1,R+2)-dDOT14(p,R+2);
dist2=dDOT14(v2,R+2)-dDOT14(p,R+2);
if(
   (dist0>0.f&&dist1>0.f&&dist2>0.f)
   ||
   (dist0<0.f&&dist1<0.f&&dist2<0.f)
   )
{
depth0=side[2]/2.f-dFabs(dist0);
depth1=side[2]/2.f-dFabs(dist1);
depth2=side[2]/2.f-dFabs(dist2);

if(depth0>depth1) 
		if(depth0>depth2) 	
			if(depth0>0.f){
				if(depth0<outDepth) 
					{
					outDepth=depth0;
					outDist=dist0;
					code=16;
					}
				}
			else return 0;
		else
			if(depth2>0.f){
				if(depth2<outDepth) 
					{
					outDepth=depth2;
					outDist=dist2;
					code=18;
					}
			}
			else return 0;
else
		if(depth1>depth2)
			if(depth1>0.f){
				if(depth1<outDepth) 
					{
					outDepth=depth1;
					outDist=dist1;
					code=17;
					}
			}
			else return 0;

		else
			if(depth2>0.f){
				if(depth2<outDepth) 
					{
					outDepth=depth2;
					outDist=dist2;
					code=18;
					}
			}
			else return 0;
			

}
int i;
//sepparation along cross axes (tri normal X box axes) code 19-27

dVector3 axis,outAx;
/*
for(i=0;i<3;i++){
	dCROSS114(axis,=,triAx,R+i);
	dNormalize3(axis);
	sidePr=
		dFabs(dDOT14(axis,R+0)*side[0])+
		dFabs(dDOT14(axis,R+1)*side[1])+
		dFabs(dDOT14(axis,R+2)*side[2]);

	dist0=dDOT(v0,axis)-dDOT(p,axis);
	dist1=dDOT(v1,axis)-dDOT(p,axis);
	dist2=dDOT(v2,axis)-dDOT(p,axis);

if(!(
   (dist0>0.f&&dist1>0.f&&dist2>0.f)
   ||
   (dist0<0.f&&dist1<0.f&&dist2<0.f)
   )
   ) continue;

{
depth0=sidePr/2.f-dFabs(dist0);
depth1=sidePr/2.f-dFabs(dist1);
depth2=sidePr/2.f-dFabs(dist2);

if(depth0>depth1) 
		if(depth0>depth2) 	
			if(depth0>0.f){
				if(depth0<outDepth) 
					{
						outDepth=depth0;
						outDist=dist0;
						outAx[0]=axis[0];
						outAx[1]=axis[1];
						outAx[2]=axis[2];
						code=18+i*3+1;
					}
				}
			else return 0;
		else
			if(depth2>0.f){
				if(depth2<outDepth) 
					{
						 outDepth=depth2;
						 outDist=dist2;
						 outAx[0]=axis[0];
						 outAx[1]=axis[1];
						 outAx[2]=axis[2];
					     code=18+i*3+3;
					}
			}
			else return 0;
else
		if(depth1>depth2)
			if(depth1>0.f){
				if(depth1<outDepth) 
					{
						 outDepth=depth1;
						 outDist=dist1;
						 outAx[0]=axis[0];
						 outAx[1]=axis[1];
						 outAx[2]=axis[2];
						 code=18+i*3+2;
					}
			}
			else return 0;

		else
			if(depth2>0.f){
				if(depth2<outDepth) 
					{
						 outDepth=depth2;
						 outDist=dist2;
						 outAx[0]=axis[0];
						 outAx[1]=axis[1];
						 outAx[2]=axis[2];
					     code=18+i*3+3;
					}
			}
			else return 0;
			

}

}

*/
//sepparation along cross axes (tri side0 X box axes) code 28-36
for(i=0;i<3;i++){
	dCROSS114(axis,=,triSideAx0,R+i);
	dNormalize3(axis);
	sidePr=
		dFabs(dDOT14(axis,R+0)*side[0])+
		dFabs(dDOT14(axis,R+1)*side[1])+
		dFabs(dDOT14(axis,R+2)*side[2]);

	dist0=dDOT(v0,axis)-dDOT(p,axis);
	dist1=dDOT(v1,axis)-dDOT(p,axis);
	dist2=dDOT(v2,axis)-dDOT(p,axis);

	if(!(
	(dist0>0.f&&dist1>0.f&&dist2>0.f)
	||
	(dist0<0.f&&dist1<0.f&&dist2<0.f)
		)
	) continue;

{
depth0=sidePr/2.f-dFabs(dist0);
depth1=sidePr/2.f-dFabs(dist1);
depth2=sidePr/2.f-dFabs(dist2);

if(depth0>depth1) 
		if(depth0>depth2) 	
			if(depth0>0.f){
				if(depth0<outDepth) 
					{
						outDepth=depth0;
						outDist=dist0;
						outAx[0]=axis[0];
						outAx[1]=axis[1];
						outAx[2]=axis[2];
						code=27+i*3+1;
					}
				}
			else return 0;
		else
			if(depth2>0.f){
				if(depth2<outDepth) 
					{
						 continue;
						 outDepth=depth2;
						 outDist=dist2;
						 outAx[0]=axis[0];
						 outAx[1]=axis[1];
						 outAx[2]=axis[2];
					     code=27+i*3+3;
					}
			}
			else return 0;
else
		if(depth1>depth2)
			if(depth1>0.f){
				if(depth1<outDepth) 
					{
						 outDepth=depth1;
						 outDist=dist1;
						 outAx[0]=axis[0];
						 outAx[1]=axis[1];
						 outAx[2]=axis[2];
						 code=27+i*3+2;
					}
			}
			else return 0;

		else
			if(depth2>0.f){
				if(depth2<outDepth) 
					{
						 continue;
						 outDepth=depth2;
						 outDist=dist2;
						 outAx[0]=axis[0];
						 outAx[1]=axis[1];
						 outAx[2]=axis[2];
					     code=27+i*3+3;
					}
			}
			else return 0;
			

}


	}


//sepparation along cross axes (tri side1 X box axes) code 37-45
for(i=0;i<3;i++){
	dCROSS114(axis,=,triSideAx1,R+i);
	dNormalize3(axis);
	sidePr=
		dFabs(dDOT14(axis,R+0)*side[0])+
		dFabs(dDOT14(axis,R+1)*side[1])+
		dFabs(dDOT14(axis,R+2)*side[2]);

	dist0=dDOT(v0,axis)-dDOT(p,axis);
	dist1=dDOT(v1,axis)-dDOT(p,axis);
	dist2=dDOT(v2,axis)-dDOT(p,axis);

	if(!(
   (dist0>0.f&&dist1>0.f&&dist2>0.f)
   ||
   (dist0<0.f&&dist1<0.f&&dist2<0.f)
		)
   ) continue;

{
depth0=sidePr/2.f-dFabs(dist0);
depth1=sidePr/2.f-dFabs(dist1);
depth2=sidePr/2.f-dFabs(dist2);

if(depth0>depth1) 
		if(depth0>depth2) 	
			if(depth0>0.f){
				if(depth0<outDepth) 
					{	
						continue;
						outDepth=depth0;
						outDist=dist0;
						outAx[0]=axis[0];
						outAx[1]=axis[1];
						outAx[2]=axis[2];
						code=36+i*3+1;
					}
				}
			else return 0;
		else
			if(depth2>0.f){
				if(depth2<outDepth) 
					{
						 outDepth=depth2;
						 outDist=dist2;
						 outAx[0]=axis[0];
						 outAx[1]=axis[1];
						 outAx[2]=axis[2];
					     code=36+i*3+3;
					}
			}
			else return 0;
else
		if(depth1>depth2)
			if(depth1>0.f){
				if(depth1<outDepth) 
					{
						 outDepth=depth1;
						 outDist=dist1;
						 outAx[0]=axis[0];
						 outAx[1]=axis[1];
						 outAx[2]=axis[2];
						 code=36+i*3+2;
					}
			}
			else return 0;

		else
			if(depth2>0.f){
				if(depth2<outDepth) 
					{
						 outDepth=depth2;
						 outDist=dist2;
						 outAx[0]=axis[0];
						 outAx[1]=axis[1];
						 outAx[2]=axis[2];
					     code=36+i*3+3;
					}
			}
			else return 0;
			

}

}

//sepparation along cross axes (tri side2 X box axes) code 46-54
for(i=0;i<3;i++){
	dCROSS114(axis,=,triSideAx2,R+i);
	dNormalize3(axis);
	sidePr=
		dFabs(dDOT14(axis,R+0)*side[0])+
		dFabs(dDOT14(axis,R+1)*side[1])+
		dFabs(dDOT14(axis,R+2)*side[2]);

	dist0=dDOT(v0,axis)-dDOT(p,axis);
	dist1=dDOT(v1,axis)-dDOT(p,axis);
	dist2=dDOT(v2,axis)-dDOT(p,axis);

	if(!(
   (dist0>0.f&&dist1>0.f&&dist2>0.f)
   ||
   (dist0<0.f&&dist1<0.f&&dist2<0.f)
		)
   ) continue;

{
depth0=sidePr/2.f-dFabs(dist0);
depth1=sidePr/2.f-dFabs(dist1);
depth2=sidePr/2.f-dFabs(dist2);

if(depth0>depth1) 
		if(depth0>depth2) 	
			if(depth0>0.f){
				if(depth0<outDepth) 
					{
						outDepth=depth0;
						outDist=dist0;
						outAx[0]=axis[0];
						outAx[1]=axis[1];
						outAx[2]=axis[2];
						code=45+i*3+1;
					}
				}
			else return 0;
		else
			if(depth2>0.f){
				if(depth2<outDepth) 
					{
						 outDepth=depth2;
						 outDist=dist2;
						 outAx[0]=axis[0];
						 outAx[1]=axis[1];
						 outAx[2]=axis[2];
					     code=45+i*3+3;
					}
			}
			else return 0;
else
		if(depth1>depth2)
			if(depth1>0.f){
				if(depth1<outDepth) 
					{
						 continue;
						 outDepth=depth1;
						 outDist=dist1;
						 outAx[0]=axis[0];
						 outAx[1]=axis[1];
						 outAx[2]=axis[2];
						 code=45+i*3+2;
					}
			}
			else return 0;

		else
			if(depth2>0.f){
				if(depth2<outDepth) 
					{
						 outDepth=depth2;
						 outDist=dist2;
						 outAx[0]=axis[0];
						 outAx[1]=axis[1];
						 outAx[2]=axis[2];
					     code=45+i*3+3;
					}
			}
			else return 0;
			

}


	}
	
//////////////////////////////////////////////////////////////////////
///if we get to this poit tri touches box
dVector3 norm,pos;
unsigned int ret=1;

if(code==0){
	norm[0]=triAx[0]*outDist;
	norm[1]=triAx[1]*outDist;
	norm[2]=triAx[2]*outDist;
	char signum=outDist>=0 ? 1 : -1;
/////////////////////////////////////////// from geom.cpp dCollideBP
  dReal Q1 = -signum*dDOT14(triAx,R+0);
  dReal Q2 = -signum*dDOT14(triAx,R+1);
  dReal Q3 = -signum*dDOT14(triAx,R+2);
  dReal A1 = side[0] * Q1;
  dReal A2 = side[1] * Q2;
  dReal A3 = side[2] * Q3;
  dReal B1 = dFabs(A1);
  dReal B2 = dFabs(A2);
  dReal B3 = dFabs(A3);

  pos[0]=p[0];
  pos[1]=p[1];
  pos[2]=p[2];

#define FOO(i,op) \
  pos[0] op REAL(0.5)*side[i] * R[0+i]; \
  pos[1] op REAL(0.5)*side[i] * R[4+i]; \
  pos[2] op REAL(0.5)*side[i] * R[8+i];
#define BAR(i,iinc) if (A ## iinc > 0) { FOO(i,-=) } else { FOO(i,+=) }
  BAR(0,1);
  BAR(1,2);
  BAR(2,3);
#undef FOO
#undef BAR

///////////////////////////////////////////////////////////


if (maxc == 1) goto done;

  // get the second and third contact points by starting from `p' and going
  // along the two sides with the smallest projected length.

//(@slipch) it is not perfectly right for triangle collision 
//because it need to check if additional points are in the triangle but it seems cause no problem

#define FOO(i,j,op) \
  CONTACT(contact,i*skip)->pos[0] = p[0] op side[j] * R[0+j]; \
  CONTACT(contact,i*skip)->pos[1] = p[1] op side[j] * R[4+j]; \
  CONTACT(contact,i*skip)->pos[2] = p[2] op side[j] * R[8+j];
#define BAR(ctact,side,sideinc) \
  depth -= B ## sideinc; \
  if (depth < 0) goto done; \
  if (A ## sideinc > 0) { FOO(ctact,side,+) } else { FOO(ctact,side,-) } \
  CONTACT(contact,ctact*skip)->depth = depth; \
  ret++;

  CONTACT(contact,skip)->normal[0] =  triAx[0]*signum;
  CONTACT(contact,skip)->normal[1] =  triAx[1]*signum;
  CONTACT(contact,skip)->normal[2] =  triAx[2]*signum;
  if (maxc == 3) {
    CONTACT(contact,2*skip)->normal[0] = triAx[0]*signum;
    CONTACT(contact,2*skip)->normal[1] = triAx[1]*signum;
    CONTACT(contact,2*skip)->normal[2] = triAx[2]*signum;
  }

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

 done: ;

////////////////////////////////////////////////////////////// end (from geom.cpp dCollideBP)










	}

if(code>=1&&code<=27)
{
	switch((code-1)%3){
	case 0:
	pos[0]=v0[0];
	pos[1]=v0[1];
	pos[2]=v0[2];
	break;
	case 1:
	pos[0]=v1[0];
	pos[1]=v1[1];
	pos[2]=v1[2];
	break;
	case 2:
	pos[0]=v2[0];
	pos[1]=v2[1];
	pos[2]=v2[2];
	break;
	}

if(code>=1&&code<=3){
	norm[0]=triSideAx0[0]*outDist;
	norm[1]=triSideAx0[1]*outDist;
	norm[2]=triSideAx0[2]*outDist;
	}

else if(code>=4&&code<=6){
	norm[0]=triSideAx1[0]*outDist;
	norm[1]=triSideAx1[1]*outDist;
	norm[2]=triSideAx1[2]*outDist;
	}

else if(code>=7&&code<=9){
	norm[0]=triSideAx2[0]*outDist;
	norm[1]=triSideAx2[1]*outDist;
	norm[2]=triSideAx2[2]*outDist;
	}

else if(code>=10&&code<=12){
	norm[0]=R[0]*outDist;
	norm[1]=R[4]*outDist;
	norm[2]=R[8]*outDist;
	}

else if(code>=13&&code<=15){
	norm[0]=R[1]*outDist;
	norm[1]=R[5]*outDist;
	norm[2]=R[9]*outDist;
	}
else if(code>=16&&code<=18){
	norm[0]=R[2]*outDist;
	norm[1]=R[6]*outDist;
	norm[2]=R[10]*outDist;
	}
else if(code>=19&&code<=27){
	norm[0]=outAx[0]*outDist;
	norm[1]=outAx[1]*outDist;
	norm[2]=outAx[2]*outDist;
	}
}

else if(code>27){
	norm[0]=outAx[0]*outDist;
	norm[1]=outAx[1]*outDist;
	norm[2]=outAx[2]*outDist;



/////////////
  char signum=outDist>=0 ? 1 : -1;
 
  dReal Q1 = -signum*dDOT14(outAx,R+0);
  dReal Q2 = -signum*dDOT14(outAx,R+1);
  dReal Q3 = -signum*dDOT14(outAx,R+2);
  dReal A1 = side[0] * Q1;
  dReal A2 = side[1] * Q2;
  dReal A3 = side[2] * Q3;
  pos[0]=p[0];
  pos[1]=p[1];
  pos[2]=p[2];

#define FOO(i,op) \
  pos[0] op REAL(0.5)*side[i] * R[0+i]; \
  pos[1] op REAL(0.5)*side[i] * R[4+i]; \
  pos[2] op REAL(0.5)*side[i] * R[8+i];
#define BAR(i,iinc) if (A ## iinc > 0) { FOO(i,-=) } else { FOO(i,+=) }
  BAR(0,1);
  BAR(1,2);
  BAR(2,3);
#undef FOO
#undef BAR
////////////////



if(code>=28&&code<=36)
	if((code-1)%3!=2){
			i=(code-28)/3;
			CrossProjLine1(v0,triSideAx0,pos,R+i,pos);
			if(pos[0]==dInfinity){
									pos[0]=(v1[0]+v0[0])/2.f;
									pos[1]=(v1[1]+v0[1])/2.f;
									pos[2]=(v1[2]+v0[2])/2.f;
								}
		}
	else {
		pos[0]=v2[0];
		pos[1]=v2[1];
		pos[2]=v2[2];
		}

else if(code>=37&&code<=45)
	if((code-1)%3!=0){
			i=(code-37)/3;
			CrossProjLine1(v1,triSideAx1,pos,R+i,pos);
			if(pos[0]==dInfinity){
									pos[0]=(v2[0]+v1[0])/2.f;
									pos[1]=(v2[1]+v1[1])/2.f;
									pos[2]=(v2[2]+v1[2])/2.f;
								}
		}
	else {
		pos[0]=v0[0];
		pos[1]=v0[1];
		pos[2]=v0[2];
		}
else if(code>=46&&code<=54)
	if((code-1)%3!=1){
			i=(code-46)/3;
			CrossProjLine1(v0,triSideAx2,pos,R+i,pos);
			if(pos[0]==dInfinity){
									pos[0]=(v2[0]+v0[0])/2.f;
									pos[1]=(v2[1]+v0[1])/2.f;
									pos[2]=(v2[2]+v0[2])/2.f;
								}
		}
	else {
		pos[0]=v1[0];
		pos[1]=v1[1];
		pos[2]=v1[2];
		}


}

dNormalize3(norm);

contact->normal[0] = norm[0];
contact->normal[1] = norm[1];
contact->normal[2] = norm[2];


contact->pos[0] = pos[0];
contact->pos[1] = pos[1];
contact->pos[2] = pos[2];

contact->depth = outDepth;
//contact->g1 = const_cast<dxGeom*> (o2);
//contact->g2 = const_cast<dxGeom*> (o1);

 for (i=0; i<ret; i++) {
    CONTACT(contact,i*skip)->g1 = const_cast<dxGeom*> (o2);
    CONTACT(contact,i*skip)->g2 = const_cast<dxGeom*> (o1);
  }
  return ret;
//  for (i=0; i<3; i++) contact[0].pos[i] = vertex[i];
 // contact[0].depth = *depth;
  return 1;
}