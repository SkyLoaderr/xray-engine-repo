////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager_level.cpp
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager for level paths
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"

#ifdef	NDEBUG
#define Msg(a)
#endif

void CMovementManager::process_level_path()
{
	switch (m_path_state) {

		/*******************************/
		case ePathStateSelectLevelVertex : {
		/*******************************/

			//Msg("* Path::SelectLevelVertex...");

			CLevelLocationSelector::select_location(level_vertex_id(), true);
			if (CLevelLocationSelector::failed()) {
			//	Msg("* Path::SelectLevelVertex :: [failed]");
				break;
			} else {
			//	Msg("* Path::SelectLevelVertex :: [success]");
//				const xr_vector<u32> &m_path = CLevelPathManager::path();
//				
//				for (u32 i = 0; i<m_path.size(); i++) {
//					if (i == 0) {
//				//		Msg("* Path:: Selector choose path: P[%i] = [%u]", i, m_path[i]);
//					} else {
//						float dist = ai().level_graph().vertex_position(m_path[i-1]).distance_to(ai().level_graph().vertex_position(m_path[i]));
//						float dist2 = ai().level_graph().vertex_position(m_path[0]).distance_to(ai().level_graph().vertex_position(m_path[i]));
//
//				//		Msg("* Path:: Selector choose: P[%i] = [%u]  DTP = [%f] DTF = [%f]", i, m_path[i], dist, dist2);
//					}
//					
//				}
			}
			//	Msg("* Path:: Selector choose CLevelPathManager::m_dest_vertex_id = [%u]", CLevelPathManager::m_dest_vertex_id);

				m_path_state		= ePathStateBuildLevelPath;
			if (time_over()) 	break;
		}
	    
		/*******************************/
		case ePathStateBuildLevelPath : {
		/*******************************/

			//Msg("* Path::BuildLevelPath...");

			if (!CLevelLocationSelector::used() || !m_selector_path_usage) {

				CLevelPathManager::build_path(level_vertex_id(),level_dest_vertex_id());
			}
			else { 
				CLevelPathManager::build_path(level_vertex_id(),level_dest_vertex_id(),m_selector_path_usage);
			}

			if (CLevelPathManager::failed()) {
			//	Msg("* Path::BuildLevelPath :: [failed]");
				break;
			} //else Msg("* Path::BuildLevelPath :: [success]");


			m_path_state		= ePathStateContinueLevelPath;
			if (time_over()) break;
		}

		/*******************************/
		case ePathStateContinueLevelPath : {
		/*******************************/

			//Msg("* Path:: ContinueLevelPath...");

			CLevelPathManager::select_intermediate_vertex();
			m_path_state		= ePathStateBuildDetailPath;
			if (time_over()) break;
		}

		/*******************************/
		case ePathStateBuildDetailPath : {
		/*******************************/

			Device.Statistic.TEST2.Begin();
			//Msg("* Path:: BuildDetailPath...");

			CDetailPathManager::set_state_patrol_path(false);
			CDetailPathManager::set_start_position(Position());
			CDetailPathManager::set_start_direction(Fvector().setHP(-m_body.current.yaw,0));
			if (CLevelLocationSelector::used())
				CDetailPathManager::set_dest_position(
					ai().level_graph().vertex_position(
						CLevelPathManager::intermediate_vertex_id()
					)
				);
            
			//Msg("* Path:: BuildDetailPath:: Pre Actuality = [%u]", CDetailPathManager::actual());
			
			CDetailPathManager::build_path(
				CLevelPathManager::path(),
				CLevelPathManager::intermediate_index()
			);
			//Msg("* Path:: BuildDetailPath:: Post Actuality = [%u]", CDetailPathManager::actual());
	
			Device.Statistic.TEST2.End();

			if (CDetailPathManager::failed()) {
				m_path_state	= ePathStateBuildLevelPath;
				//Msg("* Path:: BuildDetailPath :: [failed]");
				break;
			} //else Msg("* Path:: BuildDetailPath :: [success]");

			
			m_path_state		= ePathStatePathVerification;

			if (time_over())
				break;
		}
		
		/*******************************/
		case ePathStatePathVerification : {
		/*******************************/

//			Msg("* Path:: PathVerification...");
			
			if (!CLevelLocationSelector::actual(level_vertex_id(),path_completed()) ) {
				m_path_state	= ePathStateBuildLevelPath;
				//Msg("* Path:: CLevelLocationSelector [!actual]");

//				const xr_vector<u32> &m_path = CLevelPathManager::path();
//				
//				for (u32 i = 0; i<m_path.size(); i++) {
//					if (i == 0) {
//						Msg("* Path:: Selector choose path: P[%i] = [%u]", i, m_path[i]);
//					} else {
//						float dist = ai().level_graph().vertex_position(m_path[i-1]).distance_to(ai().level_graph().vertex_position(m_path[i]));
//						float dist2 = ai().level_graph().vertex_position(m_path[0]).distance_to(ai().level_graph().vertex_position(m_path[i]));
//
//						Msg("* Path:: Selector choose: P[%i] = [%u]  DTP = [%f] DTF = [%f]", i, m_path[i], dist, dist2);
//					}
//					
//				}
//			
//				Msg("* Path:: Selector choose CLevelPathManager::m_dest_vertex_id = [%u]", CLevelPathManager::m_dest_vertex_id);

			} else
			if (!CLevelPathManager::actual()) {
//				Msg("* Path:: CLevelPathManager [!actual]");
				m_path_state	= ePathStateBuildLevelPath;
			} else
				if (!CDetailPathManager::actual()) {
//					Msg("* Path:: CDetailPathManager [!actual]");
					m_path_state	= ePathStateBuildLevelPath;
				}
			else
				if (CDetailPathManager::completed(Position())) {
//					Msg("* Path:: CDetailPathManager [completed]");
					m_path_state	= ePathStateContinueLevelPath;
					
//					Msg("* Path:: LevelPath size = [%u] Intermediate index = [%u]", CLevelPathManager::path().size(), CLevelPathManager::m_intermediate_index);
					if (CLevelPathManager::completed()) {
						if (CLevelLocationSelector::used())
							m_path_state	= ePathStateSelectLevelVertex;
						else
							m_path_state	= ePathStatePathCompleted;
//						Msg("* Path:: CLevelPathManager [completed]");
					}
				}
				break;
		}
		/*****************************/
		case ePathStatePathCompleted : {
		/*****************************/
//			Msg("* Path:: PathCompleted");
			if (CLevelLocationSelector::used()){
//				Msg("* Path:: Completed :: Selector used :: [goto SelectLevelVertex]");
				m_path_state	= ePathStateSelectLevelVertex;
			} else
				if  (!CLevelPathManager::actual()) {
//					Msg("* Path:: Completed :: LevelPath [!actual]:: [goto BuildLevelPath]");
					m_path_state = ePathStateBuildLevelPath;
				} else 
					if (!CDetailPathManager::actual()){
//						Msg("* Path:: Completed :: DetailPath [!actual]:: [goto BuildDetailPath]");
						m_path_state = ePathStateBuildDetailPath;
					}
			break;
		}
		default : NODEFAULT;
	}
}

