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
	m_base_game_selector			= xr_new<CGraphEngine::CBaseParameters>();
	m_base_level_selector			= xr_new<CGraphEngine::CBaseParameters>();
	Init							();
}

CMovementManager::~CMovementManager	()
{
	xr_delete						(m_base_game_selector);
	xr_delete						(m_base_level_selector);
}

void CMovementManager::Init			()
{
	m_time_work						= 300*CPU::cycles_per_microsec;
	
	set_enabled						(true);
	CGameLocationSelector::Init		(&ai().game_graph());
	CGamePathManager::Init			(&ai().game_graph());
	CLevelLocationSelector::Init	(&ai().level_graph());
	CLevelPathManager::Init			(&ai().level_graph());
	CDetailPathManager::Init		();
	CEnemyLocationPredictor::Init	();

	CGamePathManager::set_evaluator	(m_base_game_selector);
	CLevelPathManager::set_evaluator(m_base_level_selector);
}

void CMovementManager::move_along_path	(CPHMovementControl *movement_control, float time_delta)
{
	Fvector				motion;

#ifndef NO_PHYSICS_IN_AI_MOVE
	float				precision = 1.f;
#endif

	if (!get_enabled() || CDetailPathManager::m_path.empty() || (CDetailPathManager::m_path.size() - 1 <= CDetailPathManager::m_current_travel_point))	{
		m_speed			= 0;
#ifndef NO_PHYSICS_IN_AI_MOVE
		if(movement_control->IsCharacterEnabled()) {
			motion.set	(0,0,0);
			movement_control->Calculate(CDetailPathManager::m_path,0.f,CDetailPathManager::m_current_travel_point,precision);
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
	float				dist		=	CDetailPathManager::m_path[CDetailPathManager::m_current_travel_point].m_linear_speed*time_delta;
	float				dist_save	=	dist;

//	Position()			=	p_src;

	// move full steps
	Fvector				mdir,target;
	target.set			(CDetailPathManager::m_path[CDetailPathManager::m_current_travel_point + 1].m_position);
	mdir.sub			(target, Position());
	float				mdist =	mdir.magnitude();
	
	while (dist>mdist) {
		Position().set	(target);

		if (CDetailPathManager::m_current_travel_point + 1 >= CDetailPathManager::m_path.size())
			break;
		else {
			dist			-= mdist;
			++CDetailPathManager::m_current_travel_point;
			if ((CDetailPathManager::m_current_travel_point+1) >= CDetailPathManager::m_path.size())
				break;
			target.set	(CDetailPathManager::m_path[CDetailPathManager::m_current_travel_point + 1].m_position);
			mdir.sub	(target, Position());
			mdist		= mdir.magnitude();
		}
	}

	if (mdist < EPS_L) {
		CDetailPathManager::m_current_travel_point = CDetailPathManager::m_path.size() - 1;
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
			movement_control->Calculate(CDetailPathManager::m_path,CDetailPathManager::m_path[CDetailPathManager::m_current_travel_point].m_linear_speed,CDetailPathManager::m_current_travel_point,precision);
			movement_control->GetPosition(Position());
			if (movement_control->gcontact_HealthLost)
				Hit	(movement_control->gcontact_HealthLost,mdir,this,movement_control->ContactBone(),Position(),0);
		}
		else
			movement_control->b_exect_position=true;
	}
	else {
		movement_control->Calculate(CDetailPathManager::m_path,CDetailPathManager::m_path[CDetailPathManager::m_current_travel_point].m_linear_speed,CDetailPathManager::m_current_travel_point,precision);
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
	Device.Statistic.AI_Path.Begin();
	time_start				();
	if (!actual()) {
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
		m_path_actuality	= true;
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
	Device.Statistic.AI_Path.End();
}

void CMovementManager::process_game_path()
{
	switch (m_path_state) {
		case ePathStateSelectGameVertex : {
			CGameLocationSelector::select_location(game_vertex_id(),m_game_dest_vertex_id);
			if (CGameLocationSelector::failed())
				break;
			m_path_state	= ePathStateBuildGamePath;
			if (time_over())
				break;
		}
		case ePathStateBuildGamePath : {
			Device.Statistic.TEST0.Begin();
			CGamePathManager::build_path(game_vertex_id(),m_game_dest_vertex_id);
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
					CGamePathManager::get_intermediate_vertex_id()
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
			CDetailPathManager::m_start_position = Position();
			m_detail_dest_position  = 
				ai().level_graph().vertex_position(
					CLevelPathManager::get_intermediate_vertex_id()
				);
			CDetailPathManager::build_path(
				CLevelPathManager::path(),
				CLevelPathManager::get_intermediate_index(),
				ai().level_graph().vertex_position(
					CLevelPathManager::get_intermediate_vertex_id()
				)
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
			if (!CGamePathManager::actual(game_vertex_id(),m_game_dest_vertex_id))
				m_path_state	= ePathStateBuildGamePath;
			else
				if (!CLevelPathManager::actual(
						level_vertex_id(),
						CGamePathManager::get_intermediate_vertex_id()
					))
				m_path_state	= ePathStateBuildLevelPath;
			else
			if (!CDetailPathManager::actual())
				m_path_state	= ePathStateBuildDetailPath;
			else
				if (CDetailPathManager::completed()) {
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

void CMovementManager::process_level_path()
{
	switch (m_path_state) {
		case ePathStateSelectLevelVertex : {
			CLevelLocationSelector::select_location(level_vertex_id(),m_level_dest_vertex_id);
			if (CLevelLocationSelector::failed())
				break;
			m_path_state		= ePathStateBuildLevelPath;
			if (time_over())
				break;
		}
		case ePathStateBuildLevelPath : {
			CLevelPathManager::build_path(level_vertex_id(),m_level_dest_vertex_id);
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
			if (CLevelLocationSelector::used())
                CDetailPathManager::build_path(
					CLevelPathManager::path(),
					CLevelPathManager::get_intermediate_index(),
					ai().level_graph().vertex_position(
						CLevelPathManager::get_intermediate_vertex_id()
					)
				);
			else
                CDetailPathManager::build_path(
					CLevelPathManager::path(),
					CLevelPathManager::get_intermediate_index(),
					m_detail_dest_position
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
			if (!CLevelPathManager::actual(level_vertex_id(),m_level_dest_vertex_id))
				m_path_state	= ePathStateBuildLevelPath;
			else
			if (!CDetailPathManager::actual())
				m_path_state	= ePathStateBuildDetailPath;
			else
				if (CDetailPathManager::completed()) {
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
			CLevelPathManager::build_path(level_vertex_id(),m_level_dest_vertex_id);
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
			CDetailPathManager::build_path	(
				CLevelPathManager::path(),
				CLevelPathManager::get_intermediate_index(),
				ai().level_graph().vertex_position(
					CLevelPathManager::get_intermediate_vertex_id()
				)
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
			if (!CEnemyLocationPredictor::enemy_prediction_actual())
				m_path_state	= ePathStatePredictEnemyVertices;
			else
			if (!CLevelPathManager::actual(level_vertex_id(),m_level_dest_vertex_id))
				m_path_state	= ePathStateBuildLevelPath;
			else
			if (!CDetailPathManager::actual())
				m_path_state	= ePathStateBuildDetailPath;
			else
				if (CDetailPathManager::completed()) {
					m_path_state	= ePathStateContinueLevelPath;
					if (CLevelPathManager::completed())
						m_path_state	= ePathStateContinueGamePath;
				}
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