////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_movement_manager_inline.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker velocity manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC const MonsterSpace::SBoneRotation &CStalkerMovementManager::head_orientation() const
{
	return				(m_head);
}

IC const MonsterSpace::EMovementType CStalkerMovementManager::movement_type() const
{
	return				(m_movement_type);
}

IC const MonsterSpace::EMentalState CStalkerMovementManager::mental_state() const
{
	return				(m_mental_state);
}

IC void CStalkerMovementManager::set_head_orientation(const MonsterSpace::SBoneRotation &orientation)
{
	m_head				= orientation;
}

IC	const MonsterSpace::EBodyState CStalkerMovementManager::body_state() const
{
	return				(m_body_state);
}

IC	void CStalkerMovementManager::set_desired_position(const Fvector *desired_position)
{
	if (desired_position) {
		m_use_desired_position	= true;
		m_desired_position		= *desired_position;
	}
	else {
		m_use_desired_position	= false;
#ifdef DEBUG
		m_desired_position		= Fvector().set(_sqr(flt_max),_sqr(flt_max),_sqr(flt_max));
#endif
	}
}

IC	void CStalkerMovementManager::set_desired_direction(const Fvector *desired_direction)
{
	if (desired_direction) {
		m_use_desired_direction	= true;
		m_desired_direction		= *desired_direction;
	}
	else {
		m_use_desired_direction	= false;
#ifdef DEBUG
		m_desired_direction		= Fvector().set(_sqr(flt_max),_sqr(flt_max),_sqr(flt_max));
#endif
	}
}

IC	void CStalkerMovementManager::set_body_state(EBodyState body_state)
{
	m_body_state		= body_state;
}

IC	void CStalkerMovementManager::set_movement_type(EMovementType movement_type)
{
	m_movement_type		= movement_type;
}

IC	void CStalkerMovementManager::set_mental_state(EMentalState mental_state)
{
	m_mental_state		= mental_state;
}

IC	void CStalkerMovementManager::set_path_type(EPathType path_type)
{
	m_path_type			= path_type;
}

IC	void CStalkerMovementManager::set_detail_path_type(EDetailPathType detail_path_type)
{
	m_detail_path_type	= detail_path_type;
}

IC	void CStalkerMovementManager::set_node_evaluator(CAbstractVertexEvaluator *node_evaluator)
{
	m_node_evaluator	= node_evaluator;
}

IC	void CStalkerMovementManager::set_path_evaluator(CAbstractVertexEvaluator *path_evaluator)
{
	m_path_evaluator	= path_evaluator;
}
