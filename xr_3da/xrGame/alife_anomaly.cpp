////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_zone.cpp
//	Created 	: 19.08.2003
//  Modified 	: 19.08.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life zones simulation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "alife_simulator.h"
#include "alife_time_manager.h"
#include "ai_space.h"

using namespace ALife;

void CSE_ALifeAnomalousZone::update()
{
//	R_ASSERT3					(false,"This function shouldn't be called!",s_name_replace);
	m_maxPower					= m_fStartPower*(ai().alife().time_manager().next_surge_time() - m_tTimeID)/(ai().alife().time_manager().next_surge_time() - ai().alife().time_manager().last_surge_time());
//	ai().alife().vfCheckForInteraction(this);
}

CSE_ALifeItemWeapon	*CSE_ALifeAnomalousZone::tpfGetBestWeapon(EHitType &tHitType, float &fHitPower)
{
	m_tpCurrentBestWeapon		= 0;
	m_tTimeID					= ai().alife().time_manager().game_time();
	m_maxPower					= m_fStartPower*(ai().alife().time_manager().next_surge_time() - m_tTimeID)/(ai().alife().time_manager().next_surge_time() - ai().alife().time_manager().last_surge_time());
	fHitPower					= m_maxPower;
	tHitType					= m_tHitType;
	return						(m_tpCurrentBestWeapon);
}

EMeetActionType	CSE_ALifeAnomalousZone::tfGetActionType(CSE_ALifeSchedulable *tpALifeSchedulable, int iGroupIndex, bool bMutualDetection)
{
	return						(eMeetActionTypeAttack);
}

bool CSE_ALifeAnomalousZone::bfActive()
{
	return						(m_maxPower > EPS_L);
}

CSE_ALifeDynamicObject *CSE_ALifeAnomalousZone::tpfGetBestDetector()
{
	VERIFY2						(false,"This function shouldn't be called");
	NODEFAULT;
#ifdef DEBUG
	return						(0);
#endif
}

CSE_ALifeItemWeapon	*CSE_ALifeSmartZone::tpfGetBestWeapon	(ALife::EHitType		&tHitType,			float		&fHitPower)
{
	m_tpCurrentBestWeapon		= 0;
	return						(m_tpCurrentBestWeapon);
}

ALife::EMeetActionType CSE_ALifeSmartZone::tfGetActionType	(CSE_ALifeSchedulable	*tpALifeSchedulable,int			iGroupIndex, bool bMutualDetection)
{
	CSE_ALifeObject				*object = smart_cast<CSE_ALifeObject*>(tpALifeSchedulable->base());
	VERIFY						(object);
	return						((object->m_tGraphID == m_tGraphID) ? ALife::eMeetActionSmartTerrain : ALife::eMeetActionTypeIgnore);
}

bool CSE_ALifeSmartZone::bfActive							()
{
	return						(true);
}

CSE_ALifeDynamicObject *CSE_ALifeSmartZone::tpfGetBestDetector	()
{
	VERIFY2						(false,"This function shouldn't be called");
	return						(0);
}
