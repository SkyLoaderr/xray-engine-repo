////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager_patrol.cpp
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager for patrol paths
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"
#include "patrol_path_manager.h"
#include "level_path_manager.h"
#include "detail_path_manager.h"
#include "ai_object_location.h"
#include "custommonster.h"

void CMovementManager::process_patrol_path()
{
	EPathState	prev_state	= m_path_state;
	for (;;) {
		switch (m_path_state) {
			case ePathStateSelectPatrolPoint : {
				patrol_path_manager().select_point(object().Position(),level_path_manager().m_dest_vertex_id);
				if (patrol_path_manager().failed())
					break;

				if (patrol_path_manager().completed()) {
					m_path_state	= ePathStatePathCompleted;
					break;
				}

				m_path_state		= ePathStateBuildLevelPath;
				if (time_over())
					break;
			}
			case ePathStateBuildLevelPath : {
				level_path_manager().build_path(object().ai_location().level_vertex_id(),level_dest_vertex_id());
				if (level_path_manager().failed())
					break;
				m_path_state		= ePathStateContinueLevelPath;
				if (time_over())
					break;
			}
			case ePathStateContinueLevelPath : {
				level_path_manager().select_intermediate_vertex();
				m_path_state		= ePathStateBuildDetailPath;
				if (time_over())
					break;
			}
			case ePathStateBuildDetailPath : {
				detail_path_manager().set_state_patrol_path(true);
				detail_path_manager().set_start_position(object().Position());
				detail_path_manager().set_start_direction(Fvector().setHP(-m_body.current.yaw,0));
				detail_path_manager().set_dest_position(patrol_path_manager().destination_position());
				detail_path_manager().build_path(
					level_path_manager().path(),
					level_path_manager().intermediate_index()
				);
				
				if (detail_path_manager().failed()) {
					m_path_state	= ePathStateBuildLevelPath;
					break;
				}
	
				m_path_state		= ePathStatePathVerification;

				if (time_over())
					break;
			}
			case ePathStatePathVerification : {
				if (!patrol_path_manager().actual())
					m_path_state	= ePathStateSelectPatrolPoint;
				else
				if (!level_path_manager().actual())
					m_path_state	= ePathStateBuildLevelPath;
				else
				if (!detail_path_manager().actual())
					m_path_state	= ePathStateBuildLevelPath;
				else
					if (detail_path_manager().completed(object().Position(),!detail_path_manager().state_patrol_path())) {
						m_path_state	= ePathStateContinueLevelPath;
						if (level_path_manager().completed()) {
							m_path_state	= ePathStateSelectPatrolPoint;
							if (patrol_path_manager().completed()) 
								m_path_state	= ePathStatePathCompleted;
						}
					}
				break;
			}
			case ePathStatePathCompleted : {
				if (!patrol_path_manager().actual())
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