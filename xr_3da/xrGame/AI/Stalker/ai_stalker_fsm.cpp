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
#include "..\\..\\bolt.h"
#include "..\\..\\weapon.h"

#define AI_BEST
/**
	Msg("Speed      : %7.3f",m_fCurSpeed);\
	Msg("Monster %s : \n* State : %s\n* Time delta : %7.3f\n* Global time : %7.3f",cName(),s,m_fTimeUpdateDelta,float(Level().timeServer())/1000.f);\
	Msg("Path state : %s",(m_tPathState == ePathStateSearchNode) ? "Searching for the node" : (m_tPathState == ePathStateBuildNodePath) ? "Building path" : (m_tPathState == ePathStateBuildTravelLine) ? "Building travel line" : "Dodging travel line");\
/**/

#undef	WRITE_TO_LOG
#ifdef SILENCE
	#define WRITE_TO_LOG(s) {\
		m_bPlayHumming = false;\
		m_bStopThinking = true;\
	}
#else
	#define WRITE_TO_LOG(s) {\
		m_bPlayHumming = false;\
		m_bStopThinking = true;\
	}
#endif

#ifndef DEBUG
	#undef	WRITE_TO_LOG
	#define WRITE_TO_LOG(s) m_bStopThinking = true;
#endif

void CAI_Stalker::Death()
{
	WRITE_TO_LOG("Death");
	
//#ifdef DEBUG
//	Msg								("Death position : [%f][%f][%f]",VPUSH(Position()));
//#endif
	if (!m_bPlayHumming && m_tpCurrentSound) {
		m_tpCurrentSound->stop();
		m_tpCurrentSound = 0;
	}
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(XFORM().k,dir,AI_Path.fSpeed);
//#ifdef DEBUG
//	Msg								("Death position : [%f][%f][%f]",VPUSH(Position()));
//#endif
}

void CAI_Stalker::BackCover(bool bFire)
{
	WRITE_TO_LOG				("Back cover");
	
	if (m_bStateChanged && !m_bPlayHumming && m_tpCurrentSound) {
		m_tActionState			= eActionStateWatchGo;
		m_dwActionStartTime		= Level().timeServer();
		m_tpCurrentSound->stop	();
		m_tpCurrentSound		= 0;
	}
	
	m_bPlayHumming				= false;

	m_dwInertion				= bFire ? 20000 : 60000;
	
	m_tSelectorCover.m_fMinEnemyDistance = m_tSavedEnemyPosition.distance_to(Position()) + 3.f;
	
	Fvector						tPoint;
	
	if (m_tEnemy.Enemy)
		m_tEnemy.Enemy->Center(tPoint);
	else
		tPoint					= m_tSavedEnemyPosition;

	switch (m_tActionState) {
		case eActionStateWatchGo : {
			WRITE_TO_LOG			("WatchGo : BackCover");
			if (Level().timeServer() - m_dwActionStartTime < 8500)
				vfSetParameters			(&m_tSelectorCover,0,true,eWeaponStateIdle,ePathTypeDodgeCriteria,eBodyStateStand, /**(m_tSavedEnemyPosition.distance_to(Position()) > 3.f) && m_tEnemy.Enemy ? eMovementTypeRun : eMovementTypeWalk/**/eMovementTypeRun,eStateTypeDanger,eLookTypeDirection);
			else
				vfSetParameters			(0,0,true,eWeaponStateIdle,ePathTypeDodgeCriteria,eBodyStateStand, eMovementTypeWalk,eStateTypeDanger,eLookTypeFirePoint,tPoint);
			if ((Level().timeServer() - m_dwActionStartTime > 10000) && ((getAI().dwfCheckPositionInDirection(AI_NodeID,Position(),tPoint) != u32(-1)) || (Level().timeServer() - m_dwActionStartTime > 8000))) {
				m_tActionState		= eActionStateWatchLook;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatchLook : {
			WRITE_TO_LOG			("WatchLook : BackCover");
			if (!m_tEnemy.Enemy && getAI().bfTooSmallAngle(r_torso_current.yaw, r_torso_target.yaw,PI_DIV_6) && bfCheckIfCanKillTarget(this,m_tSavedEnemyPosition,-r_torso_current.yaw,-r_torso_current.pitch,ffGetFov()/180.f*PI))
				Camp(bFire);
			else {
				vfSetParameters			(&m_tSelectorCover,0,true,bFire ? eWeaponStatePrimaryFire : eWeaponStateIdle,ePathTypeDodgeCriteria,eBodyStateCrouch,eMovementTypeStand,eStateTypeDanger,eLookTypeFirePoint,tPoint);
				if (Level().timeServer() - m_dwActionStartTime > 6000) {
					m_tActionState		= eActionStateWatchGo;
					m_dwActionStartTime = Level().timeServer();
				}
			}
			break;
		}
		default : {
			m_tActionState			= eActionStateWatchGo;
			break;
		}
	}
}

void CAI_Stalker::ForwardStraight()
{
	bool bPlayingHumming = m_bPlayHumming;
	
	WRITE_TO_LOG("Forward straight");
	
	if (m_bStateChanged && !m_bPlayHumming && m_tpCurrentSound) {
		m_tpCurrentSound->stop();
		m_tpCurrentSound = 0;
	}

	m_dwInertion				= 20000;
	
	if (!m_tEnemy.Enemy && (m_dwSavedEnemyNodeID != u32(-1))) {
		SearchEnemy();
		return;
	}
	
	Fvector						tPoint;
	m_tEnemy.Enemy->Center	(tPoint);
	float						fDistance = Position().distance_to(m_tEnemy.Enemy->Position());

	if (bPlayingHumming && bfIfHuman()) {
		m_tpCurrentSound = &m_tpSoundAlarm[::Random.randI((int)m_tpSoundAlarm.size())];
		m_tpCurrentSound->play_at_pos(this,eye_matrix.c);
		m_tpCurrentSound->feedback->set_volume(1.f);
	}
	else
		if (m_tpCurrentSound && m_tpCurrentSound->feedback)
			m_tpCurrentSound->feedback->set_position(eye_matrix.c);

	m_bPlayHumming				= false;
	CWeapon						*tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
	if (tpWeapon) {
		m_tSelectorFreeHunting.m_fMaxEnemyDistance = tpWeapon->m_fMaxRadius;
		m_tSelectorFreeHunting.m_fMinEnemyDistance = 15.f;//tpWeapon->m_fMinRadius;
		m_tSelectorFreeHunting.m_fOptEnemyDistance = 15.f;//tpWeapon->m_fMinRadius + 3.f;
	}

	vfSetParameters				(&m_tSelectorFreeHunting,0,true,(m_tpCurrentSound && m_tpCurrentSound->feedback) ? eWeaponStateIdle : eWeaponStatePrimaryFire,fDistance > 15.f ? ePathTypeStraightDodge : ePathTypeCriteria,eBodyStateStand,m_tEnemy.Enemy->Position().distance_to(Position()) > 15.f ? eMovementTypeRun : eMovementTypeWalk,eStateTypeDanger,eLookTypeFirePoint,tPoint);
}

void CAI_Stalker::Camp(bool bWeapon)
{
	WRITE_TO_LOG			("Camping...");

	if (m_bStateChanged && !m_bPlayHumming && m_tpCurrentSound) {
		m_tpCurrentSound->stop();
		m_tpCurrentSound = 0;
	}

	m_bPlayHumming			= false;
	int						iIndex = ifFindDynamicObject(m_tSavedEnemy);
	if (iIndex == -1)
		return;
	float					fDistance = Position().distance_to(m_tpaDynamicObjects[iIndex].tSavedPosition);

	if (Position().distance_to(m_tpaDynamicObjects[iIndex].tMySavedPosition) > .1f) {
		AI_Path.DestNode		= m_tpaDynamicObjects[iIndex].dwMyNodeID;
		if (!AI_Path.DestNode) {
			Msg("! Object %s is in invalid node",m_tpaDynamicObjects[iIndex].tpEntity ? m_tpaDynamicObjects[iIndex].tpEntity->cName() : "world");
		}
		vfSetParameters			(0,&(m_tpaDynamicObjects[iIndex].tMySavedPosition),false,eWeaponStateIdle,ePathTypeStraight,eBodyStateCrouch,eMovementTypeWalk,eStateTypeDanger,eLookTypeFirePoint,m_tpaDynamicObjects[iIndex].tSavedPosition);
	}
	else {
		float fDistanceToCover = getAI().ffFindFarthestNodeInDirection(AI_NodeID,Position(),m_tpaDynamicObjects[iIndex].tSavedPosition,AI_Path.DestNode);
		if ((fDistanceToCover < fDistance/3.f) && (fDistanceToCover > .5f)) {
			m_tpaDynamicObjects[iIndex].tMySavedPosition.sub(m_tpaDynamicObjects[iIndex].tSavedPosition,Position());
			m_tpaDynamicObjects[iIndex].tMySavedPosition.mul((fDistanceToCover - .5f)/fDistance);
			m_tpaDynamicObjects[iIndex].tMySavedPosition.add(Position());
			m_tpaDynamicObjects[iIndex].dwMyNodeID			= AI_Path.DestNode;
			//AI_Path.TravelPath.clear();
			//AI_Path.Nodes.clear();
			CWeapon	*tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
			if (tpWeapon && (tpWeapon->GetAmmoElapsed() < tpWeapon->GetAmmoMagSize()/2))
				m_inventory.Action(kWPN_RELOAD,CMD_START);
			vfSetParameters		(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateCrouch,eMovementTypeStand,eStateTypeDanger,eLookTypeFirePoint,m_tpaDynamicObjects[iIndex].tSavedPosition);
		}
	}
}

void CAI_Stalker::Panic()
{
	WRITE_TO_LOG				("Panic");
	
	if (m_bStateChanged && !m_bPlayHumming && m_tpCurrentSound) {
		m_tpCurrentSound->stop();
		m_tpCurrentSound = 0;
		m_tActionState = ::Random.randI(1) ? eActionStateStand : eActionStateWatchGo;
		m_dwActionStartTime = Level().timeServer() + ::Random.randI(2000,5000);
	}

	m_dwInertion				= 60000;
	
	if ((AI_Path.fSpeed < EPS_L) && !m_tEnemy.Enemy) {
		switch (m_tActionState) {
			case eActionStateWatch : {
				vfSetParameters			(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateCrouch,eMovementTypeStand,eStateTypeDanger,eLookTypeDanger);
				if (r_torso_current.yaw == r_torso_target.yaw) {
					m_ls_yaw = r_torso_current.yaw;
					m_tActionState = eActionStateDontWatch;
				}
				break;
			}
			case eActionStateDontWatch : {
				Fvector					tPoint;
				tPoint.setHP			(m_ls_yaw,0);
				tPoint.mul				(100.f);
				tPoint.add				(Position());
				vfSetParameters			(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateCrouch,eMovementTypeStand,eStateTypeDanger,eLookTypeLookOver,tPoint,2500);
				break;
			}
			default : {
				m_tActionState = eActionStateWatch;
				break;
			}
		}
		return;
	}

	m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tSavedEnemyPosition.distance_to(Position()) + m_tSelectorFreeHunting.m_fSearchRange;
	m_tSelectorFreeHunting.m_fOptEnemyDistance = m_tSelectorFreeHunting.m_fMaxEnemyDistance;
	m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tSavedEnemyPosition.distance_to(Position()) + 3.f;

	m_dwRandomFactor			= 50;
	
	float						yaw,pitch;
	GetDirectionAngles			(yaw,pitch);
	yaw *= -1;
	if (getAI().bfTooSmallAngle(yaw, r_current.yaw, PI_DIV_6))
        vfSetParameters				(&m_tSelectorFreeHunting,0,true,eWeaponStateIdle,ePathTypeStraightDodge,eBodyStateStand,eMovementTypeRun,eStateTypePanic,eLookTypeDirection);
	else
        vfSetParameters				(&m_tSelectorFreeHunting,0,true,eWeaponStateIdle,ePathTypeStraightDodge,eBodyStateStand,eMovementTypeRun,eStateTypeDanger,eLookTypeDirection);

	if (bfIfHuman(m_tSavedEnemy)) {
		switch (m_tActionState) {
			case eActionStateWatchGo : {
				if (m_dwActionStartTime < Level().timeServer())
					m_tActionState = eActionStateStand;
				break;
			}
			case eActionStateStand : {
				if (!m_tpCurrentSound) {
					m_tpCurrentSound = &m_tpSoundSurrender[::Random.randI((int)m_tpSoundSurrender.size())];
					m_tpCurrentSound->play_at_pos(this,eye_matrix.c);
					m_tpCurrentSound->feedback->set_volume(1.f);
				}
				else
					if (m_tpCurrentSound->feedback)
						m_tpCurrentSound->feedback->set_position(eye_matrix.c);
					else {
						m_tpCurrentSound = 0;
						m_tActionState = eActionStateWatchGo;
						m_dwActionStartTime = Level().timeServer() + ::Random.randI(2000,5000);
					}
				break;
			}
			default : {
				m_tActionState = eActionStateStand;
				break;
			}
		}
	}
	else
		if (m_tpCurrentSound) {
			if (m_tpCurrentSound->feedback)
				m_tpCurrentSound->stop();
			m_tpCurrentSound = 0;
		}
}

void CAI_Stalker::Hide()
{
	WRITE_TO_LOG				("Hide");

	if (m_bStateChanged && !m_bPlayHumming && m_tpCurrentSound) {
		m_tpCurrentSound->stop();
		m_tpCurrentSound = 0;
	}

	m_dwInertion				= 60000;
	if (!m_tEnemy.Enemy) {
		Camp(false);
		return;
	}
	m_tSelectorCover.m_fMaxEnemyDistance = m_tEnemy.Enemy->Position().distance_to(Position()) + m_tSelectorCover.m_fSearchRange;
	m_tSelectorCover.m_fOptEnemyDistance = m_tSelectorCover.m_fMaxEnemyDistance;
	m_tSelectorCover.m_fMinEnemyDistance = m_tEnemy.Enemy->Position().distance_to(Position()) + 3.f;
	
	Fvector						tPoint;
	m_tEnemy.Enemy->Center	(tPoint);
	vfSetParameters				(&m_tSelectorCover,0,true,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypeFirePoint,tPoint);
}

void CAI_Stalker::Detour()
{
	WRITE_TO_LOG("Detour");

	if (m_bStateChanged && !m_bPlayHumming && m_tpCurrentSound) {
		m_tpCurrentSound->stop();
		m_tpCurrentSound = 0;
	}

	m_dwInertion				= 60000;
	if (m_bStateChanged) {
		m_tActionState = eActionStateWatchLook;
		m_dwActionStartTime = Level().timeServer();
		//AI_Path.TravelPath.clear();
	}
	
	Fvector tPoint;
	if (m_tEnemy.Enemy)
		m_tEnemy.Enemy->Center(tPoint);
	else
		tPoint = m_tSavedEnemyPosition;
	m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tSavedEnemyPosition.distance_to(Position()) + m_tSelectorFreeHunting.m_fSearchRange;
	m_tSelectorFreeHunting.m_fOptEnemyDistance = 15;
	m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tSavedEnemyPosition.distance_to(Position()) + 3.f;

	u32 dwDelay1, dwDelay2;
	float f = 0;
	if (C)	f =100.f;
	if (D)	f =80.f;
	if (E)	f =60.f;
	if (F)	f =40.f;
	if (G)	f =20.f;
	
	float fDistance = _min(Position().distance_to(m_tSavedEnemyPosition),f);
	dwDelay1 = iFloor((fDistance / f) * 20000);
	dwDelay2 = _max(u32(20000 - dwDelay1),u32(2000));
	switch (m_tActionState) {
		case eActionStateWatchGo : {
			WRITE_TO_LOG			("WatchGo : Detour");
			vfContinueWithALifeGoals((F || G) ? 0 : &m_tSelectorFreeHunting);
			if ((Level().timeServer() - m_dwActionStartTime > dwDelay1) && (getAI().dwfCheckPositionInDirection(AI_NodeID,Position(),tPoint) != u32(-1))) {
				m_tActionState		= eActionStateWatchLook;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatchLook : {
			WRITE_TO_LOG			("WatchLook : Detour");
			vfUpdateSearchPosition	();
			AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGraphID].tNodeID;
			if (!AI_Path.DestNode) {
				Msg("! Invalid graph point node (graph index %d)",m_tNextGraphID);
				for (int i=0; i<(int)getAI().GraphHeader().dwVertexCount; i++)
					Msg("%3d : %6d",i,getAI().m_tpaGraph[i].tNodeID);
			}
			vfSetParameters			((F || G) ? 0 : &m_tSelectorFreeHunting,0,false,eWeaponStateIdle,(F || G) ? ePathTypeStraight : ePathTypeCriteria,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,(F || G) ? eLookTypePoint : eLookTypeFirePoint,tPoint);
			if ((Level().timeServer() - m_dwActionStartTime > dwDelay2) || (getAI().dwfCheckPositionInDirection(AI_NodeID,Position(),tPoint) == u32(-1))) {
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
	bool bPlayingHumming = m_bPlayHumming;

	WRITE_TO_LOG("Forward cover");
	
	if (m_bStateChanged && !m_bPlayHumming && m_tpCurrentSound) {
		m_tpCurrentSound->stop();
		m_tpCurrentSound = 0;
	}

	m_dwInertion = 60000;

//	if (!m_tEnemy.Enemy && (m_tActionState != eActionStateWatchGo)) {
	if (!m_tEnemy.Enemy) {
		if (!E || (Position().distance_to(m_tSavedEnemyPosition) < 5.f) || (Level().timeServer() - m_dwLostEnemyTime > 6000))
			SearchEnemy();
		else
			Camp(true);
		return;
	}
	
	if (m_bStateChanged) {
		m_tActionState			= eActionStateWatchLook;
		m_dwActionStartTime		= Level().timeServer();
		float					fDistance = m_tSavedEnemyPosition.distance_to(Position());
		CWeapon					*tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
		if (tpWeapon)
			m_tSelectorCover.m_fOptEnemyDistance = (tpWeapon->m_fMinRadius + 0*tpWeapon->m_fMaxRadius)/1;
		m_tSelectorCover.m_fMaxEnemyDistance = _max(fDistance - 1.f,m_tSelectorCover.m_fOptEnemyDistance + 3.f);
		m_tSelectorCover.m_fMinEnemyDistance = _max(fDistance - m_tSelectorCover.m_fSearchRange,m_tSelectorCover.m_fOptEnemyDistance - 3.f);
		m_dwLastRangeSearch		= 0;
		//AI_Path.TravelPath.clear();
	}

	Fvector						tPoint;
	if (m_tEnemy.Enemy)
		m_tEnemy.Enemy->Center	(tPoint);
	else
		tPoint					=	m_tSavedEnemyPosition;
	
	bool bBackMove = false;
	if (!AI_Path.TravelPath.empty() && (AI_Path.TravelStart < AI_Path.TravelPath.size() - 1)) {
		Fvector tTemp0, tTemp1;
		tTemp0.sub(m_tSavedEnemyPosition,Position());
		tTemp1.sub(AI_Path.TravelPath[AI_Path.TravelStart + 1].P,Position());
		float yaw1,yaw2,pitch1,pitch2;
		tTemp0.getHP(yaw1,pitch1);
		tTemp1.getHP(yaw2,pitch2);
		bBackMove = !getAI().bfTooSmallAngle(yaw1,yaw2,4*PI_DIV_6);
	}

	if (bPlayingHumming && bfIfHuman()) {
		m_tpCurrentSound = &m_tpSoundAlarm[::Random.randI((int)m_tpSoundAlarm.size())];
		m_tpCurrentSound->play_at_pos(this,eye_matrix.c);
		m_tpCurrentSound->feedback->set_volume(1.f);
	}
	else
		if (m_tpCurrentSound && m_tpCurrentSound->feedback)
			m_tpCurrentSound->feedback->set_position(eye_matrix.c);

	m_bPlayHumming				= false;
	switch (m_tActionState) {
		case eActionStateWatchGo : {
			WRITE_TO_LOG			("WatchGo : ForwardCover");
			if ((Level().timeServer() - m_dwActionStartTime < 5500) && (AI_Path.TravelPath.size() > AI_Path.TravelStart + 1))
				vfSetParameters			(&m_tSelectorCover,0,true,eWeaponStateIdle,ePathTypeDodgeCriteria,eBodyStateStand, /**(m_tSavedEnemyPosition.distance_to(Position()) > 3.f) && m_tEnemy.Enemy ? eMovementTypeRun : eMovementTypeWalk/**/eMovementTypeRun,eStateTypeDanger,eLookTypeDirection);
			else
				vfSetParameters			(0,0,true,eWeaponStateIdle,ePathTypeDodgeCriteria,eBodyStateStand, eMovementTypeWalk,eStateTypeDanger,eLookTypeFirePoint,tPoint);
			if ((Level().timeServer() - m_dwActionStartTime > 7000) && ((getAI().dwfCheckPositionInDirection(AI_NodeID,Position(),tPoint) != u32(-1)) || (Level().timeServer() - m_dwActionStartTime > 8000))) {
				m_tActionState		= eActionStateWatchLook;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatchLook : {
			WRITE_TO_LOG			("WatchLook : ForwardCover");
			float					fDistance = m_tSavedEnemyPosition.distance_to(Position());
			CWeapon					*tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
			if (tpWeapon)
				m_tSelectorCover.m_fOptEnemyDistance = (tpWeapon->m_fMinRadius + 0*tpWeapon->m_fMaxRadius)/1;
			m_tSelectorCover.m_fMaxEnemyDistance = _max(fDistance - 1.f,m_tSelectorCover.m_fOptEnemyDistance + 3.f);
			m_tSelectorCover.m_fMinEnemyDistance = _max(fDistance - m_tSelectorCover.m_fSearchRange,m_tSelectorCover.m_fOptEnemyDistance - 3.f);
			vfSetParameters			(&m_tSelectorCover,0,true,(m_tpCurrentSound && m_tpCurrentSound->feedback) ? eWeaponStateIdle : eWeaponStatePrimaryFire,ePathTypeDodgeCriteria,eBodyStateCrouch,eMovementTypeStand,eStateTypeDanger,eLookTypeFirePoint,tPoint);
			if (!tpWeapon || ((tpWeapon->STATE != CWeapon::eFire) && !tpWeapon->GetAmmoElapsed() && (!m_bIfSearchFailed || ((!AI_Path.TravelPath.empty() && AI_Path.TravelPath.size() > AI_Path.TravelStart + 1) && (Level().timeServer() - m_dwActionStartTime > 5000))))) {
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
	
	if (m_bStateChanged && !m_bPlayHumming && m_tpCurrentSound) {
		m_tpCurrentSound->stop();
		m_tpCurrentSound = 0;
	}

	m_bPlayHumming				= false;
	m_dwInertion				= 180000;

	INIT_SQUAD_AND_LEADER;
	
	CGroup						&Group = *getGroup();
	
	if (bfCheckIfSound()) {
		m_dwLastEnemySearch = 0;
		vfUpdateVisibilityBySensitivity();
	}

	if (m_dwLastEnemySearch != m_dwLastUpdate) {
		m_tActionState			= eActionStateDontWatch;
		m_iCurrentSuspiciousNodeIndex = 0;
		m_bSearchedForEnemy		= true;
		m_tPathState			= ePathStateBuildNodePath;
		Group.m_tpaSuspiciousNodes.clear();
		//Msg						("[%d] : Updating search params",Level().timeServer());
	}
	m_bSearchedForEnemy			= true;

	switch (m_tActionState) {
		case eActionStateDontWatch : {
			OUT_TEXT("Last enemy position");
			AI_Path.DestNode	= m_dwSavedEnemyNodeID;
			if (int(AI_Path.DestNode) <= 0) {
				Msg("! Invalid Saved Enemy Node in Search Enemy (%s)",m_tSavedEnemy ? m_tSavedEnemy->cName() : "world");
				m_tSavedEnemy = 0;
				m_dwInertion = 0;
				return;
			}
			if (Group.m_tpaSuspiciousNodes.empty()) {
				if (!m_iCurrentSuspiciousNodeIndex) {
					vfFindAllSuspiciousNodes(m_dwSavedEnemyNodeID,m_tSavedEnemyPosition,m_tSavedEnemyPosition,40.f,Group);
					m_iCurrentSuspiciousNodeIndex = -1;
				}
			}
			bool bB = false;//m_bStateChanged && !::Random.randI(2);
			if (bB || ((getAI().dwfCheckPositionInDirection(AI_NodeID,Position(),m_tSavedEnemyPosition) != -1) && (bfIf_I_SeePosition(m_tSavedEnemyPosition))) || (AI_NodeID == m_dwSavedEnemyNodeID))
				m_tActionState = eActionStateWatchGo;
			else {
				bool bOk = false;	
#ifdef AI_BEST
				for (u32 i=0; i<Group.m_tpaSuspiciousNodes.size(); i++)
					if (!Group.m_tpaSuspiciousNodes[i].dwSearched) {
						if (getAI().bfCheckNodeInDirection(AI_NodeID,Position(),Group.m_tpaSuspiciousNodes[i].dwNodeID)) {
							bOk = true;
							AI_Path.DestNode	= m_dwSavedEnemyNodeID;
							vfSetParameters		(0,&m_tSavedEnemyPosition,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeStand,eStateTypeDanger,eLookTypePoint,getAI().tfGetNodeCenter(Group.m_tpaSuspiciousNodes[i].dwNodeID));
							if (bfIf_I_SeePosition(getAI().tfGetNodeCenter(Group.m_tpaSuspiciousNodes[i].dwNodeID)))
								Group.m_tpaSuspiciousNodes[i].dwSearched = 2;
							else {
								m_iSuspPoint = i;
								m_tLastActionState = m_tActionState;
								m_tActionState = eActionStateWatch;
							}
							break;
						}
					}
#endif
				if (!bOk)
					if (getAI().dwfCheckPositionInDirection(AI_NodeID,Position(),m_tSavedEnemyPosition) == -1) {
						if ((Level().timeServer() - m_dwLostEnemyTime < 15000) && (m_tSavedEnemyPosition.distance_to(Position()) < 6.f) && !G) {
							Fvector				tPoint = m_tSavedEnemyPosition;
							tPoint.y			+= 1.5f;
							vfSetParameters		(0,&m_tSavedEnemyPosition,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateCrouch,eMovementTypeWalk,eStateTypeDanger,eLookTypeFirePoint,tPoint);
						}
						else
							vfSetParameters		(0,&m_tSavedEnemyPosition,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypeSearch);
					}
					else {
						Fvector				tPoint = m_tSavedEnemyPosition;
						tPoint.y			+= 1.5f;
						AI_Path.DestNode	= m_dwSavedEnemyNodeID;
						if (!AI_Path.DestNode) {
							Msg("! Invalid Saved Enemy Node in Search Enemy 2 (%s)",m_tSavedEnemy ? m_tSavedEnemy->cName() : "world");
							m_tSavedEnemy = 0;
							m_dwInertion = 0;
							return;
						}
						vfSetParameters		(0,&m_tSavedEnemyPosition,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypePoint,tPoint);
					}
			}
			break;
		}
		case eActionStateWatch   : {
			if (((int)Group.m_tpaSuspiciousNodes.size() - 1) < m_iSuspPoint) {
				m_tSavedEnemy = 0;
				m_dwInertion = 0;
				return;
			}
			vfSetParameters		(0,&m_tSavedEnemyPosition,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeStand,eStateTypeDanger,eLookTypePoint,getAI().tfGetNodeCenter(Group.m_tpaSuspiciousNodes[m_iSuspPoint].dwNodeID));
			if (bfIf_I_SeePosition(getAI().tfGetNodeCenter(Group.m_tpaSuspiciousNodes[m_iSuspPoint].dwNodeID))) {
				Group.m_tpaSuspiciousNodes[m_iSuspPoint].dwSearched = 2;
				m_tActionState = m_tLastActionState;
			}
			break;
		}
		case eActionStateWatchGo : {
			OUT_TEXT("The higher-level prediction");
			if ((int)Group.m_tpaSuspiciousNodes.size() < (m_iCurrentSuspiciousNodeIndex + 1)) {
				Group.m_tpaSuspiciousNodes.clear();
				m_iCurrentSuspiciousNodeIndex = -1;
				m_tActionState = eActionStateWatchLook;
				break;
			}

			if ((m_iCurrentSuspiciousNodeIndex == -1) || getAI().bfCheckNodeInDirection(AI_NodeID,Position(),Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID)) {
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
					if (!AI_Path.DestNode) {
						Msg("! Invalid Saved Enemy Node in Search Enemy 3 (%s)",m_tSavedEnemy ? m_tSavedEnemy->cName() : "world");
					}
					m_tPathState = ePathStateBuildNodePath;
				}
			}
			
			if ((m_iCurrentSuspiciousNodeIndex >= 0) && ((int)Group.m_tpaSuspiciousNodes.size() > m_iCurrentSuspiciousNodeIndex)) {
				bool bOk = false;	
#ifdef AI_BEST
				for (u32 i=0; i<Group.m_tpaSuspiciousNodes.size(); i++)
					if (!Group.m_tpaSuspiciousNodes[i].dwSearched) {
						if (getAI().bfCheckNodeInDirection(AI_NodeID,Position(),Group.m_tpaSuspiciousNodes[i].dwNodeID)) {
							bOk = true;
							AI_Path.DestNode	= Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID;
							vfSetParameters		(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypePoint,getAI().tfGetNodeCenter(Group.m_tpaSuspiciousNodes[i].dwNodeID));
							if (bfIf_I_SeePosition(getAI().tfGetNodeCenter(Group.m_tpaSuspiciousNodes[i].dwNodeID)))
								Group.m_tpaSuspiciousNodes[i].dwSearched = 2;
							else {
								m_iSuspPoint = i;
								m_tLastActionState = m_tActionState;
								m_tActionState = eActionStateWatch;
							}
							break;
						}
					}
#endif
				if (!bOk)
					if ((getAI().dwfCheckPositionInDirection(AI_NodeID,Position(),getAI().tfGetNodeCenter(Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID)) == -1) && (getAI().u_SqrDistance2Node(Position(),getAI().Node(Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID)) < .35f)) {
						AI_Path.DestNode	= Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID;
						if (!AI_Path.DestNode) {
							Msg("! Invalid Saved Enemy Node in Search Enemy 4 (%s)",m_tSavedEnemy ? m_tSavedEnemy->cName() : "world");
						}
						vfSetParameters		(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypeSearch);
					}
					else {
						Fvector				tPoint = getAI().tfGetNodeCenter(Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID);
						tPoint.y			+= 1.5f;
						AI_Path.DestNode	= Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID;
						if (!AI_Path.DestNode) {
							Msg("! Invalid Saved Enemy Node in Search Enemy 5 (%s)",m_tSavedEnemy ? m_tSavedEnemy->cName() : "world");
						}
						vfSetParameters		(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypePoint,tPoint);
					}
			}
	
			break;
		}
		case eActionStateWatchLook : {
//			OUT_TEXT("Predicting by selector");
//			m_tSelectorRetreat.m_tEnemyPosition = m_tMySavedPosition;
//			m_tSelectorRetreat.m_tpEnemyNode	= getAI().Node(m_dwMyNodeID);
//			m_tSelectorRetreat.m_tMyPosition	= Position();
//			m_tSelectorRetreat.m_tpMyNode		= getAI().Node(AI_NodeID);
//			m_tSelectorRetreat.m_fMinEnemyDistance = Position().distance_to(m_tMySavedPosition) + 3.f;
//			vfSetParameters		(&m_tSelectorRetreat,0,true,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypeDanger);
//			if (m_bIfSearchFailed)
//				m_dwInertion						= 0;
//			else
//				m_dwInertion						= 120000;
			m_dwInertion					= 0;
			break;
		}
	}
}

void CAI_Stalker::ExploreDNE()
{
	if (m_bStateChanged && !m_bPlayHumming && m_tpCurrentSound) {
		m_tpCurrentSound->stop();
		m_tpCurrentSound = 0;
	}

	m_dwInertion			= 60000;
	Fvector					tPoint = m_tpaDynamicSounds[m_iSoundIndex].tSavedPosition;
	if (getAI().bfInsideNode(getAI().Node(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID),tPoint))
		tPoint.y			= getAI().ffGetY(*getAI().Node(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID),tPoint.x,tPoint.z);
	else
		tPoint				= getAI().tfGetNodeCenter(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID);
	if (m_bStateChanged) {
		m_tActionState		= eActionStateDontWatch;
		m_dwActionStartTime = Level().timeServer();
		//AI_Path.TravelPath.clear();
	}
	m_tSelectorFreeHunting.m_fOptEnemyDistance = _max(25.f,Position().distance_to(tPoint));
	switch (m_tActionState) {
		case eActionStateDontWatch : {
			WRITE_TO_LOG			("DontWatch : Exploring danger non-expedient ref_sound");
			vfContinueWithALifeGoals(&m_tSelectorFreeHunting);
			if (getAI().dwfCheckPositionInDirection(AI_NodeID,Position(),tPoint) != u32(-1)) {
				m_tActionState		= eActionStateWatch;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatch : {
			WRITE_TO_LOG			("Watch : Exploring danger non-expedient ref_sound");
			vfSetParameters			(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateCrouch,eMovementTypeStand,eStateTypeDanger,eLookTypeLookFireOver,tPoint,2500);
			if (Level().timeServer() - m_dwActionStartTime > 9000) {
				m_tActionState		= eActionStateWatchGo;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatchGo : {
			WRITE_TO_LOG			("WatchGo : Exploring danger non-expedient ref_sound");
			vfContinueWithALifeGoals(&m_tSelectorFreeHunting);
			if ((Level().timeServer() - m_dwActionStartTime > 7000) && (getAI().dwfCheckPositionInDirection(AI_NodeID,Position(),tPoint) != u32(-1))) {
				m_tActionState		= eActionStateWatchLook;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatchLook : {
			WRITE_TO_LOG			("WatchLook : Exploring danger non-expedient ref_sound");
			vfUpdateSearchPosition	();
			AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGraphID].tNodeID;
			if (!AI_Path.DestNode) {
				Msg("! Invalid graph point node (graph index %d)",m_tNextGraphID);
				for (int i=0; i<(int)getAI().GraphHeader().dwVertexCount; i++)
					Msg("%3d : %6d",i,getAI().m_tpaGraph[i].tNodeID);
			}
			vfSetParameters			(0,0,false,eWeaponStateIdle,ePathTypeCriteria,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypePoint,tPoint);
			if ((Level().timeServer() - m_dwActionStartTime > 5000) || (getAI().dwfCheckPositionInDirection(AI_NodeID,Position(),tPoint) == u32(-1))) {
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
	if (m_bStateChanged && !m_bPlayHumming && m_tpCurrentSound) {
		m_tpCurrentSound->stop();
		m_tpCurrentSound = 0;
	}

	m_dwInertion			= 60000;
	
	if (!m_inventory.ActiveItem() && M) {
		TakeItems();
		return;
	}

	if (m_tpaDynamicSounds[m_iSoundIndex].tpEntity && !m_tpaDynamicSounds[m_iSoundIndex].tpEntity->g_Alive()) {
		m_iSoundIndex = -1;
		return;
	}
	m_tSavedEnemyPosition	= m_tpaDynamicSounds[m_iSoundIndex].tSavedPosition;
	m_dwSavedEnemyNodeID	= m_tpaDynamicSounds[m_iSoundIndex].dwNodeID;
	R_ASSERT2				(int(m_dwSavedEnemyNodeID) > 0, "Invalid enemy node");
	m_tMySavedPosition		= m_tpaDynamicSounds[m_iSoundIndex].tMySavedPosition;
	m_dwMyNodeID			= m_tpaDynamicSounds[m_iSoundIndex].dwMyNodeID;
	vfValidatePosition		(m_tSavedEnemyPosition,m_dwSavedEnemyNodeID);
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
	R_ASSERT2				(int(m_dwSavedEnemyNodeID) > 0, "Invalid enemy node");
	m_tMySavedPosition		= m_tpaDynamicSounds[m_iSoundIndex].tMySavedPosition;
	m_dwMyNodeID			= m_tpaDynamicSounds[m_iSoundIndex].dwMyNodeID;
	vfValidatePosition		(m_tSavedEnemyPosition,m_dwSavedEnemyNodeID);
	if (m_tpaDynamicSounds[m_iSoundIndex].dwTime - m_dwLastSoundUpdate > SOUND_UPDATE_DELAY)
		m_dwLastEnemySearch = 0;
	m_dwLastSoundUpdate		= m_tpaDynamicSounds[m_iSoundIndex].dwTime;
	SearchEnemy();
}

void CAI_Stalker::ExploreNDNE()
{
	if (m_bStateChanged && !m_bPlayHumming && m_tpCurrentSound) {
		m_tpCurrentSound->stop();
		m_tpCurrentSound = 0;
	}

	m_dwInertion			= 60000;
	Fvector					tPoint = m_tpaDynamicSounds[m_iSoundIndex].tSavedPosition;
	if (getAI().bfInsideNode(getAI().Node(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID),tPoint))
		tPoint.y			= getAI().ffGetY(*getAI().Node(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID),tPoint.x,tPoint.z);
	else
		tPoint				= getAI().tfGetNodeCenter(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID);
	if (m_bStateChanged) {
		m_tActionState = eActionStateDontWatch;
		m_dwActionStartTime = Level().timeServer();
		//AI_Path.TravelPath.clear();
	}
	switch (m_tActionState) {
		case eActionStateDontWatch : {
			WRITE_TO_LOG			("DontWatch : Exploring non-danger non-expedient ref_sound");
			vfContinueWithALifeGoals();
			if (getAI().dwfCheckPositionInDirection(AI_NodeID,Position(),tPoint) != u32(-1)) {
				m_tActionState = eActionStateWatch;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatch : {
			WRITE_TO_LOG			("Watch : Exploring non-danger non-expedient ref_sound");
			vfSetParameters			(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeStand,eStateTypeDanger,eLookTypeLookOver,tPoint,1200);
			if (Level().timeServer() - m_dwActionStartTime > 5000) {
				m_tActionState = eActionStateWatchGo;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatchGo : {
			WRITE_TO_LOG			("WatchGo : Exploring non-danger non-expedient ref_sound");
			vfContinueWithALifeGoals();
			if ((Level().timeServer() - m_dwActionStartTime > 10000) && (getAI().dwfCheckPositionInDirection(AI_NodeID,Position(),tPoint) != u32(-1))) {
				m_tActionState = eActionStateWatchLook;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatchLook : {
			WRITE_TO_LOG			("WatchLook : Exploring non-danger non-expedient ref_sound");
			vfUpdateSearchPosition	();
			AI_Path.DestNode		= getAI().m_tpaGraph[m_tNextGraphID].tNodeID;
			if (!AI_Path.DestNode) {
				Msg("! Invalid graph point node (graph index %d)",m_tNextGraphID);
				for (int i=0; i<(int)getAI().GraphHeader().dwVertexCount; i++)
					Msg("%3d : %6d",i,getAI().m_tpaGraph[i].tNodeID);
			}
			vfSetParameters(0,0,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypePoint,tPoint);
			if ((Level().timeServer() - m_dwActionStartTime > 3000) || (getAI().dwfCheckPositionInDirection(AI_NodeID,Position(),tPoint) == u32(-1))) {
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
	if (m_bStateChanged && !m_bPlayHumming && m_tpCurrentSound) {
		m_tpCurrentSound->stop();
		m_tpCurrentSound = 0;
	}

	bool bPlayingHumming = m_bPlayHumming;
	WRITE_TO_LOG			("Taking items");
	m_bPlayHumming = bPlayingHumming;
	if (m_tpCurrentSound) {
		m_tpCurrentSound->stop();
		m_tpCurrentSound = 0;
	}

	// taking items
	if (m_bStateChanged) {
		//AI_Path.TravelPath.clear();
	}
	Fvector					tPoint;

	CInventoryItem *tpItemToTake = 0;
	
	bool bOk;
	do {
		bOk = true;
		vfSelectItemToTake(tpItemToTake);

		if (!tpItemToTake)
			return;

		if (!tpItemToTake->AI_NodeID || (tpItemToTake->AI_NodeID >= getAI().Header().count)) {
			bOk = false;
			for (u32 i=0; i<m_tpItemsToTake.size(); i++)
				if (m_tpItemsToTake[i] == tpItemToTake) {
					m_tpItemsToTake.erase(m_tpItemsToTake.begin() + i);
					break;
				}
		}
	}
	while (!bOk);

	tpItemToTake->Center(tPoint);
	AI_Path.DestNode		= tpItemToTake->AI_NodeID;
	if (!AI_Path.DestNode) {
		Msg("! Invalid item node %s",tpItemToTake->cName());
	}
	if (getAI().bfInsideNode(getAI().Node(AI_Path.DestNode),tPoint))
		tPoint.y			= getAI().ffGetY(*getAI().Node(AI_Path.DestNode),tPoint.x,tPoint.z);
	else
		tPoint				= getAI().tfGetNodeCenter(AI_Path.DestNode);
	
	if (getAI().dwfCheckPositionInDirection(AI_NodeID,Position(),tPoint) != -1)
		vfSetParameters(0,&tPoint,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypePoint,tPoint);
	else
		vfSetParameters(0,&tPoint,false,eWeaponStateIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eStateTypeDanger,eLookTypeDanger);
	
	if (Position().distance_to(tPoint) < 5.f)
		J = A = B = false;
}

void CAI_Stalker::Think()
{
	if (m_bScriptControl) {
		ProcessScripts();
		return;
	}
	if (!m_dwLastUpdate) {
		Level().ObjectSpace.GetNearest(Position(),3.f);
		if (Level().ObjectSpace.q_nearest.size()) {
			for (u32 i=0, n = Level().ObjectSpace.q_nearest.size(); i<n; i++) {
				CInventoryItem	*tpInventoryItem	= dynamic_cast<CInventoryItem*>(Level().ObjectSpace.q_nearest[i]);
				CBolt			*tpBolt				= dynamic_cast<CBolt*>(Level().ObjectSpace.q_nearest[i]);
				if (tpInventoryItem && !tpBolt)
					m_tpItemsToTake.push_back(tpInventoryItem);
			}
		}
	}
	m_dwLastUpdate			= m_dwCurrentUpdate;
	m_dwCurrentUpdate		= Level().timeServer();
	m_bStopThinking			= false;
	m_bSearchedForEnemy		= false;
	
	m_tEnemy.Enemy			= 0;
	vfUpdateDynamicObjects	();
	vfUpdateParameters		(A,B,C,D,E,F,G,H,I,J,K,L,M);
	int						iIndex;
//	if (m_tSavedEnemy && !m_tSavedEnemy->g_Alive()) {
//		A = false;
//		B = false;
//		K = false;
//		C = false;
//		E = false;
//		D = false;
//		F = false;
//		G = false;
//		H = false;
//		I = false;
//	}
	if (!K && _K && m_tSavedEnemy && ((iIndex = ifFindDynamicObject(m_tSavedEnemy)) != -1) && m_tSavedEnemy->g_Alive() && (Level().timeServer() - m_tpaDynamicObjects[iIndex].dwTime < m_dwInertion)) {
		K = true;
		C = _C;
		E = _E;
		D = _D;
		F = _F;
		G = _G;
		H = _H;
		I = _I;
		m_bStateChanged = false;
	}
	
//	if (bfCheckIfSound()) {
//		vfUpdateVisibilityBySensitivity();
//		m_dwInertion	= 0;
//		m_dwActionStartTime = m_dwCurrentUpdate;
//	}

	m_bStateChanged = ((_A	!= A) || (_B	!= B) || (_C	!= C) || (_D	!= D) || (_E	!= E) || (_F	!= F) || (_G	!= G) || (_H	!= H) || (_I	!= I) || (_J	!= J) || (_K	!= K) || (_L	!= L));// || (_M	!= M));

//	if (m_tEnemy.Enemy && (_K != K))
//		AI_Path.TravelPath.clear();

	m_dwUpdateCount++;

#ifndef SILENCE
	if (g_Alive())
		Msg("%s : [S=%d][A=%d][B=%d][C=%d][D=%d][E=%d][F=%d][G=%d][H=%d][I=%d][J=%d][K=%d][L=%d][M=%d]",cName(),m_bStateChanged ? 1 : 0,A,B,C,D,E,F,G,H,I,J,K,L,M);
#endif
	
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
		ALifeUpdate();
	}

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