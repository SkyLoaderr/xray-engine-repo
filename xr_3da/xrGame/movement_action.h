////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_action.h
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Movement action
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "control_action.h"
#include "movement_manager.h"

class CMovementAction : public CControlAction {
protected:
	CMovementManager::EPathType			m_path_type;
	CMovementManager::EDetailPathType	m_detail_path_type;
	MonsterSpace::EBodyState			m_body_state;
	MonsterSpace::EMovementType			m_movement_type;
	u32									m_level_dest_vertex_id;
	Fvector								*m_desired_position;
	Fvector								*m_desired_direction;
	CAbstractVertexEvaluator			*m_node_evaluator;
	CAbstractVertexEvaluator			*m_path_evaluator;

public:
	IC					CMovementAction		();
	IC					CMovementAction		(const CMovementManager::EPathType &path_type, const CMovementManager::EDetailPathType &detail_path_type, const MonsterSpace::EBodyState &body_state, const MonsterSpace::EMovementType &movement_type, u32 level_vertex_id = u32(-1), Fvector *desired_position = 0, Fvector *desired_direction = 0, CAbstractVertexEvaluator *node_evaluator = 0, CAbstractVertexEvaluator *path_evaluator = 0);
			void		execute				();
			void		finalize			();
};

#include "movement_action_inline.h"