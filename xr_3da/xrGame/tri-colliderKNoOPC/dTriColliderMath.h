#ifndef D_TRI_COLLIDER_MATH_H
#define D_TRI_COLLIDER_MATH_H

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

	//dReal a1 =dDOT(cross0,pos)-ds0;
	//dReal a2 =dDOT(cross1,pos)-ds1;
	//dReal a3 =dDOT(cross2,pos)-ds2;

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


inline bool  TriContainPoint(const dReal* v0,const dReal* v1,const dReal* v2,
							 const dReal* triSideAx0,const dReal* triSideAx1,const dReal* triSideAx2,
							 const dReal* triAx, const dReal* pos){
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
							
							 inline bool TriPlaneContainPoint(Triangle* T)
							 {
								 return T->dist>0.f;
							 }

							 inline void PlanePoint(const Triangle& tri,const dReal* from,const dReal* to,float from_dist,dReal* point)
							 {
								 dVector3	dir		=	{to[0]-from[0],to[1]-from[1],to[2]-from[2]}	;
								 //dReal		mag	=	dDOT(dir,dir)									;
								 //if(mag<EPS_S)	
								 //{
									// //in the case when "from" & "to" are same the call suppose they lies on the plane
									// point[0]=from[0]												;	
									// point[1]=from[1]												;	
									// point[2]=from[2]												;
									// return;
								 //}
								// mag	=	dSqrt(mag)												;
								// float rmag	=	1.f/rmag											;

								 //dir[0]*=rmag;	dir[1]*=rmag;	dir[2]*=rmag						;

								 dReal		cosinus	=	(tri.dist-from_dist)							;
								 VERIFY2(cosinus<0.f,"wrong positions")								;
								 dReal mul=(from_dist)/cosinus								;
								 dir[0]*=mul;	dir[1]*=mul;	dir[2]*=mul							;
								 point[0]=from[0]-dir[0]												;	
								 point[1]=from[1]-dir[1]												;	
								 point[2]=from[2]-dir[2]												;

							 }
 #endif