#include "stdafx.h"
#define SHAREDLIBIMPORT __declspec (dllimport)
#define SHAREDLIBEXPORT __declspec (dllexport)
#include <include/ode/common.h>
#include <include/ode/geom.h>
#include <include/ode/rotation.h>
#include <include/ode/odemath.h>
#include <ode/src/geom_internal.h>

#include "dTriCollideK.h"
#define CONTACT(Ptr, Stride) ((dContactGeom*) (((byte*)Ptr) + (Stride)))
extern int dCylinderClass;


extern "C" int dSortedTriBox (
						const dReal* triSideAx0,const dReal* triSideAx1,
						const dReal* triAx,
						const dReal* v0,
						const dReal* v1,
						const dReal* v2,
						dReal dist,
						const dxGeom *o1, const dxGeom *o2,
						int flags, dContactGeom *contact, int skip
						)
{

  dIASSERT (skip >= (int)sizeof(dContactGeom));
  dIASSERT (o1->_class->num == dBoxClass);
  
  dxBox *box = (dxBox*) CLASSDATA(o1);
  
  const dReal *R = o1->R;
  const dReal* p=o1->pos;
  const dVector3 hside={box->side[0]/2.f,box->side[1]/2.f,box->side[2]/2.f,-1};
 // dVector3 triSideAx2={v0[0]-v2[0],v0[1]-v2[1],v0[2]-v2[2]};
    // find number of contacts requested
  int maxc = flags & NUMC_MASK;
  if (maxc < 1) maxc = 1;
  if (maxc > 3) maxc = 3;	// no more than 3 contacts per box allowed

  int code=0;
  dReal outDepth;
  char signum;//,sn;
  //sepparation along tri plane normal;
//dNormalize3(triAx);


 dReal sidePr=
	dFabs(dDOT14(triAx,R+0)*hside[0])+
	dFabs(dDOT14(triAx,R+1)*hside[1])+
	dFabs(dDOT14(triAx,R+2)*hside[2]);

//dReal dist=dDOT(triAx,v0)-dDOT(triAx,p);
dReal depth=sidePr-dist;//dFabs(dist);
outDepth=depth;
signum=-1;//dist<0.f ? -1 : 1;
code=0;

if(depth<0.f) return 0;

unsigned int i;

//dVector3 axis,outAx;
/*
if(dist<0.f) goto toch;
//if(dist>0.f){
bool isPdist0,isPdist1,isPdist2;
bool test0=true,test1=true,test2=true;
bool test00,test01,test02;
bool test10,test11,test12;
bool test20,test21,test22;

dReal depth0,depth1,depth2;
dReal dist0,dist1,dist2;



#define CMP(sd,c)	\
if(depth0>depth1)\
		if(depth0>depth2) \
			if(test0##sd){\
			  if(test0)\
				if(depth0<outDepth)\
					{\
					outDepth=depth0;\
					signum=-sn;\
					code=c;\
					}\
			}\
			else return 0;\
		else\
			if(test2##sd){\
			  if(test2)\
				if(depth2<outDepth) \
					{\
					outDepth=depth2;\
					signum=-sn;\
					code=c+2;\
					}\
			}\
			else return 0;\
else\
		if(depth1>depth2)\
			if(test1##sd){\
			  if(test1)\
				if(depth1<outDepth) \
					{\
					outDepth=depth1;\
					signum=-sn;\
					code=c+1;\
					}\
			}\
			else return 0;\
\
		else\
			if(test2##sd){\
			  if(test2)\
				if(depth2<outDepth) \
					{\
					outDepth=depth2;\
					signum=sn;\
					code=c+2;\
					}\
			}\
			else return 0;


#define TEST(sd, c) \
\
sn=dDOT14(triAx,R+sd)>0.f ? 1 : -1;\
dist0=-sn*(dDOT14(v0,R+sd)-dDOT14(p,R+sd));\
dist1=-sn*(dDOT14(v1,R+sd)-dDOT14(p,R+sd));\
dist2=-sn*(dDOT14(v2,R+sd)-dDOT14(p,R+sd));\
\
isPdist0=dist0>0.f;\
isPdist1=dist1>0.f;\
isPdist2=dist2>0.f;\
\
depth0=hside[sd]-dist0;\
depth1=hside[sd]-dist1;\
depth2=hside[sd]-dist2;\
test0##sd = depth0>0.f;\
test1##sd = depth1>0.f;\
test2##sd = depth2>0.f;\
\
test0 =test0 && test0##sd;\
test1 =test1 && test1##sd;\
test2 =test2 && test2##sd;\
\
if(isPdist0==isPdist1 && isPdist1==isPdist2)\
{\
CMP(sd,c)\
}

TEST(0,1)
TEST(1,4)
TEST(2,7)

#undef CMP
#undef TEST


#define TEST(ax,ox,c) \
for(i=0;i<3;i++){\
	dCROSS114(axis,=,triSideAx##ax,R+i);\
	dNormalize3(axis);\
	sn=dDOT(triAx,axis)>0.f ? 1 : -1;\
	int ix1=(i+1)%3;\
	int ix2=(i+2)%3;\
	sidePr=\
		dFabs(dDOT14(axis,R+ix1)*hside[ix1])+\
		dFabs(dDOT14(axis,R+ix2)*hside[ix2]);\
\
	dist##ax=-sn*(dDOT(v##ax,axis)-dDOT(p,axis));\
	dist##ox=-sn*(dDOT(v##ox,axis)-dDOT(p,axis));\
\
isPdist##ax=dist##ax>0.f;\
isPdist##ox=dist##ox>0.f;\
if(isPdist##ax != isPdist##ox) continue;\
\
depth##ax=sidePr-dist##ax;\
depth##ox=sidePr-dist##ox;\
	if(depth##ax>depth##ox){\
			if(depth##ax>0.f){\
				if(depth##ax<outDepth) \
					{\
						signum=-sn;\
						outDepth=depth##ax;\
						outAx[0]=axis[0];\
						outAx[1]=axis[1];\
						outAx[2]=axis[2];\
						code=c+i;\
					}\
				}\
			else return 0;\
	}\
}

TEST(0,2,10)
TEST(1,0,13)
TEST(2,1,16)

#undef TEST
//}
//////////////////////////////////////////////////////////////////////
///if we get to this poit tri touches box
toch:;
*/
dVector3 norm,pos;
unsigned int ret=1;

if(code==0){
	norm[0]=triAx[0]*signum;
	norm[1]=triAx[1]*signum;
	norm[2]=triAx[2]*signum;

/////////////////////////////////////////// from geom.cpp dCollideBP
  dReal Q1 = -signum*dDOT14(triAx,R+0);
  dReal Q2 = -signum*dDOT14(triAx,R+1);
  dReal Q3 = -signum*dDOT14(triAx,R+2);
  dReal A1 = 2.f*hside[0] * Q1;
  dReal A2 = 2.f*hside[1] * Q2;
  dReal A3 = 2.f*hside[2] * Q3;
  dReal B1 = dFabs(A1);
  dReal B2 = dFabs(A2);
  dReal B3 = dFabs(A3);

  pos[0]=p[0];
  pos[1]=p[1];
  pos[2]=p[2];

#define FOO(i,op) \
  pos[0] op hside[i] * R[0+i]; \
  pos[1] op hside[i] * R[4+i]; \
  pos[2] op hside[i] * R[8+i];
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
  CONTACT(contact,i*skip)->pos[0] = pos[0] op 2.f*hside[j] * R[0+j]; \
  CONTACT(contact,i*skip)->pos[1] = pos[1] op 2.f*hside[j] * R[4+j]; \
  CONTACT(contact,i*skip)->pos[2] = pos[2] op 2.f*hside[j] * R[8+j];
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
/*
else if(code<=9)
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
switch((code-1)/3){
	case 0:
		{
		norm[0]=R[0]*signum;
		norm[1]=R[4]*signum;
		norm[2]=R[8]*signum;
		}
	break;

	case 1:	
		{
		norm[0]=R[1]*signum;
		norm[1]=R[5]*signum;
		norm[2]=R[9]*signum;
		}
	break;
	case 2:
		{
		norm[0]=R[2]*signum;
		norm[1]=R[6]*signum;
		norm[2]=R[10]*signum;
		}
	break;
	}
}
else {
	norm[0]=outAx[0]*signum;
	norm[1]=outAx[1]*signum;
	norm[2]=outAx[2]*signum;



/////////////
 
  dReal Q1 = -signum*dDOT14(outAx,R+0);
  dReal Q2 = -signum*dDOT14(outAx,R+1);
  dReal Q3 = -signum*dDOT14(outAx,R+2);
  dReal A1 = 2.f*hside[0] * Q1;
  dReal A2 = 2.f*hside[1] * Q2;
  dReal A3 = 2.f*hside[2] * Q3;
  pos[0]=p[0];
  pos[1]=p[1];
  pos[2]=p[2];

#define FOO(i,op) \
  pos[0] op hside[i] * R[0+i]; \
  pos[1] op hside[i] * R[4+i]; \
  pos[2] op hside[i] * R[8+i];
#define BAR(i,iinc) if (A ## iinc > 0) { FOO(i,-=) } else { FOO(i,+=) }
  BAR(0,1);
  BAR(1,2);
  BAR(2,3);
#undef FOO
#undef BAR
////////////////


switch((code-10)/3){

case 0:
			CrossProjLine1(v0,triSideAx0,pos,R+(code-10),pos);
			if(pos[0]==dInfinity){
									pos[0]=(v1[0]+v0[0])/2.f;
									pos[1]=(v1[1]+v0[1])/2.f;
									pos[2]=(v1[2]+v0[2])/2.f;
								}
break;

case 1:
			CrossProjLine1(v1,triSideAx1,pos,R+(code-13),pos);
			if(pos[0]==dInfinity){
									pos[0]=(v2[0]+v1[0])/2.f;
									pos[1]=(v2[1]+v1[1])/2.f;
									pos[2]=(v2[2]+v1[2])/2.f;
									}
break;

case 2:
			CrossProjLine1(v0,triSideAx2,pos,R+(code-16),pos);
			if(pos[0]==dInfinity){
									pos[0]=(v2[0]+v0[0])/2.f;
									pos[1]=(v2[1]+v0[1])/2.f;
									pos[2]=(v2[2]+v0[2])/2.f;
								}
}
}


*/

contact->pos[0] = pos[0];
contact->pos[1] = pos[1];
contact->pos[2] = pos[2];

contact->depth = outDepth;


 for (i=0; i<ret; i++) {
    CONTACT(contact,i*skip)->g1 = const_cast<dxGeom*> (o2);
    CONTACT(contact,i*skip)->g2 = const_cast<dxGeom*> (o1);
	CONTACT(contact,i*skip)->normal[0] = norm[0];
	CONTACT(contact,i*skip)->normal[1] = norm[1];
	CONTACT(contact,i*skip)->normal[2] = norm[2];
  }
  return ret;




 
}
extern "C" int dTriBox (
						const dReal* v0,const dReal* v1,const dReal* v2,
						const dxGeom *o1, const dxGeom *o2,
						int flags, dContactGeom *contact, int skip
						)
{

  dIASSERT (skip >= (int)sizeof(dContactGeom));
  dIASSERT (o1->_class->num == dBoxClass);
  
  dxBox *box = (dxBox*) CLASSDATA(o1);
  
  const dReal *R = o1->R;
  const dReal* p=o1->pos;
  const dVector3 hside={box->side[0]/2.f,box->side[1]/2.f,box->side[2]/2.f,-1};

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
  dReal outDepth;
  char signum;
  //sepparation along tri plane normal;
dNormalize3(triAx);


 dReal sidePr=
	dFabs(dDOT14(triAx,R+0)*hside[0])+
	dFabs(dDOT14(triAx,R+1)*hside[1])+
	dFabs(dDOT14(triAx,R+2)*hside[2]);

dReal dist=dDOT(triAx,v0)-dDOT(triAx,p);
dReal depth=sidePr-dFabs(dist);
outDepth=depth;
signum=dist<0.f ? -1 : 1;
code=0;
if(depth<0.f) return 0;


bool isPdist0,isPdist1,isPdist2;
bool test0=true,test1=true,test2=true;
bool test00,test01,test02;
bool test10,test11,test12;
bool test20,test21,test22;

dReal depth0,depth1,depth2;
dReal dist0,dist1,dist2;



#define CMP(sd,c)	\
if(depth0>depth1)\
		if(depth0>depth2) \
			if(test0##sd){\
			  if(test0)\
				if(depth0<outDepth)\
					{\
					outDepth=depth0;\
					signum=dist0<0.f ? -1 : 1;\
					code=c;\
					}\
			}\
			else return 0;\
		else\
			if(test2##sd){\
			  if(test2)\
				if(depth2<outDepth) \
					{\
					outDepth=depth2;\
					signum=dist2<0.f ? -1 : 1;\
					code=c+2;\
					}\
			}\
			else return 0;\
else\
		if(depth1>depth2)\
			if(test1##sd){\
			  if(test1)\
				if(depth1<outDepth) \
					{\
					outDepth=depth1;\
					signum=dist1<0.f ? -1 : 1;\
					code=c+1;\
					}\
			}\
			else return 0;\
\
		else\
			if(test2##sd){\
			  if(test2)\
				if(depth2<outDepth) \
					{\
					outDepth=depth2;\
					signum=dist2<0.f ? -1 : 1;\
					code=c+2;\
					}\
			}\
			else return 0;


#define TEST(sd, c) \
\
dist0=dDOT14(v0,R+sd)-dDOT14(p,R+sd);\
dist1=dDOT14(v1,R+sd)-dDOT14(p,R+sd);\
dist2=dDOT14(v2,R+sd)-dDOT14(p,R+sd);\
\
isPdist0=dist0>0.f;\
isPdist1=dist1>0.f;\
isPdist2=dist2>0.f;\
\
depth0=hside[sd]-dFabs(dist0);\
depth1=hside[sd]-dFabs(dist1);\
depth2=hside[sd]-dFabs(dist2);\
test0##sd = depth0>0.f;\
test1##sd = depth1>0.f;\
test2##sd = depth2>0.f;\
\
test0 =test0 && test0##sd;\
test1 =test1 && test1##sd;\
test2 =test2 && test2##sd;\
\
if(isPdist0==isPdist1 && isPdist1==isPdist2)\
{\
CMP(sd,c)\
}

TEST(0,1)
TEST(1,4)
TEST(2,7)

#undef CMP
#undef TEST

unsigned int i;

dVector3 axis,outAx;

/*
#define TEST(ax,ox,c) \
for(i=0;i<3;i++){\
	dCROSS114(axis,=,triSideAx##ax,R+i);\
	dNormalize3(axis);\
	int ix1=(i+1)%3;\
	int ix2=(i+2)%3;\
	sidePr=\
		dFabs(dDOT14(axis,R+ix1)*hside[ix1])+\
		dFabs(dDOT14(axis,R+ix2)*hside[ix2]);\
\
	dist##ax=(dDOT(v##ax,axis)-dDOT(p,axis));\
	dist##ox=(dDOT(v##ox,axis)-dDOT(p,axis));\
			signum=dist##ox<0.f ? -1.f : 1.f;\
\
depth##ax=sidePr-signum*dist##ax;\
depth##ox=sidePr-signum*dist##ox;\
	if(depth##ax<depth##ox){\
			if(depth##ax>0.f){\
			if(depth##ax<outDepth){ \
				if(dDOT(axis,triAx)*signum<0.f)	{\
						outDepth=depth##ax;\
						outAx[0]=axis[0];\
						outAx[1]=axis[1];\
						outAx[2]=axis[2];\
						code=c+i;\
					}\
			}\
				}\
			else return 0;\
	}\
}
*/
#define TEST(ax,ox,c) \
for(i=0;i<3;i++){\
	dCROSS114(axis,=,triSideAx##ax,R+i);\
	dNormalize3(axis);\
	int ix1=(i+1)%3;\
	int ix2=(i+2)%3;\
	sidePr=\
		dFabs(dDOT14(axis,R+ix1)*hside[ix1])+\
		dFabs(dDOT14(axis,R+ix2)*hside[ix2]);\
\
	dist##ax=dDOT(v##ax,axis)-dDOT(p,axis);\
	dist##ox=dDOT(v##ox,axis)-dDOT(p,axis);\
\
isPdist##ax=dist##ax>0.f;\
isPdist##ox=dist##ox>0.f;\
if(isPdist##ax != isPdist##ox) continue;\
\
depth##ax=sidePr-dFabs(dist##ax);\
depth##ox=sidePr-dFabs(dist##ox);\
	if(depth##ax>depth##ox){\
			if(depth##ax>0.f){\
				if(depth##ax<outDepth) \
					{\
						outDepth=depth##ax;\
						signum=dist##ax<0.f ? -1 : 1;\
						outAx[0]=axis[0];\
						outAx[1]=axis[1];\
						outAx[2]=axis[2];\
						code=c+i;\
					}\
				}\
			else return 0;\
	}\
}

TEST(0,2,10)
TEST(1,0,13)
TEST(2,1,16)

#undef TEST

//////////////////////////////////////////////////////////////////////
///if we get to this poit tri touches box
dVector3 norm,pos;
unsigned int ret=1;

if(code==0){
	norm[0]=triAx[0]*signum;
	norm[1]=triAx[1]*signum;
	norm[2]=triAx[2]*signum;

/////////////////////////////////////////// from geom.cpp dCollideBP
  dReal Q1 = -signum*dDOT14(triAx,R+0);
  dReal Q2 = -signum*dDOT14(triAx,R+1);
  dReal Q3 = -signum*dDOT14(triAx,R+2);
  dReal A1 = 2.f*hside[0] * Q1;
  dReal A2 = 2.f*hside[1] * Q2;
  dReal A3 = 2.f*hside[2] * Q3;
  dReal B1 = dFabs(A1);
  dReal B2 = dFabs(A2);
  dReal B3 = dFabs(A3);

  pos[0]=p[0];
  pos[1]=p[1];
  pos[2]=p[2];

#define FOO(i,op) \
  pos[0] op hside[i] * R[0+i]; \
  pos[1] op hside[i] * R[4+i]; \
  pos[2] op hside[i] * R[8+i];
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
  CONTACT(contact,i*skip)->pos[0] = pos[0] op 2.f*hside[j] * R[0+j]; \
  CONTACT(contact,i*skip)->pos[1] = pos[1] op 2.f*hside[j] * R[4+j]; \
  CONTACT(contact,i*skip)->pos[2] = pos[2] op 2.f*hside[j] * R[8+j];
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
else if(code<=9)
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
switch((code-1)/3){
	case 0:
		{
		norm[0]=R[0]*signum;
		norm[1]=R[4]*signum;
		norm[2]=R[8]*signum;
		}
	break;

	case 1:	
		{
		norm[0]=R[1]*signum;
		norm[1]=R[5]*signum;
		norm[2]=R[9]*signum;
		}
	break;
	case 2:
		{
		norm[0]=R[2]*signum;
		norm[1]=R[6]*signum;
		norm[2]=R[10]*signum;
		}
	break;
	}
}
else {
	norm[0]=outAx[0]*signum;
	norm[1]=outAx[1]*signum;
	norm[2]=outAx[2]*signum;



/////////////
 
  dReal Q1 = -signum*dDOT14(outAx,R+0);
  dReal Q2 = -signum*dDOT14(outAx,R+1);
  dReal Q3 = -signum*dDOT14(outAx,R+2);
  dReal A1 = 2.f*hside[0] * Q1;
  dReal A2 = 2.f*hside[1] * Q2;
  dReal A3 = 2.f*hside[2] * Q3;
  pos[0]=p[0];
  pos[1]=p[1];
  pos[2]=p[2];

#define FOO(i,op) \
  pos[0] op hside[i] * R[0+i]; \
  pos[1] op hside[i] * R[4+i]; \
  pos[2] op hside[i] * R[8+i];
#define BAR(i,iinc) if (A ## iinc > 0) { FOO(i,-=) } else { FOO(i,+=) }
  BAR(0,1);
  BAR(1,2);
  BAR(2,3);
#undef FOO
#undef BAR
////////////////


switch((code-10)/3){

case 0:
			CrossProjLine1(v0,triSideAx0,pos,R+(code-10),pos);
			if(pos[0]==dInfinity){
									pos[0]=(v1[0]+v0[0])/2.f;
									pos[1]=(v1[1]+v0[1])/2.f;
									pos[2]=(v1[2]+v0[2])/2.f;
								}
break;

case 1:
			CrossProjLine1(v1,triSideAx1,pos,R+(code-13),pos);
			if(pos[0]==dInfinity){
									pos[0]=(v2[0]+v1[0])/2.f;
									pos[1]=(v2[1]+v1[1])/2.f;
									pos[2]=(v2[2]+v1[2])/2.f;
									}
break;

case 2:
			CrossProjLine1(v0,triSideAx2,pos,R+(code-16),pos);
			if(pos[0]==dInfinity){
									pos[0]=(v2[0]+v0[0])/2.f;
									pos[1]=(v2[1]+v0[1])/2.f;
									pos[2]=(v2[2]+v0[2])/2.f;
								}
}
}




//if(code!=0){
contact->pos[0] = pos[0];
contact->pos[1] = pos[1];
contact->pos[2] = pos[2];

contact->depth = outDepth;

//}
 for (u32 i=0; i<ret; i++) {
    CONTACT(contact,i*skip)->g1 = const_cast<dxGeom*> (o2);
    CONTACT(contact,i*skip)->g2 = const_cast<dxGeom*> (o1);
	CONTACT(contact,i*skip)->normal[0] = norm[0];
	CONTACT(contact,i*skip)->normal[1] = norm[1];
	CONTACT(contact,i*skip)->normal[2] = norm[2];
  }
  return ret;


 
}



//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
bool circleLineIntersection(const dReal* cn,const dReal* cp,dReal r,const dReal* lv,const dReal* lp,dReal sign,dVector3 point){

dVector3 LC={lp[0]-cp[0],lp[1]-cp[1],lp[2]-cp[2]};

dReal A,B,C,B_A,B_A_2,D;
dReal t1,t2;
A=dDOT(lv,lv);
B=dDOT(LC,lv);
C=dDOT(LC,LC)-r*r;
B_A=B/A;
B_A_2=B_A*B_A;
D=B_A_2-C;
if(D<0.f){
	
	point[0]=lp[0]-lv[0]*B;
	point[1]=lp[1]-lv[1]*B;
	point[2]=lp[2]-lv[2]*B;
	return false;
	
}
else{
t1=-B_A-sqrtf(D);
t2=-B_A+sqrtf(D);

dVector3 O1={lp[0]+lv[0]*t1,lp[1]+lv[1]*t1,lp[2]+lv[2]*t1};
dVector3 O2={lp[0]+lv[0]*t2,lp[1]+lv[1]*t2,lp[2]+lv[2]*t2};
//dVector3 test1={O1[0]-cp[0],O1[1]-cp[1],O1[2]-cp[2]};
//dVector3 test2={O2[0]-cp[0],O2[1]-cp[1],O2[2]-cp[2]};
//dReal t=sqrtf(dDOT(test1,test1));
//t=sqrtf(dDOT(test2,test2));

dReal cpPr=sign*dDOT(cn,cp);

dReal dist1=(sign*dDOT41(cn,O1)-cpPr);
dReal dist2=(sign*dDOT41(cn,O2)-cpPr);

if(dist1<dist2){
				point[0]=O1[0];
				point[1]=O1[1];
				point[2]=O1[2];
				}
else{			point[0]=O2[0];
				point[1]=O2[1];
				point[2]=O2[2];
				}
				

return true;
}
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

bool inline cylinderCrossesLine(const dReal* p,const dReal* R,dReal hlz,
						 const dReal* v0,const dReal* v1,const dReal* l,dVector3 pos){
	dReal cos=dDOT14(l,R);

	if(cos>=1.f) return false;
	
	dReal sin2=1.f-cos*cos;

	dVector3 vp={v0[0]-p[0],v0[1]-p[1],v0[2]-p[2]};
    dReal c1=dDOT(vp,l);
	dReal c2=dDOT14(vp,R);

	dReal t=(c2*cos-c1)/sin2;
	dReal q=(c2-c1*cos)/sin2;

	if(dFabs(q)>hlz) return false;

	dVector3 v01={v1[0]-v0[0],v1[1]-v0[1],v1[2]-v0[2]};
	dReal sidelength2=dDOT(v01,v01);

	if(t*t>sidelength2) return false;
	
	pos[0]=v0[0]+l[0]*t;
	pos[1]=v0[1]+l[1]*t;
	pos[2]=v0[2]+l[2]*t;

	return true;
	

}
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

extern "C" int dTriCyl (
						const dReal* v0,const dReal* v1,const dReal* v2,
						const dxGeom *o1, const dxGeom *o2,
						int flags, dContactGeom *contact, int skip
						

						)
{

 // dIASSERT (skip >= (int)sizeof(dContactGeom));
  dIASSERT (o1->_class->num == dCylinderClass);
  

  dxCylinder *cylinder = (dxCylinder*) CLASSDATA(o1);
  
  const dReal *R = o1->R;
  const dReal* p=o1->pos;
  const dReal radius=cylinder->radius;
  const dReal hlz=cylinder->lz/2.f;

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
  dReal signum, outDepth,cos0,cos1,cos2,sin1;
////////////////////////////////////////////////////////////////////////////
//sepparation along tri plane normal;///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
dNormalize3(triAx);

//cos0=dDOT14(triAx,R+0);
cos1=dFabs(dDOT14(triAx,R+1));
//cos2=dDOT14(triAx,R+2);

//sin1=sqrtf(cos0*cos0+cos2*cos2);

////////////////////////
//another way //////////
cos1=cos1<REAL(1.) ? cos1 : REAL(1.); //cos1 may slightly exeed 1.f
sin1=sqrtf(REAL(1.)-cos1*cos1);
//////////////////////////////

dReal sidePr=cos1*hlz+sin1*radius;

dReal dist=dDOT(triAx,v0)-dDOT(triAx,p);
if(dist>0.f) return 0;
dReal depth=sidePr-dFabs(dist);
outDepth=depth;
signum=dist>0.f ? 1.f : -1.f;

code=0;
if(depth<0.f) return 0;

dReal depth0,depth1,depth2,dist0,dist1,dist2;
bool isPdist0,isPdist1,isPdist2;
bool testV0,testV1,testV2;
bool sideTestV00,sideTestV01,sideTestV02;
bool sideTestV10,sideTestV11,sideTestV12;
bool sideTestV20,sideTestV21,sideTestV22;


//////////////////////////////////////////////////////////////////////////////
//cylinder axis - one of the triangle vertexes touches cylinder's flat surface
//////////////////////////////////////////////////////////////////////////////
dist0=dDOT14(v0,R+1)-dDOT14(p,R+1);
dist1=dDOT14(v1,R+1)-dDOT14(p,R+1);
dist2=dDOT14(v2,R+1)-dDOT14(p,R+1);

isPdist0=dist0>0.f;
isPdist1=dist1>0.f;
isPdist2=dist2>0.f;

depth0=hlz-dFabs(dist0);
depth1=hlz-dFabs(dist1);
depth2=hlz-dFabs(dist2);

testV0=depth0>0.f;
testV1=depth1>0.f;
testV2=depth2>0.f;

if(isPdist0==isPdist1 && isPdist1== isPdist2) //(here and lower) check the tryangle is on one side of the cylinder
   
{
if(depth0>depth1) 
		if(depth0>depth2) 	
			if(testV0){
				if(depth0<outDepth) 
					{
					signum= isPdist0 ? 1.f : -1.f;
					outDepth=depth0;			
					code=1;
					}
				}
			else 
				return 0;
		else
			if(testV2){
				if(depth2<outDepth) 
					{
					outDepth=depth2;
					signum= isPdist2 ? 1.f : -1.f;
					code=3;
					}
			}
			else 
				return 0;
else
		if(depth1>depth2)
			if(testV1){
				if(depth1<outDepth) 
					{
					outDepth=depth1;
					signum= isPdist1 ? 1.f : -1.f;
					code=2;
					}
			}
			else 
				return 0;

		else
			if(testV2){
				if(depth2<outDepth) 
					{
					outDepth=depth2;
					signum= isPdist2 ? 1.f : -1.f;
					code=2;
					}
			}
			else return 0;
}


dVector3 axis,outAx;
dReal posProj;
dReal pointDepth=0.f;


#define TEST(vx,ox1,ox2,c)	\
	{\
	posProj=dDOT14(v##vx,R+1)-dDOT14(p,R+1);\
\
	axis[0]=v##vx[0]-p[0]-R[1]*posProj;\
	axis[1]=v##vx[1]-p[1]-R[5]*posProj;\
	axis[2]=v##vx[2]-p[2]-R[9]*posProj;\
\
	dNormalize3(axis);\
\
\
	dist0=dDOT(v0,axis)-dDOT(p,axis);\
	dist1=dDOT(v1,axis)-dDOT(p,axis);\
	dist2=dDOT(v2,axis)-dDOT(p,axis);\
\
	isPdist0=dist0>0.f;\
	isPdist1=dist1>0.f;\
	isPdist2=dist2>0.f;\
\
	depth0=radius-dFabs(dist0);\
	depth1=radius-dFabs(dist1);\
	depth2=radius-dFabs(dist2);\
\
	sideTestV##vx##0=depth0>0.f;\
	sideTestV##vx##1=depth1>0.f;\
	sideTestV##vx##2=depth2>0.f;\
\
	if(isPdist0==isPdist1 && isPdist1== isPdist2)\
\
	{\
	if(sideTestV##vx##0||sideTestV##vx##1||sideTestV##vx##2){\
	if(!(depth##vx<depth##ox1 || depth##vx<depth##ox2))\
					{\
						if(depth##vx<outDepth && depth##vx > pointDepth)\
							{\
							pointDepth=depth##vx;\
							signum= isPdist##vx ? 1.f : -1.f;\
							outAx[0]=axis[0];\
							outAx[1]=axis[1];\
							outAx[2]=axis[2];\
							code=c;\
							}\
					}\
	}\
	else return 0;\
			\
\
\
	}\
}

if(testV0) TEST(0,1,2,4)
if(testV1 ) TEST(1,2,0,5)
//&& sideTestV01
if(testV2 ) TEST(2,0,1,6)
//&& sideTestV02 && sideTestV12
#undef TEST

dVector3 tpos,pos;
if(code>3) outDepth=pointDepth; //deepest vertex axis used if its depth less than outDepth
//else{
//bool outV0=!(testV0&&sideTestV00&&sideTestV10&&sideTestV20);
//bool outV1=!(testV1&&sideTestV01&&sideTestV11&&sideTestV21);
//bool outV2=!(testV2&&sideTestV02&&sideTestV12&&sideTestV22);
bool outV0=true;
bool outV1=true;
bool outV2=true;
/////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
///crosses between triangle sides and cylinder axis//////////////////////////
/////////////////////////////////////////////////////////////////////////////
#define TEST(ax,nx,ox,c)	if(cylinderCrossesLine(p,R+1,hlz,v##ax,v##nx,triSideAx##ax,tpos))	{\
	dCROSS114(axis,=,triSideAx##ax,R+1);\
	dNormalize3(axis);\
	dist##ax=dDOT(v##ax,axis)-dDOT(p,axis);\
	dist##ox=dDOT(v##ox,axis)-dDOT(p,axis);\
\
	isPdist##ax=dist##ax>0.f;\
	isPdist##ox=dist##ox>0.f;\
\
	if(isPdist##ax == isPdist##ox)\
{\
depth##ax=radius-dFabs(dist##ax);\
depth##ox=radius-dFabs(dist##ox);\
	\
			if(depth##ax>0.f){\
				if(depth##ax<outDepth && depth##ax>depth##ox) \
					{\
						outDepth=depth##ax;\
						signum= isPdist##ax ? 1.f : -1.f;\
						outAx[0]=axis[0];\
						outAx[1]=axis[1];\
						outAx[2]=axis[2];\
						pos[0]=tpos[0];\
						pos[1]=tpos[1];\
						pos[2]=tpos[2];\
						code=c;\
					}\
				}\
			else if(depth##ox<0.f) return 0;\
\
}\
}

dNormalize3(triSideAx0);
if(outV0&&outV1) 
TEST(0,1,2,7)

dNormalize3(triSideAx1);
if(outV1&&outV2) 
TEST(1,2,0,8)

dNormalize3(triSideAx2);
if(outV2&&outV0) 
TEST(2,0,1,9)
#undef TEST

////////////////////////////////////
//test cylinder rings on triangle sides////
////////////////////////////////////

dVector3 tAx,cen;
dReal sign;
bool cs;

#define TEST(ax,nx,ox,c)	\
{\
posProj=dDOT(p,triSideAx##ax)-dDOT(v##ax,triSideAx##ax);\
axis[0]=p[0]-v0[0]-triSideAx##ax[0]*posProj;\
axis[1]=p[1]-v0[1]-triSideAx##ax[1]*posProj;\
axis[2]=p[2]-v0[2]-triSideAx##ax[2]*posProj;\
	\
sign=dDOT14(axis,R+1)>0.f ? 1.f :-1.f;\
cen[0]=p[0]-sign*R[1]*hlz;\
cen[1]=p[1]-sign*R[5]*hlz;\
cen[2]=p[2]-sign*R[9]*hlz;\
\
cs=circleLineIntersection(R+1,cen,radius,triSideAx##ax,v##ax,-sign,tpos);\
\
axis[0]=tpos[0]-cen[0];\
axis[1]=tpos[1]-cen[1];\
axis[2]=tpos[2]-cen[2];\
\
if(cs){ \
\
cos0=dDOT14(axis,R+0);\
cos2=dDOT14(axis,R+2);\
tAx[0]=R[2]*cos0-R[0]*cos2;\
tAx[1]=R[6]*cos0-R[4]*cos2;\
tAx[2]=R[10]*cos0-R[8]*cos2;\
\
dCROSS(axis,=,triSideAx##ax,tAx);\
\
}\
dNormalize3(axis);\
dist##ax=dDOT(v##ax,axis)-dDOT(p,axis);\
if(dist##ax*dDOT(axis,triSideAx##nx)>0.f){\
\
cos0=dDOT14(axis,R+0);\
cos1=dFabs(dDOT14(axis,R+1));\
cos2=dDOT14(axis,R+2);\
\
\
sin1=sqrtf(cos0*cos0+cos2*cos2);\
\
sidePr=cos1*hlz+sin1*radius;\
\
\
	dist##ox=dDOT(v##ox,axis)-dDOT(p,axis);\
\
	isPdist##ax=dist##ax>0.f;\
	isPdist##ox=dist##ox>0.f;\
\
	if(isPdist##ax == isPdist##ox) \
\
{\
depth##ax=sidePr-dFabs(dist##ax);\
depth##ox=sidePr-dFabs(dist##ox);\
	\
			if(depth##ax>0.f){\
				if(depth##ax<outDepth) \
					{\
						outDepth=depth##ax;\
						signum= isPdist##ax ? 1.f : -1.f;\
						outAx[0]=axis[0];\
						outAx[1]=axis[1];\
						outAx[2]=axis[2];\
						pos[0]=tpos[0];\
						pos[1]=tpos[1];\
						pos[2]=tpos[2];\
						code=c;\
					}\
				}\
			else if(depth##ox<0.f) return 0;\
\
\
}\
}\
}

if(code!=7)
TEST(0,1,2,10)

if(code!=8)
TEST(1,2,0,11)

if(code!=9)
TEST(2,0,1,12)

#undef TEST

//}
//////////////////////////////////////////////////////////////////////
///if we get to this poit tri touches cylinder///////////////////////
/////////////////////////////////////////////////////////////////////
dVector3 norm;
unsigned int ret;

if(code==0){
	norm[0]=triAx[0]*signum;
	norm[1]=triAx[1]*signum;
	norm[2]=triAx[2]*signum;


  dReal Q1 = -signum*dDOT14(triAx,R+0);
  dReal Q2 = -signum*dDOT14(triAx,R+1);
  dReal Q3 = -signum*dDOT14(triAx,R+2);
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
  
  ret=0;
  dVector3 cross0, cross1, cross2;
  dReal ds0,ds1,ds2;
  
  dCROSS(cross0,=,triAx,triSideAx0);
  ds0=dDOT(cross0,v0);

  dCROSS(cross1,=,triAx,triSideAx1);
  ds1=dDOT(cross1,v1);

  dCROSS(cross2,=,triAx,triSideAx2);
  ds2=dDOT(cross2,v2);

  if(dDOT(cross0,pos)-ds0>0.f && 
	 dDOT(cross1,pos)-ds1>0.f && 
	 dDOT(cross2,pos)-ds2>0.f){
	  						   contact->pos[0] = pos[0];
							   contact->pos[1] = pos[1];
							   contact->pos[2] = pos[2];
							   contact->depth = outDepth;
							   ret=1;
  }
if(dFabs(Q2)>M_SQRT1_2){

  CONTACT(contact,ret*skip)->pos[0]=pos[0]+2.f*A1*R[0];
  CONTACT(contact,ret*skip)->pos[1]=pos[1]+2.f*A1*R[4];
  CONTACT(contact,ret*skip)->pos[2]=pos[2]+2.f*A1*R[8];
  CONTACT(contact,ret*skip)->depth=outDepth-dFabs(Q1*2.f*A1);

  if(CONTACT(contact,ret*skip)->depth>0.f)
    if(dDOT(cross0,CONTACT(contact,ret*skip)->pos)-ds0>0.f && 
	   dDOT(cross1,CONTACT(contact,ret*skip)->pos)-ds1>0.f && 
	   dDOT(cross2,CONTACT(contact,ret*skip)->pos)-ds2>0.f) ret++;
  
  
  CONTACT(contact,ret*skip)->pos[0]=pos[0]+2.f*A3*R[2];
  CONTACT(contact,ret*skip)->pos[1]=pos[1]+2.f*A3*R[6];
  CONTACT(contact,ret*skip)->pos[2]=pos[2]+2.f*A3*R[10];
  CONTACT(contact,ret*skip)->depth=outDepth-dFabs(Q3*2.f*A3);

  if(CONTACT(contact,ret*skip)->depth>0.f)
    if(dDOT(cross0,CONTACT(contact,ret*skip)->pos)-ds0>0.f && 
	   dDOT(cross1,CONTACT(contact,ret*skip)->pos)-ds1>0.f && 
	   dDOT(cross2,CONTACT(contact,ret*skip)->pos)-ds2>0.f) ret++;
} else {

  CONTACT(contact,ret*skip)->pos[0]=pos[0]+2.f*(A2>0 ? hlz*R[1]:-hlz*R[1]);
  CONTACT(contact,ret*skip)->pos[1]=pos[1]+2.f*(A2>0 ? hlz*R[5]:-hlz*R[5]);
  CONTACT(contact,ret*skip)->pos[2]=pos[2]+2.f*(A2>0 ? hlz*R[9]:-hlz*R[9]);
  CONTACT(contact,ret*skip)->depth=outDepth-dFabs(Q2*2.f*A2);

  if(CONTACT(contact,ret*skip)->depth>0.f)
    if(dDOT(cross0,CONTACT(contact,ret*skip)->pos)-ds0>0.f && 
	   dDOT(cross1,CONTACT(contact,ret*skip)->pos)-ds1>0.f && 
	   dDOT(cross2,CONTACT(contact,ret*skip)->pos)-ds2>0.f) ret++;
}
	}
else if(code<7)//1-6
	{
	ret=1;
	contact->depth = outDepth;
	switch((code-1)%3){
	case 0:
	contact->pos[0]=v0[0];
	contact->pos[1]=v0[1];
	contact->pos[2]=v0[2];
	break;
	case 1:
	contact->pos[0]=v1[0];
	contact->pos[1]=v1[1];
	contact->pos[2]=v1[2];
	break;
	case 2:
	contact->pos[0]=v2[0];
	contact->pos[1]=v2[1];
	contact->pos[2]=v2[2];
	break;
	}

if(code<4){//1-3
	norm[0]=R[1]*signum;
	norm[1]=R[5]*signum;
	norm[2]=R[9]*signum;
	}
else {
	norm[0]=outAx[0]*signum;
	norm[1]=outAx[1]*signum;
	norm[2]=outAx[2]*signum;
	}
}

else {//7-12
	ret=1;
	contact->depth = outDepth;
	norm[0]=outAx[0]*signum;
	norm[1]=outAx[1]*signum;
	norm[2]=outAx[2]*signum;
	contact->pos[0] = pos[0];
	contact->pos[1] = pos[1];
	contact->pos[2] = pos[2];	
}

if(ret>maxc) ret=maxc;

 for (unsigned int i=0; i<ret; i++) {
    CONTACT(contact,i*skip)->g1 = const_cast<dxGeom*> (o2);
    CONTACT(contact,i*skip)->g2 = const_cast<dxGeom*> (o1);
	CONTACT(contact,i*skip)->normal[0] = norm[0];
	CONTACT(contact,i*skip)->normal[1] = norm[1];
	CONTACT(contact,i*skip)->normal[2] = norm[2];
  }
  return ret;  
}