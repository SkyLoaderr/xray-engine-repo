////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"

const float verify_distance = 15.f;

CMovementManager::CMovementManager	()
{
	m_base_game_selector			= xr_new<CGraphEngine::CBaseParameters>();
	m_base_level_selector			= xr_new<CGraphEngine::CBaseParameters>();
	m_dwFrameLoad					= u32(-1);
	m_dwFrameReinit					= u32(-1);
	m_dwFrameReload					= u32(-1);
	m_dwFrameNetSpawn				= u32(-1);
	m_dwFrameNetDestroy				= u32(-1);
	init							();
}

CMovementManager::~CMovementManager	()
{
	xr_delete						(m_base_game_selector);
	xr_delete						(m_base_level_selector);
}

void CMovementManager::init			()
{
}

void CMovementManager::Load			(LPCSTR section)
{
	if (!frame_check(m_dwFrameLoad))
		return;

	CRestrictedObject::Load			(section);
	CPhysicsShellHolder::Load		(section);
}

void CMovementManager::reinit		()
{
	if (!frame_check(m_dwFrameReinit))
		return;
	
	CRestrictedObject::reinit		();
	CPhysicsShellHolder::reinit		();
	
	m_time_work								= 300*CPU::cycles_per_microsec;
	m_speed									= 0.f;
	m_path_type								= ePathTypeNoPath;
	m_path_state							= ePathStateDummy;
	m_path_actuality						= true;
	m_speed									= 0.f;
	m_selector_path_usage					= false;//true;
	m_old_desirable_speed					= 0.f;
	m_refresh_rate							= 0;
	m_last_update							= Level().timeServer();

	enable_movement							(true);
	CGameLocationSelector::reinit			(&ai().game_graph());
	CLevelLocationSelector::reinit			(ai().get_level_graph());
	CDetailPathManager::reinit				();
	CGamePathManager::reinit				(&ai().game_graph());
	CLevelPathManager::reinit				(ai().get_level_graph());
	CPatrolPathManager::reinit				();
	CEnemyLocationPredictor::reinit			();

	CGameLocationSelector::set_dest_path	(CGamePathManager::m_path);
	CLevelLocationSelector::set_dest_path	(CLevelPathManager::m_path);
	CLevelLocationSelector::set_dest_vertex	(CLevelPathManager::m_dest_vertex_id);
}

void CMovementManager::reload		(LPCSTR section)
{
	if (!frame_check(m_dwFrameReload))
		return;
	
	CRestrictedObject::reload		(section);
	CPhysicsShellHolder::reload		(section);
}

BOOL CMovementManager::net_Spawn	(LPVOID data)
{
	if (!frame_check(m_dwFrameNetSpawn))
		return						(TRUE);

	return							(CRestrictedObject::net_Spawn(data) && CPhysicsShellHolder::net_Spawn(data));
}

void CMovementManager::net_Destroy	()
{
	if (!frame_check(m_dwFrameNetDestroy))
		return;

	CRestrictedObject::net_Destroy	();
	CPhysicsShellHolder::net_Destroy();
}

void CMovementManager::Hit			(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
{
	CPhysicsShellHolder::Hit		(P,dir,who,element,p_in_object_space,impulse,hit_type);
}

void CMovementManager::update_path	()
{
	if (!enabled() || ((/**actual_all() || /**/(m_last_update > Level().timeServer())) && !path_completed()))
		return;

	m_last_update			= Level().timeServer() + m_refresh_rate;

	time_start				();
	
	if (!CGamePathManager::evaluator()) 		
		CGamePathManager::set_evaluator	(m_base_game_selector);

	if (!CLevelPathManager::evaluator()) 
		CLevelPathManager::set_evaluator(m_base_level_selector);

	if (!actual()) {

		CGamePathManager::make_inactual();
		CLevelPathManager::make_inactual();
		CPatrolPathManager::make_inactual();
		switch (m_path_type) {
			case ePathTypeGamePath : {
				m_path_state	= ePathStateSelectGameVertex;
				break;
			}
			case ePathTypeLevelPath : {
				m_path_state	= ePathStateSelectLevelVertex;
				break;
			}
			case ePathTypeEnemySearch : {
				m_path_state	= ePathStatePredictEnemyVertices;
				break;
			}
			case ePathTypePatrolPath : {
				m_path_state	= ePathStateSelectPatrolPoint;
				break;
			}
			case ePathTypeNoPath : {
				m_path_state	= ePathStateDummy;
				break;
			}
			default :			NODEFAULT;
		}
		m_path_actuality	= true;
	}

	switch (m_path_type) {
		case ePathTypeGamePath : {
			process_game_path	();
			break;
		}
		case ePathTypeLevelPath : {
			process_level_path	();
			break;
		}
		case ePathTypeEnemySearch : {
			process_enemy_search();
			break;
									}
		case ePathTypePatrolPath : {
			process_patrol_path();
			break;
		}
		case ePathTypeNoPath : {
			break;
		}
		default :				NODEFAULT;
	}

	if (accessible(Position()))
		verify_detail_path		();
}

void CMovementManager::verify_detail_path	()
{
	if (CDetailPathManager::path().empty() || CDetailPathManager::completed(CDetailPathManager::dest_position()))
		return;

	float distance = 0.f;
	for (u32 i=CDetailPathManager::curr_travel_point_index() + 1, n=CDetailPathManager::path().size(); i<n; ++i) {
		if (!accessible(CDetailPathManager::path()[i].position)) {
			m_path_actuality	= false;
			return;
		}
		distance	+= CDetailPathManager::path()[i].position.distance_to(CDetailPathManager::path()[i-1].position);
		if (distance >= verify_distance)
			break;
	}
}

