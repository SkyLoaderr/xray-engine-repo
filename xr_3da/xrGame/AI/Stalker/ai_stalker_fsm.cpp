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

void CAI_Stalker::Death()
{
	WRITE_TO_LOG("Death");
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);
}

void CAI_Stalker::BackDodge()
{
	WRITE_TO_LOG				("Back dodging");
	
	m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tEnemy.Enemy->Position().distance_to(vPosition) + m_tSelectorFreeHunting.m_fSearchRange;
	m_tSelectorFreeHunting.m_fOptEnemyDistance = m_tSelectorFreeHunting.m_fMaxEnemyDistance;
	m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tEnemy.Enemy->Position().distance_to(vPosition) + 3.f;

	Fvector						tPoint;
	m_tEnemy.Enemy->clCenter	(tPoint);
	vfSetParameters				(
		&m_tSelectorFreeHunting,
		0,
		eWeaponStatePrimaryFire,
		ePathTypeDodge,
		eBodyStateStand,
		eMovementTypeWalkDanger,
		eLookTypeFirePoint,
		tPoint);
}

void CAI_Stalker::BackCover()
{
	WRITE_TO_LOG				("Back cover");
	
	m_tSelectorCover.m_fMinEnemyDistance = m_tEnemy.Enemy->Position().distance_to(vPosition) + 3.f;

	Fvector						tPoint;
	m_tEnemy.Enemy->clCenter	(tPoint);
	vfSetParameters				(
		&m_tSelectorCover,
		0,
		eWeaponStatePrimaryFire,
		ePathTypeCriteria,
		eBodyStateStand,
		eMovementTypeRunDanger,
		eLookTypeFirePoint,
		tPoint);
}

void CAI_Stalker::ForwardCover()
{
	WRITE_TO_LOG("Back cover");
	
	Fvector						tPoint;
	m_tEnemy.Enemy->clCenter	(tPoint);

	if (m_bStateChanged) {
		float						fDistance = m_tEnemy.Enemy->Position().distance_to(vPosition);
		m_tSelectorCover.m_fMaxEnemyDistance = max(fDistance - 3.f,m_tSelectorCover.m_fOptEnemyDistance + 3.f);
		m_tSelectorCover.m_fMinEnemyDistance = max(1*fDistance - 1*m_tSelectorCover.m_fSearchRange,m_tSelectorCover.m_fOptEnemyDistance - 3.f);
		CWeapon						*tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
		if (tpWeapon)
			m_tSelectorCover.m_fOptEnemyDistance = tpWeapon->m_fMinRadius;
		m_dwLastRangeSearch = 0;
		m_tActionState = eActionStateRun;
	}
	switch (m_tActionState) {
		case eActionStateRun : {
			CWeapon						*tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
			if (tpWeapon && (tpWeapon->STATE == CWeapon::eIdle) && (!tpWeapon->GetAmmoElapsed())) {
				m_inventory.Action(kWPN_FIRE, CMD_START);
				m_inventory.Action(kWPN_FIRE, CMD_STOP);
			}
			vfSetParameters				(
				&m_tSelectorCover,
				0,
				eWeaponStatePrimaryFire,
				ePathTypeDodge,
				eBodyStateStand,
				eMovementTypeRunDanger,
				eLookTypeFirePoint,
				tPoint);
			if (m_bIfSearchFailed && (AI_Path.fSpeed < EPS_L))
				m_tActionState = eActionStateStand;
			break;
		}
		case eActionStateStand : {
			float						fDistance = m_tEnemy.Enemy->Position().distance_to(vPosition);
			m_tSelectorCover.m_fMaxEnemyDistance = max(fDistance - 3.f,m_tSelectorCover.m_fOptEnemyDistance + 3.f);
			m_tSelectorCover.m_fMinEnemyDistance = max(1*fDistance - 1*m_tSelectorCover.m_fSearchRange,m_tSelectorCover.m_fOptEnemyDistance - 3.f);
			CWeapon						*tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
			if (tpWeapon)
				m_tSelectorCover.m_fOptEnemyDistance = tpWeapon->m_fMinRadius;

			vfSetParameters				(
				&m_tSelectorCover,
				0,
				eWeaponStatePrimaryFire,
				ePathTypeDodge,
				eBodyStateCrouch,
				eMovementTypeStandDanger,
				eLookTypeFirePoint,
				tPoint);
			
			if (!tpWeapon || (tpWeapon->STATE != CWeapon::eFire) && !tpWeapon->GetAmmoElapsed() && (!m_bIfSearchFailed || (!AI_Path.TravelPath.empty() && AI_Path.TravelPath.size() > AI_Path.TravelStart + 1)))
				m_tActionState			= eActionStateRun;
			break;
		}
		default : NODEFAULT;
	}
}

void CAI_Stalker::ForwardStraight()
{
	WRITE_TO_LOG("Forward straight");
	
	Fvector						tPoint;
	m_tEnemy.Enemy->clCenter	(tPoint);
	float						fDistance = vPosition.distance_to(m_tEnemy.Enemy->Position());

	CWeapon						*tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
	if (tpWeapon) {
		m_tSelectorFreeHunting.m_fMaxEnemyDistance = tpWeapon->m_fMaxRadius;
		m_tSelectorFreeHunting.m_fMinEnemyDistance = tpWeapon->m_fMinRadius;
		m_tSelectorFreeHunting.m_fOptEnemyDistance = tpWeapon->m_fMinRadius + 3.f;
	}

	vfSetParameters				(
		&m_tSelectorFreeHunting,
		0,
		eWeaponStatePrimaryFire,
		ePathTypeStraight,
		eBodyStateStand,
		eMovementTypeRunDanger,
		eLookTypeFirePoint,
		tPoint);
}

void CAI_Stalker::ForwardDodge()
{
	WRITE_TO_LOG("Forward dodge");
	
	Fvector						tPoint;
	m_tEnemy.Enemy->clCenter	(tPoint);
	float						fDistance = vPosition.distance_to(m_tEnemy.Enemy->Position());

	CWeapon						*tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
	if (tpWeapon) {
		m_tSelectorFreeHunting.m_fMaxEnemyDistance = tpWeapon->m_fMaxRadius;
		m_tSelectorFreeHunting.m_fMinEnemyDistance = tpWeapon->m_fMinRadius;
		m_tSelectorFreeHunting.m_fOptEnemyDistance = tpWeapon->m_fMinRadius + 3.f;
	}

	vfSetParameters				(
		&m_tSelectorFreeHunting,
		0,
		eWeaponStatePrimaryFire,
		ePathTypeDodge,
		eBodyStateStand,
		eMovementTypeRunDanger,
		eLookTypeFirePoint,
		tPoint);
}

void CAI_Stalker::Camp()
{
	WRITE_TO_LOG			("Camping...");
	int						iIndex = ifFindDynamicObject(m_tSavedEnemy);
	if (iIndex == -1)
		return;
	float					fDistance = vPosition.distance_to(m_tpaDynamicObjects[iIndex].tSavedPosition);

	if (vPosition.distance_to(m_tpaDynamicObjects[iIndex].tMySavedPosition) > .1f) {
		AI_Path.DestNode		= m_tpaDynamicObjects[iIndex].dwMyNodeID;
		vfSetParameters			(
			0,
			&(m_tpaDynamicObjects[iIndex].tMySavedPosition),
			eWeaponStateIdle,
			ePathTypeStraight,
			eBodyStateCrouch,
			eMovementTypeWalkDanger,
			eLookTypeFirePoint,
			m_tpaDynamicObjects[iIndex].tSavedPosition);
	}
	else {
		float fDistanceToCover = getAI().ffFindFarthestNodeInDirection(AI_NodeID,vPosition,m_tpaDynamicObjects[iIndex].tSavedPosition,AI_Path.DestNode);
		if ((fDistanceToCover < fDistance/3.f) && (fDistanceToCover > .5f)) {
			m_tpaDynamicObjects[iIndex].tMySavedPosition.sub(m_tpaDynamicObjects[iIndex].tSavedPosition,vPosition);
			m_tpaDynamicObjects[iIndex].tMySavedPosition.mul((fDistanceToCover - .5f)/fDistance);
			m_tpaDynamicObjects[iIndex].tMySavedPosition.add(vPosition);
			m_tpaDynamicObjects[iIndex].dwMyNodeID			= AI_Path.DestNode;
			AI_Path.TravelPath.clear();
			AI_Path.Nodes.clear();
			CWeapon	*tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
			if (tpWeapon && (tpWeapon->GetAmmoElapsed() < tpWeapon->GetAmmoMagSize()/2))
				m_inventory.Action(kWPN_RELOAD,CMD_START);
		}
	}
}

void CAI_Stalker::Panic()
{
	WRITE_TO_LOG				("Panic");
	
	if (!m_tEnemy.Enemy) {
		int						iIndex = ifFindDynamicObject(m_tSavedEnemy);
		if (iIndex != -1) {
			m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tpaDynamicObjects[iIndex].tSavedPosition.distance_to(vPosition) + m_tSelectorFreeHunting.m_fSearchRange;
			m_tSelectorFreeHunting.m_fOptEnemyDistance = m_tSelectorFreeHunting.m_fMaxEnemyDistance;
			m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tpaDynamicObjects[iIndex].tSavedPosition.distance_to(vPosition) + 3.f;
		}
		else {
			m_tSelectorFreeHunting.m_fMaxEnemyDistance = 1000.f;
			m_tSelectorFreeHunting.m_fOptEnemyDistance = 1000.f;
			m_tSelectorFreeHunting.m_fMinEnemyDistance = 0.f;
		}
	}
	else {
		m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tEnemy.Enemy->Position().distance_to(vPosition) + m_tSelectorFreeHunting.m_fSearchRange;
		m_tSelectorFreeHunting.m_fOptEnemyDistance = m_tSelectorFreeHunting.m_fMaxEnemyDistance;
		m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tEnemy.Enemy->Position().distance_to(vPosition) + 3.f;
	}

	vfSetParameters				(
		&m_tSelectorFreeHunting,
		0,
		eWeaponStateIdle,
		ePathTypeStraight,
		eBodyStateStand,
		eMovementTypeRunPanic,
		eLookTypeDirection);
}

void CAI_Stalker::Hide()
{
	WRITE_TO_LOG				("Hide");

	if (!m_tEnemy.Enemy) {
		int						iIndex = ifFindDynamicObject(m_tSavedEnemy);
		if (iIndex != -1) {
			m_tSelectorCover.m_fMaxEnemyDistance = m_tpaDynamicObjects[iIndex].tSavedPosition.distance_to(vPosition) + m_tSelectorCover.m_fSearchRange;
			m_tSelectorCover.m_fOptEnemyDistance = m_tSelectorCover.m_fMaxEnemyDistance;
			m_tSelectorCover.m_fMinEnemyDistance = m_tpaDynamicObjects[iIndex].tSavedPosition.distance_to(vPosition) + 3.f;
		}
		else {
			m_tSelectorCover.m_fMaxEnemyDistance = 1000.f;
			m_tSelectorCover.m_fOptEnemyDistance = 1000.f;
			m_tSelectorCover.m_fMinEnemyDistance = 0.f;
		}
	}
	else {
		m_tSelectorCover.m_fMaxEnemyDistance = m_tEnemy.Enemy->Position().distance_to(vPosition) + m_tSelectorCover.m_fSearchRange;
		m_tSelectorCover.m_fOptEnemyDistance = m_tSelectorCover.m_fMaxEnemyDistance;
		m_tSelectorCover.m_fMinEnemyDistance = m_tEnemy.Enemy->Position().distance_to(vPosition) + 3.f;
	}

	Fvector						tPoint;
	m_tEnemy.Enemy->clCenter	(tPoint);
	vfSetParameters				(
		&m_tSelectorCover,
		0,
		eWeaponStateIdle,
		ePathTypeStraight,
		eBodyStateStand,
		eMovementTypeWalkDanger,
		eLookTypeFirePoint,
		tPoint);
}

void CAI_Stalker::Think()
{
	bool					A,B,C,D,E,F,G,H,I,J,K,L,M;
	
	m_dwLastUpdate			= m_dwCurrentUpdate;
	m_dwCurrentUpdate		= Level().timeServer();
	m_bStopThinking			= false;
	
	m_tEnemy.Enemy			= 0;
	vfUpdateDynamicObjects	();
	vfUpdateParameters		(A,B,C,D,E,F,G,H,I,J,K,L,M);
	int						iIndex;
	if (!K && _K && (((iIndex = ifFindDynamicObject(m_tSavedEnemy)) != -1) && (Level().timeServer() - m_tpaDynamicObjects[iIndex].dwTime < 20000)) && m_tpaDynamicObjects[iIndex].tpEntity->g_Alive())
		K = true;
//	Msg("[A=%d][B=%d][C=%d][D=%d][E=%d][F=%d][G=%d][H=%d][I=%d][J=%d][K=%d][L=%d][M=%d]",A,B,C,D,E,F,G,H,I,J,K,L,M);
	m_dwUpdateCount++;
	m_ePreviousState		= m_eCurrentState;
	
	if (!g_Alive())
		Death				();
	else
//	if (C && H && I) {
//		BackDodge		();
//	} else
//	if (C && H && !I) {
//		ForwardStraight	();
//	} else
//	if (C && !H && I) {
//		BackDodge		();
//	} else
//	if (C && !H && !I) {
//		ForwardStraight	();
//	} else
//	
//	if (D && H && I) {
//		ForwardStraight	();
//	} else
//	if (D && H && !I) {
//		ForwardStraight	();
//	} else
//	if (D && !H && I) {
//		ForwardStraight	();
//	} else
//	if (D && !H && !I) {
//		ForwardStraight	();
//	} else
//	
//	if (E && H && I) {
//		ForwardStraight	();
//	} else
//	if (E && H && !I) {
//		ForwardStraight	();
//	} else
//	if (E && !H && I) {
//		ForwardStraight	();
//	} else
//	if (E && !H && !I) {
//		ForwardStraight	();
//	} else
//	
//	if (F && H && I) {
//		ForwardStraight	();
//	} else
//	if (F && H && !I) {
//		ForwardStraight	();
//	} else
//	if (F && !H && I) {
//		ForwardStraight	();
//	} else
//	if (F && !H && !I) {
//		ForwardStraight	();
//	} else
//	
//	if (G && H && I) {
//		ForwardStraight	();
//	} else
//	if (G && H && !I) {
//		ForwardStraight	();
//	} else
//	if (G && !H && I) {
//		ForwardStraight	();
//	} else
//	if (G && !H && !I) {
//		ForwardStraight	();
//	} else
//	
//	if (A && !K && !H && !L) {
//		ForwardStraight	();
//	} else
//	if (A && !K && H && !L) {
//		ForwardStraight	();
//	} else
//	if (A && !K && !H && L) {
//		ForwardStraight	();
//	} else
//	if (A && !K && H && L) {
//		ForwardStraight	();
//	} else
//	
//	if (B && !K && !H && !L) {
//		ForwardStraight	();
//	} else
//	if (B && !K && H && !L) {
//		ForwardStraight	();
//	} else
//	if (B && !K && !H && L) {
//		ForwardStraight	();
//	} else
//	if (B && !K && H && L) {
//		ForwardStraight	();
//	} else
	
	if (K) {
		if (m_tEnemy.Enemy) {
//			if (C) {
//				m_bStateChanged = C != _C;
//				Msg("Back dodge");
//				BackDodge();
//			} else
//			if (D) {
//				m_bStateChanged = D != _D;
//				Msg("Back cover");
//				BackCover();
//			} else
//			if (E) {
//				m_bStateChanged = E != _E;
//				Msg("Forward cover");
//				ForwardCover();
//			} else
//			if (F) {
//				m_bStateChanged = F != _F;
//				Msg("Forward dodge");
//				ForwardDodge();
//			} else
//			if (G) {
//				m_bStateChanged = G != _G;
//				Msg("Forward straight");
//				ForwardStraight();
//			}
			Msg("Hide");
			Hide();
		} else {
			C = _C;
			E = _E;
			D = _D;
			F = _F;
			G = _G;
			Msg("Camping");
			Hide();
//			Camp();
		}

	} else
	if (A | B) {
		Fvector					tPoint = m_tpaDynamicSounds[m_iSoundIndex].tSavedPosition;
		AI_Path.DestNode		= m_tpaDynamicSounds[m_iSoundIndex].dwNodeID;
		tPoint.y				= getAI().ffGetY(*m_tpItemToTake->AI_Node,tPoint.x,tPoint.z);
		vfSetParameters(0,&tPoint,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalkDanger,eLookTypePoint,tPoint);
	} else
	if (M) {
		Fvector					tPoint;
		m_tpItemToTake->svCenter(tPoint);
		AI_Path.DestNode		= m_tpItemToTake->AI_NodeID;
		tPoint.y				= getAI().ffGetY(*m_tpItemToTake->AI_Node,tPoint.x,tPoint.z);
		vfSetParameters(0,&tPoint,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalkDanger,eLookTypePoint,tPoint);
	} else
	{
		vfUpdateSearchPosition	();
		AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
		vfSetParameters(0,0,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalkFree,eLookTypeSearch);
	}
	
	m_bStateChanged			= m_ePreviousState != m_eCurrentState;

	_A	= A;
	_B	= B;
	_C	= C;
	_D	= D;
	_E	= E;
	_F	= F;
	_G	= G;
	_H	= H;
	_I	= I;
	_J	= J;
	_K	= K;
	_L	= L;
	_M	= M;
}