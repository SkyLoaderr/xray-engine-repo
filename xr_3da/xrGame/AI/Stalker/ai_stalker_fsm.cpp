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
	
//	if (m_bStateChanged) {
//		m_dwActionStartTime = Level().timeServer() + ::Random.randI(4000,7000);
//		m_tActionState = eActionStateStand;
//	}
//
//	if (m_tActionState == eActionStateRun) {
//		bool bTurn = (m_dwActionStartTime - Level().timeServer()) < 1000;
//		vfSetParameters				(&m_tSelectorCover,0,true,eWeaponStateIdle,ePathTypeDodgeCriteria,eBodyStateStand,m_tSavedEnemyPosition.distance_to(vPosition) > 0.f ? eMovementTypeRun : eMovementTypeWalk,eStateTypeDanger,bTurn ? eLookTypeFirePoint : eLookTypeDirection,m_tSavedEnemyPosition);
//		if (Level().timeServer() >= m_dwActionStartTime) {
//			m_tActionState = eActionStateStand;
//			m_dwActionStartTime = Level().timeServer() + ::Random.randI(4000,7000);
//		}
//	}
//	else {
//		Fvector						tPoint;
//		if (m_tEnemy.Enemy)
//			m_tEnemy.Enemy->clCenter	(tPoint);
//		else
//			tPoint					= m_tSavedEnemyPosition;
//		m_dwRandomFactor			= 50;
//		vfSetParameters				(&m_tSelectorCover,0,true,bFire ? eWeaponStatePrimaryFire : eWeaponStateIdle,ePathTypeDodgeCriteria,eBodyStateCrouch,eMovementTypeStand,eStateTypeDanger,eLookTypeFirePoint,tPoint);
//		if (!m_tEnemy.Enemy && getAI().bfTooSmallAngle(r_torso_current.yaw, r_torso_target.yaw,PI_DIV_6) && bfCheckIfCanKillTarget(this,m_tSavedEnemyPosition,-r_torso_current.yaw,-r_torso_current.pitch,ffGetFov()/180.f*PI)) {
//			Camp(bFire);
//			return;
//		}
//		else
//			if (Level().timeServer() >= m_dwActionStartTime) {
//				m_tActionState = eActionStateRun;
//				m_dwActionStartTime = Level().timeServer() + ::Random.randI(4000,7000);
//			}
//	}
	if (m_bStateChanged) {
		m_tActionState			= eActionStateWatchLook;
		m_dwActionStartTime		= Level().timeServer();
	}
	Fvector						tPoint;
	if (m_tEnemy.Enemy)
		m_tEnemy.Enemy->clCenter	(tPoint);
	else
		tPoint					=	m_tSavedEnemyPosition;
	switch (m_tActionState) {
		case eActionStateWatchGo : {
			WRITE_TO_LOG			("WatchGo : Detour");
			vfSetParameters			(&m_tSelectorCover,0,true,eWeaponStateIdle,ePathTypeDodgeCriteria,eBodyStateStand,m_tSavedEnemyPosition.distance_to(vPosition) > 0.f ? eMovementTypeRun : eMovementTypeWalk,eStateTypeDanger,eLookTypeDirection);
			if ((Level().timeServer() - m_dwActionStartTime > 4000) && ((getAI().dwfCheckPositionInDirection(AI_NodeID,vPosition,tPoint) != u32(-1)) || (Level().timeServer() - m_dwActionStartTime > 8000))) {
				m_tActionState		= eActionStateWatchLook;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatchLook : {
			WRITE_TO_LOG			("WatchLook : Detour");
			if (!m_tEnemy.Enemy && getAI().bfTooSmallAngle(r_torso_current.yaw, r_torso_target.yaw,PI_DIV_6) && bfCheckIfCanKillTarget(this,m_tSavedEnemyPosition,-r_torso_current.yaw,-r_torso_current.pitch,ffGetFov()/180.f*PI))
				Camp(bFire);
			else {
				vfSetParameters			(&m_tSelectorCover,0,true,bFire ? eWeaponStatePrimaryFire : eWeaponStateIdle,ePathTypeDodgeCriteria,eBodyStateCrouch,eMovementTypeStand,eStateTypeDanger,eLookTypeFirePoint,tPoint);
				if (Level().timeServer() - m_dwActionStartTime > 3000) {
					m_tActionState		= eActionStateWatchGo;
					m_dwActionStartTime = Level().timeServer();
				}
			}
			break;
		}
		default : NODEFAULT;
	}
}

void CAI_Stalker::ForwardStraight()
{
	WRITE_TO_LOG("Forward straight");
	
	m_dwInertion				= 20000;
	if (!m_tEnemy.Enemy) {
		SearchEnemy();
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

	vfSetParameters				(&m_tSelectorFreeHunting,0,true,eWeaponStatePrimaryFire,fDistance > 15.f ? ePathTypeStraightDodge : ePathTypeCriteria,eBodyStateStand,m_tEnemy.Enemy->Position().distance_to(vPosition) > 15.f ? eMovementTypeRun : eMovementTypeWalk,eStateTypeDanger,eLookTypeFirePoint,tPoint);
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
		vfSetParameters			(0,&(m_tpaDynamicObjects[iIndex].tMySavedPosition),true,eWeaponStateIdle,ePathTypeStraight,eBodyStateCrouch,eMovementTypeWalk,eStateTypeDanger,eLookTypeFirePoint,m_tpaDynamicObjects[iIndex].tSavedPosition);
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
	vfSetParameters				(&m_tSelectorFreeHunting,0,true,eWeaponStateIdle,ePathTypeStraightDodge,eBodyStateStand,eMovementTypeRun,eStateTypePanic,eLookTypeDirection);
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
	vfSetParameters				(&m_tSelectorCover,0,true,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypeFirePoint,tPoint);
}

void CAI_Stalker::Detour()
{
	WRITE_TO_LOG("Detour");

	m_dwInertion				= 60000;
	if (m_bStateChanged) {
		m_tActionState = eActionStateWatchLook;
		m_dwActionStartTime = Level().timeServer();
		AI_Path.TravelPath.clear();
	}
	
	Fvector tPoint;
	if (m_tEnemy.Enemy)
		m_tEnemy.Enemy->clCenter(tPoint);
	else
		tPoint = m_tSavedEnemyPosition;
	m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tSavedEnemyPosition.distance_to(vPosition) + m_tSelectorFreeHunting.m_fSearchRange;
	m_tSelectorFreeHunting.m_fOptEnemyDistance = 15;
	m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tSavedEnemyPosition.distance_to(vPosition) + 3.f;

	u32 dwDelay1, dwDelay2;
	float f;
	if (C)	f =100.f;
	if (D)	f =80.f;
	if (E)	f =60.f;
	if (F)	f =40.f;
	if (G)	f =20.f;
	
	float fDistance = min(vPosition.distance_to(m_tSavedEnemyPosition),f);
	dwDelay1 = iFloor((fDistance / f) * 20000);
	dwDelay2 = max(u32(20000 - dwDelay1),u32(2000));
	switch (m_tActionState) {
		case eActionStateWatchGo : {
			WRITE_TO_LOG			("WatchGo : Detour");
			AccomplishTask			((F || G) ? 0 : &m_tSelectorFreeHunting);
			if ((Level().timeServer() - m_dwActionStartTime > dwDelay1) && (getAI().dwfCheckPositionInDirection(AI_NodeID,vPosition,tPoint) != u32(-1))) {
				m_tActionState		= eActionStateWatchLook;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatchLook : {
			WRITE_TO_LOG			("WatchLook : Detour");
			vfUpdateSearchPosition	();
			AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
			vfSetParameters			((F || G) ? 0 : &m_tSelectorFreeHunting,0,false,eWeaponStateIdle,(F || G) ? ePathTypeStraight : ePathTypeCriteria,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,(F || G) ? eLookTypePoint : eLookTypeFirePoint,tPoint);
			if ((Level().timeServer() - m_dwActionStartTime > dwDelay2) || (getAI().dwfCheckPositionInDirection(AI_NodeID,vPosition,tPoint) == u32(-1))) {
				m_tActionState		= eActionStateWatchGo;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		default : NODEFAULT;
	}
}

void CAI_Stalker::ForwardCover()
{
	WRITE_TO_LOG("Forward cover");
	
	m_dwInertion = 60000;

	if (!m_tEnemy.Enemy) {
		if (Level().timeServer() - m_dwLostEnemyTime > 6000)
			SearchEnemy();
		else
			Camp(true);
		return;
	}
	
	if (m_bStateChanged) {
		m_tActionState			= eActionStateWatchLook;
		m_dwActionStartTime		= Level().timeServer();
		float					fDistance = m_tEnemy.Enemy->Position().distance_to(vPosition);
		CWeapon					*tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
		if (tpWeapon)
			m_tSelectorCover.m_fOptEnemyDistance = (tpWeapon->m_fMinRadius + 0*tpWeapon->m_fMaxRadius)/1;
		m_tSelectorCover.m_fMaxEnemyDistance = max(fDistance - 1.f,m_tSelectorCover.m_fOptEnemyDistance + 3.f);
		m_tSelectorCover.m_fMinEnemyDistance = max(fDistance - m_tSelectorCover.m_fSearchRange,m_tSelectorCover.m_fOptEnemyDistance - 3.f);
		m_dwLastRangeSearch		= 0;
		AI_Path.TravelPath.clear();
	}

	Fvector						tPoint;
	if (m_tEnemy.Enemy)
		m_tEnemy.Enemy->clCenter	(tPoint);
	else
		tPoint					=	m_tSavedEnemyPosition;
	
	bool bBackMove = false;
	if (!AI_Path.TravelPath.empty() && (AI_Path.TravelStart < AI_Path.TravelPath.size() - 1)) {
		Fvector tTemp0, tTemp1;
		tTemp0.sub(m_tSavedEnemyPosition,vPosition);
		tTemp1.sub(AI_Path.TravelPath[AI_Path.TravelStart + 1].P,vPosition);
		float yaw1,yaw2,pitch1,pitch2;
		tTemp0.getHP(yaw1,pitch1);
		tTemp1.getHP(yaw2,pitch2);
		bBackMove = !getAI().bfTooSmallAngle(yaw1,yaw2,4*PI_DIV_6);
	}

	switch (m_tActionState) {
		case eActionStateWatchGo : {
			WRITE_TO_LOG			("WatchGo : Detour");
			if (bBackMove)
				vfSetParameters			(&m_tSelectorCover,0,true,eWeaponStatePrimaryFire,ePathTypeDodgeCriteria,eBodyStateStand,eMovementTypeRun,eStateTypeDanger,eLookTypeDirection);
			else
				vfSetParameters			(&m_tSelectorCover,0,true,eWeaponStatePrimaryFire,ePathTypeDodgeCriteria,eBodyStateStand,eMovementTypeRun,eStateTypeDanger,eLookTypeFirePoint,tPoint);
			if ((m_bIfSearchFailed && (AI_Path.fSpeed < EPS_L)) && (Level().timeServer() - m_dwActionStartTime > 4000) && ((getAI().dwfCheckPositionInDirection(AI_NodeID,vPosition,tPoint) != u32(-1)) || (Level().timeServer() - m_dwActionStartTime > 7000))) {
				m_tActionState		= eActionStateWatchLook;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatchLook : {
			WRITE_TO_LOG			("WatchLook : Detour");
			float					fDistance = m_tEnemy.Enemy->Position().distance_to(vPosition);
			CWeapon					*tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
			if (tpWeapon)
				m_tSelectorCover.m_fOptEnemyDistance = (tpWeapon->m_fMinRadius + 0*tpWeapon->m_fMaxRadius)/1;
			m_tSelectorCover.m_fMaxEnemyDistance = max(fDistance - 1.f,m_tSelectorCover.m_fOptEnemyDistance + 3.f);
			m_tSelectorCover.m_fMinEnemyDistance = max(fDistance - m_tSelectorCover.m_fSearchRange,m_tSelectorCover.m_fOptEnemyDistance - 3.f);
			vfSetParameters			(&m_tSelectorCover,0,true,eWeaponStatePrimaryFire,ePathTypeDodgeCriteria,eBodyStateCrouch,eMovementTypeStand,eStateTypeDanger,eLookTypeFirePoint,tPoint);
			if (!tpWeapon || (tpWeapon->STATE != CWeapon::eFire) && !tpWeapon->GetAmmoElapsed() && (!m_bIfSearchFailed || ((!AI_Path.TravelPath.empty() && AI_Path.TravelPath.size() > AI_Path.TravelStart + 1) && (Level().timeServer() - m_dwActionStartTime > 5000)))) {
				m_tActionState		= eActionStateWatchGo;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		default : {
			m_tActionState = eActionStateWatchLook;
		}
	}
}

void CAI_Stalker::SearchEnemy()
{
	WRITE_TO_LOG				("Search enemy");
	m_dwInertion				= 180000;

	INIT_SQUAD_AND_LEADER;
	CGroup						&Group = *getGroup();
	
	if (m_dwLastEnemySearch != m_dwLastUpdate) {
		m_tActionState			= eActionStateDontWatch;
		m_iCurrentSuspiciousNodeIndex = 0;
		m_bSearchedForEnemy		= true;
		m_tPathState			= ePathStateBuildNodePath;
		Group.m_tpaSuspiciousNodes.clear();
	}
	m_bSearchedForEnemy			= true;

	switch (m_tActionState) {
		case eActionStateDontWatch : {
			Msg("Last enemy position");
			AI_Path.DestNode	= m_dwSavedEnemyNodeID;
			if (Group.m_tpaSuspiciousNodes.empty()) {
				if (!m_iCurrentSuspiciousNodeIndex) {
					vfFindAllSuspiciousNodes(m_dwSavedEnemyNodeID,m_tSavedEnemyPosition,m_tSavedEnemyPosition,40.f,Group);
					m_iCurrentSuspiciousNodeIndex = -1;
				}
			}
			if ((getAI().dwfCheckPositionInDirection(AI_NodeID,vPosition,m_tSavedEnemyPosition) != -1) && bfIf_I_SeePosition(m_tSavedEnemyPosition))
				m_tActionState = eActionStateWatchGo;
			else {
				bool bOk = false;	
				for (u32 i=0; i<Group.m_tpaSuspiciousNodes.size(); i++)
					if (!Group.m_tpaSuspiciousNodes[i].dwSearched) {
						if (getAI().bfCheckNodeInDirection(AI_NodeID,vPosition,Group.m_tpaSuspiciousNodes[i].dwNodeID)) {
							bOk = true;
							vfSetParameters		(0,&m_tSavedEnemyPosition,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeStand,eStateTypeDanger,eLookTypePoint,getAI().tfGetNodeCenter(Group.m_tpaSuspiciousNodes[i].dwNodeID));
							if (bfIf_I_SeePosition(getAI().tfGetNodeCenter(Group.m_tpaSuspiciousNodes[i].dwNodeID)))
								Group.m_tpaSuspiciousNodes[i].dwSearched = 2;
							break;
						}
					}
				if (!bOk)
//					if (getAI().dwfCheckPositionInDirection(AI_NodeID,vPosition,m_tSavedEnemyPosition) == -1)
//						vfSetParameters		(0,&m_tSavedEnemyPosition,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypeSearch);
//					else 
					{
						Fvector				tPoint = m_tSavedEnemyPosition;
						tPoint.y			+= 1.5f;
						vfSetParameters		(0,&m_tSavedEnemyPosition,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypePoint,tPoint);
					}
			}
			break;
		}
		case eActionStateWatchGo : {
			Msg("The higher-level prediction");
			if ((m_iCurrentSuspiciousNodeIndex == -1) || getAI().bfCheckNodeInDirection(AI_NodeID,vPosition,Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID)) {
				if (m_iCurrentSuspiciousNodeIndex != -1)
					Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwSearched = 2;
				m_iCurrentSuspiciousNodeIndex = ifGetSuspiciousAvailableNode(m_iCurrentSuspiciousNodeIndex,Group);
				if (m_iCurrentSuspiciousNodeIndex == -1) {
					Group.m_tpaSuspiciousNodes.clear();
					m_iCurrentSuspiciousNodeIndex = -1;
					m_tActionState = eActionStateWatchLook;
				}
				else {
					Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwSearched = 1;
					AI_Path.DestNode = Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID;
					m_tPathState = ePathStateBuildNodePath;
				}
			}
			
			if (m_iCurrentSuspiciousNodeIndex >= 0) {
				bool bOk = false;	
				for (u32 i=0; i<Group.m_tpaSuspiciousNodes.size(); i++)
					if (!Group.m_tpaSuspiciousNodes[i].dwSearched) {
						if (getAI().bfCheckNodeInDirection(AI_NodeID,vPosition,Group.m_tpaSuspiciousNodes[i].dwNodeID)) {
							bOk = true;
							vfSetParameters		(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeStand,eStateTypeDanger,eLookTypePoint,getAI().tfGetNodeCenter(Group.m_tpaSuspiciousNodes[i].dwNodeID));
							if (bfIf_I_SeePosition(getAI().tfGetNodeCenter(Group.m_tpaSuspiciousNodes[i].dwNodeID)))
								Group.m_tpaSuspiciousNodes[i].dwSearched = 2;
							break;
						}
					}
				if (!bOk)
					if (getAI().dwfCheckPositionInDirection(AI_NodeID,vPosition,m_tSavedEnemyPosition) == -1)
						vfSetParameters		(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypeSearch);
					else {
						Fvector				tPoint = getAI().tfGetNodeCenter(Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID);
						tPoint.y			+= 1.5f;
						vfSetParameters		(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypePoint,tPoint);
					}
			}
	
			break;
		}
		case eActionStateWatchLook : {
			Msg("Predicting by selector");
			m_tSelectorRetreat.m_tEnemyPosition = m_tMySavedPosition;
			m_tSelectorRetreat.m_tpEnemyNode	= getAI().Node(m_dwMyNodeID);
			m_tSelectorRetreat.m_tMyPosition	= vPosition;
			m_tSelectorRetreat.m_tpMyNode		= getAI().Node(AI_NodeID);
			m_tSelectorRetreat.m_fMinEnemyDistance = vPosition.distance_to(m_tMySavedPosition) + 3.f;
			vfSetParameters		(&m_tSelectorRetreat,0,true,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypeDanger);
			if (m_bIfSearchFailed)
				m_dwInertion						= 0;
			else
				m_dwInertion						= 120000;
			break;
		}
	}
}

void CAI_Stalker::ExploreDNE()
{
	m_dwInertion			= 60000;
	Fvector					tPoint = m_tpaDynamicSounds[m_iSoundIndex].tSavedPosition;
	if (getAI().bfInsideNode(getAI().Node(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID),tPoint))
		tPoint.y			= getAI().ffGetY(*getAI().Node(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID),tPoint.x,tPoint.z);
	else
		tPoint				= getAI().tfGetNodeCenter(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID);
	if (m_bStateChanged) {
		m_tActionState		= eActionStateDontWatch;
		m_dwActionStartTime = Level().timeServer();
		AI_Path.TravelPath.clear();
	}
	m_tSelectorFreeHunting.m_fOptEnemyDistance = max(25.f,vPosition.distance_to(tPoint));
	switch (m_tActionState) {
		case eActionStateDontWatch : {
			WRITE_TO_LOG			("DontWatch : Exploring danger non-expedient sound");
			AccomplishTask			(&m_tSelectorFreeHunting);
			if (getAI().dwfCheckPositionInDirection(AI_NodeID,vPosition,tPoint) != u32(-1)) {
				m_tActionState		= eActionStateWatch;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatch : {
			WRITE_TO_LOG			("Watch : Exploring danger non-expedient sound");
			vfSetParameters			(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateCrouch,eMovementTypeStand,eStateTypeDanger,eLookTypeLookFireOver,tPoint,2500);
			if (Level().timeServer() - m_dwActionStartTime > 9000) {
				m_tActionState		= eActionStateWatchGo;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatchGo : {
			WRITE_TO_LOG			("WatchGo : Exploring danger non-expedient sound");
			AccomplishTask			(&m_tSelectorFreeHunting);
			if ((Level().timeServer() - m_dwActionStartTime > 7000) && (getAI().dwfCheckPositionInDirection(AI_NodeID,vPosition,tPoint) != u32(-1))) {
				m_tActionState		= eActionStateWatchLook;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatchLook : {
			WRITE_TO_LOG			("WatchLook : Exploring danger non-expedient sound");
			vfUpdateSearchPosition	();
			AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
			vfSetParameters			(0,0,false,eWeaponStateIdle,ePathTypeCriteria,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypePoint,tPoint);
			if ((Level().timeServer() - m_dwActionStartTime > 5000) || (getAI().dwfCheckPositionInDirection(AI_NodeID,vPosition,tPoint) == u32(-1))) {
				m_tActionState		= eActionStateWatchGo;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		default : NODEFAULT;
	}
}

void CAI_Stalker::ExploreDE()
{
//	m_dwInertion			= 60000;
//	Fvector					tPoint = m_tpaDynamicSounds[m_iSoundIndex].tSavedPosition;
//	if (getAI().bfInsideNode(getAI().Node(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID),tPoint))
//		tPoint.y			= getAI().ffGetY(*getAI().Node(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID),tPoint.x,tPoint.z);
//	else
//		tPoint				= getAI().tfGetNodeCenter(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID);
//	if (m_bStateChanged) {
//		m_tActionState		= eActionStateDontWatch;
//		m_dwActionStartTime = Level().timeServer();
//		AI_Path.TravelPath.clear();
//	}
//	m_tSelectorFreeHunting.m_fOptEnemyDistance = max(15.f,vPosition.distance_to(tPoint));
//	switch (m_tActionState) {
//		case eActionStateDontWatch : {
//			WRITE_TO_LOG			("DontWatch : Exploring danger expedient sound");
//			AccomplishTask			(&m_tSelectorFreeHunting);
//			if (getAI().dwfCheckPositionInDirection(AI_NodeID,vPosition,tPoint) != u32(-1)) {
//				m_tActionState		= eActionStateWatch;
//				m_dwActionStartTime = Level().timeServer();
//			}
//			break;
//		}
//		case eActionStateWatch : {
//			WRITE_TO_LOG			("Watch : Exploring danger expedient sound");
//			if (vPosition.distance_to(tPoint) < 5.f)
//				vfSetParameters			(0,0,false,eWeaponStateIdle,ePathTypeCriteria,eBodyStateCrouch,eMovementTypeStand,eStateTypeDanger,eLookTypeLookFireOver,tPoint,3000);
//			else
//				vfSetParameters			(0,0,false,eWeaponStateIdle,ePathTypeCriteria,eBodyStateCrouch,eMovementTypeWalk,eStateTypeDanger,eLookTypeLookFireOver,tPoint,3000);
//			if (Level().timeServer() - m_dwActionStartTime > 10000) {
//				m_tActionState		= eActionStateWatchGo;
//				m_dwActionStartTime = Level().timeServer();
//			}
//			break;
//		}
//		case eActionStateWatchGo : {
//			WRITE_TO_LOG			("WatchGo : Exploring danger expedient sound");
//			AccomplishTask			(&m_tSelectorFreeHunting);
//			if ((Level().timeServer() - m_dwActionStartTime > 6000) && (getAI().dwfCheckPositionInDirection(AI_NodeID,vPosition,tPoint) != u32(-1))) {
//				m_tActionState		= eActionStateWatchLook;
//				m_dwActionStartTime = Level().timeServer();
//			}
//			break;
//		}
//		case eActionStateWatchLook : {
//			WRITE_TO_LOG			("WatchLook : Exploring danger expedient sound");
//			vfUpdateSearchPosition	();
//			AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
//			vfSetParameters			(0,0,false,eWeaponStateIdle,ePathTypeCriteria,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypePoint,tPoint);
//			if ((Level().timeServer() - m_dwActionStartTime > 6000) || (getAI().dwfCheckPositionInDirection(AI_NodeID,vPosition,tPoint) == u32(-1))) {
//				m_tActionState		= eActionStateWatchGo;
//				m_dwActionStartTime = Level().timeServer();
//			}
//			break;
//		}
//		default : NODEFAULT;
//	}
	m_tSavedEnemyPosition	= m_tpaDynamicSounds[m_iSoundIndex].tSavedPosition;
	m_dwSavedEnemyNodeID	= m_tpaDynamicSounds[m_iSoundIndex].dwNodeID;
	m_tMySavedPosition		= m_tpaDynamicSounds[m_iSoundIndex].tMySavedPosition;
	m_dwMyNodeID			= m_tpaDynamicSounds[m_iSoundIndex].dwMyNodeID;
	if (m_tpaDynamicSounds[m_iSoundIndex].dwTime - m_dwLastSoundUpdate > SOUND_UPDATE_DELAY)
		m_dwLastEnemySearch = 0;
	m_dwLastSoundUpdate		= m_tpaDynamicSounds[m_iSoundIndex].dwTime;
	SearchEnemy();
}

void CAI_Stalker::ExploreNDE()
{
	m_dwInertion			= 60000;

	m_tSavedEnemyPosition	= m_tpaDynamicSounds[m_iSoundIndex].tSavedPosition;
	m_dwSavedEnemyNodeID	= m_tpaDynamicSounds[m_iSoundIndex].dwNodeID;
	m_tMySavedPosition		= m_tpaDynamicSounds[m_iSoundIndex].tMySavedPosition;
	m_dwMyNodeID			= m_tpaDynamicSounds[m_iSoundIndex].dwMyNodeID;
	if (m_tpaDynamicSounds[m_iSoundIndex].dwTime - m_dwLastSoundUpdate > SOUND_UPDATE_DELAY)
		m_dwLastEnemySearch = 0;
	m_dwLastSoundUpdate		= m_tpaDynamicSounds[m_iSoundIndex].dwTime;
	SearchEnemy();
}

void CAI_Stalker::ExploreNDNE()
{
	m_dwInertion			= 60000;
	Fvector					tPoint = m_tpaDynamicSounds[m_iSoundIndex].tSavedPosition;
	if (getAI().bfInsideNode(getAI().Node(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID),tPoint))
		tPoint.y			= getAI().ffGetY(*getAI().Node(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID),tPoint.x,tPoint.z);
	else
		tPoint				= getAI().tfGetNodeCenter(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID);
	if (m_bStateChanged) {
		m_tActionState = eActionStateDontWatch;
		m_dwActionStartTime = Level().timeServer();
		AI_Path.TravelPath.clear();
	}
	switch (m_tActionState) {
		case eActionStateDontWatch : {
			WRITE_TO_LOG			("DontWatch : Exploring non-danger non-expedient sound");
			AccomplishTask();
			if (getAI().dwfCheckPositionInDirection(AI_NodeID,vPosition,tPoint) != u32(-1)) {
				m_tActionState = eActionStateWatch;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatch : {
			WRITE_TO_LOG			("Watch : Exploring non-danger non-expedient sound");
			vfSetParameters			(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeStand,eStateTypeDanger,eLookTypeLookOver,tPoint,1200);
			if (Level().timeServer() - m_dwActionStartTime > 5000) {
				m_tActionState = eActionStateWatchGo;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatchGo : {
			WRITE_TO_LOG			("WatchGo : Exploring non-danger non-expedient sound");
			AccomplishTask();
			if ((Level().timeServer() - m_dwActionStartTime > 10000) && (getAI().dwfCheckPositionInDirection(AI_NodeID,vPosition,tPoint) != u32(-1))) {
				m_tActionState = eActionStateWatchLook;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatchLook : {
			WRITE_TO_LOG			("WatchLook : Exploring non-danger non-expedient sound");
			vfUpdateSearchPosition	();
			AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
			vfSetParameters(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypePoint,tPoint);
			if ((Level().timeServer() - m_dwActionStartTime > 3000) || (getAI().dwfCheckPositionInDirection(AI_NodeID,vPosition,tPoint) == u32(-1))) {
				m_tActionState = eActionStateWatchGo;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		default : NODEFAULT;
	}
}

void CAI_Stalker::TakeItems()
{
	WRITE_TO_LOG			("Taking items");
	// taking items
	if (m_bStateChanged) {
		AI_Path.TravelPath.clear();
	}
	Fvector					tPoint;
	m_tpItemToTake->svCenter(tPoint);
	AI_Path.DestNode		= m_tpItemToTake->AI_NodeID;
	if (getAI().bfInsideNode(getAI().Node(AI_Path.DestNode),tPoint))
		tPoint.y			= getAI().ffGetY(*getAI().Node(AI_Path.DestNode),tPoint.x,tPoint.z);
	else
		tPoint				= getAI().tfGetNodeCenter(AI_Path.DestNode);
	vfSetParameters(0,&tPoint,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypePoint,tPoint);
	if (vPosition.distance_to(tPoint) < 5.f)
		J = A = B = false;
}

void CAI_Stalker::AccomplishTask(IBaseAI_NodeEvaluator *tpNodeEvaluator)
{
	// going via graph nodes
	WRITE_TO_LOG			("Accomplishing task");
	vfUpdateSearchPosition	();
	if (m_bStateChanged)
		AI_Path.TravelPath.clear();

	AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGP].tNodeID;
	float					yaw,pitch;
	GetDirectionAngles		(yaw,pitch);
	yaw						= angle_normalize_signed(-yaw);
	if (getAI().bfTooSmallAngle(r_torso_current.yaw,yaw,PI_DIV_6))
		vfSetParameters(tpNodeEvaluator,0,false,eWeaponStateIdle,!tpNodeEvaluator ? ePathTypeStraight : ePathTypeCriteria,eBodyStateStand,eMovementTypeWalk,eStateTypeNormal,eLookTypeSearch);
	else
		vfSetParameters(tpNodeEvaluator,0,false,eWeaponStateIdle,!tpNodeEvaluator ? ePathTypeStraight : ePathTypeCriteria,eBodyStateStand,eMovementTypeWalk,eStateTypeNormal,eLookTypeDirection);
}

void CAI_Stalker::Think()
{
	m_dwLastUpdate			= m_dwCurrentUpdate;
	m_dwCurrentUpdate		= Level().timeServer();
	m_bStopThinking			= false;
	m_bSearchedForEnemy		= false;
	
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
		vfUpdateVisibilityBySensitivity();
	}
	//if (!K && !J && !M && _M && (tpItemToTake)) 

	if (m_tEnemy.Enemy && (_K != K))
		AI_Path.TravelPath.clear();

	m_dwUpdateCount++;
	m_ePreviousState		= m_eCurrentState;

	if (g_Alive())
		Msg("%s : [A=%d][B=%d][C=%d][D=%d][E=%d][F=%d][G=%d][H=%d][I=%d][J=%d][K=%d][L=%d][M=%d]",cName(),A,B,C,D,E,F,G,H,I,J,K,L,M);
	
	m_bStateChanged = ((_A	!= A) || (_B	!= B) || (_C	!= C) || (_D	!= D) || (_E	!= E) || (_F	!= F) || (_G	!= G) || (_H	!= H) || (_I	!= I) || (_J	!= J) || (_K	!= K) || (_L	!= L));// || (_M	!= M));
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
		ExploreDNE();
	} else
	if (A && !K && H && !L) {
		ExploreDE();
	} else
	if (A && !K && !H && L) {
		ExploreDNE();
	} else
	if (A && !K && H && L) {
		ExploreDE();
	} else
	
	if (B && !K && !H && !L) {
		ExploreNDNE();
	} else
	if (B && !K && H && !L) {
		ExploreNDE();
	} else
	if (B && !K && !H && L) {
		ExploreNDNE();
	} else
	if (B && !K && H && L) {
		ExploreNDE();
	} else
	if (M) {
		TakeItems();
	} else {
		AccomplishTask();
	}
	
	m_bStateChanged			= m_ePreviousState != m_eCurrentState;
	if (m_bSearchedForEnemy)
		m_dwLastEnemySearch		= m_dwCurrentUpdate;
	else {
		getGroup()->m_tpaSuspiciousNodes.clear();
		m_iCurrentSuspiciousNodeIndex = 0;
	}

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