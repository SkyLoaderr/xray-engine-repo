#include "stdafx.h"
#include "dTriColliderCommon.h"
#include "dTriColliderMath.h"
#include "dTriSphere.h"
////////////////////////////////////////////////////////////////////////////
float	dSphereProj(dxGeom* sphere,const dReal* /**normal/**/)
{
	dIASSERT (dGeomGetClass(sphere)== dSphereClass);
	return	dGeomSphereGetRadius(sphere);
}
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
						 dVector3 ContactNormal={triAx[0],triAx[1],triAx[2]};
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

					 int dTriSphere(const dReal* v0,const dReal* v1,const dReal* v2,
						 CDB::TRI* T,
						 dxGeom* Sphere,dxGeom* Geometry, int Flags, 
						 dContactGeom* Contacts,int /**skip/**/)
					 {

						 const dReal*	SphereCenter	=	dGeomGetPosition	(Sphere);
						 const float		SphereRadius	=	dGeomSphereGetRadius(Sphere);
						 //const dReal* v0	=(dReal*)&V_array[T->verts[0]];
						 //const dReal* v1	=(dReal*)&V_array[T->verts[1]];
						 //const dReal* v2	=(dReal*)&V_array[T->verts[2]];
						 //dVector3 triSideAx0={V[1][0]-V[0][0],V[1][1]-V[0][1],V[1][2]-V[0][2]};
						 //	dVector3 triSideAx1={V[2][0]-V[1][0],V[2][1]-V[1][1],V[2][2]-V[1][2]};
						 //		dVector3 triSideAx2={V[0][0]-V[2][0],V[0][1]-V[2][1],V[0][2]-V[2][2]};
						 dVector3 triSideAx0={v1[0]-v0[0],v1[1]-v0[1],v1[2]-v0[2]};
						 dVector3 triSideAx1={v2[0]-v1[0],v2[1]-v1[1],v2[2]-v1[2]};
						 dVector3 triAx;
						 dCROSS(triAx,=,triSideAx0,triSideAx1);

						 if(!TriPlaneContainPoint(triAx,v0,SphereCenter)) return 0;

						 dVector3 norm;	
						 bool isLC=false;
						 bool isPC=false;
						 float Depth=0;
						 /*
						 for(int i=0;i<3;++i){

						 Depth=FragmentonSphereTest(dGeomGetPosition(Sphere),
						 dGeomSphereGetRadius(Sphere),
						 V[i],V[(i+1)%3],norm);

						 if	(Depth>0.f) {

						 isLC=true;
						 break;
						 }
						 ;

						 }
						 */
						 Depth=FragmentonSphereTest(dGeomGetPosition(Sphere),
							 dGeomSphereGetRadius(Sphere),
							 v0,v1,norm);
						 isLC=Depth>0.f;
						 if(!isLC){ Depth=FragmentonSphereTest(dGeomGetPosition(Sphere),
							 dGeomSphereGetRadius(Sphere),
							 v1,v2,norm);
						 isLC=Depth>0.f;
						 }
						 if(!isLC){ Depth=FragmentonSphereTest(dGeomGetPosition(Sphere),
							 dGeomSphereGetRadius(Sphere),
							 v2,v0,norm);
						 }
						 /*
						 if(!isLC)
						 for(int i=0;i<3;++i){

						 Depth=PointSphereTest(dGeomGetPosition(Sphere),
						 dGeomSphereGetRadius(Sphere),
						 V[i],norm);

						 if	(Depth>0.f) {

						 isPC=true;
						 break;
						 };

						 }		

						 */
						 if(!isLC){
							 Depth=PointSphereTest(dGeomGetPosition(Sphere),
								 dGeomSphereGetRadius(Sphere),v0,norm);
							 isPC=Depth>0.f;
							 if(!isPC){Depth=PointSphereTest(dGeomGetPosition(Sphere),
								 dGeomSphereGetRadius(Sphere),v1,norm);
							 isPC=Depth>0.f;
							 }

							 if(!isPC){Depth=PointSphereTest(dGeomGetPosition(Sphere),
								 dGeomSphereGetRadius(Sphere),v2,norm);
							 isPC=Depth>0.f;
							 }

						 }

						 dNormalize3(triAx);
						 dVector3 ContactNormal={triAx[0],triAx[1],triAx[2]};
						 dVector3 ContactPos={SphereCenter[0]-triAx[0]* SphereRadius,SphereCenter[1]-triAx[1]* SphereRadius,SphereCenter[2]-triAx[2]* SphereRadius};

						 float ContactDepth= dDOT(triAx,v0) - dDOT(SphereCenter,triAx) + SphereRadius;
						 if (ContactDepth >= 0){


							 bool contains=TriContainPoint(v0,v1,v2,ContactPos);

							 if(contains&&ContactDepth>Depth&&ContactDepth>0.f)
							 {
								 isLC=false;
								 isPC=false;
							 }

							 if(isLC||isPC){

								 ContactNormal[0]=norm[0];
								 ContactNormal[1]=norm[1];
								 ContactNormal[2]=norm[2];
								 ContactPos[0]=SphereCenter[0]-ContactNormal[0]*SphereRadius;
								 ContactPos[1]=SphereCenter[1]-ContactNormal[1]*SphereRadius;
								 ContactPos[2]=SphereCenter[2]-ContactNormal[2]*SphereRadius;

								 ContactDepth=Depth;
							 }


							 dNormalize3(ContactNormal);


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
								 ((dxGeomUserData*)dGeomGetData(Sphere))->tri_material=T->material;
								 if(dGeomGetUserData(Sphere)->callback)dGeomGetUserData(Sphere)->callback(T,Contacts);
								 SURFACE(Contacts,0)->mode=T->material;
								 //////////////////////////////////
								 //	++OutTriCount;
								 return 1;


							 }

						 }

						 return 0;
					 }