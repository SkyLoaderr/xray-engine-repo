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

void CAI_Stalker::Think()
{
	bool					A,B,C,D,E,F,G,H,I,J,K,L,M;
	
	m_dwLastUpdate			= m_dwCurrentUpdate;
	m_dwCurrentUpdate		= Level().timeServer();
	m_bStopThinking			= false;
	
	m_tEnemy.Enemy			= 0;
	vfUpdateDynamicObjects	();
	vfUpdateParameters		(A,B,C,D,E,F,G,H,I,J,K,L,M);
	Msg("[A=%d][B=%d][C=%d][D=%d][E=%d][F=%d][G=%d][H=%d][I=%d][J=%d][K=%d][L=%d][M=%d]",A,B,C,D,E,F,G,H,I,J,K,L,M);
	m_dwUpdateCount++;
	m_ePreviousState		= m_eCurrentState;
	
	if (!g_Alive())
		Death				();
	else
//	if (C && H && I) {
//		ForwardStraight	();
//	} else
//	if (C && H && !I) {
//		ForwardStraight	();
//	} else
//	if (C && !H && I) {
//		ForwardStraight	();
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
//		BackDodge();

//		BackCover();

		if ((m_tActionState != eActionStateStand) && (m_tActionState != eActionStateRun))
			m_bStateChanged = true;
		ForwardCover();
		
//		ForwardDodge();
		
//		ForwardStraight();
	} else
	if (M) {
		m_tSelectorFreeHunting.m_fMaxEnemyDistance	= vPosition.distance_to(m_tpItemToTake->Position());
		m_tSelectorFreeHunting.m_fMinEnemyDistance	= 0;
		m_tSelectorFreeHunting.m_fOptEnemyDistance	= 0;
		m_tSelectorFreeHunting.m_tEnemy				= 0;
		m_tSelectorFreeHunting.m_tEnemyPosition		= m_tpItemToTake->Position();
		vfSetParameters(&m_tSelectorFreeHunting,&(m_tpItemToTake->Position()),eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eLookTypePoint,m_tpItemToTake->Position());
	} else
	{
		vfUpdateSearchPosition	();
		AI_Path.DestNode = getAI().m_tpaGraph[m_tNextGP].tNodeID;
		vfSetParameters(0,0,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eLookTypeSearch);
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
};

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
	
	SelectEnemy					(m_tEnemy);

	m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tEnemy.Enemy->Position().distance_to(vPosition) + m_tSelectorRetreat.m_fSearchRange;
	m_tSelectorFreeHunting.m_fOptEnemyDistance = m_tSelectorFreeHunting.m_fMaxEnemyDistance;
	m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tEnemy.Enemy->Position().distance_to(vPosition) + 3.f;

	Fvector						tPoint;
	m_tEnemy.Enemy->svCenter	(tPoint);
	vfSetParameters				(
		&m_tSelectorFreeHunting,
		0,
		eWeaponStateIdle,
		ePathTypeDodge,
		eBodyStateStand,
		eMovementTypeWalk,
		eLookTypeFirePoint,
		tPoint);
}

void CAI_Stalker::BackCover()
{
	WRITE_TO_LOG				("Back cover");
	
	SelectEnemy					(m_tEnemy);

	m_tSelectorCover.m_fMinEnemyDistance = m_tEnemy.Enemy->Position().distance_to(vPosition) + 3.f;

	Fvector						tPoint;
	m_tEnemy.Enemy->svCenter	(tPoint);
	vfSetParameters				(
		&m_tSelectorCover,
		0,
		eWeaponStateIdle,
		ePathTypeCriteria,
		eBodyStateStand,
		eMovementTypeRun,
		eLookTypeFirePoint,
		tPoint);
}

void CAI_Stalker::ForwardCover()
{
	WRITE_TO_LOG("Back cover");
	
	SelectEnemy					(m_tEnemy);
	if (!m_tEnemy.Enemy)
		return;
	Fvector						tPoint;
	m_tEnemy.Enemy->svCenter	(tPoint);

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
			Msg							("State RUN");
			
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
				eMovementTypeRun,
				eLookTypeFirePoint,
				tPoint);
			if (m_bIfSearchFailed && (AI_Path.fSpeed < EPS_L))
				m_tActionState = eActionStateStand;
			break;
		}
		case eActionStateStand : {
			Msg							("State STAND");
			
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
				eMovementTypeStand,
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
	
	SelectEnemy					(m_tEnemy);
	if (!m_tEnemy.Enemy)
		return;
	Fvector						tPoint;
	m_tEnemy.Enemy->svCenter	(tPoint);
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
		eMovementTypeRun,
		eLookTypeFirePoint,
		tPoint);
}

void CAI_Stalker::ForwardDodge()
{
	WRITE_TO_LOG("Forward dodge");
	
	SelectEnemy					(m_tEnemy);
	Fvector						tPoint;
	m_tEnemy.Enemy->svCenter	(tPoint);
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
		eMovementTypeRun,
		eLookTypeFirePoint,
		tPoint);
}