////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_action.cpp
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Movement action
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_action.h"
#include "ai/stalker/ai_stalker.h"
#include "movement_manager_space.h"
#include "detail_path_manager_space.h"

CMovementAction::CMovementAction	()
{
	m_path_type					= MovementManager::ePathTypeNoPath;
	m_detail_path_type			= DetailPathManager::eDetailPathTypeSmooth;
	m_body_state				= MonsterSpace::eBodyStateStand;
	m_movement_type				= MonsterSpace::eMovementTypeStand;
	m_level_dest_vertex_id		= u32(-1);
	m_desired_position			= 0;
	m_desired_direction			= 0;
	m_node_evaluator			= 0;
	m_path_evaluator			= 0;
}

void CMovementAction::execute		()
{
	m_object->set_path_type			(m_path_type);
	m_object->set_detail_path_type	(m_detail_path_type);
	m_object->set_body_state		(m_body_state);
	m_object->set_movement_type		(m_movement_type);
	m_object->set_level_dest_vertex	(m_level_dest_vertex_id);
	m_object->set_desired_position	(m_desired_position);
	m_object->set_desired_direction	(m_desired_direction);
	m_object->set_node_evaluator	(m_node_evaluator);
	m_object->set_node_evaluator	(m_path_evaluator);
}

void CMovementAction::finalize	()
{
	m_object->set_desired_position	(0);
	m_object->set_desired_direction	(0);
	m_object->set_node_evaluator	(0);
	m_object->set_node_evaluator	(0);
}
