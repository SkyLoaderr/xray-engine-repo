////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"
#include "PHMovementControl.h"
#include "gameobject.h"

CMovementManager::CMovementManager	()
{
	init					();
}

CMovementManager::~CMovementManager	()
{
}

void CMovementManager::init			()
{
	CGamePathManager::init	();
	CLevelPathManager::init	();
	CDetailPathManager::init();
}

void CMovementManager::move_along_path	(CPHMovementControl *movement_control, float time_delta)
{
	Fvector				motion;

#ifndef NO_PHYSICS_IN_AI_MOVE
	float				precision = 1.f;
#endif

	if ((m_detail_path.empty()) || (m_detail_path.size() - 1 <= m_detail_cur_point_index))	{
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
			m_detail_cur_point_index++;
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

bool CMovementManager::path_actual() const
{
	return				(m_path_type_changed);
}

void CMovementManager::set_path_type(EPathType path_type)
{
	m_path_type_changed	= m_path_type != path_type;
	m_path_type			= path_type;
}

void CMovementManager::build_path()
{
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
			select_game_vertex	();
			m_path_state		= ePathStateBuildGamePath;
			break;
		}
		case ePathStateBuildGamePath : {
			build_game_path		();
			m_path_state		= ePathStateContinueGamePath;
			break;
		}
		case ePathStateContinueGamePath : {
			select_game_path_vertex	();
			m_path_state		= ePathStateBuildLevelPath;
			break;
		}
		case ePathStateBuildLevelPath : {
			build_level_path	();
			m_path_state		= ePathStateContinueLevelPath;
			break;
		}
		case ePathStateContinueLevelPath : {
			select_level_path_vertex ();
			m_path_state		= ePathStateBuildDetailPath;
			break;
		}
		case ePathStateBuildDetailPath : {
			build_detail_path	(m_level_path);
			m_path_state		= ePathStatePathVerification;
			break;
		}
		case ePathStatePathVerification : {
			if (!game_vertex_selection_actual())
				m_path_state	= ePathStateSelectGameVertex;
			else
			if (!game_path_actual())
				m_path_state	= ePathStateBuildGamePath;
			else
			if (!level_path_actual())
				m_path_state	= ePathStateBuildLevelPath;
			else
			if (!detail_path_actual())
				m_path_state	= ePathStateBuildDetailPath;
			else
			if (game_path_completed())
				m_path_state	= ePathStatePathCompleted;
			else
			if (level_path_completed())
				m_path_state	= ePathStateContinueGamePath;
			else
			if (detail_path_completed())
				m_path_state	= ePathStateContinueLevelPath;
			break;
		}
		case ePathStatePathCompleted : {
			if (!game_vertex_selection_actual())
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
			select_level_vertex	();
			m_path_state		= ePathStateBuildLevelPath;
			break;
		}
		case ePathStateBuildLevelPath : {
			build_level_path	();
			m_path_state		= ePathStateContinueLevelPath;
			break;
		}
		case ePathStateContinueLevelPath : {
			select_level_path_vertex ();
			m_path_state		= ePathStateBuildDetailPath;
			break;
		}
		case ePathStateBuildDetailPath : {
			build_detail_path	(m_level_path);
			m_path_state		= ePathStatePathVerification;
			break;
		}
		case ePathStatePathVerification : {
			if (!level_vertex_selection_actual())
				m_path_state	= ePathStateSelectLevelVertex;
			else
			if (!level_path_actual())
				m_path_state	= ePathStateBuildLevelPath;
			else
			if (!detail_path_actual())
				m_path_state	= ePathStateBuildDetailPath;
			else
			if (level_path_completed())
				m_path_state	= ePathStatePathCompleted;
			else
			if (detail_path_completed())
				m_path_state	= ePathStateContinueLevelPath;
			break;
		}
		case ePathStatePathCompleted : {
			if (!level_vertex_selection_actual())
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
			predict_enemy_locations();
			m_path_state		= ePathStateSelectEnemyVertex;
			break;
		}
		case ePathStateSelectEnemyVertex : {
			select_enemy_location();
			m_path_state		= ePathStateBuildLevelPath;
			break;
		}
		case ePathStateBuildLevelPath : {
			build_level_path	();
			m_path_state		= ePathStateContinueLevelPath;
			break;
		}
		case ePathStateContinueLevelPath : {
			select_level_path_vertex ();
			m_path_state		= ePathStateBuildDetailPath;
			break;
		}
		case ePathStateBuildDetailPath : {
			build_detail_path	(m_level_path);
			m_path_state		= ePathStatePathVerification;
			break;
		}
		case ePathStatePathVerification : {
			if (!enemy_prediction_actual())
				m_path_state	= ePathStatePredictEnemyVertices;
			else
			if (!level_path_actual())
				m_path_state	= ePathStateBuildLevelPath;
			else
			if (!detail_path_actual())
				m_path_state	= ePathStateBuildDetailPath;
			else
			if (level_path_completed())
				m_path_state	= ePathStateSelectEnemyVertex;
			else
			if (detail_path_completed())
				m_path_state	= ePathStateContinueLevelPath;
			break;
		}
		case ePathStatePathCompleted : {
			if (!enemy_prediction_actual())
				m_path_state	= ePathStatePredictEnemyVertices;
			break;
		}
		default : NODEFAULT;
	}
}

//template <u64 flags>
//void CMovementManager::find_location(PathManagers::CNodeEvaluator<flags> *node_evaluator)
//{
//	if (!m_locate_query_time ||
//		(m_locate_query_time < m_location_query_interval + m_current_update) ||
//		m_detail_path.empty() ||
//		(int(m_detail_cur_point_index) > int(m_detail_path.size()) - 4) ||
//		(speed() < EPS_L)
//		) 
//	{
//	   select_location	(node_evaluator);
//	   if ((m_level_dest_node != node_evaluator->m_dwBestNode) && !m_selector_failed){
//		   m_level_dest_node	= node_evaluator->m_dwBestNode;
//		   m_level_path.clear	();
////		   m_tPathState			= ePathStateBuildNodePath;
//	   }
//	}
