#include "stdafx.h"	
#include "PHCollisionDamageReceiver.h"
#include "PhysicObject.h"
#include "PHDestroyable.h"
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

BOOL CDestroyablePhysicsObject::net_Spawn(LPVOID DC)
{
	BOOL res=inherited::net_Spawn(DC);
	CInifile* ini=smart_cast<CKinematics*>(Visual())->LL_UserData();
	//R_ASSERT2(ini->section_exist("destroyed"),"destroyable_object must have -destroyed- section in model user data");
	if(ini&&ini->section_exist("destroyed"))
		CPHDestroyable::Load(ini,"destroyed");
	CPHDestroyable::Init();
	if(ini)CPHCollisionDamageReceiver::Init();
	return res;
}

void CDestroyablePhysicsObject::Hit							(float P,Fvector &dir,CObject *who,s16 element,Fvector p_in_object_space, float impulse,  ALife::EHitType hit_type)
{
	inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
	m_fHealth-=P;
	if(m_fHealth<=0.f) Destroy(u16(-1),"physic_destroyable_object");
		
}

void CDestroyablePhysicsObject::InitServerObject(CSE_Abstract* D)
{
	CPHDestroyable::InitServerObject(D);
	smart_cast<CSE_ALifeObjectPhysic*>(D)->type=epotSkeleton;
}