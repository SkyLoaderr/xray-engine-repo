////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_fire.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Fuzzy State Machine for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "..\\ai_monsters_misc.h"

//	Msg("Path state : %s",(m_tPathState == ePathStateSearchNode) ? "Searching for the node" : (m_tPathState == ePathStateBuildNodePath) ? "Building path" : (m_tPathState == ePathStateBuildTravelLine) ? "Building travel line" : "Dodging travel line");\
	Msg("Monster %s : \n* State : %s\n* Time delta : %7.3f\n* Global time : %7.3f",cName(),s,m_fTimeUpdateDelta,float(Level().timeServer())/1000.f);\

#undef	WRITE_TO_LOG
#define WRITE_TO_LOG(s) {\
	m_bStopThinking = true;\
}

#ifndef DEBUG
	#undef	WRITE_TO_LOG
	#define WRITE_TO_LOG(s) m_bStopThinking = true;
#endif

void CAI_Stalker::vfUpdateSearchPosition()
{
	if (!g_Alive())
		return;
	INIT_SQUAD_AND_LEADER;
	if (this != Leader)	{
		CAI_Stalker *tpLeader = dynamic_cast<CAI_Stalker*>(Leader);
		if (tpLeader) {
//			if (m_tNextGraphPoint.distance_to(tpLeader->m_tNextGraphPoint) > EPS_L)
//				m_eCurrentState = eStalkerStateSearching;
			m_tNextGraphPoint			= tpLeader->m_tNextGraphPoint;
		}
	}
	else {
		if ((Level().timeServer() >= m_dwTimeToChange) && (getAI().m_tpaCrossTable[AI_NodeID].tGraphIndex == m_tNextGP)) {
			m_tNextGP					= getAI().m_tpaCrossTable[AI_NodeID].tGraphIndex;
			vfChooseNextGraphPoint		();
			m_tNextGraphPoint.set		(getAI().m_tpaGraph[m_tNextGP].tLocalPoint);
			if (m_eCurrentState == eStalkerStateAccomplishingTask)
				AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
		}
		else
			if (m_eCurrentState == eStalkerStateAccomplishingTask)
				AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
	}
}

void CAI_Stalker::Death()
{
	WRITE_TO_LOG("Death");
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);
}

void CAI_Stalker::BackDodge()
{
	WRITE_TO_LOG("Back dodging");
	
	m_tEnemy.Enemy				= dynamic_cast<CEntity *>(Level().CurrentEntity());

	Fvector						tPoint;
	m_tEnemy.Enemy->svCenter	(tPoint);
	vfSetParameters				(m_tSelectorRetreat,0,eWeaponStateIdle,ePathTypeDodge,eBodyStateStand,eMovementTypeRun,eLookTypeDirection,tPoint);
}

void CAI_Stalker::BackCover()
{
	WRITE_TO_LOG("Back cover");
	
	m_tEnemy.Enemy				= dynamic_cast<CEntity *>(Level().CurrentEntity());

	m_tSelectorCover.m_fMinEnemyDistance = m_tEnemy.Enemy->Position().distance_to(vPosition) + 3.f;

	Fvector						tPoint;
	m_tEnemy.Enemy->svCenter	(tPoint);
	vfSetParameters				(m_tSelectorCover,0,eWeaponStateIdle,ePathTypeCriteria,eBodyStateStand,eMovementTypeRun,eLookTypeFirePoint,tPoint);
}

void CAI_Stalker::ForwardCover()
{
	WRITE_TO_LOG("Back cover");
	
	m_tEnemy.Enemy				= dynamic_cast<CEntity *>(Level().CurrentEntity());
	Fvector						tPoint;
	m_tEnemy.Enemy->svCenter	(tPoint);

	float						fDistance = m_tEnemy.Enemy->Position().distance_to(vPosition) - 3.f;
	
	if (m_bStateChanged) {
		m_tSelectorCover.m_fMaxEnemyDistance = max(fDistance,m_tSelectorCover.m_fOptEnemyDistance + 3.f);
		m_tSelectorCover.m_fMinEnemyDistance = m_tSelectorCover.m_fOptEnemyDistance - 3.f;
		m_dwLastRangeSearch = 0;
	}
//	if (Level().timeServer() >= m_dwActionEndTime) {
//		m_dwActionStartTime = Level().timeServer();
//		switch (m_tActionState) {
//			case eActionStateRun : {
//				m_tActionState		= eActionStateStand;
//				m_dwActionEndTime	= m_dwActionStartTime + ::Random.randI(2000,3000);
//				break;
//			}
//			case eActionStateStand : {
//				if (AI_Path.TravelPath.size() > AI_Path.TravelStart + 1)
//					m_tActionState		= eActionStateRun;
//				else
//					m_tActionState		= eActionStateStand;
//				m_dwActionEndTime	= m_dwActionStartTime + ::Random.randI(4000,5000);
//				break;
//			}
//		}
//	}
	switch (m_tActionState) {
		case eActionStateRun : {
			Msg							("State RUN");
			EBodyState					tBodyState;
			CWeapon						*tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
			if (tpWeapon && (tpWeapon->STATE != CWeapon::eFire) && (tpWeapon->STATE != CWeapon::eFire2) && (tpWeapon->STATE != CWeapon::eIdle))
				tBodyState = m_tBodyState;
			else
				tBodyState = eBodyStateStand;
			if (tpWeapon && (tpWeapon->STATE == CWeapon::eIdle) && (!tpWeapon->GetAmmoElapsed())) {
				m_inventory.Action(kWPN_FIRE, CMD_START);
				m_inventory.Action(kWPN_FIRE, CMD_STOP);
			}
			vfSetParameters				(
				m_tSelectorCover,
				0,
				eWeaponStateIdle,
				ePathTypeDodge,
				tBodyState,
				eMovementTypeRun,
				eLookTypeFirePoint,
				tPoint);
			if (m_bIfSearchFailed && (AI_Path.fSpeed < EPS_L))
				m_tActionState = eActionStateStand;
			break;
		}
		case eActionStateStand : {
			Msg							("State STAND");
			m_tSelectorCover.m_fMaxEnemyDistance = max(fDistance,m_tSelectorCover.m_fOptEnemyDistance + 3.f);
			m_tSelectorCover.m_fMinEnemyDistance = m_tSelectorCover.m_fOptEnemyDistance - 3.f;
			m_dwLastRangeSearch = 0;

			EBodyState					tBodyState;
			CWeapon						*tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
			if (tpWeapon && (tpWeapon->STATE != CWeapon::eFire) && (tpWeapon->STATE != CWeapon::eFire2) && (tpWeapon->STATE != CWeapon::eIdle))
				tBodyState = m_tBodyState;
			else
				tBodyState = eBodyStateCrouch;;
			vfSetParameters				(
				m_tSelectorCover,
				0,
				eWeaponStatePrimaryFire,
				ePathTypeDodge,
				tBodyState,
				eMovementTypeStand,
				eLookTypeFirePoint,
				tPoint);
			if ((!m_inventory.ActiveItem() || !dynamic_cast<CWeapon*>(m_inventory.ActiveItem()) || (dynamic_cast<CWeapon*>(m_inventory.ActiveItem())->STATE != CWeapon::eFire)) && !m_bIfSearchFailed)
				m_tActionState		= eActionStateRun;
			break;
		}
		default : NODEFAULT;
	}
}

void CAI_Stalker::Think()
{
	vfUpdateDynamicObjects	();
//	vfUpdateSearchPosition	();
	m_dwUpdateCount++;
	m_dwLastUpdate			= m_dwCurrentUpdate;
	m_dwCurrentUpdate		= Level().timeServer();
	m_bStopThinking			= false;
	do {
		m_ePreviousState	= m_eCurrentState;
		if (!g_Alive())
			Death();
		else
			ForwardCover();
		m_bStateChanged		= m_ePreviousState != m_eCurrentState;
	}
	while (!m_bStopThinking);
};