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
	if (!level_path().actual() && (m_path_state > ePathStateBuildLevelPath))
		m_path_state		= ePathStateBuildLevelPath;

	if (!patrol().actual() && (m_path_state > ePathStateSelectPatrolPoint))
		m_path_state		= ePathStateSelectPatrolPoint;

	EPathState	prev_state	= m_path_state;
	for (;;) {
		switch (m_path_state) {
			case ePathStateSelectPatrolPoint : {
				patrol().select_point(object().Position(),level_path().m_dest_vertex_id);
				if (patrol().failed())
					break;

				if (patrol().completed()) {
					m_path_state	= ePathStatePathCompleted;
					break;
				}

				m_path_state		= ePathStateBuildLevelPath;
				
				if (time_over())
					break;
			}
			case ePathStateBuildLevelPath : {
				level_path().build_path(object().ai_location().level_vertex_id(),level_dest_vertex_id());
				
				if (level_path().failed())
					break;
				
				m_path_state		= ePathStateContinueLevelPath;
				
				if (time_over())
					break;
			}
			case ePathStateContinueLevelPath : {
				level_path().select_intermediate_vertex();
				
				m_path_state		= ePathStateBuildDetailPath;
				
				if (time_over())
					break;
			}
			case ePathStateBuildDetailPath : {
				detail().set_state_patrol_path(patrol().extrapolate_path());
				detail().set_start_position(object().Position());
				detail().set_start_direction(Fvector().setHP(-m_body.current.yaw,0));
				detail().set_dest_position(patrol().destination_position());
				detail().build_path(
					level_path().path(),
					level_path().intermediate_index()
				);
				
				if (detail().failed()) {
					m_path_state	= ePathStateBuildLevelPath;
					break;
				}
	
				m_path_state		= ePathStatePathVerification;

				if (time_over())
					break;
			}
			case ePathStatePathVerification : {
				if (!patrol().actual())
					m_path_state	= ePathStateSelectPatrolPoint;
				else
					if (!level_path().actual())
						m_path_state	= ePathStateBuildLevelPath;
					else
						if (!detail().actual())
							m_path_state	= ePathStateBuildLevelPath;
						else
							if (detail().completed(object().Position(),!detail().state_patrol_path())) {
								m_path_state	= ePathStateContinueLevelPath;
								if (level_path().completed()) {
									m_path_state	= ePathStateSelectPatrolPoint;
									if (patrol().completed()) 
										m_path_state	= ePathStatePathCompleted;
								}
							}
				break;
			}
			case ePathStatePathCompleted : {
				if (!patrol().actual())
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
