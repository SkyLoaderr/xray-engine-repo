



#ifndef DTROCOLLIDERH
#define DTROCOLLIDERH

extern "C" int dTriBox (
						const dReal* v0,const dReal* v1,const dReal* v2,
						dxGeom *o1,dxGeom *o2,
						int flags, dContactGeom *contact, int skip
						);
extern "C" int dSortedTriBox (
						const dReal* triSideAx0,const dReal* triSideAx1,
						const dReal* triAx,
						const dReal* v0,
						const dReal* v1,
						const dReal* v2,
						dReal dist,
						dxGeom *o1, dxGeom *o2,
						int flags, dContactGeom *contact, int skip
						);

extern "C" int dTriCyl (
						const dReal* v0,const dReal* v1,const dReal* v2,
						dxGeom *o1, dxGeom *o2,
						int flags, dContactGeom *contact, int skip
						

						);

struct dxBox {
  dVector3 side;	// side lengths (x,y,z)
};

struct dxCylinder {	// cylinder
  dReal radius,lz;	// radius, length along z axis */
};

struct KLine {
	dVector3 p1;
	dVector3 p2;
};


inline void CrossProjLine(const dReal* pt1,const dReal* vc1,const dReal* pt2,const dReal* vc2,dReal* proj){
	dVector3 ac={pt1[0]-pt2[0],pt1[1]-pt2[1],pt1[2]-pt2[2]};
	dReal factor=(dDOT(vc2,vc2)*dDOT44(vc1,vc1)-dDOT14(vc2,vc1)*dDOT14(vc2,vc1));
	if(factor==0.f){
	proj[0]=dInfinity;
	//proj[1]=dInfinity;
	//proj[2]=dInfinity;
	return;
	}
	dReal t=(dDOT(ac,vc2)*dDOT41(vc1,vc2)-dDOT41(vc1,ac)*dDOT(vc2,vc2))
			/
			factor;

	proj[0]=pt1[0]+vc1[0]*t;
	proj[1]=pt1[1]+vc1[4]*t;
	proj[2]=pt1[2]+vc1[8]*t;


}

inline void CrossProjLine1(const dReal* pt1,const dReal* vc1,const dReal* pt2,const dReal* vc2,dReal* proj){
	dVector3 ac={pt1[0]-pt2[0],pt1[1]-pt2[1],pt1[2]-pt2[2]};
	dReal factor=(dDOT44(vc2,vc2)*dDOT(vc1,vc1)-dDOT41(vc2,vc1)*dDOT41(vc2,vc1));
	if(factor==0.f){
	proj[0]=dInfinity;
	//proj[1]=dInfinity;
	//proj[2]=dInfinity;
	return;
	}
	dReal t=(dDOT14(ac,vc2)*dDOT14(vc1,vc2)-dDOT(vc1,ac)*dDOT44(vc2,vc2))
			/
			factor;

	proj[0]=pt1[0]+vc1[0]*t;
	proj[1]=pt1[1]+vc1[1]*t;
	proj[2]=pt1[2]+vc1[2]*t;


}


//is point in Box
inline bool IsPtInBx(const dReal* Pt,const dReal* BxP,const dReal* BxEx,const dReal* BxR){
dVector3 BxPR,PtR;

dMULTIPLY1_331 (BxPR,BxR,BxP);
dMULTIPLY1_331 (PtR,BxR,Pt);
return

dFabs(BxPR[0]-PtR[0])<BxEx[0]/2&&
dFabs(BxPR[1]-PtR[1])<BxEx[1]/2&&
dFabs(BxPR[2]-PtR[2])<BxEx[2]/2;
}

inline dReal FragmentonSphereTest(const dReal* center, const dReal radius,
						   const dReal* pt1, const dReal* pt2,dVector3 norm)
{
	dVector3 direction={pt2[0]-pt1[0],pt2[1]-pt1[1],pt2[2]-pt1[2]};
	dMatrix3 R;
	dVector3 centerR,pt1R,pt2R;
	if(!(direction[1]==0.f && direction[2]==0.f))
		dRFrom2Axes(R,direction[0],direction[1],direction[2],0,-direction[2],direction[1]);
	else
		dRFrom2Axes(R,direction[0],direction[1],direction[2],direction[2],0,-direction[0]);

	dMULTIPLY1_331 (centerR,R,center);
	dMULTIPLY1_331 (pt1R,R,pt1);
	dMULTIPLY1_331 (pt2R,R,pt2);

	if((centerR[0]-pt1R[0])*(centerR[0]-pt2R[0])>0.f) return -1;

	dReal depth=-sqrtf(
				 (pt1R[1]-centerR[1])*(pt1R[1]-centerR[1])+
				 (pt1R[2]-centerR[2])*(pt1R[2]-centerR[2])
				 )
				+
				radius;
	if(depth<0.f) return -1;

	
	dVector3 normR={0,centerR[1]-pt1R[1],centerR[2]-pt1R[2]};
	dMULTIPLY0_331(norm,R,normR);
	return depth;
	
	
	
	}


inline dReal PointSphereTest(const dReal* center, const dReal radius,
						   const dReal* pt, dVector3 norm)
{

	dReal depth=-sqrtf(
				 (pt[0]-center[0])*(pt[0]-center[0])+
				 (pt[1]-center[1])*(pt[1]-center[1])+
				 (pt[2]-center[2])*(pt[2]-center[2])
				 )
				+
				radius;
	
	if(depth>0.f){
		norm[0]=center[0]-pt[0];
		norm[1]=center[1]-pt[1];
		norm[2]=center[2]-pt[2];
		return depth;
	}
	else return -1.f;
	
	
	}


inline dReal PointBoxTest(const dReal* Pt,const dReal* BxP,const dReal* BxEx,const dReal* R,dReal* norm){
dVector3 BxPR,PtR; 
dVector3 normR={0.f,0.f,0.f};

dMULTIPLY1_331 (BxPR,R,BxP);
dMULTIPLY1_331 (PtR,R,Pt);
dReal depth0,depth1,depth2;

depth0=-dFabs(BxPR[0]-PtR[0])+BxEx[0]/2;
if(depth0<0) return -1.f;

depth1=-dFabs(BxPR[1]-PtR[1])+BxEx[1]/2;
if(depth1<0) return -1.f;

depth2=-dFabs(BxPR[2]-PtR[2])+BxEx[2]/2;
if(depth2<0) return -1;
		
	

if(depth0<depth1){

		if(depth0<depth2)	{
							normR[0]=PtR[0]-BxPR[0];
							dMULTIPLY0_331(norm,R,normR);
							return depth0;
							}

		else				{
							normR[2]=PtR[2]-BxPR[2];
							dMULTIPLY0_331(norm,R,normR);
							return depth2;
							}
					}
else{

		if(depth1<depth2)	{ 
							normR[1]=PtR[1]-BxPR[1];
							dMULTIPLY0_331(norm,R,normR);
							return depth1;
							}
		else				{
							normR[2]=PtR[2]-BxPR[2];
							dMULTIPLY0_331(norm,R,normR);
							return   depth2;
							}
	}

}

inline dReal FragmentonBoxTest(const dReal* Pt1,const dReal* Pt2,const dReal* BxP,const dReal* BxEx,const dReal* R,dReal* norm,dReal* pos){

dVector3 fragmentonAx={Pt2[0]-Pt1[0],Pt2[1]-Pt1[1],Pt2[2]-Pt1[2]};
dReal BxExPr;
dNormalize3(fragmentonAx);
dReal BxPPr=dDOT(fragmentonAx,BxP);
BxExPr=
	dFabs(dDOT14(fragmentonAx,R+0)*BxEx[0])+
	dFabs(dDOT14(fragmentonAx,R+1)*BxEx[1])+
	dFabs(dDOT14(fragmentonAx,R+2)*BxEx[2]);

if(
    (dDOT(fragmentonAx,Pt1)-BxPPr-BxExPr/2.f)*
	(dDOT(fragmentonAx,Pt2)-BxPPr-BxExPr/2.f)>0.f
	&&
    (dDOT(fragmentonAx,Pt1)-BxPPr+BxExPr/2.f)*
	(dDOT(fragmentonAx,Pt2)-BxPPr+BxExPr/2.f)>0.f	
	) return -1.f;


dVector3 crossAx0;
dCROSS114(crossAx0,=,fragmentonAx,R+0);
dNormalize3(crossAx0);	
BxExPr=
	dFabs(dDOT14(crossAx0,R+0)*BxEx[0])+
	dFabs(dDOT14(crossAx0,R+1)*BxEx[1])+
	dFabs(dDOT14(crossAx0,R+2)*BxEx[2]);
dReal distance0=dDOT(crossAx0,Pt1)-dDOT(crossAx0,BxP);
if(dFabs(distance0)>BxExPr/2.f) return -1.f;
dReal depth0=BxExPr/2.f-dFabs(distance0);

dVector3 crossAx1;
dCROSS114(crossAx1,=,fragmentonAx,R+1);
dNormalize3(crossAx1);
BxExPr=
	dFabs(dDOT14(crossAx1,R+0)*BxEx[0])+
	dFabs(dDOT14(crossAx1,R+1)*BxEx[1])+
	dFabs(dDOT14(crossAx1,R+2)*BxEx[2]);
dReal distance1=dDOT(crossAx1,Pt1)-dDOT(crossAx1,BxP);
if(dFabs(distance1)>BxExPr/2.f) return -1.f;
dReal depth1=BxExPr/2.f-dFabs(distance1);

dVector3 crossAx2;
dCROSS114(crossAx2,=,fragmentonAx,R+2);
dNormalize3(crossAx2);
BxExPr=
	dFabs(dDOT14(crossAx2,R+0)*BxEx[0])+
	dFabs(dDOT14(crossAx2,R+1)*BxEx[1])+
	dFabs(dDOT14(crossAx2,R+2)*BxEx[2]);
dReal distance2=dDOT(crossAx2,Pt1)-dDOT(crossAx2,BxP);
if(dFabs(distance2)>BxExPr/2.f) return -1.f;
dReal depth2=BxExPr/2.f-dFabs(distance2);


if(depth0<depth1){

		if(depth0<depth2)						{
												norm[0]=distance0*crossAx0[0];
												norm[1]=distance0*crossAx0[1];
												norm[2]=distance0*crossAx0[2];
												CrossProjLine(BxP,R+0,Pt1,fragmentonAx,pos);
												pos[0]+=norm[0];
												pos[1]+=norm[1];
												pos[2]+=norm[2];
												return depth0;
												}

		else									{

												norm[0]=distance2*crossAx2[0];
												norm[1]=distance2*crossAx2[1];
												norm[2]=distance2*crossAx2[2];
												CrossProjLine(BxP,R+2,Pt1,fragmentonAx,pos);
												pos[0]+=norm[0];
												pos[1]+=norm[1];
												pos[2]+=norm[2];
												return depth2;
												}
				}


else{

		if(depth1<depth2)						{ 
												norm[0]=distance1*crossAx1[0];
												norm[1]=distance1*crossAx1[1];
												norm[2]=distance1*crossAx1[2];
												CrossProjLine(BxP,R+1,Pt1,fragmentonAx,pos);
												pos[0]+=norm[0];
												pos[1]+=norm[1];
												pos[2]+=norm[2];
												return depth1;
												}
		else 									{
												norm[0]=distance2*crossAx2[0];
												norm[1]=distance2*crossAx2[1];
												norm[2]=distance2*crossAx2[2];
												CrossProjLine(BxP,R+2,Pt1,fragmentonAx,pos);
												pos[0]+=norm[0];
												pos[1]+=norm[1];
												pos[2]+=norm[2];
												return depth2;
												}
	}

}


inline bool  TriContainPoint(const dReal* v0,const dReal* v1,const dReal* v2,const dReal* triAx,const dReal* triSideAx0,const dReal* triSideAx1, const dReal* pos){
	dVector3 cross0, cross1, cross2;
  dReal ds0,ds1,ds2;

  dVector3 triSideAx2={v0[0]-v2[0],v0[1]-v2[1],v0[2]-v2[2]};

  dCROSS(cross0,=,triAx,triSideAx0);
  ds0=dDOT(cross0,v0);

  dCROSS(cross1,=,triAx,triSideAx1);
  ds1=dDOT(cross1,v1);

  dCROSS(cross2,=,triAx,triSideAx2);
  ds2=dDOT(cross2,v2);

  if(dDOT(cross0,pos)-ds0>0.f && 
	 dDOT(cross1,pos)-ds1>0.f && 
	 dDOT(cross2,pos)-ds2>0.f) return true;
  else return false;
  

}


inline bool  TriContainPoint(const dReal* v0,const dReal* v1,const dReal* v2, const dReal* pos){
  dVector3 cross0, cross1, cross2;
  dReal ds0,ds1,ds2;

  
  dVector3 triSideAx0={v1[0]-v0[0],v1[1]-v0[1],v1[2]-v0[2]};
  dVector3 triSideAx1={v2[0]-v1[0],v2[1]-v1[1],v2[2]-v1[2]};
  dVector3 triSideAx2={v0[0]-v2[0],v0[1]-v2[1],v0[2]-v2[2]};

  dVector3 triAx;
  dCROSS(triAx,=,triSideAx0,triSideAx1);

  dCROSS(cross0,=,triAx,triSideAx0);
  ds0=dDOT(cross0,v0);

  dCROSS(cross1,=,triAx,triSideAx1);
  ds1=dDOT(cross1,v1);

  dCROSS(cross2,=,triAx,triSideAx2);
  ds2=dDOT(cross2,v2);

  if(dDOT(cross0,pos)-ds0>0.f && 
	 dDOT(cross1,pos)-ds1>0.f && 
	 dDOT(cross2,pos)-ds2>0.f) return true;
  else return false;
  

}

inline bool  TriPlaneContainPoint(const dReal* v0,const dReal* v1,const dReal* v2, const dReal* pos){
  
 
  dVector3 triSideAx0={v1[0]-v0[0],v1[1]-v0[1],v1[2]-v0[2]};
  dVector3 triSideAx1={v2[0]-v1[0],v2[1]-v1[1],v2[2]-v1[2]};
  
  dVector3 triAx;
  dCROSS(triAx,=,triSideAx0,triSideAx1);
  

  if(dDOT(triAx,pos)-dDOT(triAx,v0)>0.f) return true;
  else									 return false;
  

}

inline bool  TriPlaneContainPoint(const dReal* triAx,const dReal* v0, const dReal* pos){
  

  if(dDOT(triAx,pos)-dDOT(triAx,v0)>0.f) return true;
  else									 return false;
  

}


/*
inline dReal FragmentonBoxTest1(const dReal* Pt1,const dReal* Pt2,const dReal* BxP,const dReal* BxEx,const dReal* R,dReal* norm){

dVector3 fragmentonAx={Pt2[0]-Pt1[0],Pt2[1]-Pt1[1],Pt2[2]-Pt1[2]};
dVector3 currentAx;
dReal* axis;
dReal currentDepth,depth;
dReal currentDistance,distanse;
dReal BxExPr;
dNormalize3(fragmentonAx);
dReal BxPPr=dDOT(fragmentonAx,BxP);

BxExPr=
	dFabs(dDOT14(fragmentonAx,R+0)*BxEx[0])+
	dFabs(dDOT14(fragmentonAx,R+1)*BxEx[1])+
	dFabs(dDOT14(fragmentonAx,R+2)*BxEx[2]);

currentDistance=dDOT(fragmentonAx,Pt1)-BxPPr;
currentDepth=BxExPr-dFabs(currentDistance);

distance=dDOT(fragmentonAx,Pt2)-BxPPr;
depth=BxExPr-dFabs(distance);

if(
    (currentDistance-BxExPr/2.f)*
	(distance-BxExPr/2.f)>0.f
	&&
    (currentDistance+BxExPr/2.f)*
	(distance+BxExPr/2.f)>0.f	
	) return -1.f;

if(depth<0.f&&currentDepth>=0.f) {
		depth=currentDepth;
		distance=currentDistance;
		}
else 
dVector3 crossAx0;
dCROSS114(crossAx0,=,fragmentonAx,R+0);
dNormalize3(crossAx0);	
BxExPr=
	dFabs(dDOT14(crossAx0,R+0)*BxEx[0])+
	dFabs(dDOT14(crossAx0,R+1)*BxEx[1])+
	dFabs(dDOT14(crossAx0,R+2)*BxEx[2]);
dReal distance0=dDOT(crossAx0,Pt1)-dDOT(crossAx0,BxP);
if(dFabs(distance0)>BxExPr/2.f) return -1.f;
dReal depth0=BxExPr/2.f-dFabs(distance0);

dVector3 crossAx1;
dCROSS114(crossAx1,=,fragmentonAx,R+1);
dNormalize3(crossAx1);
BxExPr=
	dFabs(dDOT14(crossAx1,R+0)*BxEx[0])+
	dFabs(dDOT14(crossAx1,R+1)*BxEx[1])+
	dFabs(dDOT14(crossAx1,R+2)*BxEx[2]);
dReal distance1=dDOT(crossAx1,Pt1)-dDOT(crossAx1,BxP);
if(dFabs(distance1)>BxExPr/2.f) return -1.f;
dReal depth1=BxExPr/2.f-dFabs(distance1);

dVector3 crossAx2;
dCROSS114(crossAx2,=,fragmentonAx,R+2);
dNormalize3(crossAx2);
BxExPr=
	dFabs(dDOT14(crossAx2,R+0)*BxEx[0])+
	dFabs(dDOT14(crossAx2,R+1)*BxEx[1])+
	dFabs(dDOT14(crossAx2,R+2)*BxEx[2]);
dReal distance2=dDOT(crossAx2,Pt1)-dDOT(crossAx2,BxP);
if(dFabs(distance2)>BxExPr/2.f) return -1.f;
dReal depth2=BxExPr/2.f-dFabs(distance2);


if(depth0<depth1){

		if(depth0<depth2)						{
												norm[0]=distance0*crossAx0[0];
												norm[1]=distance0*crossAx0[1];
												norm[2]=distance0*crossAx0[2];
												return depth0;
												}

		else									{

												norm[0]=distance2*crossAx2[0];
												norm[1]=distance2*crossAx2[1];
												norm[2]=distance2*crossAx2[2];
												return depth2;
												}
				}


else{

		if(depth1<depth2)						{ 
												norm[0]=distance1*crossAx1[0];
												norm[1]=distance1*crossAx1[1];
												norm[2]=distance1*crossAx1[2];
												return depth1;
												}
		else 									{
												norm[0]=distance2*crossAx2[0];
												norm[1]=distance2*crossAx2[1];
												norm[2]=distance2*crossAx2[2];
												return depth2;
												}
	}

}
*/
#endif