////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager_enemy.cpp
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager for enemy searches
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"

void CMovementManager::process_enemy_search()
{
	switch (m_path_state) {
		case ePathStatePredictEnemyVertices : {
			CEnemyLocationPredictor::predict_enemy_locations();
			if (CEnemyLocationPredictor::prediction_failed())
				break;
			m_path_state		= ePathStateSelectEnemyVertex;
			if (time_over())
				break;
		}
		case ePathStateSelectEnemyVertex : {
			CEnemyLocationPredictor::select_enemy_location();
			m_path_state		= ePathStateBuildLevelPath;
			if (time_over())
				break;
		}
		case ePathStateBuildLevelPath : {
			CLevelPathManager::build_path(level_vertex_id(),level_dest_vertex_id());
			if (CLevelPathManager::failed())
				break;
			m_path_state		= ePathStateContinueLevelPath;
			if (time_over())
				break;
		}
		case ePathStateContinueLevelPath : {
			CLevelPathManager::select_intermediate_vertex();
			m_path_state		= ePathStateBuildDetailPath;
			if (time_over())
				break;
		}
		case ePathStateBuildDetailPath : {
			Device.Statistic.TEST2.Begin();
			CDetailPathManager::set_state_patrol_path(true);
			CDetailPathManager::set_start_position(Position());
			CDetailPathManager::set_start_direction(Fvector().setHP(-m_body.current.yaw,0));
			CDetailPathManager::set_dest_position(
				ai().level_graph().vertex_position(
					CLevelPathManager::intermediate_vertex_id()
				)
			);
			CDetailPathManager::build_path	(
				CLevelPathManager::path(),
				CLevelPathManager::intermediate_index()
			);
			
			Device.Statistic.TEST2.End();
			
			if (CDetailPathManager::failed()) {
				m_path_state	= ePathStateBuildLevelPath;
				break;
			}
			m_path_state		= ePathStatePathVerification;
			if (time_over())
				break;
		}
		case ePathStatePathVerification : {
			if (!CEnemyLocationPredictor::enemy_prediction_actual())
				m_path_state	= ePathStatePredictEnemyVertices;
			else
			if (!CLevelPathManager::actual())
				m_path_state	= ePathStateBuildLevelPath;
			else
			if (!CDetailPathManager::actual())
				m_path_state	= ePathStateBuildLevelPath;
			else
				if (CDetailPathManager::completed(Position(),!state_patrol_path())) {
					m_path_state	= ePathStateContinueLevelPath;
					if (CLevelPathManager::completed())
						m_path_state	= ePathStateContinueGamePath;
				}
			break;
		}
		case ePathStatePathCompleted : {
			if (!CEnemyLocationPredictor::enemy_prediction_actual())
				m_path_state	= ePathStatePredictEnemyVertices;
			break;
		}
		default : NODEFAULT;
	}
}

