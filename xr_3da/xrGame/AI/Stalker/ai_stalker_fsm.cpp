////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_fire.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Fuzzy State Machine for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../ai_monsters_misc.h"
#include "../../hudmanager.h"
#include "../../bolt.h"
#include "../../weapon.h"
#include "../../ai_script_actions.h"
#include "../../game_graph.h"

#define AI_BEST
/**
	Msg("Speed      : %7.3f",m_fCurSpeed);\
	Msg("Monster %s : \n* State : %s\n* Time delta : %7.3f\n* Global time : %7.3f",cName(),s,m_fTimeUpdateDelta,float(Level().timeServer())/1000.f);\
	Msg("Path state : %s",(m_tPathState == ePathStateSearchNode) ? "Searching for the vertex" : (m_tPathState == ePathStateBuildNodePath) ? "Building path" : (m_tPathState == ePathStateBuildTravelLine) ? "Building travel line" : "Dodging travel line");\
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
	
	SelectAnimation(XFORM().k,direction(),speed());
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
	
	m_tpSelectorCover->m_fMinEnemyDistance = m_tSavedEnemyPosition.distance_to(Position()) + 3.f;
	
	Fvector						tPoint;
	
	if (m_tEnemy.m_enemy)
		m_tEnemy.m_enemy->Center(tPoint);
	else
		tPoint					= m_tSavedEnemyPosition;

	switch (m_tActionState) {
		case eActionStateWatchGo : {
			WRITE_TO_LOG			("WatchGo : BackCover");
//			if (Level().timeServer() - m_dwActionStartTime < 8500)
//				vfSetParameters			(m_tSelectorCover,0,true,eObjectActionIdle,ePathTypeDodgeCriteria,eBodyStateStand, /**(m_tSavedEnemyPosition.distance_to(Position()) > 3.f) && m_tEnemy.m_enemy ? eMovementTypeRun : eMovementTypeWalk/**/eMovementTypeRun,eMentalStateDanger,eLookTypePathDirection);
//			else
//				vfSetParameters			(0,0,true,eObjectActionIdle,ePathTypeDodgeCriteria,eBodyStateStand, eMovementTypeWalk,eMentalStateDanger,eLookTypeFirePoint,tPoint);
			if ((Level().timeServer() - m_dwActionStartTime > 10000) && (ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),tPoint)) || (Level().timeServer() - m_dwActionStartTime > 8000))) {
				m_tActionState		= eActionStateWatchLook;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatchLook : {
			WRITE_TO_LOG			("WatchLook : BackCover");
			if (!m_tEnemy.m_enemy && (angle_difference(m_body.current.yaw, m_body.target.yaw) <= PI_DIV_6) && bfCheckIfCanKillTarget(this,m_tSavedEnemyPosition,-m_body.current.yaw,-m_body.current.pitch,ffGetFov()/180.f*PI))
				Camp(bFire);
			else {
//				vfSetParameters			(m_tSelectorCover,0,true,bFire ? eObjectActionPrimaryFire : eObjectActionIdle,ePathTypeDodgeCriteria,eBodyStateCrouch,eMovementTypeStand,eMentalStateDanger,eLookTypeFirePoint,tPoint);
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
	
	if (!m_tEnemy.m_enemy && ai().level_graph().valid_vertex_id(m_dwSavedEnemyNodeID)) {
		SearchEnemy();
		return;
	}
	
	Fvector						tPoint;
	m_tEnemy.m_enemy->Center	(tPoint);
//	float						fDistance = Position().distance_to(m_tEnemy.m_enemy->Position());

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
		m_tpSelectorFreeHunting->m_fMaxEnemyDistance = tpWeapon->m_fMaxRadius;
		m_tpSelectorFreeHunting->m_fMinEnemyDistance = 15.f;//tpWeapon->m_fMinRadius;
		m_tpSelectorFreeHunting->m_fOptEnemyDistance = 15.f;//tpWeapon->m_fMinRadius + 3.f;
	}

//	vfSetParameters				(m_tSelectorFreeHunting,0,true,(m_tpCurrentSound && m_tpCurrentSound->feedback) ? eObjectActionIdle : eObjectActionPrimaryFire,fDistance > 15.f ? ePathTypeStraightDodge : ePathTypeCriteria,eBodyStateStand,m_tEnemy.m_enemy->Position().distance_to(Position()) > 15.f ? eMovementTypeRun : eMovementTypeWalk,eMentalStateDanger,eLookTypeFirePoint,tPoint);
}

void CAI_Stalker::Camp(bool /**bWeapon/**/)
{
//	WRITE_TO_LOG			("Camping...");
//
//	if (m_bStateChanged && !m_bPlayHumming && m_tpCurrentSound) {
//		m_tpCurrentSound->stop();
//		m_tpCurrentSound = 0;
//	}
//
//	m_bPlayHumming			= false;
//	int						iIndex = ifFindDynamicObject(m_tSavedEnemy);
//	if (iIndex == -1)
//		return;
//	float					fDistance = Position().distance_to(m_tpaDynamicObjects[iIndex].tSavedPosition);
//
//	if (Position().distance_to(m_tpaDynamicObjects[iIndex].tMySavedPosition) > .1f) {
//		m_level_dest_vertex_id		= m_tpaDynamicObjects[iIndex].dwMyNodeID;
//		if (!m_level_dest_vertex_id) {
//			Msg("! Object %s is in invalid vertex",m_tpaDynamicObjects[iIndex].tpEntity ? m_tpaDynamicObjects[iIndex].tpEntity->cName() : "world");
//		}
//		vfSetParameters			(0,&(m_tpaDynamicObjects[iIndex].tMySavedPosition),false,eObjectActionIdle,ePathTypeStraight,eBodyStateCrouch,eMovementTypeWalk,eMentalStateDanger,eLookTypeFirePoint,m_tpaDynamicObjects[iIndex].tSavedPosition);
//	}
//	else {
//		float fDistanceToCover = ai().ffFindFarthestNodeInDirection(level_vertex_id(),Position(),m_tpaDynamicObjects[iIndex].tSavedPosition,m_level_dest_vertex_id);
//		if ((fDistanceToCover < fDistance/3.f) && (fDistanceToCover > .5f)) {
//			m_tpaDynamicObjects[iIndex].tMySavedPosition.sub(m_tpaDynamicObjects[iIndex].tSavedPosition,Position());
//			m_tpaDynamicObjects[iIndex].tMySavedPosition.mul((fDistanceToCover - .5f)/fDistance);
//			m_tpaDynamicObjects[iIndex].tMySavedPosition.add(Position());
//			m_tpaDynamicObjects[iIndex].dwMyNodeID			= m_level_dest_vertex_id;
//			//CDetailPathManager::path().clear();
//			//m_level_path.clear();
//			CWeapon	*tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
//			if (tpWeapon && (tpWeapon->GetAmmoElapsed() < tpWeapon->GetAmmoMagSize()/2))
//				m_inventory.Action(kWPN_RELOAD,CMD_START);
//			vfSetParameters		(0,0,false,eObjectActionIdle,ePathTypeStraight,eBodyStateCrouch,eMovementTypeStand,eMentalStateDanger,eLookTypeFirePoint,m_tpaDynamicObjects[iIndex].tSavedPosition);
//		}
//	}
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
	
	if ((speed() < EPS_L) && !m_tEnemy.m_enemy) {
		switch (m_tActionState) {
			case eActionStateWatch : {
//				vfSetParameters			(0,0,false,eObjectActionIdle,ePathTypeStraight,eBodyStateCrouch,eMovementTypeStand,eMentalStateDanger,eLookTypeDanger);
				if (m_body.current.yaw == m_body.target.yaw) {
					m_ls_yaw = m_body.current.yaw;
					m_tActionState = eActionStateDontWatch;
				}
				break;
			}
			case eActionStateDontWatch : {
				Fvector					tPoint;
				tPoint.setHP			(m_ls_yaw,0);
				tPoint.mul				(100.f);
				tPoint.add				(Position());
//				vfSetParameters			(0,0,false,eObjectActionIdle,ePathTypeStraight,eBodyStateCrouch,eMovementTypeStand,eMentalStateDanger,eLookTypeLookOver,tPoint,2500);
				break;
			}
			default : {
				m_tActionState = eActionStateWatch;
				break;
			}
		}
		return;
	}

	m_tpSelectorFreeHunting->m_fMaxEnemyDistance = m_tSavedEnemyPosition.distance_to(Position()) + m_tpSelectorFreeHunting->m_fSearchRange;
	m_tpSelectorFreeHunting->m_fOptEnemyDistance = m_tpSelectorFreeHunting->m_fMaxEnemyDistance;
	m_tpSelectorFreeHunting->m_fMinEnemyDistance = m_tSavedEnemyPosition.distance_to(Position()) + 3.f;

	m_dwRandomFactor			= 50;
	
	float						yaw,pitch;
	GetDirectionAngles			(yaw,pitch);
	yaw *= -1;
//	if (angle_difference(yaw, m_head.current.yaw) <= PI_DIV_6)
//        vfSetParameters				(m_tSelectorFreeHunting,0,true,eObjectActionIdle,ePathTypeStraightDodge,eBodyStateStand,eMovementTypeRun,eMentalStatePanic,eLookTypePathDirection);
//	else
//        vfSetParameters				(m_tSelectorFreeHunting,0,true,eObjectActionIdle,ePathTypeStraightDodge,eBodyStateStand,eMovementTypeRun,eMentalStateDanger,eLookTypePathDirection);

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
	if (!m_tEnemy.m_enemy) {
		Camp(false);
		return;
	}
	m_tpSelectorCover->m_fMaxEnemyDistance = m_tEnemy.m_enemy->Position().distance_to(Position()) + m_tpSelectorCover->m_fSearchRange;
	m_tpSelectorCover->m_fOptEnemyDistance = m_tpSelectorCover->m_fMaxEnemyDistance;
	m_tpSelectorCover->m_fMinEnemyDistance = m_tEnemy.m_enemy->Position().distance_to(Position()) + 3.f;
	
	Fvector						tPoint;
	m_tEnemy.m_enemy->Center	(tPoint);
//	vfSetParameters				(m_tSelectorCover,0,true,eObjectActionIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eMentalStateDanger,eLookTypeFirePoint,tPoint);
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
		//CDetailPathManager::path().clear();
	}
	
	Fvector tPoint;
	if (m_tEnemy.m_enemy)
		m_tEnemy.m_enemy->Center(tPoint);
	else
		tPoint = m_tSavedEnemyPosition;
	m_tpSelectorFreeHunting->m_fMaxEnemyDistance = m_tSavedEnemyPosition.distance_to(Position()) + m_tpSelectorFreeHunting->m_fSearchRange;
	m_tpSelectorFreeHunting->m_fOptEnemyDistance = 15;
	m_tpSelectorFreeHunting->m_fMinEnemyDistance = m_tSavedEnemyPosition.distance_to(Position()) + 3.f;

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
			vfContinueWithALifeGoals((F || G) ? 0 : m_tpSelectorFreeHunting);
			if ((Level().timeServer() - m_dwActionStartTime > dwDelay1) && ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),tPoint))) {
				m_tActionState		= eActionStateWatchLook;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatchLook : {
			WRITE_TO_LOG			("WatchLook : Detour");
			vfUpdateSearchPosition	();
			set_level_dest_vertex	(ai().game_graph().vertex(m_tNextGraphID)->level_vertex_id());
			if (!ai().level_graph().valid_vertex_id(level_dest_vertex_id())) {
				Msg("! Invalid graph point vertex (graph index %d)",m_tNextGraphID);
				for (int i=0; i<(int)ai().game_graph().header().vertex_count(); ++i)
					Msg("%3d : %6d",i,ai().game_graph().vertex(i)->level_vertex_id());
			}
//			vfSetParameters			((F || G) ? 0 : m_tSelectorFreeHunting,0,false,eObjectActionIdle,(F || G) ? ePathTypeStraight : ePathTypeCriteria,eBodyStateStand,eMovementTypeWalk,eMentalStateDanger,(F || G) ? eLookTypePoint : eLookTypeFirePoint,tPoint);
			if ((Level().timeServer() - m_dwActionStartTime > dwDelay2) || (ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),tPoint) == u32(-1))) {
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

//	if (!m_tEnemy.m_enemy && (eActionStateWatchGo != m_tActionState)) {
	if (!m_tEnemy.m_enemy) {
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
			m_tpSelectorCover->m_fOptEnemyDistance = (tpWeapon->m_fMinRadius + 0*tpWeapon->m_fMaxRadius)/1;
		m_tpSelectorCover->m_fMaxEnemyDistance = _max(fDistance - 1.f,m_tpSelectorCover->m_fOptEnemyDistance + 3.f);
		m_tpSelectorCover->m_fMinEnemyDistance = _max(fDistance - m_tpSelectorCover->m_fSearchRange,m_tpSelectorCover->m_fOptEnemyDistance - 3.f);
		m_previous_query_time		= 0;
		//CDetailPathManager::path().clear();
	}

	Fvector						tPoint;
	if (m_tEnemy.m_enemy)
		m_tEnemy.m_enemy->Center	(tPoint);
	else
		tPoint					=	m_tSavedEnemyPosition;
	
	bool bBackMove = false;
	if (!path_completed() && (curr_travel_point_index() < CDetailPathManager::path().size() - 1)) {
		Fvector tTemp0, tTemp1;
		tTemp0.sub(m_tSavedEnemyPosition,Position());
		tTemp1.sub(CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index() + 1].m_position,Position());
		float yaw1,yaw2,pitch1,pitch2;
		tTemp0.getHP(yaw1,pitch1);
		tTemp1.getHP(yaw2,pitch2);
		bBackMove = angle_difference(yaw1,yaw2) > 4*PI_DIV_6;
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
//			if ((Level().timeServer() - m_dwActionStartTime < 5500) && (CDetailPathManager::path().size() > CDetailPathManager::curr_travel_point_index() + 1))
//				vfSetParameters			(m_tSelectorCover,0,true,eObjectActionIdle,ePathTypeDodgeCriteria,eBodyStateStand, /**(m_tSavedEnemyPosition.distance_to(Position()) > 3.f) && m_tEnemy.m_enemy ? eMovementTypeRun : eMovementTypeWalk/**/eMovementTypeRun,eMentalStateDanger,eLookTypePathDirection);
//			else
//				vfSetParameters			(0,0,true,eObjectActionIdle,ePathTypeDodgeCriteria,eBodyStateStand, eMovementTypeWalk,eMentalStateDanger,eLookTypeFirePoint,tPoint);
			if ((Level().timeServer() - m_dwActionStartTime > 7000) && (ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),tPoint)) || (Level().timeServer() - m_dwActionStartTime > 8000))) {
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
				m_tpSelectorCover->m_fOptEnemyDistance = (tpWeapon->m_fMinRadius + 0*tpWeapon->m_fMaxRadius)/1;
			m_tpSelectorCover->m_fMaxEnemyDistance = _max(fDistance - 1.f,m_tpSelectorCover->m_fOptEnemyDistance + 3.f);
			m_tpSelectorCover->m_fMinEnemyDistance = _max(fDistance - m_tpSelectorCover->m_fSearchRange,m_tpSelectorCover->m_fOptEnemyDistance - 3.f);
//			vfSetParameters			(m_tSelectorCover,0,true,(m_tpCurrentSound && m_tpCurrentSound->feedback) ? eObjectActionIdle : eObjectActionPrimaryFire,ePathTypeDodgeCriteria,eBodyStateCrouch,eMovementTypeStand,eMentalStateDanger,eLookTypeFirePoint,tPoint);
			if (!tpWeapon || ((CWeapon::eFire != tpWeapon->STATE) && !tpWeapon->GetAmmoElapsed() && (!m_bIfSearchFailed || ((!CDetailPathManager::path().empty() && CDetailPathManager::path().size() > CDetailPathManager::curr_travel_point_index() + 1) && (Level().timeServer() - m_dwActionStartTime > 5000))))) {
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
//	WRITE_TO_LOG				("Search enemy");
//	
//	if (m_bStateChanged && !m_bPlayHumming && m_tpCurrentSound) {
//		m_tpCurrentSound->stop();
//		m_tpCurrentSound = 0;
//	}
//
//	m_bPlayHumming				= false;
//	m_dwInertion				= 180000;
//
//	INIT_SQUAD_AND_LEADER;
//	
//	CGroup						&Group = *getGroup();
//	
//	if (bfCheckIfSound()) {
//		m_dwLastEnemySearch = 0;
//		vfUpdateVisibilityBySensitivity();
//	}
//
//	if (m_dwLastEnemySearch != m_dwLastUpdate) {
//		m_tActionState			= eActionStateDontWatch;
//		m_iCurrentSuspiciousNodeIndex = 0;
//		m_bSearchedForEnemy		= true;
//		m_tPathState			= ePathStateBuildNodePath;
//		Group.m_tpaSuspiciousNodes.clear();
//		//Msg						("[%d] : Updating search params",Level().timeServer());
//	}
//	m_bSearchedForEnemy			= true;
//
//	switch (m_tActionState) {
//		case eActionStateDontWatch : {
//			OUT_TEXT("Last enemy position");
//			m_level_dest_vertex_id	= m_dwSavedEnemyNodeID;
//			if (int(m_level_dest_vertex_id) <= 0) {
//				Msg("! Invalid Saved m_enemy vertex in Search m_enemy (%s)",m_tSavedEnemy ? m_tSavedEnemy->cName() : "world");
//				m_tSavedEnemy = 0;
//				m_dwInertion = 0;
//				return;
//			}
//			if (Group.m_tpaSuspiciousNodes.empty()) {
//				if (!m_iCurrentSuspiciousNodeIndex) {
//					vfFindAllSuspiciousNodes(m_dwSavedEnemyNodeID,m_tSavedEnemyPosition,m_tSavedEnemyPosition,40.f,Group);
//					m_iCurrentSuspiciousNodeIndex = -1;
//				}
//			}
//			bool bB = false;//m_bStateChanged && !::Random.randI(2);
//			if (bB || (ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),m_tSavedEnemyPosition)) && (bfIf_I_SeePosition(m_tSavedEnemyPosition))) || (level_vertex_id() == m_dwSavedEnemyNodeID))
//				m_tActionState = eActionStateWatchGo;
//			else {
//				bool bOk = false;	
//#ifdef AI_BEST
//				for (u32 i=0; i<Group.m_tpaSuspiciousNodes.size(); ++i)
//					if (!Group.m_tpaSuspiciousNodes[i].dwSearched) {
//						if (ai().level_graph().check_vertex_in_direction(level_vertex_id(),Position(),Group.m_tpaSuspiciousNodes[i].dwNodeID)) {
//							bOk = true;
//							m_level_dest_vertex_id	= m_dwSavedEnemyNodeID;
//							vfSetParameters		(0,&m_tSavedEnemyPosition,false,eObjectActionIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeStand,eMentalStateDanger,eLookTypePoint,ai().level_graph().vertex_position(Group.m_tpaSuspiciousNodes[i].dwNodeID));
//							if (bfIf_I_SeePosition(ai().level_graph().vertex_position(Group.m_tpaSuspiciousNodes[i].dwNodeID)))
//								Group.m_tpaSuspiciousNodes[i].dwSearched = 2;
//							else {
//								m_iSuspPoint = i;
//								m_tLastActionState = m_tActionState;
//								m_tActionState = eActionStateWatch;
//							}
//							break;
//						}
//					}
//#endif
//				if (!bOk)
//					if (ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),m_tSavedEnemyPosition) == -1) {
//						if ((Level().timeServer() - m_dwLostEnemyTime < 15000) && (m_tSavedEnemyPosition.distance_to(Position()) < 6.f) && !G) {
//							Fvector				tPoint = m_tSavedEnemyPosition;
//							tPoint.y			+= 1.5f;
//							vfSetParameters		(0,&m_tSavedEnemyPosition,false,eObjectActionIdle,ePathTypeStraight,eBodyStateCrouch,eMovementTypeWalk,eMentalStateDanger,eLookTypeFirePoint,tPoint);
//						}
//						else
//							vfSetParameters		(0,&m_tSavedEnemyPosition,false,eObjectActionIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eMentalStateDanger,eLookTypeSearch);
//					}
//					else {
//						Fvector				tPoint = m_tSavedEnemyPosition;
//						tPoint.y			+= 1.5f;
//						m_level_dest_vertex_id	= m_dwSavedEnemyNodeID;
//						if (!m_level_dest_vertex_id) {
//							Msg("! Invalid Saved m_enemy vertex in Search m_enemy 2 (%s)",m_tSavedEnemy ? m_tSavedEnemy->cName() : "world");
//							m_tSavedEnemy = 0;
//							m_dwInertion = 0;
//							return;
//						}
//						vfSetParameters		(0,&m_tSavedEnemyPosition,false,eObjectActionIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eMentalStateDanger,eLookTypePoint,tPoint);
//					}
//			}
//			break;
//		}
//		case eActionStateWatch   : {
//			if (((int)Group.m_tpaSuspiciousNodes.size() - 1) < m_iSuspPoint) {
//				m_tSavedEnemy = 0;
//				m_dwInertion = 0;
//				return;
//			}
//			vfSetParameters		(0,&m_tSavedEnemyPosition,false,eObjectActionIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeStand,eMentalStateDanger,eLookTypePoint,ai().level_graph().vertex_position(Group.m_tpaSuspiciousNodes[m_iSuspPoint].dwNodeID));
//			if (bfIf_I_SeePosition(ai().level_graph().vertex_position(Group.m_tpaSuspiciousNodes[m_iSuspPoint].dwNodeID))) {
//				Group.m_tpaSuspiciousNodes[m_iSuspPoint].dwSearched = 2;
//				m_tActionState = m_tLastActionState;
//			}
//			break;
//		}
//		case eActionStateWatchGo : {
//			OUT_TEXT("The higher-level prediction");
//			if ((int)Group.m_tpaSuspiciousNodes.size() < (m_iCurrentSuspiciousNodeIndex + 1)) {
//				Group.m_tpaSuspiciousNodes.clear();
//				m_iCurrentSuspiciousNodeIndex = -1;
//				m_tActionState = eActionStateWatchLook;
//				break;
//			}
//
//			if ((m_iCurrentSuspiciousNodeIndex == -1) || ai().level_graph().check_vertex_in_direction(level_vertex_id(),Position(),Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID)) {
//				if (-1 != m_iCurrentSuspiciousNodeIndex)
//					Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwSearched = 2;
//				m_iCurrentSuspiciousNodeIndex = ifGetSuspiciousAvailableNode(m_iCurrentSuspiciousNodeIndex,Group);
//				if (m_iCurrentSuspiciousNodeIndex == -1) {
//					Group.m_tpaSuspiciousNodes.clear();
//					m_iCurrentSuspiciousNodeIndex = -1;
//					m_tActionState = eActionStateWatchLook;
//				}
//				else {
//					Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwSearched = 1;
//					m_level_dest_vertex_id = Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID;
//					if (!m_level_dest_vertex_id) {
//						Msg("! Invalid Saved m_enemy vertex in Search m_enemy 3 (%s)",m_tSavedEnemy ? m_tSavedEnemy->cName() : "world");
//					}
//					m_tPathState = ePathStateBuildNodePath;
//				}
//			}
//			
//			if ((m_iCurrentSuspiciousNodeIndex >= 0) && ((int)Group.m_tpaSuspiciousNodes.size() > m_iCurrentSuspiciousNodeIndex)) {
//				bool bOk = false;	
//#ifdef AI_BEST
//				for (u32 i=0; i<Group.m_tpaSuspiciousNodes.size(); ++i)
//					if (!Group.m_tpaSuspiciousNodes[i].dwSearched) {
//						if (ai().level_graph().check_vertex_in_direction(level_vertex_id(),Position(),Group.m_tpaSuspiciousNodes[i].dwNodeID)) {
//							bOk = true;
//							m_level_dest_vertex_id	= Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID;
//							vfSetParameters		(0,0,false,eObjectActionIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eMentalStateDanger,eLookTypePoint,ai().level_graph().vertex_position(Group.m_tpaSuspiciousNodes[i].dwNodeID));
//							if (bfIf_I_SeePosition(ai().level_graph().vertex_position(Group.m_tpaSuspiciousNodes[i].dwNodeID)))
//								Group.m_tpaSuspiciousNodes[i].dwSearched = 2;
//							else {
//								m_iSuspPoint = i;
//								m_tLastActionState = m_tActionState;
//								m_tActionState = eActionStateWatch;
//							}
//							break;
//						}
//					}
//#endif
//				if (!bOk)
//					if ((ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),ai().level_graph().vertex_position(Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID)) == -1) && (ai().u_SqrDistance2Node(Position(),ai().level_graph().vertex(Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID)) < .35f)) {
//						m_level_dest_vertex_id	= Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID;
//						if (!m_level_dest_vertex_id) {
//							Msg("! Invalid Saved m_enemy vertex in Search m_enemy 4 (%s)",m_tSavedEnemy ? m_tSavedEnemy->cName() : "world");
//						}
//						vfSetParameters		(0,0,false,eObjectActionIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eMentalStateDanger,eLookTypeSearch);
//					}
//					else {
//						Fvector				tPoint = ai().level_graph().vertex_position(Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID);
//						tPoint.y			+= 1.5f;
//						m_level_dest_vertex_id	= Group.m_tpaSuspiciousNodes[m_iCurrentSuspiciousNodeIndex].dwNodeID;
//						if (!m_level_dest_vertex_id) {
//							Msg("! Invalid Saved m_enemy vertex in Search m_enemy 5 (%s)",m_tSavedEnemy ? m_tSavedEnemy->cName() : "world");
//						}
//						vfSetParameters		(0,0,false,eObjectActionIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eMentalStateDanger,eLookTypePoint,tPoint);
//					}
//			}
//	
//			break;
//		}
//		case eActionStateWatchLook : {
////			OUT_TEXT("Predicting by selector");
////			m_tSelectorRetreat.m_tEnemyPosition = m_tMySavedPosition;
////			m_tSelectorRetreat.m_tpEnemyNode	= ai().level_graph().vertex(m_dwMyNodeID);
////			m_tSelectorRetreat.m_tMyPosition	= Position();
////			m_tSelectorRetreat.m_tpMyNode		= ai().level_graph().vertex(level_vertex_id());
////			m_tSelectorRetreat.m_fMinEnemyDistance = Position().distance_to(m_tMySavedPosition) + 3.f;
////			vfSetParameters		(&m_tSelectorRetreat,0,true,eObjectActionIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eMentalStateDanger,eLookTypeDanger);
////			if (m_bIfSearchFailed)
////				m_dwInertion						= 0;
////			else
////				m_dwInertion						= 120000;
//			m_dwInertion					= 0;
//			break;
//		}
//	}
}

void CAI_Stalker::ExploreDNE()
{
	if (m_bStateChanged && !m_bPlayHumming && m_tpCurrentSound) {
		m_tpCurrentSound->stop();
		m_tpCurrentSound = 0;
	}

	m_dwInertion			= 60000;
	Fvector					tPoint = m_tpaDynamicSounds[m_iSoundIndex].tSavedPosition;
	if (ai().level_graph().inside(ai().level_graph().vertex(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID),tPoint))
		tPoint.y			= ai().level_graph().vertex_plane_y(*ai().level_graph().vertex(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID),tPoint.x,tPoint.z);
	else
		tPoint				= ai().level_graph().vertex_position(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID);
	if (m_bStateChanged) {
		m_tActionState		= eActionStateDontWatch;
		m_dwActionStartTime = Level().timeServer();
		//CDetailPathManager::path().clear();
	}
	m_tpSelectorFreeHunting->m_fOptEnemyDistance = _max(25.f,Position().distance_to(tPoint));
	switch (m_tActionState) {
		case eActionStateDontWatch : {
			WRITE_TO_LOG			("DontWatch : Exploring danger non-expedient ref_sound");
			vfContinueWithALifeGoals(m_tpSelectorFreeHunting);
			if (ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),tPoint))) {
				m_tActionState		= eActionStateWatch;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatch : {
			WRITE_TO_LOG			("Watch : Exploring danger non-expedient ref_sound");
//			vfSetParameters			(0,0,false,eObjectActionIdle,ePathTypeStraight,eBodyStateCrouch,eMovementTypeStand,eMentalStateDanger,eLookTypeLookFireOver,tPoint,2500);
			if (Level().timeServer() - m_dwActionStartTime > 9000) {
				m_tActionState		= eActionStateWatchGo;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatchGo : {
			WRITE_TO_LOG			("WatchGo : Exploring danger non-expedient ref_sound");
			vfContinueWithALifeGoals(m_tpSelectorFreeHunting);
			if ((Level().timeServer() - m_dwActionStartTime > 7000) && ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),tPoint))) {
				m_tActionState		= eActionStateWatchLook;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatchLook : {
			WRITE_TO_LOG			("WatchLook : Exploring danger non-expedient ref_sound");
			vfUpdateSearchPosition	();
			set_level_dest_vertex	(ai().game_graph().vertex(m_tNextGraphID)->level_vertex_id());
			if (!level_dest_vertex_id()) {
				Msg("! Invalid graph point vertex (graph index %d)",m_tNextGraphID);
				for (int i=0; i<(int)ai().game_graph().header().vertex_count(); ++i)
					Msg("%3d : %6d",i,ai().game_graph().vertex(i)->level_vertex_id());
			}
//			vfSetParameters			(0,0,false,eObjectActionIdle,ePathTypeCriteria,eBodyStateStand,eMovementTypeWalk,eMentalStateDanger,eLookTypePoint,tPoint);
			if ((Level().timeServer() - m_dwActionStartTime > 5000) || (ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),tPoint) == u32(-1))) {
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
	R_ASSERT2				(ai().level_graph().valid_vertex_id(m_dwSavedEnemyNodeID), "Invalid enemy vertex");
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
	R_ASSERT2				(ai().level_graph().valid_vertex_id(m_dwSavedEnemyNodeID), "Invalid enemy vertex");
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
	if (ai().level_graph().inside(ai().level_graph().vertex(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID),tPoint))
		tPoint.y			= ai().level_graph().vertex_plane_y(*ai().level_graph().vertex(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID),tPoint.x,tPoint.z);
	else
		tPoint				= ai().level_graph().vertex_position(m_tpaDynamicSounds[m_iSoundIndex].dwNodeID);
	if (m_bStateChanged) {
		m_tActionState = eActionStateDontWatch;
		m_dwActionStartTime = Level().timeServer();
		//CDetailPathManager::path().clear();
	}
	switch (m_tActionState) {
		case eActionStateDontWatch : {
			WRITE_TO_LOG			("DontWatch : Exploring non-danger non-expedient ref_sound");
			vfContinueWithALifeGoals();
			if (ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),tPoint))) {
				m_tActionState = eActionStateWatch;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatch : {
			WRITE_TO_LOG			("Watch : Exploring non-danger non-expedient ref_sound");
//			vfSetParameters			(0,0,false,eObjectActionIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeStand,eMentalStateDanger,eLookTypeLookOver,tPoint,1200);
			if (Level().timeServer() - m_dwActionStartTime > 5000) {
				m_tActionState = eActionStateWatchGo;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatchGo : {
			WRITE_TO_LOG			("WatchGo : Exploring non-danger non-expedient ref_sound");
			vfContinueWithALifeGoals();
			if ((Level().timeServer() - m_dwActionStartTime > 10000) && ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),tPoint))) {
				m_tActionState = eActionStateWatchLook;
				m_dwActionStartTime = Level().timeServer();
			}
			break;
		}
		case eActionStateWatchLook : {
			WRITE_TO_LOG			("WatchLook : Exploring non-danger non-expedient ref_sound");
			vfUpdateSearchPosition	();
			set_level_dest_vertex	(ai().game_graph().vertex(m_tNextGraphID)->level_vertex_id());
			if (!level_dest_vertex_id()) {
				Msg("! Invalid graph point vertex (graph index %d)",m_tNextGraphID);
				for (int i=0; i<(int)ai().game_graph().header().vertex_count(); ++i)
					Msg("%3d : %6d",i,ai().game_graph().vertex(i)->level_vertex_id());
			}
//			vfSetParameters(0,0,false,eObjectActionIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eMentalStateDanger,eLookTypePoint,tPoint);
			if ((Level().timeServer() - m_dwActionStartTime > 3000) || (ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),tPoint) == u32(-1))) {
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
		//CDetailPathManager::path().clear();
	}
	Fvector					tPoint;

	CInventoryItem *tpItemToTake = 0;
	
	bool bOk;
	do {
		bOk = true;
		vfSelectItemToTake(tpItemToTake);

		if (!tpItemToTake)
			return;

		if (!tpItemToTake->level_vertex_id() || (tpItemToTake->level_vertex_id() >= ai().level_graph().header().vertex_count())) {
			bOk = false;
			for (u32 i=0; i<m_tpItemsToTake.size(); ++i)
				if (m_tpItemsToTake[i] == tpItemToTake) {
					m_tpItemsToTake.erase(m_tpItemsToTake.begin() + i);
					break;
				}
		}
	}
	while (!bOk);

	tpItemToTake->Center(tPoint);
	set_level_dest_vertex	(tpItemToTake->level_vertex_id());
	if (!level_dest_vertex_id()) {
		Msg("! Invalid item vertex %s",tpItemToTake->cName());
	}
	if (ai().level_graph().inside(ai().level_graph().vertex(level_dest_vertex_id()),tPoint))
		tPoint.y			= ai().level_graph().vertex_plane_y(*ai().level_graph().vertex(level_dest_vertex_id()),tPoint.x,tPoint.z);
	else
		tPoint				= ai().level_graph().vertex_position(level_dest_vertex_id());
	
//	if (ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(level_vertex_id(),Position(),tPoint)))
//		vfSetParameters(0,&tPoint,false,eObjectActionIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eMentalStateDanger,eLookTypePoint,tPoint);
//	else
//		vfSetParameters(0,&tPoint,false,eObjectActionIdle,ePathTypeStraight,eBodyStateStand,eMovementTypeWalk,eMentalStateDanger,eLookTypeDanger);
	
	if (Position().distance_to(tPoint) < 5.f)
		J = A = B = false;
}

void CAI_Stalker::Think()
{
//	for (;;) {
//		u32 v_id = ::Random.randI(ai().game_graph().header().vertex_count());
//		if (ai().game_graph().vertex(v_id).level_point().distance_to(Position())) {
//			set_game_dest_node	(ALife::_GRAPH_ID(v_id));
//			set_path_type		(ePathTypeGamePath);
//			update_path			();
//			break;
//		}
//	}
//
//	set_game_dest_vertex(34);
//	m_tMovementType		= eMovementTypeWalk;
//	m_tMentalState		= eMentalStateDanger;
//	set_path_type		(ePathTypeGamePath);
//	CMovementManager::CGameLocationSelector::set_selection_type	(eSelectionTypeRandomBranching);
//	update_path			();
//	m_tMovementType		= eMovementTypeWalk;
//	m_tMentalState		= eMentalStateDanger;
//	CMovementManager::set_path_type		(ePathTypePatrolPath);
//	CPatrolPathManager::set_path		("way0000");
//	CPatrolPathManager::set_start_type	(ePatrolStartTypeNearest);
//	CPatrolPathManager::set_route_type	(ePatrolRouteTypeContinue);
//	CPatrolPathManager::set_random		(true);
//	update_path			();
//	SetDirectionLook	();
	return;
//	if (!m_dwLastUpdate) {
//		Level().ObjectSpace.GetNearest(Position(),3.f);
//		if (Level().ObjectSpace.q_nearest.size()) {
//			for (u32 i=0, n = Level().ObjectSpace.q_nearest.size(); i<n; ++i) {
//				CInventoryItem	*tpInventoryItem	= dynamic_cast<CInventoryItem*>(Level().ObjectSpace.q_nearest[i]);
//				CBolt			*tpBolt				= dynamic_cast<CBolt*>(Level().ObjectSpace.q_nearest[i]);
//				if (tpInventoryItem && !tpBolt)
//					m_tpItemsToTake.push_back(tpInventoryItem);
//			}
//		}
//	}
//	m_dwLastUpdate			= m_current_update;
//	m_current_update		= Level().timeServer();
//	m_bStopThinking			= false;
//	m_bSearchedForEnemy		= false;
//	
//	m_tEnemy.m_enemy			= 0;
//	vfUpdateDynamicObjects	();
//	vfUpdateParameters		(A,B,C,D,E,F,G,H,I,J,K,L,M);
//	int						iIndex;
////	if (m_tSavedEnemy && !m_tSavedEnemy->g_Alive()) {
////		A = false;
////		B = false;
////		K = false;
////		C = false;
////		E = false;
////		D = false;
////		F = false;
////		G = false;
////		H = false;
////		I = false;
////	}
//	if (!K && _K && m_tSavedEnemy && (((iIndex = ifFindDynamicObject(m_tSavedEnemy)) != -1)) && m_tSavedEnemy->g_Alive() && (Level().timeServer() - m_tpaDynamicObjects[iIndex].dwTime < m_dwInertion)) {
//		K = true;
//		C = _C;
//		E = _E;
//		D = _D;
//		F = _F;
//		G = _G;
//		H = _H;
//		I = _I;
//		m_bStateChanged = false;
//	}
//	
////	if (bfCheckIfSound()) {
////		vfUpdateVisibilityBySensitivity();
////		m_dwInertion	= 0;
////		m_dwActionStartTime = m_current_update;
////	}
//
//	m_bStateChanged = ((_A	!= A) || (_B	!= B) || (_C	!= C) || (_D	!= D) || (_E	!= E) || (_F	!= F) || (_G	!= G) || (_H	!= H) || (_I	!= I) || (_J	!= J) || (_K	!= K) || (_L	!= L));// || (_M	!= M));
//
////	if (m_tEnemy.m_enemy && (_K != K))
////		CDetailPathManager::path().clear();
//
//	++m_dwUpdateCount;
//
//#ifndef SILENCE
//	if (g_Alive())
//		Msg("%s : [S=%d][A=%d][B=%d][C=%d][D=%d][E=%d][F=%d][G=%d][H=%d][I=%d][J=%d][K=%d][L=%d][M=%d]",cName(),m_bStateChanged ? 1 : 0,A,B,C,D,E,F,G,H,I,J,K,L,M);
//#endif
//	
//	if (!g_Alive()) {
//		Death				();
//	}				   
//	else 
// 	if ((-1 != m_dwParticularState) && (K || (7 == m_dwParticularState))) {
//		switch (m_dwParticularState) {
//			case 0 : {
//				m_dwRandomFactor = 100;
//				ForwardStraight();
//				break;
//			}
//			case 1 : {
//				m_dwRandomFactor = 0;
//				ForwardStraight();
//				break;
//			}
//			case 2 : {
//				m_dwRandomFactor = 100;
//				ForwardCover();
//				break;
//			}
//			case 3 : {
//				m_dwRandomFactor = 50;
//				BackCover();
//				break;
//			}
//			case 4 : {
//				m_dwRandomFactor = 50;
//				Panic();
//				break;
//			}
//			case 5 : {
//				m_dwRandomFactor = 50;
//				Hide();
//				break;
//			}
//			case 6 : {
//				m_dwRandomFactor = 50;
//				Detour();
//				break;
//			}
//			case 7 : {
//				m_tBodyState	= eBodyStateStand;
//				m_tMovementType = eMovementTypeStand;
//				m_tMentalState	= eMentalStateDanger;
//				break;
//			}
//			default : {
//				break;
//			}
//		}
//	}
//	else
//	if (C && H && I) {
//		m_dwRandomFactor	= 50;
//		Panic			();
//	} else
//	if (C && H && !I) {
//		Hide();
//	} else
//	if (C && !H && I) {
//		m_dwRandomFactor	= 50;
//		Panic			();
//	} else
//	if (C && !H && !I) {
//		Hide();
//	} else
//	
//	if (D && H && I) {
//		m_dwRandomFactor	= 50;
//		BackCover	(true);
//	} else
//	if (D && H && !I) {
//		Hide		();
//	} else
//	if (D && !H && I) {
//		m_dwRandomFactor	= 50;
//		BackCover	(false);
//	} else
//	if (D && !H && !I) {
//		Hide	();
//	} else
//	
//	if (E && H && I) {
//		m_dwRandomFactor	= 0;
//		ForwardCover	();
//	} else
//	if (E && H && !I) {
//		m_dwRandomFactor	= 0;
//		ForwardCover	();
//	} else
//	if (E && !H && I) {
//		Detour			();
//	} else
//	if (E && !H && !I) {
//		Detour			();
//	} else
//	
//	if (F && H && I) {
//		m_dwRandomFactor	= 50;
//		ForwardStraight	();
//	} else
//	if (F && H && !I) {
//		m_dwRandomFactor	= 100;
//		ForwardStraight	();
//	} else
//	if (F && !H && I) {
//		Detour			();
//	} else
//	if (F && !H && !I) {
//		Detour			();
//	} else
//	
//	if (G && H && I) {
//		m_dwRandomFactor	= 75;
//		ForwardStraight	();
//	} else
//	if (G && H && !I) {
//		m_dwRandomFactor	= 100;
//		ForwardStraight	();
//	} else
//	if (G && !H && I) {
//		Detour			();
//	} else
//	if (G && !H && !I) {
//		Detour			();
//	} else
//	
//	if (A && !K && !H && !L) {
//		ExploreDNE();
//	} else
//	if (A && !K && H && !L) {
//		ExploreDE();
//	} else
//	if (A && !K && !H && L) {
//		ExploreDNE();
//	} else
//	if (A && !K && H && L) {
//		ExploreDE();
//	} else
//	
//	if (B && !K && !H && !L) {
//		ExploreNDNE();
//	} else
//	if (B && !K && H && !L) {
//		ExploreNDE();
//	} else
//	if (B && !K && !H && L) {
//		ExploreNDNE();
//	} else
//	if (B && !K && H && L) {
//		ExploreNDE();
//	} else
//	if (M) {
//		TakeItems();
//	} else {
//		ALifeUpdate();
//	}
//
//	if (m_bSearchedForEnemy)
//		m_dwLastEnemySearch		= m_current_update;
//	else {
//		getGroup()->m_tpaSuspiciousNodes.clear();
//		m_iCurrentSuspiciousNodeIndex = 0;
//	}
//
//	_A	= A;
//	_B	= B;
//	_C	= C;
//	_D	= D;
//	_E	= E;
//	_F	= F;
//	_G	= G;
//	_H	= H;
//	_I	= I;
//	_J	= J;
//	_K	= K;
//	_L	= L;
//	_M	= M;
}