////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager_game.cpp
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager for game paths
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"

void CMovementManager::process_game_path()
{
	switch (m_path_state) {
		case ePathStateSelectGameVertex : {
			CGameLocationSelector::select_location(game_vertex_id(),CGamePathManager::m_dest_vertex_id);
			if (CGameLocationSelector::failed())
				break;
			m_path_state	= ePathStateBuildGamePath;
			if (time_over())
				break;
		}
		case ePathStateBuildGamePath : {
			Device.Statistic.TEST0.Begin();
			CGamePathManager::build_path(game_vertex_id(),game_dest_vertex_id());
			if (CGamePathManager::failed()) {
				Device.Statistic.TEST0.End();
				break;
			}
			m_path_state	= ePathStateContinueGamePath;
			Device.Statistic.TEST0.End();
			if (time_over())
				break;
		}
		case ePathStateContinueGamePath : {
			CGamePathManager::select_intermediate_vertex();
			m_path_state	= ePathStateBuildLevelPath;
			if (time_over())
				break;
		}
		case ePathStateBuildLevelPath : {
			Device.Statistic.TEST1.Begin();
			CLevelPathManager::build_path(
				level_vertex_id(),
				ai().game_graph().vertex(
					CGamePathManager::intermediate_vertex_id()
				)->level_vertex_id()
			);
			if (CLevelPathManager::failed()) {
				Device.Statistic.TEST1.End();
				break;
			}
			m_path_state		= ePathStateContinueLevelPath;
			Device.Statistic.TEST1.End();
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
			CDetailPathManager::set_state_patrol_path(false);
			CDetailPathManager::set_start_position(Position());
			CDetailPathManager::set_start_direction(Fvector().setHP(-m_body.current.yaw,0));
			CDetailPathManager::set_dest_position( 
				ai().level_graph().vertex_position(
					CLevelPathManager::intermediate_vertex_id()
				)
			);
			CDetailPathManager::build_path(
				CLevelPathManager::path(),
				CLevelPathManager::intermediate_index()
			);
			if (CDetailPathManager::failed()) {
				m_path_state	= ePathStateBuildLevelPath;
				Device.Statistic.TEST2.End();
				break;
			}
			m_path_state		= ePathStatePathVerification;
			Device.Statistic.TEST2.End();
			break;
		}
		case ePathStatePathVerification : {
			if (!CGameLocationSelector::actual(game_vertex_id()))
				m_path_state	= ePathStateSelectGameVertex;
			else
			if (!CGamePathManager::actual())
				m_path_state	= ePathStateBuildGamePath;
			else
				if (!CLevelPathManager::actual())
				m_path_state	= ePathStateBuildLevelPath;
			else
			if (!CDetailPathManager::actual())
				m_path_state	= ePathStateBuildDetailPath;
			else
				if (CDetailPathManager::completed(Position())) {
					m_path_state	= ePathStateContinueLevelPath;
					if (CLevelPathManager::completed()) {
						m_path_state	= ePathStateContinueGamePath;
						if (CGamePathManager::completed())
							m_path_state	= ePathStatePathCompleted;
					}
				}
			break;
		}
		case ePathStatePathCompleted : {
			if (!CGameLocationSelector::actual(game_vertex_id()))
				m_path_state	= ePathStateSelectGameVertex;
			break;
		}
		default : NODEFAULT;
	}
}

