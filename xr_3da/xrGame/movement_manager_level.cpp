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

void CMovementManager::process_level_path()
{
	START_PROFILE("AI/Build Path/Process Level Path");

	switch (m_path_state) {
		case ePathStateSelectLevelVertex : {
			level_location_selector().select_location(object().ai_location().level_vertex_id(), true);
			if (level_location_selector().failed())
				break;

			m_path_state		= ePathStateBuildLevelPath;
			if (time_over())	
				break;
										   }
		case ePathStateBuildLevelPath : {
			if (!level_location_selector().used() || !m_selector_path_usage)
				level_path_manager().build_path(object().ai_location().level_vertex_id(),level_dest_vertex_id());
			else
				level_path_manager().build_path(object().ai_location().level_vertex_id(),level_dest_vertex_id(),m_selector_path_usage);

			if (level_path_manager().failed())
				break;

			m_path_state		= ePathStateContinueLevelPath;
			if (time_over()) break;
										}
		case ePathStateContinueLevelPath : {
			level_path_manager().select_intermediate_vertex();
			m_path_state		= ePathStateBuildDetailPath;
			if (time_over()) break;
										   }
		case ePathStateBuildDetailPath : {
			detail_path_manager().set_state_patrol_path(extrapolate_path());
			detail_path_manager().set_start_position(object().Position());
			detail_path_manager().set_start_direction(Fvector().setHP(-m_body.current.yaw,0));

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
			if (!level_location_selector().actual(object().ai_location().level_vertex_id(),path_completed()))
				m_path_state	= ePathStateBuildLevelPath;
			else
				if (!level_path_manager().actual())
					m_path_state	= ePathStateBuildLevelPath;
				else
					if (!detail_path_manager().actual())
						m_path_state	= ePathStateBuildLevelPath;
					else {
						if (detail_path_manager().completed(object().Position(),!extrapolate_path())) {
							m_path_state	= ePathStateContinueLevelPath;
							if (level_path_manager().completed()) {
								if (level_location_selector().used())
									m_path_state	= ePathStateSelectLevelVertex;
								else
									m_path_state	= ePathStatePathCompleted;
							}
						}
					}
			break;
		}
		case ePathStatePathCompleted : {
			if (level_location_selector().used())
				m_path_state	= ePathStateSelectLevelVertex;
			else
				if  (!level_path_manager().actual())
					m_path_state = ePathStateBuildLevelPath;
				else
					if (!detail_path_manager().actual())
						m_path_state = ePathStateBuildLevelPath;
#pragma todo("Jim to Dima: Detail_Path_Manager cannot be the only one inactual!!!")
						//m_path_state = ePathStateBuildDetailPath;
						
			break;
		}
		default : NODEFAULT;
	}

	STOP_PROFILE;
}
