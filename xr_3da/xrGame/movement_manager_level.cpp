////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager_level.cpp
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager for level paths
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"
#include "profiler.h"
#include "level_location_selector.h"
#include "level_path_manager.h"
#include "detail_path_manager.h"
#include "ai_object_location.h"
#include "custommonster.h"
#include "level_path_builder.h"
#include "detail_path_builder.h"
#include "movement_manager_impl.h"

void CMovementManager::process_level_path()
{
	START_PROFILE("AI/Build Path/Process Level Path");

	if (!level_path().actual() && (m_path_state > ePathStateBuildLevelPath))
		m_path_state		= ePathStateBuildLevelPath;

	switch (m_path_state) {
		case ePathStateSelectLevelVertex : {
			level_selector().select_location(object().ai_location().level_vertex_id(), true);
			if (level_selector().failed())
				break;

			m_path_state		= ePathStateBuildLevelPath;
			
			if (time_over())	
				break;
		}
		case ePathStateBuildLevelPath : {
			if (!level_selector().used() || !m_selector_path_usage) {
				if (can_use_distributed_compuations(mtLevelPath)) {
					level_path_builder().setup(
						object().ai_location().level_vertex_id(),
						level_dest_vertex_id()
					);

					break;
				}

				level_path().build_path(
					object().ai_location().level_vertex_id(),
					level_dest_vertex_id()
				);
			}
			else
				level_path().build_path(
					object().ai_location().level_vertex_id(),
					level_dest_vertex_id(),
					m_selector_path_usage
				);

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
			detail().set_state_patrol_path(extrapolate_path());
			detail().set_start_position(object().Position());
			detail().set_start_direction(Fvector().setHP(-m_body.current.yaw,0));

			if (can_use_distributed_compuations(mtDetailPath)) {
				detail_path_builder().setup(
					level_path().path(),
					level_path().intermediate_index()
				);

				break;
			}
			
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
			if (!level_selector().actual(object().ai_location().level_vertex_id(),path_completed()))
				m_path_state	= ePathStateBuildLevelPath;
			else
				if (!level_path().actual())
					m_path_state	= ePathStateBuildLevelPath;
				else
					if (!detail().actual())
						m_path_state	= ePathStateBuildLevelPath;
					else {
						if (detail().completed(object().Position(),!detail().state_patrol_path())) {
							m_path_state	= ePathStateContinueLevelPath;
							if (level_path().completed()) {
								if (level_selector().used())
									m_path_state	= ePathStateSelectLevelVertex;
								else
									m_path_state	= ePathStatePathCompleted;
							}
						}
					}
			break;
		}
		case ePathStatePathCompleted : {
			if (level_selector().used())
				m_path_state	= ePathStateSelectLevelVertex;
			else
				if  (!level_path().actual())
					m_path_state = ePathStateBuildLevelPath;
				else
					if (!detail().actual())
						m_path_state = ePathStateBuildLevelPath;
			break;
		}
		default : NODEFAULT;
	}
	STOP_PROFILE;
}
