////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : m_PhysicMovementControl manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"
#include "PHMovementControl.h"
#include "gameobject.h"

CMovementManager::CMovementManager	()
{
	Init					();
}

CMovementManager::~CMovementManager	()
{
}

void CMovementManager::Init			()
{
	m_time_work						= 300*CPU::cycles_per_microsec;
	CGameLocationSelector::Init		();
	CGamePathManager::Init			();
	CLevelLocationSelector::Init	();
	CLevelPathManager::Init			();
	CDetailPathManager::Init		();
	CEnemyLocationPredictor::Init	();
}

void CMovementManager::move_along_path	(CPHMovementControl *movement_control, float time_delta)
{
	Fvector				motion;

#ifndef NO_PHYSICS_IN_AI_MOVE
	float				precision = 1.f;
#endif

	if (m_detail_path.empty() || (m_detail_path.size() - 1 <= m_detail_cur_point_index))	{
		m_speed			= 0;
#ifndef NO_PHYSICS_IN_AI_MOVE
		if(movement_control->IsCharacterEnabled()) {
			motion.set	(0,0,0);
			movement_control->Calculate(m_detail_path,0.f,m_detail_cur_point_index,precision);
			movement_control->GetPosition(Position());
		}

		if (movement_control->gcontact_HealthLost) {
			Fvector		d;
			d.set		(0,1,0);
			Hit			(movement_control->gcontact_HealthLost,d,this,movement_control->ContactBone(),Position(),0);
		}
#endif
		return;
	}

	if (time_delta < EPS)			
		return;
#pragma todo("Dima to Kostia : Please change this piece of code to support paths with multiple desired velocities")
	float				dist		=	m_detail_path[m_detail_cur_point_index].m_linear_speed*time_delta;
	float				dist_save	=	dist;

//	Position()			=	p_src;

	// move full steps
	Fvector				mdir,target;
	target.set			(m_detail_path[m_detail_cur_point_index + 1].m_position);
	mdir.sub			(target, Position());
	float				mdist =	mdir.magnitude();
	
	while (dist>mdist) {
		Position().set	(target);

		if (m_detail_cur_point_index + 1 >= m_detail_path.size())
			break;
		else {
			dist			-= mdist;
			++m_detail_cur_point_index;
			if ((m_detail_cur_point_index+1) >= m_detail_path.size())
				break;
			target.set	(m_detail_path[m_detail_cur_point_index+1].m_position);
			mdir.sub	(target, Position());
			mdist		= mdir.magnitude();
		}
	}

	if (mdist < EPS_L) {
		m_detail_cur_point_index = m_detail_path.size() - 1;
		m_speed			= 0;
		return;
	}

	// resolve stucking
	Device.Statistic.Physics.Begin	();

#ifndef NO_PHYSICS_IN_AI_MOVE
	setEnabled(false);
	Level().ObjectSpace.GetNearest		(Position(),1.f); 
	xr_vector<CObject*> &tpNearestList	= Level().ObjectSpace.q_nearest; 
	setEnabled(true);
#endif

	motion.mul			(mdir,dist/mdist);
	Position().add		(motion);

#ifndef NO_PHYSICS_IN_AI_MOVE
	if ((tpNearestList.empty())) {
		if(!movement_control->TryPosition(Position())) {
			movement_control->Calculate(m_detail_path,m_detail_path[m_detail_cur_point_index].m_linear_speed,m_detail_cur_point_index,precision);
			movement_control->GetPosition(Position());
			if (movement_control->gcontact_HealthLost)
				Hit	(movement_control->gcontact_HealthLost,mdir,this,movement_control->ContactBone(),Position(),0);
		}
		else
			movement_control->b_exect_position=true;
	}
	else {
		movement_control->Calculate(m_detail_path,m_detail_path[m_detail_cur_point_index].m_linear_speed,m_detail_cur_point_index,precision);
		movement_control->GetPosition(Position());
		if (movement_control->gcontact_HealthLost)
			Hit	(movement_control->gcontact_HealthLost,mdir,this,movement_control->ContactBone(),Position(),0);
	}
#endif
	float				real_motion	= motion.magnitude() + dist_save-dist;
	float				real_speed	= real_motion/time_delta;
	m_speed				= 0.5f*m_speed + 0.5f*real_speed;
	Device.Statistic.Physics.End	();
}

void CMovementManager::build_path()
{
	time_start				();
	if (!path_actual()) {
		switch (m_path_type) {
			case ePathTypeGamePath : {
				m_path_state	= ePathStateSelectGameVertex;
				break;
			}
			case ePathTypeLevelPath : {
				m_path_state	= ePathStateSelectLevelVertex;
				break;
			}
			case ePathTypeEnemySearch : {
				m_path_state	= ePathStatePredictEnemyVertices;
				break;
			}
			default :			NODEFAULT;
		}
	}

	switch (m_path_type) {
		case ePathTypeGamePath : {
			process_game_path	();
			break;
		}
		case ePathTypeLevelPath : {
			process_level_path	();
			break;
		}
		case ePathTypeEnemySearch : {
			process_enemy_search();
			break;
		}
		default :				NODEFAULT;
	}
}

void CMovementManager::process_game_path()
{
	switch (m_path_state) {
		case ePathStateSelectGameVertex : {
			CGameLocationSelector::select_location(m_tGameVertexID,m_game_dest_vertex_id);
			if (!CGameLocationSelector::selector_failed())
				break;
			m_path_state	= ePathStateBuildGamePath;
			if (time_over())
				break;
		}
		case ePathStateBuildGamePath : {
			CGamePathManager::build_path(m_tGameVertexID,m_game_dest_vertex_id);
			if (CGamePathManager::path_failed())
				break;
			m_path_state	= ePathStateContinueGamePath;
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
			CLevelPathManager::build_path(
				m_dwLevelVertexID,
				CGamePathManager::get_intermediate_vertex_id()
			);
			if (CLevelPathManager::path_failed())
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
			CDetailPathManager::build_path(
				CLevelPathManager::m_path,
				CLevelPathManager::get_intermediate_index(),
				ai().level_graph().vertex_position(
					CLevelPathManager::get_intermediate_vertex_id()
				)
			);
			m_path_state		= ePathStatePathVerification;
			if (time_over())
				break;
		}
		case ePathStatePathVerification : {
			if (!CGameLocationSelector::location_selection_actual(m_tGameVertexID))
				m_path_state	= ePathStateSelectGameVertex;
			else
			if (!CGamePathManager::path_actual(m_tGameVertexID,m_game_dest_vertex_id))
				m_path_state	= ePathStateBuildGamePath;
			else
				if (!CLevelPathManager::path_actual(
						m_dwLevelVertexID,
						CGamePathManager::get_intermediate_vertex_id()
					))
				m_path_state	= ePathStateBuildLevelPath;
			else
			if (!CDetailPathManager::path_actual())
				m_path_state	= ePathStateBuildDetailPath;
			else
			if (CGamePathManager::path_completed())
				m_path_state	= ePathStatePathCompleted;
			else
			if (CLevelPathManager::path_completed())
				m_path_state	= ePathStateContinueGamePath;
			else
			if (CDetailPathManager::path_completed())
				m_path_state	= ePathStateContinueLevelPath;
			break;
		}
		case ePathStatePathCompleted : {
			if (!CGameLocationSelector::location_selection_actual(m_tGameVertexID))
				m_path_state	= ePathStateSelectGameVertex;
			break;
		}
		default : NODEFAULT;
	}
}

void CMovementManager::process_level_path()
{
	switch (m_path_state) {
		case ePathStateSelectLevelVertex : {
			CLevelLocationSelector::select_location(m_dwLevelVertexID,m_level_dest_vertex_id);
			if (CLevelLocationSelector::selector_failed())
				break;
			m_path_state		= ePathStateBuildLevelPath;
			if (time_over())
				break;
		}
		case ePathStateBuildLevelPath : {
			CLevelPathManager::build_path(m_dwLevelVertexID,m_level_dest_vertex_id);
			if (CLevelPathManager::path_failed())
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
			if (CLevelLocationSelector::selector_used())
                CDetailPathManager::build_path(
					CLevelPathManager::m_path,
					CLevelPathManager::get_intermediate_index(),
					ai().level_graph().vertex_position(
						CLevelPathManager::get_intermediate_vertex_id()
					)
				);
			else
                CDetailPathManager::build_path(
					CLevelPathManager::m_path,
					CLevelPathManager::get_intermediate_index(),
					m_detail_dest_position
				);

			m_path_state		= ePathStatePathVerification;
			if (time_over())
				break;
		}
		case ePathStatePathVerification : {
			if (!CLevelLocationSelector::location_selection_actual(m_dwLevelVertexID))
				m_path_state	= ePathStateSelectLevelVertex;
			else
			if (!CLevelPathManager::path_actual(m_dwLevelVertexID,m_level_dest_vertex_id))
				m_path_state	= ePathStateBuildLevelPath;
			else
			if (!CDetailPathManager::path_actual())
				m_path_state	= ePathStateBuildDetailPath;
			else
			if (CLevelPathManager::path_completed())
				m_path_state	= ePathStatePathCompleted;
			else
			if (CDetailPathManager::path_completed())
				m_path_state	= ePathStateContinueLevelPath;
			break;
		}
		case ePathStatePathCompleted : {
			if (!CLevelLocationSelector::location_selection_actual(m_dwLevelVertexID))
				m_path_state	= ePathStateSelectLevelVertex;
			break;
		}
		default : NODEFAULT;
	}
}

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
			CLevelPathManager::build_path(m_dwLevelVertexID,m_level_dest_vertex_id);
			if (CLevelPathManager::path_failed())
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
			CDetailPathManager::build_path	(
				CLevelPathManager::m_path,
				CLevelPathManager::get_intermediate_index(),
				ai().level_graph().vertex_position(
					CLevelPathManager::get_intermediate_vertex_id()
				)
			);
			m_path_state		= ePathStatePathVerification;
			if (time_over())
				break;
		}
		case ePathStatePathVerification : {
			if (!CEnemyLocationPredictor::enemy_prediction_actual())
				m_path_state	= ePathStatePredictEnemyVertices;
			else
			if (!CLevelPathManager::path_actual(m_dwLevelVertexID,m_level_dest_vertex_id))
				m_path_state	= ePathStateBuildLevelPath;
			else
			if (!CDetailPathManager::path_actual())
				m_path_state	= ePathStateBuildDetailPath;
			else
			if (CLevelPathManager::path_completed())
				m_path_state	= ePathStateSelectEnemyVertex;
			else
			if (CDetailPathManager::path_completed())
				m_path_state	= ePathStateContinueLevelPath;
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