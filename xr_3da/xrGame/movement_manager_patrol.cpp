////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager_patrol.cpp
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager for patrol paths
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"

void CMovementManager::process_patrol_path()
{
	EPathState	prev_state	= m_path_state;
	for (;;) {
		switch (m_path_state) {
			case ePathStateSelectPatrolPoint : {
				CPatrolPathManager::select_point(Position(),CLevelPathManager::m_dest_vertex_id);
				if (CPatrolPathManager::failed())
					break;

				if (CPatrolPathManager::completed()) {
					m_path_state	= ePathStatePathCompleted;
					break;
				}

				m_path_state		= ePathStateBuildLevelPath;
				if (time_over())
					break;
			}
			case ePathStateBuildLevelPath : {
				CLevelPathManager::build_path(level_vertex_id(),level_dest_vertex_id());
				if (CLevelPathManager::failed())
					break;
				m_path_state		= ePathStateBuildLevelPath;
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
//				Device.Statistic.TEST2.Begin();
				CDetailPathManager::set_state_patrol_path(true);
				CDetailPathManager::set_start_position(Position());
				CDetailPathManager::set_start_direction(Fvector().setHP(-m_body.current.yaw,0));
				CDetailPathManager::set_dest_position(CPatrolPathManager::destination_position());
				CDetailPathManager::build_path(
					CLevelPathManager::path(),
					CLevelPathManager::intermediate_index()
				);
				
//				Device.Statistic.TEST2.End();
				
				if (CDetailPathManager::failed()) {
					m_path_state	= ePathStateBuildLevelPath;
					break;
				}

				m_path_state		= ePathStatePathVerification;

				if (time_over())
					break;
			}
			case ePathStatePathVerification : {
				if (!CPatrolPathManager::actual())
					m_path_state	= ePathStateSelectPatrolPoint;
				else
				if (!CLevelPathManager::actual())
					m_path_state	= ePathStateBuildLevelPath;
				else
				if (!CDetailPathManager::actual())
					m_path_state	= ePathStateBuildLevelPath;
				else
					if (CDetailPathManager::completed(Position(),!state_patrol_path())) {
						m_path_state	= ePathStateContinueLevelPath;
						if (CLevelPathManager::completed()) {
							m_path_state	= ePathStateSelectPatrolPoint;
							if (CPatrolPathManager::completed()) 
								m_path_state	= ePathStatePathCompleted;
						}
					}
				break;
			}
			case ePathStatePathCompleted : {
				if (!CPatrolPathManager::actual())
					m_path_state	= ePathStateSelectPatrolPoint;
				break;
			}
			default : NODEFAULT;
		}
		if (time_over() || (m_path_state == prev_state))
			break;
		prev_state		= m_path_state;
	}
}