#include "stdafx.h"	
#include "PHCollisionDamageReceiver.h"
#include "PhysicObject.h"
#include "PHDestroyable.h"
#include "hit_immunity.h"
#include "damage_manager.h"
#include "DestroyablePhysicsObject.h"
#include "../SkeletonCustom.h"
#include "xrServer_Objects_ALife.h"

CDestroyablePhysicsObject ::CDestroyablePhysicsObject()
{
	m_fHealth=100.f;
}

CDestroyablePhysicsObject::~CDestroyablePhysicsObject()
{

}
CPhysicsShellHolder*	 CDestroyablePhysicsObject ::	PPhysicsShellHolder			()
{
	return cast_physics_shell_holder();
}

void CDestroyablePhysicsObject::net_Destroy()
{
	inherited::net_Destroy();
	CPHDestroyable::RespawnInit();
	CPHCollisionDamageReceiver::Clear();
}

BOOL CDestroyablePhysicsObject::net_Spawn(CSE_Abstract* DC)
{
	BOOL res=inherited::net_Spawn(DC);
	CKinematics		*K=smart_cast<CKinematics*>(Visual());
	CInifile* ini=K->LL_UserData();
	//R_ASSERT2(ini->section_exist("destroyed"),"destroyable_object must have -destroyed- section in model user data");
	CPHDestroyable::Init();
	if(ini&&ini->section_exist("destroyed"))
		CPHDestroyable::Load(ini,"destroyed");
	CDamageManager::init_bones();
	if(ini)
	{	

		if(ini->section_exist("immunities"))
			CHitImmunity::InitImmunities("immunities",ini);
		if(ini->section_exist("damage_section"))
		{
			CDamageManager::load_section("damage_section",ini);
		}

		CPHCollisionDamageReceiver::Init();

		if(ini->section_exist("sound"))
		{
			m_destroy_sound.create(TRUE,ini->r_string("sound","break_sound"));
		}
		if(ini->section_exist("particles"))
		{
			m_destroy_particles=ini->r_string("particles","destroy_particles");
		}
	}
	CParticlesPlayer::LoadParticles(K);

	
	return res;
}

void CDestroyablePhysicsObject::Hit							(float P,Fvector &dir,CObject *who,s16 element,Fvector p_in_object_space, float impulse,  ALife::EHitType hit_type)
{
	P=CHitImmunity::AffectHit(P,hit_type);
	float hit_scale=1.f,wound_scale=1.f;
	CDamageManager::HitScale(element,hit_scale,wound_scale);
	P*=hit_scale;
	inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
	m_fHealth-=P;
	if(m_fHealth<=0.f) Destroy();
		
}
void CDestroyablePhysicsObject::Destroy()
{
	CPHDestroyable::Destroy(ID(),"physic_destroyable_object");
	if(m_destroy_sound.handle)
	{
		m_destroy_sound.play_at_pos(this,Position());
	}
	if(*m_destroy_particles)
			StartParticles(m_destroy_particles,Fvector().set(0,1,0),ID());
}
void CDestroyablePhysicsObject::InitServerObject(CSE_Abstract* D)
{
	CPHDestroyable::InitServerObject(D);
	CSE_ALifeObjectPhysic			*PO = smart_cast<CSE_ALifeObjectPhysic*>(D);
	if(PO)PO->type=epotSkeleton;
}
void CDestroyablePhysicsObject::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	CPHDestroyable::SheduleUpdate(dt);
}

bool CDestroyablePhysicsObject::CanRemoveObject()
{
	return !CParticlesPlayer::IsPlaying()&& !m_destroy_sound.feedback;//&& sound!
}
DLL_Pure	*CDestroyablePhysicsObject::_construct()
{
	
	CDamageManager::_construct();
	return inherited::_construct();
}