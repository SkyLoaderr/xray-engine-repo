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
	m_speed							= 0.f;
	
	enable_movement					(true);
	CGameLocationSelector::Init		(&ai().game_graph());
	CGamePathManager::Init			(&ai().game_graph());
	CLevelLocationSelector::Init	(&ai().level_graph());
	CLevelPathManager::Init			(&ai().level_graph());
	CDetailPathManager::Init		();
	CEnemyLocationPredictor::Init	();

	CGamePathManager::set_evaluator	(m_base_game_selector);
	CLevelPathManager::set_evaluator(m_base_level_selector);
}

void CMovementManager::move_along_path	(CPHMovementControl *movement_control, Fvector &dest_position, float time_delta)
{
	Fvector				motion;

#ifndef NO_PHYSICS_IN_AI_MOVE
	float				precision = 1.f;
#endif

	if (!enabled() || CDetailPathManager::path().empty() || path_completed() || (CDetailPathManager::path().size() - 1 <= CDetailPathManager::curr_travel_point_index()))	{
		m_speed			= 0.f;
#ifndef NO_PHYSICS_IN_AI_MOVE
		if(movement_control->IsCharacterEnabled()) {
			motion.set	(0,0,0);
			movement_control->Calculate(CDetailPathManager::path(),0.f,CDetailPathManager::m_current_travel_point,precision);
			movement_control->GetPosition(dest_position);
		}

		if (!fsimilar(0.f,movement_control->gcontact_HealthLost)) {
			Fvector		d;
			d.set		(0,1,0);
			Hit			(movement_control->gcontact_HealthLost,d,this,movement_control->ContactBone(),dest_position,0);
		}
#endif
		return;
	}

	if (time_delta < EPS)
		return;
#pragma todo("Dima to Kostia : Please change this piece of code to support paths with multiple desired velocities")
	float				speed		=	2.15f;//CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index()].m_linear_speed
	float				dist		=	speed*time_delta;
	float				dist_save	=	dist;

	dest_position		=	Position();

	// move full steps
	Fvector				mdir,target;
	target.set			(CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index() + 1].m_position);
	mdir.sub			(target, dest_position);
	float				mdist =	mdir.magnitude();
	
	while (dist>mdist) {
		dest_position.set	(target);

		if (CDetailPathManager::curr_travel_point_index() + 1 >= CDetailPathManager::path().size())
			break;
		else {
			dist			-= mdist;
			++CDetailPathManager::m_current_travel_point;
			if ((CDetailPathManager::curr_travel_point_index()+1) >= CDetailPathManager::path().size())
				break;
			target.set	(CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index() + 1].m_position);
			mdir.sub	(target, dest_position);
			mdist		= mdir.magnitude();
		}
	}

	if (mdist < EPS_L) {
		CDetailPathManager::m_current_travel_point = CDetailPathManager::path().size() - 1;
		m_speed			= 0.f;
		return;
	}

	// resolve stucking
	Device.Statistic.Physics.Begin	();

#ifndef NO_PHYSICS_IN_AI_MOVE
	setEnabled(false);
	Level().ObjectSpace.GetNearest		(dest_position,1.f); 
	xr_vector<CObject*> &tpNearestList	= Level().ObjectSpace.q_nearest; 
	setEnabled(true);
#endif

	motion.mul			(mdir,dist/mdist);
	dest_position.add	(motion);

#ifndef NO_PHYSICS_IN_AI_MOVE
	if ((tpNearestList.empty())) {
		if(!movement_control->TryPosition(dest_position)) {
//			movement_control->Calculate(CDetailPathManager::path(),CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index()].m_linear_speed,CDetailPathManager::curr_travel_point_index(),precision);
			movement_control->Calculate(CDetailPathManager::path(),speed,CDetailPathManager::m_current_travel_point,precision);
			movement_control->GetPosition(dest_position);
			if (!fsimilar(0.f,movement_control->gcontact_HealthLost))
				Hit	(movement_control->gcontact_HealthLost,mdir,this,movement_control->ContactBone(),dest_position,0);
		}
		else
			movement_control->b_exect_position=true;
	}
	else {
//		movement_control->Calculate(CDetailPathManager::path(),CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index()].m_linear_speed,CDetailPathManager::curr_travel_point_index(),precision);
		movement_control->Calculate(CDetailPathManager::path(),speed,CDetailPathManager::m_current_travel_point,precision);
		movement_control->GetPosition(dest_position);
		if (!fsimilar(0.f,movement_control->gcontact_HealthLost))
			Hit	(movement_control->gcontact_HealthLost,mdir,this,movement_control->ContactBone(),dest_position,0);
	}
#endif
	float				real_motion	= motion.magnitude() + dist_save-dist;
	float				real_speed	= real_motion/time_delta;
	m_speed				= 0.5f*speed + 0.5f*real_speed;
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
			CGameLocationSelector::select_location(game_vertex_id(),CGamePathManager::m_dest_vertex_id);
			if (CGameLocationSelector::failed())
				break;
			m_path_state	= ePathStateBuildGamePath;
			if (time_over())
				break;
		}
		case ePathStateBuildGamePath : {
			Device.Statistic.TEST0.Begin();
			CGamePathManager::build_path(game_vertex_id(),game_dest_vertex_id());
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
					CGamePathManager::intermediate_vertex_id()
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
			CDetailPathManager::set_start_position(Position());
			CDetailPathManager::set_dest_position( 
				ai().level_graph().vertex_position(
					CLevelPathManager::intermediate_vertex_id()
				)
			);
			CDetailPathManager::build_path(
				CLevelPathManager::path(),
				CLevelPathManager::intermediate_index(),
				ai().level_graph().vertex_position(
					CLevelPathManager::intermediate_vertex_id()
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
			if (!CGamePathManager::actual())
				m_path_state	= ePathStateBuildGamePath;
			else
				if (!CLevelPathManager::actual())
				m_path_state	= ePathStateBuildLevelPath;
			else
			if (!CDetailPathManager::actual())
				m_path_state	= ePathStateBuildDetailPath;
			else
				if (CDetailPathManager::completed(Position())) {
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
			if (CLevelLocationSelector::used())
                CDetailPathManager::build_path(
					CLevelPathManager::path(),
					CLevelPathManager::intermediate_index(),
					ai().level_graph().vertex_position(
						CLevelPathManager::intermediate_vertex_id()
					)
				);
			else
                CDetailPathManager::build_path(
					CLevelPathManager::path(),
					CLevelPathManager::intermediate_index(),
					dest_position()
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
			CDetailPathManager::build_path	(
				CLevelPathManager::path(),
				CLevelPathManager::intermediate_index(),
				ai().level_graph().vertex_position(
					CLevelPathManager::intermediate_vertex_id()
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
			if (!CEnemyLocationPredictor::enemy_prediction_actual())
				m_path_state	= ePathStatePredictEnemyVertices;
			break;
		}
		default : NODEFAULT;
	}
}