#include "stdafx.h"
#include "dTriColliderCommon.h"
#include "dTriColliderMath.h"
#include "dTriSphere.h"
////////////////////////////////////////////////////////////////////////////
inline dReal PointSphereTest(const dReal* center, const dReal radius,
							 const dReal* pt,dReal* norm)
{

	norm[0]=center[0]-pt[0];
	norm[1]=center[1]-pt[1];
	norm[2]=center[2]-pt[2];
	dReal mag=dSqrt(dDOT(norm,norm));
	dReal depth=radius-mag;
	if(depth<0.f)	return -1.f;
	if(mag>0.f)
	{
		norm[0]/=mag;norm[1]/=mag;norm[2]/=mag;
	}
	else 
	{
		norm[0]=0;norm[1]=1;norm[2]=0;
	}
	return depth;

}


inline dReal FragmentonSphereTest(const dReal* center, const dReal radius,
								  const dReal* pt1, const dReal* pt2,dReal* norm)
{
	dVector3 direction={pt2[0]-pt1[0],pt2[1]-pt1[1],pt2[2]-pt1[2]};
	cast_fv(direction).normalize();
	dReal center_prg=dDOT(center,direction);
	dReal pt1_prg=dDOT(pt1,direction);
	dReal pt2_prg=dDOT(pt2,direction);
	dReal from1_dist=center_prg-pt1_prg;
	if(center_prg<pt1_prg||center_prg>pt2_prg) return -1;
	dVector3 line_to_center={
		-pt1[0]-direction[0]*from1_dist+center[0],
			-pt1[1]-direction[1]*from1_dist+center[1],
			-pt1[2]-direction[2]*from1_dist+center[2]
	};

	float mag=dSqrt(dDOT(line_to_center,line_to_center));
	//dNormalize3(norm);


	dReal depth=radius-mag;
	if(depth<0.f) return -1.f;
	if(mag>0.f)
	{
		norm[0]=line_to_center[0]/mag;
		norm[1]=line_to_center[1]/mag;
		norm[2]=line_to_center[2]/mag;
	}
	else
	{
		norm[0]=0;
		norm[1]=1;
		norm[2]=0;
	}
	return depth;
}


///////////////////////////////////////////////////////////////////////////////
inline bool FragmentonSphereTest(const dReal* center, const dReal radius,
								  const dReal* pt1, const dReal* pt2,dReal* norm,dReal& depth)
{
	dVector3 V={pt2[0]-pt1[0],pt2[1]-pt1[1],pt2[2]-pt1[2]};
	dVector3 L={pt1[0]-center[0],pt1[1]-center[1],pt1[2]-center[2]};
	dReal sq_mag_V=dDOT(V,V);
	dReal dot_L_V=dDOT(L,V);
	dReal dot_L_V_sq_mag_V=dot_L_V/sq_mag_V;
	dVector3 Pc={pt1[0]-dot_L_V_sq_mag_V*V[0],pt1[1]-dot_L_V_sq_mag_V*V[1],pt1[2]-dot_L_V_sq_mag_V*V[2]};
	dVector3 Dc={center[0]-Pc[0],center[1]-Pc[1],center[2]-Pc[2]};
	dReal sq_mag_Dc=dDOT(Dc,Dc);
	if(sq_mag_Dc>radius*radius)return false;
	dReal mag=dSqrt(sq_mag_Dc);
	depth=radius-mag;
	if(mag>0.f)
	{
		norm[0]=Dc[0]/mag;
		norm[1]=Dc[1]/mag;
		norm[2]=Dc[2]/mag;
	}
	else
	{
		norm[0]=0;
		norm[1]=1;
		norm[2]=0;
	}
	return true;
}

inline bool PointSphereTest(const dReal* center, const dReal radius,
							 const dReal* pt,dReal* norm,dReal &depth)
{

	norm[0]=center[0]-pt[0];
	norm[1]=center[1]-pt[1];
	norm[2]=center[2]-pt[2];
	dReal smag=dDOT(norm,norm);
	if(smag >radius*radius)return false;
	float mag=dSqrt(smag);
	depth=radius-mag;
	if(mag>0.f)
	{
		norm[0]/=mag;norm[1]/=mag;norm[2]/=mag;
	}
	else 
	{
		norm[0]=0;norm[1]=1;norm[2]=0;
	}
	return true;

}
/////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
int dSortedTriSphere(const dReal*	/**v1/**/,const dReal*	/**v2/**/,
					 const dReal* triAx,
					 CDB::TRI* T,
					 dReal dist,
					 dxGeom* Sphere,
					 dxGeom* Geometry, 
					 int Flags, 
					 dContactGeom* Contacts,
					 int			skip
					 ){


						 //const dReal* v1=(dReal*)T->verts[1];
						 //const dReal* v2=(dReal*)T->verts[2];
						 const dReal* SphereCenter=dGeomGetPosition(Sphere);
						 const float SphereRadius = dGeomSphereGetRadius(Sphere);



						 //	dNormalize3(triAx);
						 const dReal *ContactNormal=triAx;//{triAx[0],triAx[1],triAx[2]};
						 dVector3 ContactPos={SphereCenter[0]-triAx[0]* SphereRadius,SphereCenter[1]-triAx[1]* SphereRadius,SphereCenter[2]-triAx[2]* SphereRadius};

						 float ContactDepth= -dist + SphereRadius;
						 if (ContactDepth >= 0){

							 Contacts->normal[0] =-ContactNormal[0];
							 Contacts->normal[1] =-ContactNormal[1];
							 Contacts->normal[2] =-ContactNormal[2];
							 Contacts->depth = ContactDepth;
							 ////////////////////

							 Contacts->pos[0]=ContactPos[0];
							 Contacts->pos[1]=ContactPos[1];
							 Contacts->pos[2]=ContactPos[2];
							 Contacts->g1 = Geometry;
							 Contacts->g2 = Sphere;
							 ((dxGeomUserData*)dGeomGetData(Sphere))->tri_material=T->material;
							 if(dGeomGetUserData(Sphere)->callback)dGeomGetUserData(Sphere)->callback(T,Contacts);
							 SURFACE(Contacts,0)->mode=T->material;
							 //////////////////////////////////
							 return 1;

						 }

						 return 0;
					 }

					 int dTriSphere_o(const dReal* v0,const dReal* v1,const dReal* v2,
						 Triangle* T,
						 dxGeom* Sphere,dxGeom* Geometry, int Flags, 
						 dContactGeom* Contacts,int /**skip/**/)
					 {

						 if(!TriPlaneContainPoint(T)) return 0;//

						 //const dReal* v0	=(dReal*)&V_array[T->verts[0]];
						 //const dReal* v1	=(dReal*)&V_array[T->verts[1]];
						 //const dReal* v2	=(dReal*)&V_array[T->verts[2]];
						 //dVector3 triSideAx0={V[1][0]-V[0][0],V[1][1]-V[0][1],V[1][2]-V[0][2]};
						 //	dVector3 triSideAx1={V[2][0]-V[1][0],V[2][1]-V[1][1],V[2][2]-V[1][2]};
						 //		dVector3 triSideAx2={V[0][0]-V[2][0],V[0][1]-V[2][1],V[0][2]-V[2][2]};

						 //dVector3 triSideAx0={v1[0]-v0[0],v1[1]-v0[1],v1[2]-v0[2]};
						 //dVector3 triSideAx1={v2[0]-v1[0],v2[1]-v1[1],v2[2]-v1[2]};
						 //dVector3 triAx;
						 //dCROSS(triAx,=,triSideAx0,triSideAx1);

						
						const dVector3 &triSideAx0	=T->side0	;
						const dVector3 &triSideAx1	=T->side1	;
						const dVector3 &triAx		=T->norm	;

						 //if(!TriPlaneContainPoint(triAx,v0,SphereCenter)) return 0;

						 dVector3 norm;	
						 bool isLC=false;
						 bool isPC=false;
						 float Depth=0;
						 const dReal* pos=dGeomGetPosition(Sphere);
						 const dReal radius=dGeomSphereGetRadius(Sphere);

						 Depth=FragmentonSphereTest(pos,radius,v0,v1,norm);
						 isLC=Depth>0.f;
						 if(!isLC){	Depth=FragmentonSphereTest(pos,radius,v1,v2,norm);
									isLC=Depth>0.f;
								}
						 if(!isLC){ Depth=FragmentonSphereTest(pos,radius,v2,v0,norm);
									isLC=Depth>0.f;//.
							}
						 if(!isLC){	
								Depth=PointSphereTest(pos,radius,v0,norm);
								isPC=Depth>0.f;
								if(!isPC){	Depth=PointSphereTest(pos,radius,v1,norm);
											isPC=Depth>0.f;
									}
								if(!isPC){	Depth=PointSphereTest(pos,radius,v2,norm);
											isPC=Depth>0.f;
									}

							}
//						
						 dVector3 ContactNormal={triAx[0],triAx[1],triAx[2]};
						 dVector3 ContactPos={pos[0]-triAx[0]* radius,pos[1]-triAx[1]* radius,pos[2]-triAx[2]* radius};
						 float ContactDepth= -T->dist+radius; //dDOT(triAx,v0) - dDOT(SphereCenter,triAx) + SphereRadius;
						 if (ContactDepth >= 0){


							 bool contains=TriContainPoint(v0,v1,v2,triAx,triSideAx0,triSideAx1,ContactPos);
							 if(contains&&ContactDepth>Depth&&ContactDepth>0.f)
							 {
								 isLC=false;
								 isPC=false;
							 }
							 if(isLC||isPC){

								 ContactNormal[0]=norm[0];
								 ContactNormal[1]=norm[1];
								 ContactNormal[2]=norm[2];
								 ContactPos[0]=pos[0]-ContactNormal[0]*radius;
								 ContactPos[1]=pos[1]-ContactNormal[1]*radius;
								 ContactPos[2]=pos[2]-ContactNormal[2]*radius;
								 ContactDepth=Depth;
							 }


							 //dNormalize3(ContactNormal);


							 if (contains||isPC||isLC){

								 //	dContactGeom* Contact = CONTACT(Contacts, Skip);

								 Contacts->normal[0] =-ContactNormal[0];
								 Contacts->normal[1] =-ContactNormal[1];
								 Contacts->normal[2] =-ContactNormal[2];
								 Contacts->depth = ContactDepth;
								 ////////////////////
										
								 Contacts->pos[0]=ContactPos[0];
								 Contacts->pos[1]=ContactPos[1];
								 Contacts->pos[2]=ContactPos[2];
								 Contacts->g1 = Geometry;
								 Contacts->g2 = Sphere;
								 ((dxGeomUserData*)dGeomGetData(Sphere))->tri_material=T->T->material;
								 if(dGeomGetUserData(Sphere)->callback)dGeomGetUserData(Sphere)->callback(T->T,Contacts);
								 SURFACE(Contacts,0)->mode=T->T->material;
								 //////////////////////////////////
								 //	++OutTriCount;
								 return 1;


							 }

						 }

						 return 0;
					 }

 int dTriSphere(const dReal* v0,const dReal* v1,const dReal* v2,
	 Triangle* T,
	 dxGeom* Sphere,dxGeom* Geometry, int Flags, 
	 dContactGeom* Contacts,int /**skip/**/)
 {
 
 
	 const dVector3 &triSideAx0	=T->side0	;
	 const dVector3 &triSideAx1	=T->side1	;
	 const dVector3 &triAx		=T->norm	;

	 //if(!TriPlaneContainPoint(triAx,v0,SphereCenter)) return 0;


	 const dReal radius=dGeomSphereGetRadius(Sphere);
	 float Depth=-T->dist+radius;
	 if(Depth<0.f)	return 0;
	 const dReal* pos=dGeomGetPosition(Sphere);
	 dVector3 ContactNormal;
	 if(TriContainPoint(v0,v1,v2,triAx,triSideAx0,triSideAx1,pos))
	 {
		 ContactNormal[0]=triAx[0];ContactNormal[1]=triAx[1];ContactNormal[2]=triAx[2];
		 //dVector3 ContactPos={pos[0]-triAx[0]* radius,pos[1]-triAx[1]* radius,pos[2]-triAx[2]* radius};
	 }
	 else
	 {
		if(	!(
			FragmentonSphereTest(pos,radius,v0,v1,ContactNormal,Depth)	||
			FragmentonSphereTest(pos,radius,v1,v2,ContactNormal,Depth)	||
			FragmentonSphereTest(pos,radius,v2,v0,ContactNormal,Depth)	||
			PointSphereTest(pos,radius,v0,ContactNormal,Depth)			||
			PointSphereTest(pos,radius,v1,ContactNormal,Depth)			||
			PointSphereTest(pos,radius,v2,ContactNormal,Depth)
			)
			)	return 0;
	 }

	 Contacts->normal[0] =-ContactNormal[0];
	 Contacts->normal[1] =-ContactNormal[1];
	 Contacts->normal[2] =-ContactNormal[2];
	 Contacts->depth = Depth;
	 ////////////////////

	 Contacts->pos[0]=pos[0]-ContactNormal[0]*radius;
	 Contacts->pos[1]=pos[1]-ContactNormal[1]*radius;
	 Contacts->pos[2]=pos[2]-ContactNormal[2]*radius;
	 Contacts->g1 = Geometry;
	 Contacts->g2 = Sphere;
	 ((dxGeomUserData*)dGeomGetData(Sphere))->tri_material=T->T->material;
	 if(dGeomGetUserData(Sphere)->callback)dGeomGetUserData(Sphere)->callback(T->T,Contacts);
	 SURFACE(Contacts,0)->mode=T->T->material;
	 //////////////////////////////////
	 //	++OutTriCount;
	 return 1;
 }

 TRI_PRIMITIVE_COLIDE_CLASS_IMPLEMENT(Sphere)