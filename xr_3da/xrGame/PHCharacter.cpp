#include "stdafx.h"

#include "phcharacter.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "ExtendedGeom.h"
#include "../cl_intersect.h"
#include "tri-colliderKNoOPC\__aabb_tri.h"
#include "GameObject.h"




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

m_contact_velocity=0.f;
m_safe_velocity[0]=0.f;
m_safe_velocity[1]=0.f;
m_safe_velocity[2]=0.f;
}

CPHCharacter::~CPHCharacter(void)
{

}

void	CPHCharacter::Freeze()
{
	was_enabled_before_freeze=!!dBodyIsEnabled(m_body);
	dBodyDisable(m_body);
}

void	CPHCharacter::getForce(Fvector& force)
{
	if(!b_exist)return;
	force.set(*(Fvector*)dBodyGetForce(m_body));
}
void	CPHCharacter::setForce(const Fvector &force)
{
	if(!b_exist)return;
	dBodySetForce(m_body,force.x,force.y,force.z);
}
void	CPHCharacter::UnFreeze()
{
	if(was_enabled_before_freeze)dBodyEnable(m_body);
}

void CPHCharacter::get_State(SPHNetState& state)
{
	GetPosition(state.position);
	m_body_interpolation.GetPosition(state.previous_position,0);
	GetVelocity(state.linear_vel);
	getForce(state.force);
}
void CPHCharacter::set_State(const SPHNetState& state)
{
	m_body_interpolation.SetPosition(state.previous_position,0);
	m_body_interpolation.SetPosition(state.position,1);
	SetPosition(state.position);
	SetVelocity(state.linear_vel);
	setForce(state.force);
}
//////////////////////////////////////////////////////////////////////////
/////////////////////CPHWheeledCharacter//////////////////////////////////
//////////////////////////////////////////////////////////////////////////








void __stdcall CarHitCallback(bool& /**do_colide/**/,dContact& /**c/**/)
{
	/*
	dxGeomUserData* usr_data_1=NULL;
	dxGeomUserData* usr_data_2=NULL;
	l_pUD1 = retrieveGeomUserData(c.geom.g1);
	l_pUD2 = retrieveGeomUserData(c.geom.g2);

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