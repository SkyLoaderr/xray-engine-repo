////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager_level.cpp
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager for level paths
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"

void CMovementManager::process_level_path()
{
	switch (m_path_state) {
		case ePathStateSelectLevelVertex : {
			CLevelLocationSelector::select_location(level_vertex_id(),CLevelPathManager::m_dest_vertex_id);
			if (CLevelLocationSelector::failed())
				break;
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
			CDetailPathManager::set_start_position(Position());
			if (CLevelLocationSelector::used())
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

			if (time_over())
				break;
		}
		case ePathStatePathVerification : {
			if (!CLevelLocationSelector::actual(level_vertex_id()))
				m_path_state	= ePathStateSelectLevelVertex;
			else
			if (!CLevelPathManager::actual())
				m_path_state	= ePathStateBuildLevelPath;
			else
			if (!CDetailPathManager::actual())
				m_path_state	= ePathStateBuildDetailPath;
			else
				if (CDetailPathManager::completed(Position())) {
					m_path_state	= ePathStateContinueLevelPath;
					if (CLevelPathManager::completed())
						m_path_state	= ePathStateContinueGamePath;
				}
			break;
		}
		case ePathStatePathCompleted : {
			if (!CLevelLocationSelector::actual(level_vertex_id()))
				m_path_state	= ePathStateSelectLevelVertex;
			break;
		}
		default : NODEFAULT;
	}
}

