#include "stdafx.h"
#include "entity_alive.h"
#include "inventoryowner.h"

/////////////////////////////////////////////
// CEntityAlive
/////////////////////////////////////////////
CEntityAlive::CEntityAlive()
{
	Init				();
}

CEntityAlive::~CEntityAlive()
{
}

void CEntityAlive::Init			()
{
}

void CEntityAlive::Load		(LPCSTR section)
{
	CEntity::Load			(section);
	CEntityCondition::Load	(section);
	m_fFood					= 100*pSettings->r_float	(section,"ph_mass");
}

void CEntityAlive::reinit			()
{
	CEntity::reinit			();
	CEntityCondition::reinit();

	m_dwDeathTime			= 0;
	m_fAccuracy				= 25.f;
	m_fIntelligence			= 25.f;
}

void CEntityAlive::reload		(LPCSTR section)
{
	CEntity::reload			(section);
//	CEntityCondition::reload(section);

	m_fFood					= 100*pSettings->r_float	(section,"ph_mass");
}

void CEntityAlive::shedule_Update(u32 dt)
{
	inherited::shedule_Update	(dt);

	//condition update with the game time pass
	UpdateCondition();

	//убить сущность
	if(Local() && !g_Alive() && !AlreadyDie())
	{
		if(GetWhoHitLastTime())
			KillEntity(GetWhoHitLastTime());
		else
			KillEntity(this);
	}
}

BOOL CEntityAlive::net_Spawn	(LPVOID DC)
{
	inherited::net_Spawn	(DC);

	//m_PhysicMovementControl.SetPosition	(Position());
	//m_PhysicMovementControl.SetVelocity	(0,0,0);
	return					TRUE;
}

void CEntityAlive::net_Destroy	()
{
	CInventoryOwner	*l_tpInventoryOwner = dynamic_cast<CInventoryOwner*>(this);
	if (l_tpInventoryOwner) {
		l_tpInventoryOwner->m_inventory.ClearAll();
		l_tpInventoryOwner->m_trade_storage.ClearAll();
	}

	inherited::net_Destroy		();
}

void CEntityAlive::HitImpulse	(float /**amount/**/, Fvector& /**vWorldDir/**/, Fvector& /**vLocalDir/**/)
{
	//	float Q					= 2*float(amount)/m_PhysicMovementControl.GetMass();
	//	m_PhysicMovementControl.vExternalImpulse.mad	(vWorldDir,Q);
}

void CEntityAlive::Hit(float P, Fvector &dir,CObject* who, s16 element,Fvector position_in_object_space, float impulse, ALife::EHitType hit_type)
{
	//изменить состояние, перед тем как родительский класс обработае хит
	ConditionHit(who, P, hit_type, element);


	inherited::Hit(P,dir,who,element,position_in_object_space,impulse, hit_type);
}

void CEntityAlive::BuyItem(LPCSTR buf)
{
	NET_Packet P;
	u_EventGen	(P,GE_BUY,ID());
	P.w_string	(buf);
	u_EventSend	(P);
}

//вывзывает при подсчете хита
float CEntityAlive::CalcCondition(float /**hit/**/)
{	
	UpdateCondition();

	//dont call inherited::CalcCondition it will be meaning less
	return GetHealthLost()*100.f;
}

///////////////////////////////////////////////////////////////////////
u16	CEntityAlive::PHGetSyncItemsNumber()
{
	if(m_PhysicMovementControl.CharacterExist()) return 1;
	else										  return 0;
}
CPHSynchronize* CEntityAlive::PHGetSyncItem	(u16/*item*/)
{
	if(m_PhysicMovementControl.CharacterExist()) return m_PhysicMovementControl.GetSyncItem();
	else										 return 0;
}
void CEntityAlive::PHUnFreeze()
{
	if(m_PhysicMovementControl.CharacterExist()) m_PhysicMovementControl.UnFreeze();
}
void CEntityAlive::PHFreeze()
{
	if(m_PhysicMovementControl.CharacterExist()) m_PhysicMovementControl.Freeze();
}
//////////////////////////////////////////////////////////////////////