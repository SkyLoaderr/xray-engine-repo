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
#include "..\\..\\hudmanager.h"

/**
	Msg("Path state : %s",(m_tPathState == ePathStateSearchNode) ? "Searching for the node" : (m_tPathState == ePathStateBuildNodePath) ? "Building path" : (m_tPathState == ePathStateBuildTravelLine) ? "Building travel line" : "Dodging travel line");\
	Msg("Monster %s : \n* State : %s\n* Time delta : %7.3f\n* Global time : %7.3f",cName(),s,m_fTimeUpdateDelta,float(Level().timeServer())/1000.f);\
/**/

#undef	WRITE_TO_LOG
#define WRITE_TO_LOG(s) {\
	Msg("Monster %s : \n* State : %s",cName(),s);\
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

void CAI_Stalker::BackCover(bool bFire)
{
	WRITE_TO_LOG				("Back cover");
	
	m_dwInertion				= bFire ? 20000 : 60000;
	
	m_tSelectorCover.m_fMinEnemyDistance = m_tSavedEnemyPosition.distance_to(vPosition) + 3.f;
	
	if (m_bStateChanged) {
		m_dwActionStartTime = Level().timeServer() + ::Random.randI(4000,7000);
		m_tActionState = eActionStateStand;
	}

	if (m_tActionState == eActionStateRun) {
		bool bTurn = (m_dwActionStartTime - Level().timeServer()) < 1000;
		vfSetParameters				(
			&m_tSelectorCover,
			0,
			true,
			eWeaponStateIdle,
			ePathTypeDodgeCriteria,
			eBodyStateStand,
			m_tSavedEnemyPosition.distance_to(vPosition) > 0.f ? eMovementTypeRun : eMovementTypeWalk,
			eStateTypeDanger,
			bTurn ? eLookTypeFirePoint : eLookTypeDirection,
			m_tSavedEnemyPosition);
		if (Level().timeServer() >= m_dwActionStartTime) {
			m_tActionState = eActionStateStand;
			m_dwActionStartTime = Level().timeServer() + ::Random.randI(4000,7000);
		}
	}
	else {
		Fvector						tPoint;
		if (m_tEnemy.Enemy)
			m_tEnemy.Enemy->clCenter	(tPoint);
		else
			tPoint					= m_tSavedEnemyPosition;
		m_dwRandomFactor			= 50;
		vfSetParameters				(
			&m_tSelectorCover,
			0,
			true,
			bFire ? eWeaponStatePrimaryFire : eWeaponStateIdle,
			ePathTypeDodgeCriteria,
			eBodyStateCrouch,
			eMovementTypeStand,
			eStateTypeDanger,
			eLookTypeFirePoint,
			tPoint);
		if (!m_tEnemy.Enemy && getAI().bfTooSmallAngle(r_torso_current.yaw, r_torso_target.yaw,PI_DIV_6) && bfCheckIfCanKillTarget(vPosition,m_tSavedEnemyPosition,-r_torso_current.yaw,-r_torso_current.pitch,ffGetFov()/180.f*PI)) {
			Camp(bFire);
			return;
		}
		else
			if (Level().timeServer() >= m_dwActionStartTime) {
				m_tActionState = eActionStateRun;
				m_dwActionStartTime = Level().timeServer() + ::Random.randI(4000,7000);
			}
	}
}

void CAI_Stalker::ForwardStraight()
{
	WRITE_TO_LOG("Forward straight");
	
	m_dwInertion				= 20000;
	if (!m_tEnemy.Enemy) {
		Camp(true);
		return;
	}
	Fvector						tPoint;
	m_tEnemy.Enemy->clCenter	(tPoint);
	float						fDistance = vPosition.distance_to(m_tEnemy.Enemy->Position());

	CWeapon						*tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
	if (tpWeapon) {
		m_tSelectorFreeHunting.m_fMaxEnemyDistance = tpWeapon->m_fMaxRadius;
		m_tSelectorFreeHunting.m_fMinEnemyDistance = 6.f;//tpWeapon->m_fMinRadius;
		m_tSelectorFreeHunting.m_fOptEnemyDistance = 6.f;//tpWeapon->m_fMinRadius + 3.f;
	}

	vfSetParameters				(
		&m_tSelectorFreeHunting,
		0,
		true,
		eWeaponStatePrimaryFire,
		fDistance > 15.f ? ePathTypeStraightDodge : ePathTypeCriteria,
		eBodyStateStand,
		m_tEnemy.Enemy->Position().distance_to(vPosition) > 15.f ? eMovementTypeRun : eMovementTypeWalk,
		eStateTypeDanger,
		eLookTypeFirePoint,
		tPoint);
}

void CAI_Stalker::Camp(bool bWeapon)
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
			true,
			eWeaponStateIdle,
			ePathTypeStraight,
			eBodyStateCrouch,
			eMovementTypeWalk,
			eStateTypeDanger,
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
	
	m_dwInertion				= 60000;
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

	m_dwRandomFactor			= 50;
	vfSetParameters				(
		&m_tSelectorFreeHunting,
		0,
		true,
		eWeaponStateIdle,
		ePathTypeStraightDodge,
		eBodyStateStand,
		eMovementTypeRun,
		eStateTypePanic,
		eLookTypeDirection);
}

void CAI_Stalker::Hide()
{
	WRITE_TO_LOG				("Hide");

	m_dwInertion				= 60000;
	if (!m_tEnemy.Enemy) {
		Camp(false);
		return;
	}
	m_tSelectorCover.m_fMaxEnemyDistance = m_tEnemy.Enemy->Position().distance_to(vPosition) + m_tSelectorCover.m_fSearchRange;
	m_tSelectorCover.m_fOptEnemyDistance = m_tSelectorCover.m_fMaxEnemyDistance;
	m_tSelectorCover.m_fMinEnemyDistance = m_tEnemy.Enemy->Position().distance_to(vPosition) + 3.f;
	
	Fvector						tPoint;
	m_tEnemy.Enemy->clCenter	(tPoint);
	vfSetParameters				(
		&m_tSelectorCover,
		0,
		true,
		eWeaponStateIdle,
		ePathTypeStraight,
		eBodyStateStand,
		eMovementTypeWalk,
		eStateTypeDanger,
		eLookTypeFirePoint,
		tPoint);
}

void CAI_Stalker::Detour()
{
	WRITE_TO_LOG("Detour");
	vfUpdateSearchPosition();

	INIT_SQUAD_AND_LEADER;
	vfInitSelector(m_tSelectorFreeHunting,Squad,Leader);
	if (m_tEnemy.Enemy) {
		m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tEnemy.Enemy->Position().distance_to(vPosition) + m_tSelectorFreeHunting.m_fSearchRange;
		m_tSelectorFreeHunting.m_fOptEnemyDistance = 50;//vPosition.distance_to(m_tEnemy.Enemy->Position());
		m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tEnemy.Enemy->Position().distance_to(vPosition) + 3.f;

		AI_Path.DestNode			= getAI().m_tpaGraph[m_tNextGP].tNodeID;
		Fvector						tPoint;
		m_tEnemy.Enemy->clCenter	(tPoint);
		vfSetParameters				(
			&m_tSelectorFreeHunting,
			0,
			false,
			eWeaponStateIdle,
			ePathTypeCriteria,
			eBodyStateStand,
			eMovementTypeWalk,
			eStateTypeDanger,
			eLookTypePoint,
			tPoint);
	}
	else {
		vfUpdateSearchPosition	();
		AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
		vfSetParameters(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeNormal,eLookTypeSearch);
	}
}

void CAI_Stalker::ForwardCover()
{
	WRITE_TO_LOG("Forward cover");
	
	if (!m_tEnemy.Enemy) {
		if (Level().timeServer() - m_dwLostEnemyTime > 0*6000)
			SearchEnemy();
		else
			Camp(true);
		return;
	}
	Fvector						tPoint;
	m_tEnemy.Enemy->clCenter	(tPoint);

	if (m_bStateChanged) {
		m_dwInertion				= ::Random.randI(20000,60000);
		float						fDistance = m_tEnemy.Enemy->Position().distance_to(vPosition);
		CWeapon						*tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
		if (tpWeapon)
			m_tSelectorCover.m_fOptEnemyDistance = (tpWeapon->m_fMinRadius + 0*tpWeapon->m_fMaxRadius)/1;
		m_tSelectorCover.m_fMaxEnemyDistance = max(fDistance - 1.f,m_tSelectorCover.m_fOptEnemyDistance + 3.f);
		m_tSelectorCover.m_fMinEnemyDistance = max(fDistance - m_tSelectorCover.m_fSearchRange,m_tSelectorCover.m_fOptEnemyDistance - 3.f);
		m_dwLastRangeSearch = 0;
		m_tActionState = eActionStateRun;
		m_dwActionStartTime = Level().timeServer();
	}
	m_dwRandomFactor			= 50;
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
				true,
				eWeaponStatePrimaryFire,
				ePathTypeDodgeCriteria,
				eBodyStateStand,
				eMovementTypeRun,
				eStateTypeDanger,
				eLookTypeFirePoint,
				tPoint);
			if ((m_bIfSearchFailed && (AI_Path.fSpeed < EPS_L)) || (Level().timeServer() - m_dwActionStartTime > 2000)) {
				m_dwActionStartTime = Level().timeServer();
				m_tActionState = eActionStateStand;
			}
			break;
		}
		case eActionStateStand : {
			float						fDistance = m_tEnemy.Enemy->Position().distance_to(vPosition);
			CWeapon						*tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
			if (tpWeapon)
				m_tSelectorCover.m_fOptEnemyDistance = (tpWeapon->m_fMinRadius + 0*tpWeapon->m_fMaxRadius)/1;
			m_tSelectorCover.m_fMaxEnemyDistance = max(fDistance - 1.f,m_tSelectorCover.m_fOptEnemyDistance + 3.f);
			m_tSelectorCover.m_fMinEnemyDistance = max(fDistance - m_tSelectorCover.m_fSearchRange,m_tSelectorCover.m_fOptEnemyDistance - 3.f);

			vfSetParameters				(
				&m_tSelectorCover,
				0,
				true,
				eWeaponStatePrimaryFire,
				ePathTypeDodgeCriteria,
				eBodyStateCrouch,
				eMovementTypeStand,
				eStateTypeDanger,
				eLookTypeFirePoint,
				tPoint);
			
			if (!tpWeapon || (tpWeapon->STATE != CWeapon::eFire) && !tpWeapon->GetAmmoElapsed() && (!m_bIfSearchFailed || (!AI_Path.TravelPath.empty() && AI_Path.TravelPath.size() > AI_Path.TravelStart + 1))) {
				m_tActionState			= eActionStateRun;
				m_dwActionStartTime		= Level().timeServer();
			}
			break;
		}
		default : m_tActionState = eActionStateRun;
	}
}

void CAI_Stalker::SearchEnemy()
{
	WRITE_TO_LOG				("Search enemy");
	
	m_dwInertion				= 60000;

	//if (!Group.m_tpaSuspiciousNodes.size()) 
	{
		//vfFindAllSuspiciousNodes(dwSavedEnemyNodeID,tSavedEnemyPosition,tSavedEnemyPosition,_min(20.f,_min(1*8.f*vPosition.distance_to(tSavedEnemyPosition)/4.5f,60.f)),Group);
		//vfClasterizeSuspiciousNodes(Group);
		m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tSavedEnemyPosition.distance_to(vPosition) - 3.f;
		m_tSelectorFreeHunting.m_fMinEnemyDistance = max(m_tSelectorFreeHunting.m_fMaxEnemyDistance - m_tSelectorFreeHunting.m_fSearchRange,0.f);
		m_tSelectorFreeHunting.m_fOptEnemyDistance = m_tSelectorFreeHunting.m_fMinEnemyDistance;
		vfSetParameters				(
			&m_tSelectorFreeHunting,
			&m_tSavedEnemyPosition,
			true,
			eWeaponStateIdle,
			ePathTypeStraight,
			eBodyStateStand,
			eMovementTypeWalk,
			eStateTypeDanger,
			eLookTypeFirePoint,
			m_tSavedEnemyPosition);
	}
//	else {
//		vfChooseSuspiciousNode(m_tSelectorFreeHunting);
//	}
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
	if (!K && _K && (((iIndex = ifFindDynamicObject(m_tSavedEnemy)) != -1) && (Level().timeServer() - m_tpaDynamicObjects[iIndex].dwTime < m_dwInertion)) && m_tpaDynamicObjects[iIndex].tpEntity->g_Alive()) {
		K = true;
		C = _C;
		E = _E;
		D = _D;
		F = _F;
		G = _G;
		H = _H;
		I = _I;
	}

	if (!A && _A  && (m_iSoundIndex > -1) && (Level().timeServer() - m_tpaDynamicSounds[m_iSoundIndex].dwTime < m_dwInertion))
		A = _A;

	if (m_tEnemy.Enemy && (_K != K))
		AI_Path.TravelPath.clear();

	m_dwUpdateCount++;
	m_ePreviousState		= m_eCurrentState;

	if (g_Alive())
		Msg("%s : [A=%d][B=%d][C=%d][D=%d][E=%d][F=%d][G=%d][H=%d][I=%d][J=%d][K=%d][L=%d][M=%d]",cName(),A,B,C,D,E,F,G,H,I,J,K,L,M);
	
	Fvector					tPoint;

	m_bStateChanged = K != _K;
	if (!g_Alive()) {
		Death				();
	}
	else 
 	if ((m_dwParticularState != -1) && (K || (m_dwParticularState == 7))) {
		switch (m_dwParticularState) {
			case 0 : {
				m_dwRandomFactor = 100;
				ForwardStraight();
				break;
			}
			case 1 : {
				m_dwRandomFactor = 0;
				ForwardStraight();
				break;
			}
			case 2 : {
				m_dwRandomFactor = 100;
				ForwardCover();
				break;
			}
			case 3 : {
				m_dwRandomFactor = 50;
				BackCover();
				break;
			}
			case 4 : {
				m_dwRandomFactor = 50;
				Panic();
				break;
			}
			case 5 : {
				m_dwRandomFactor = 50;
				Hide();
				break;
			}
			case 6 : {
				m_dwRandomFactor = 50;
				Detour();
				break;
			}
			case 7 : {
				m_tBodyState	= eBodyStateStand;
				m_tMovementType = eMovementTypeStand;
				m_tStateType	= eStateTypeDanger;
				break;
			}
			default : {
				break;
			}
		}
	}
	else
	if (C && H && I) {
		m_dwRandomFactor	= 50;
		Panic			();
	} else
	if (C && H && !I) {
		Hide();
	} else
	if (C && !H && I) {
		m_dwRandomFactor	= 50;
		Panic			();
	} else
	if (C && !H && !I) {
		Hide();
	} else
	
	if (D && H && I) {
		m_dwRandomFactor	= 50;
		BackCover	(true);
	} else
	if (D && H && !I) {
		Hide		();
	} else
	if (D && !H && I) {
		m_dwRandomFactor	= 50;
		BackCover	(false);
	} else
	if (D && !H && !I) {
		Hide	();
	} else
	
	if (E && H && I) {
		m_dwRandomFactor	= 0;
		ForwardCover	();
	} else
	if (E && H && !I) {
		m_dwRandomFactor	= 0;
		ForwardCover	();
	} else
	if (E && !H && I) {
		Detour			();
	} else
	if (E && !H && !I) {
		Detour			();
	} else
	
	if (F && H && I) {
		m_dwRandomFactor	= 50;
		ForwardStraight	();
	} else
	if (F && H && !I) {
		m_dwRandomFactor	= 100;
		ForwardStraight	();
	} else
	if (F && !H && I) {
		Detour			();
	} else
	if (F && !H && !I) {
		Detour			();
	} else
	
	if (G && H && I) {
		m_dwRandomFactor	= 75;
		ForwardStraight	();
	} else
	if (G && H && !I) {
		m_dwRandomFactor	= 100;
		ForwardStraight	();
	} else
	if (G && !H && I) {
		Detour			();
	} else
	if (G && !H && !I) {
		Detour			();
	} else
	
	if (A && !K && !H && !L) {
		WRITE_TO_LOG			("Exploring danger non-expedient sound");
		m_dwInertion			= 20000;
		tPoint					= m_tpaDynamicSounds[m_iSoundIndex].tSavedPosition;
		AI_Path.DestNode		= m_tpaDynamicSounds[m_iSoundIndex].dwNodeID;
		if (getAI().bfInsideNode(getAI().Node(AI_Path.DestNode),tPoint))
			tPoint.y			= getAI().ffGetY(*getAI().Node(AI_Path.DestNode),tPoint.x,tPoint.z);
		else
			tPoint				= getAI().tfGetNodeCenter(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID);
		vfSetParameters(0,&tPoint,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypeFirePoint,tPoint);
	} else
	if (A && !K && H && !L) {
		WRITE_TO_LOG			("Exploring danger expedient sound");
		m_dwInertion			= 20000;
		tPoint					= m_tpaDynamicSounds[m_iSoundIndex].tSavedPosition;
		AI_Path.DestNode		= m_tpaDynamicSounds[m_iSoundIndex].dwNodeID;
		if (getAI().bfInsideNode(getAI().Node(AI_Path.DestNode),tPoint))
			tPoint.y			= getAI().ffGetY(*getAI().Node(AI_Path.DestNode),tPoint.x,tPoint.z);
		else
			tPoint				= getAI().tfGetNodeCenter(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID);
		vfSetParameters(0,&tPoint,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypeFirePoint,tPoint);
	} else
	if (A && !K && !H && L) {
		WRITE_TO_LOG			("Exploring danger non-expedient sound");
		m_dwInertion			= 20000;
		tPoint					= m_tpaDynamicSounds[m_iSoundIndex].tSavedPosition;
		AI_Path.DestNode		= m_tpaDynamicSounds[m_iSoundIndex].dwNodeID;
		if (getAI().bfInsideNode(getAI().Node(AI_Path.DestNode),tPoint))
			tPoint.y			= getAI().ffGetY(*getAI().Node(AI_Path.DestNode),tPoint.x,tPoint.z);
		else
			tPoint				= getAI().tfGetNodeCenter(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID);
		vfSetParameters(0,&tPoint,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypeFirePoint,tPoint);
	} else
	if (A && !K && H && L) {
		WRITE_TO_LOG			("Exploring danger expedient sound");
		m_dwInertion			= 20000;
		tPoint					= m_tpaDynamicSounds[m_iSoundIndex].tSavedPosition;
		AI_Path.DestNode		= m_tpaDynamicSounds[m_iSoundIndex].dwNodeID;
		if (getAI().bfInsideNode(getAI().Node(AI_Path.DestNode),tPoint))
			tPoint.y			= getAI().ffGetY(*getAI().Node(AI_Path.DestNode),tPoint.x,tPoint.z);
		else
			tPoint				= getAI().tfGetNodeCenter(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID);
		vfSetParameters(0,&tPoint,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypeFirePoint,tPoint);
	} else
	
	if (B && !K && !H && !L) {
		WRITE_TO_LOG			("Exploring non-danger non-expedient sound");
		m_dwInertion			= 10000;
		tPoint					= m_tpaDynamicSounds[m_iSoundIndex].tSavedPosition;
		AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
		tPoint.y				= getAI().ffGetY(*getAI().Node(AI_Path.DestNode),tPoint.x,tPoint.z);
		vfSetParameters(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypePoint,tPoint);
	} else
	if (B && !K && H && !L) {
		WRITE_TO_LOG			("Exploring non-danger expedient sound");
		m_dwInertion			= 10000;
		tPoint					= m_tpaDynamicSounds[m_iSoundIndex].tSavedPosition;
		AI_Path.DestNode		= m_tpaDynamicSounds[m_iSoundIndex].dwNodeID;
		if (getAI().bfInsideNode(getAI().Node(AI_Path.DestNode),tPoint))
			tPoint.y			= getAI().ffGetY(*getAI().Node(AI_Path.DestNode),tPoint.x,tPoint.z);
		else
			tPoint				= getAI().tfGetNodeCenter(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID);
		vfSetParameters(0,&tPoint,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypeFirePoint,tPoint);
	} else
	if (B && !K && !H && L) {
		WRITE_TO_LOG			("Exploring non-danger non-expedient sound");
		m_dwInertion			= 10000;
		tPoint					= m_tpaDynamicSounds[m_iSoundIndex].tSavedPosition;
		AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
		tPoint.y				= getAI().ffGetY(*getAI().Node(AI_Path.DestNode),tPoint.x,tPoint.z);
		vfSetParameters(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypePoint,tPoint);
	} else
	if (B && !K && H && L) {
		WRITE_TO_LOG			("Exploring non-danger expedient sound");
		m_dwInertion			= 10000;
		tPoint					= m_tpaDynamicSounds[m_iSoundIndex].tSavedPosition;
		AI_Path.DestNode		= m_tpaDynamicSounds[m_iSoundIndex].dwNodeID;
		if (getAI().bfInsideNode(getAI().Node(AI_Path.DestNode),tPoint))
			tPoint.y			= getAI().ffGetY(*getAI().Node(AI_Path.DestNode),tPoint.x,tPoint.z);
		else
			tPoint				= getAI().tfGetNodeCenter(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID);
		vfSetParameters(0,&tPoint,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypeFirePoint,tPoint);
	} else
	if (M) {
		WRITE_TO_LOG			("Searching for items");
		// taking items
		m_tpItemToTake->svCenter(tPoint);
		AI_Path.DestNode		= m_tpItemToTake->AI_NodeID;
		if (getAI().bfInsideNode(getAI().Node(AI_Path.DestNode),tPoint))
			tPoint.y			= getAI().ffGetY(*getAI().Node(AI_Path.DestNode),tPoint.x,tPoint.z);
		else
			tPoint				= getAI().tfGetNodeCenter(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID);
		vfSetParameters(0,&tPoint,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypePoint,tPoint);
	} else
	{
		// going via graph nodes
		WRITE_TO_LOG			("Accomplishing task");
		vfUpdateSearchPosition	();
		AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
		float					yaw,pitch;
		GetDirectionAngles		(yaw,pitch);
		yaw						= angle_normalize_signed(-yaw);
		if (getAI().bfTooSmallAngle(r_torso_current.yaw,yaw,PI_DIV_6))
			vfSetParameters(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeNormal,eLookTypeSearch);
		else
			vfSetParameters(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeNormal,eLookTypeDirection);
	}
	
	if (J && (vPosition.distance_to(tPoint) < 5.f))
		J = A = B = false;

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