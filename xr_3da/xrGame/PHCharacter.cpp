#include "stdafx.h"

#include "phcharacter.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "ExtendedGeom.h"
#include "..\cl_intersect.h"
#include "tri-colliderKNoOPC\__aabb_tri.h"
#include "GameObject.h"
#include "Car.h"



void dBodyAngAccelFromTorqu(const dBodyID body, dReal* ang_accel, const dReal* torque){
      dMass m;
      dMatrix3 invI;
      dBodyGetMass(body,&m);
      dInvertPDMatrix (m.I, invI, 3);
      dMULTIPLY1_333(ang_accel,invI, torque);
      }


CPHCharacter::CPHCharacter(void)
{

m_body=NULL;

}

CPHCharacter::~CPHCharacter(void)
{

}



//////////////////////////////////////////////////////////////////////////
/////////////////////CPHWheeledCharacter//////////////////////////////////
//////////////////////////////////////////////////////////////////////////








void __stdcall CarHitCallback(bool& do_colide,dContact& c)
{
	/*
	dxGeomUserData* usr_data_1=NULL;
	dxGeomUserData* usr_data_2=NULL;
	if(dGeomGetClass(c.geom.g1)==dGeomTransformClass){
	const dGeomID geom=dGeomTransformGetGeom(c.geom.g1);
	usr_data_1 = dGeomGetUserData(geom);
	}
	else
	usr_data_1 = dGeomGetUserData(c.geom.g1);

	if(dGeomGetClass(c.geom.g2)==dGeomTransformClass){
	const dGeomID geom=dGeomTransformGetGeom(c.geom.g2);
	usr_data_2 = dGeomGetUserData(geom);
	}
	else
	usr_data_2 = dGeomGetUserData(c.geom.g2);

	if(!(usr_data_1&&usr_data_2)) return;

	CPHCharacter* Character;
	Character=dynamic_cast<CPHCharacter*>(usr_data_1->ph_object);
	if(Character)
	{
	CGameObject* Obj=dynamic_cast<CGameObject*>(usr_data_1->ph_ref_object);
	CCar*		 Car=dynamic_cast<CCar*>	   (usr_data_2->ph_ref_object);
	if(Obj&&Car)
	{
	Fvector vel,rvel;
	Car->GetVelocity(vel);
	rvel.sub(vel,Character->GetVelocity());
	if(rvel.dotproduct(*((Fvector*)c.geom.normal))>1.f)
	{
	vel.normalize();
	Fvector pos;

	pos.set(0,0,0);
	Obj->Hit(100.f,vel,Car,0,pos,1000.f);
	Obj->PHSetPushOut();
	}
	}
	}

	Character=dynamic_cast<CPHCharacter*>(usr_data_2->ph_object);
	if(Character)
	{
	CGameObject* Obj=dynamic_cast<CGameObject*>(usr_data_2->ph_ref_object);
	CCar*		 Car=dynamic_cast<CCar*>	   (usr_data_1->ph_ref_object);
	if(Obj&&Car)
	{
	Fvector vel,rvel;
	Car->GetVelocity(vel);
	rvel.sub(vel,Character->GetVelocity());
	if(-rvel.dotproduct(*((Fvector*)c.geom.normal))>5.f)
	{
	vel.normalize();
	Fvector pos;

	pos.set(0,0,0);
	Obj->Hit(100.f,vel,Car,0,pos,10.f);
	Obj->PHSetPushOut();
	}
	}
	}

	*/
}