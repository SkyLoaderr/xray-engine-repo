// Do NOT build this file seperately. It is included in dTriList.cpp automatically.


#include "../../cl_intersect.h"
#include "dTriCollideK.h"
#include "dSortTriPrimitive.h"
#include "../dCylinder/dCylinder.h"
#include "../MathUtils.h"
dcTriListCollider::dcTriListCollider(dxGeom* Geometry)
{
	this->Geometry = Geometry;
	GeomData = (dxTriList*)dGeomGetClassData(Geometry);

#ifdef DEBUG
	Device.seqRender.Add	(this,REG_PRIORITY_LOW-999);
	BoxCenter = 0;
#endif
}

dcTriListCollider::~dcTriListCollider()
{
#ifdef DEBUG
	Device.seqRender.Remove	(this);
#endif
}





int dCollideBP (const dxGeom* o1, const dxGeom* o2, int flags, dContactGeom *contact, int skip);	// ODE internal function



#define CONTACT(Ptr, Stride) ((dContactGeom*) (((byte*)Ptr) + (Stride)))

#define SURFACE(Ptr, Stride) ((dSurfaceParameters*) (((byte*)Ptr) + (Stride-sizeof(dSurfaceParameters))))





int dcTriListCollider::CollideBox(dxGeom* Box, int Flags, dContactGeom* Contacts, int Stride)
{


	/* Get box */
#ifndef DEBUG
	Fvector AABB;
	Fvector* BoxCenter=0;
#endif
	Fvector BoxExtents;

	BoxCenter=(Fvector*)const_cast<dReal*>(dGeomGetPosition(Box));

	dVector3 BoxSides;
	dGeomBoxGetLengths(Box,BoxSides);
	dVectorSet((dReal*)&BoxExtents,(dReal*)&BoxSides);

	dReal* R=const_cast<dReal*>(dGeomGetRotation(Box));

	AABB.x=(dFabs(BoxSides[0]*R[0])+dFabs(BoxSides[1]*R[1])+dFabs(BoxSides[2]*R[2]))/2.f+100.f*EPS_L;
	AABB.y=(dFabs(BoxSides[0]*R[4])+dFabs(BoxSides[1]*R[5])+dFabs(BoxSides[2]*R[6]))/2.f+100.f*EPS_L;
	AABB.z=(dFabs(BoxSides[0]*R[8])+dFabs(BoxSides[1]*R[9])+dFabs(BoxSides[2]*R[10]))/2.f+100.f*EPS_L;
	dBodyID box_body=dGeomGetBody(Box);
	if(box_body){
		const dReal*velocity=dBodyGetLinearVel(box_body);
		AABB.x+=dFabs(velocity[0])*0.02f;
		AABB.y+=dFabs(velocity[1])*0.02f;
		AABB.z+=dFabs(velocity[2])*0.02f;
	}
	//
	XRC.box_options                (0);
	XRC.box_query                  (Level().ObjectSpace.GetStaticModel(),*BoxCenter,AABB);

	// 
	//		int count                                       =XRC.r_count   ();
	CDB::RESULT*    R_begin                         = XRC.r_begin();
	CDB::RESULT*    R_end                           = XRC.r_end();
	CDB::TRI*       T_array                         = Level().ObjectSpace.GetStaticTris();
	const Fvector*	V_array							= Level().ObjectSpace.GetStaticVerts();
	int OutTriCount = 0;

	return dSortTriPrimitiveCollide
		(dBoxProj,dTriBox,dSortedTriBox,
		Box,
		Geometry,
		3,
		CONTACT(Contacts, OutTriCount * Stride),   
		Stride,
		R_begin,R_end,T_array,V_array,AABB
		);
}



int dcTriListCollider::CollideCylinder(dxGeom* Cylinder, int Flags, dContactGeom* Contacts, int Stride){


	Fvector* CylinderCenter;
	Fvector AABB;
	dReal CylinderRadius,CylinderLength;
	CylinderCenter=(Fvector*)const_cast<dReal*>(dGeomGetPosition(Cylinder));


	dGeomCylinderGetParams (Cylinder, &CylinderRadius,&CylinderLength);



	dReal* R=const_cast<dReal*>(dGeomGetRotation(Cylinder));

	AABB.x =  REAL(0.5) * dFabs (R[1] * CylinderLength) + (_sqrt(R[0]*R[0]+R[2]*R[2]) * CylinderRadius);

	AABB.y =  REAL(0.5) * dFabs (R[5] * CylinderLength) + (_sqrt(R[4]*R[4]+R[6]*R[6]) * CylinderRadius);

	AABB.z =  REAL(0.5) * dFabs (R[9] * CylinderLength) + (_sqrt(R[8]*R[8]+R[10]*R[10]) * CylinderRadius);

	const dReal*velocity=dBodyGetLinearVel(dGeomGetBody(Cylinder));
	AABB.x+=dFabs(velocity[0])*0.01f;
	AABB.y+=dFabs(velocity[1])*0.01f;
	AABB.z+=dFabs(velocity[2])*0.01f;


	//
	XRC.box_options                (0);
	XRC.box_query                  (Level().ObjectSpace.GetStaticModel(),* CylinderCenter,AABB);

	// 
	//	int count                                       =XRC.r_count   ();
	CDB::RESULT*    R_begin                         = XRC.r_begin();
	CDB::RESULT*    R_end                           = XRC.r_end();
	CDB::TRI*       T_array                         = Level().ObjectSpace.GetStaticTris();
	const Fvector*	V_array							= Level().ObjectSpace.GetStaticVerts();

	int OutTriCount = 0;

	///@slipch

	//for (CDB::RESULT* Res=R_begin; Res!=R_end; ++Res)
	//{
	//	CDB::TRI* T = T_array + Res->id;
	//	OutTriCount+=dTriCyl (
	//		(const dReal*)&V_array[T->verts[0]],
	//		(const dReal*)&V_array[T->verts[1]],
	//		(const dReal*)&V_array[T->verts[2]],
	//		T,
	//		Cylinder,
	//		Geometry,
	//		3,
	//		CONTACT(Contacts, OutTriCount * Stride),   Stride);
	//}
	//return OutTriCount;

	return dSortTriPrimitiveCollide
		(dCylinderProj,dTriCyl,dSortedTriCyl,
		Cylinder,
		Geometry,
		3,
		CONTACT(Contacts, OutTriCount * Stride),   
		Stride,
		R_begin,R_end,T_array,V_array,AABB
		);

}


					 ////////////////////////////////////////////////////////////////////////////
	
					 ///////////////////////////////////////////////////////////////////////////
					 int dcTriListCollider::CollideSphere(dxGeom* Sphere, int Flags, dContactGeom* Contacts, int Stride){


						 // Get sphere 


						 const dReal* SphereCenter=dGeomGetPosition(Sphere);

						 const float SphereRadius = dGeomSphereGetRadius(Sphere);

						 Fvector AABB;
						 Fvector SphereCenterF;

						 dVectorSet((dReal*)&SphereCenterF,SphereCenter);


						 // Make AABB 
						 AABB.x=SphereRadius;
						 AABB.y=SphereRadius;
						 AABB.z=SphereRadius;

						 const dReal*velocity=dBodyGetLinearVel(dGeomGetBody(Sphere));
						 AABB.x+=dFabs(velocity[0])*0.02f;
						 AABB.y+=dFabs(velocity[1])*0.02f;
						 AABB.z+=dFabs(velocity[2])*0.02f;

						 // Retrieve data 

						 // Creating minimum contacts 


						 //UINT TriangleIDCount; // Num of tries

						 XRC.box_options                (0);
						 XRC.box_query                  (Level().ObjectSpace.GetStaticModel(),SphereCenterF,AABB);

						 // 
						 //	int count                                       = (int)XRC.r_count   ();
						 //	++count;
						 //	--count;
						 CDB::RESULT*    R_begin                         = XRC.r_begin();
						 CDB::RESULT*    R_end                           = XRC.r_end();
						 CDB::TRI*       T_array                         = Level().ObjectSpace.GetStaticTris();
				 		 const Fvector*	 V_array						 = Level().ObjectSpace.GetStaticVerts();
						 return dSortTriPrimitiveCollide(dSphereProj,dTriSphere,dSortedTriSphere,Sphere,Geometry,Flags,Contacts,Stride,R_begin,R_end,T_array,V_array,AABB);

					 }


	

#ifdef DEBUG
					 void dcTriListCollider::OnRender()
					 {/*
					  if (!bDebug && BoxCenter){
					  Fvector C;
					  C.set(0,0,0);
					  RCache.dbg_DrawAABB(*BoxCenter,AABB.x,AABB.y,AABB.z,D3DCOLOR_XRGB(255,255,255));
					  }
					  */
					 }
#endif

