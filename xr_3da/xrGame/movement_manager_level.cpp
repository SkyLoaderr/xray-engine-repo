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
			CLevelLocationSelector::select_location(level_vertex_id(), true);
			if (CLevelLocationSelector::failed())
				break;
			
			m_path_state		= ePathStateBuildLevelPath;
			if (time_over())	
				break;
		}
		case ePathStateBuildLevelPath : {
			if (!CLevelLocationSelector::used() || !m_selector_path_usage)
				CLevelPathManager::build_path(level_vertex_id(),level_dest_vertex_id());
			else
				CLevelPathManager::build_path(level_vertex_id(),level_dest_vertex_id(),m_selector_path_usage);

			if (CLevelPathManager::failed())
				break;
			
			m_path_state		= ePathStateContinueLevelPath;
			if (time_over()) break;
		}
		case ePathStateContinueLevelPath : {
			CLevelPathManager::select_intermediate_vertex();
			m_path_state		= ePathStateBuildDetailPath;
			if (time_over()) break;
		}
		case ePathStateBuildDetailPath : {
//			Device.Statistic.TEST2.Begin();
			
			CDetailPathManager::set_state_patrol_path(false);
			CDetailPathManager::set_start_position(Position());
			CDetailPathManager::set_start_direction(Fvector().setHP(-m_body.current.yaw,0));
			
			CDetailPathManager::build_path(
				CLevelPathManager::path(),
				CLevelPathManager::intermediate_index()
			);
	
//			Device.Statistic.TEST2.End();

			if (CDetailPathManager::failed()) {
				m_path_state	= ePathStateBuildLevelPath;
				
				CDetailPathManager::build_path(
					CLevelPathManager::path(),
					CLevelPathManager::intermediate_index()
				);

				
				break;
			}

			m_path_state		= ePathStatePathVerification;

			if (time_over())
				break;
		}
		case ePathStatePathVerification : {
			if (!CLevelLocationSelector::actual(level_vertex_id(),path_completed()))
				m_path_state	= ePathStateBuildLevelPath;
			else
				if (!CLevelPathManager::actual())
					m_path_state	= ePathStateBuildLevelPath;
				else
					if (!CDetailPathManager::actual())
						m_path_state	= ePathStateBuildLevelPath;
					else
						if (CDetailPathManager::completed(Position())) {
							m_path_state	= ePathStateContinueLevelPath;
							if (CLevelPathManager::completed()) {
								if (CLevelLocationSelector::used())
									m_path_state	= ePathStateSelectLevelVertex;
								else
									m_path_state	= ePathStatePathCompleted;
							}
						}
			break;
		}
		case ePathStatePathCompleted : {
			if (CLevelLocationSelector::used())
				m_path_state	= ePathStateSelectLevelVertex;
			else
				if  (!CLevelPathManager::actual())
					m_path_state = ePathStateBuildLevelPath;
				else
					if (!CDetailPathManager::actual())
						m_path_state = ePathStateBuildDetailPath;
			break;
		}
		default : NODEFAULT;
	}
}
