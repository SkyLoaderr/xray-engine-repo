////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_action_inline.h
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Movement action inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CMovementAction::CMovementAction	()
{
	m_path_type				= CMovementManager::ePathTypeNoPath;
	m_detail_path_type		= CMovementManager::eDetailPathTypeSmooth;
	m_body_state			= MonsterSpace::eBodyStateStand;
	m_movement_type			= MonsterSpace::eMovementTypeStand;
	m_level_dest_vertex_id	= u32(-1);
	m_desired_position		= 0;
	m_desired_direction		= 0;
	m_node_evaluator		= 0;
	m_path_evaluator		= 0;
}

IC	CMovementAction::CMovementAction	(const CMovementManager::EPathType &path_type, const CMovementManager::EDetailPathType &detail_path_type, const MonsterSpace::EBodyState &body_state, const MonsterSpace::EMovementType &movement_type, u32 level_vertex_id, Fvector *desired_position, Fvector *desired_direction, CAbstractVertexEvaluator *node_evaluator, CAbstractVertexEvaluator *path_evaluator)
{
	m_path_type				= path_type;
	m_detail_path_type		= detail_path_type;
	m_body_state			= body_state;
	m_movement_type			= movement_type;
	m_level_dest_vertex_id	= level_vertex_id;
	m_desired_position		= desired_position;
	m_desired_direction		= desired_direction;
	m_node_evaluator		= node_evaluator;
	m_path_evaluator		= path_evaluator;
}
