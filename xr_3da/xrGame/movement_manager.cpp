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
	Init							();
}

CMovementManager::~CMovementManager	()
{
	xr_delete						(m_base_game_selector);
	xr_delete						(m_base_level_selector);
}

void CMovementManager::Init			()
{
	m_time_work						= 1300*CPU::cycles_per_microsec;
	m_speed							= 0.f;
	m_path_type						= ePathTypeDummy;
	m_path_state					= ePathStateDummy;
	
	enable_movement					(true);
	CGameLocationSelector::Init		(&ai().game_graph());
	CGamePathManager::Init			(&ai().game_graph());
	CLevelLocationSelector::Init	(ai().get_level_graph() ? &ai().level_graph() : 0);
	CLevelPathManager::Init			(ai().get_level_graph() ? &ai().level_graph() : 0);
	CDetailPathManager::Init		();
	CEnemyLocationPredictor::Init	();

	CGamePathManager::set_evaluator	(m_base_game_selector);
	CLevelPathManager::set_evaluator(m_base_level_selector);
}

void CMovementManager::Load			(LPCSTR caSection)
{
	CGameLocationSelector::Load		(caSection);
}

void CMovementManager::build_path()
{
	Device.Statistic.AI_Path.Begin();
	time_start				();
	if (!actual()) {
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
		default :				NODEFAULT;
	}
	Device.Statistic.AI_Path.End();
}