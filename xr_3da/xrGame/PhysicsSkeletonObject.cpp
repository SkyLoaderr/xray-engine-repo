#include "stdafx.h"
#include "physicsskeletonobject.h"
#include "PhysicsShell.h"
#include "phsynchronize.h"
#include "xrserver_objects_alife.h"
#include "level.h"



CPhysicsSkeletonObject::CPhysicsSkeletonObject()
{

}

CPhysicsSkeletonObject::~CPhysicsSkeletonObject()
{

}


BOOL CPhysicsSkeletonObject::net_Spawn(LPVOID DC)
{
	CSE_Abstract			  *e	= (CSE_Abstract*)(DC);

	inherited::net_Spawn	(DC);
	xr_delete(collidable.model);
	collidable.model = xr_new<CCF_Skeleton>(this);
	CPHSkeleton::Spawn(e);
	setVisible(true);
	setEnabled(true);
	return TRUE;
}

void	CPhysicsSkeletonObject::SpawnInitPhysics	(CSE_Abstract	*D)
{
	CreatePhysicsShell(D);
	PKinematics(Visual())->CalculateBones	();
}

void CPhysicsSkeletonObject::net_Destroy()
{
	CPHSkeleton::RespawnInit	();
	inherited::net_Destroy		();
	CKinematics* K=PKinematics	(Visual());
	if(K)	K->CalculateBones	();
}

void CPhysicsSkeletonObject::Load(LPCSTR section)
{
	inherited::Load(section);
	CPHSkeleton::Load(section);
}

void CPhysicsSkeletonObject::CreatePhysicsShell(CSE_Abstract* e)
{
	CSE_PHSkeleton	*po=smart_cast<CSE_PHSkeleton*>(e);
	if(m_pPhysicsShell) return;
	if (!Visual()) return;
	m_pPhysicsShell=P_build_Shell(this,!po->_flags.test(CSE_PHSkeleton::flActive));
}


void CPhysicsSkeletonObject::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);

	CPHSkeleton::Update(dt);
}

void CPhysicsSkeletonObject::net_Save(NET_Packet &P)
{
	inherited::net_Save(P);
	CPHSkeleton::SaveNetState	   (P);
}



BOOL CPhysicsSkeletonObject::net_SaveRelevant()
{
	return TRUE;//!m_flags.test(CSE_ALifeObjectPhysic::flSpawnCopy);
}


BOOL CPhysicsSkeletonObject::UsedAI_Locations()
{
	return					(FALSE);
}

void CPhysicsSkeletonObject::UpdateCL()
{
	inherited::UpdateCL		();
	PHObjectPositionUpdate	();
}

void CPhysicsSkeletonObject::	PHObjectPositionUpdate()
{
	if(m_pPhysicsShell)
	{
			m_pPhysicsShell->InterpolateGlobalTransform(&XFORM());
	}
}