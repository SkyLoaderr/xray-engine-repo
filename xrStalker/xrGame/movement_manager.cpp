////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"

CMovementManager::CMovementManager	()
{
	m_base_game_selector			= xr_new<CGraphEngine::CBaseParameters>();
	m_base_level_selector			= xr_new<CGraphEngine::CBaseParameters>();
	m_dwFrameReinit					= u32(-1);
	m_dwFrameReload					= u32(-1);
	Init							();
}

CMovementManager::~CMovementManager	()
{
	xr_delete						(m_base_game_selector);
	xr_delete						(m_base_level_selector);
}

void CMovementManager::Init			()
{
}

void CMovementManager::Load			(LPCSTR section)
{
}

void CMovementManager::reinit		()
{
	if (!frame_check(m_dwFrameReinit))
		return;
	m_time_work								= 30000*CPU::cycles_per_microsec;
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
}

void CMovementManager::update_path()
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
}