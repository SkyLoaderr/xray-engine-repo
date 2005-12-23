////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_movement_manager_inline.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker movement manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC const MonsterSpace::SBoneRotation &CStalkerMovementManager::head_orientation() const
{
	return				(m_head);
}

IC void CStalkerMovementManager::set_head_orientation(const MonsterSpace::SBoneRotation &orientation)
{
	m_head							= orientation;
}

IC	void CStalkerMovementManager::set_desired_direction(const Fvector *desired_direction)
{
	if (desired_direction) {
		m_target.m_use_desired_direction	= true;
		m_target.m_desired_direction		= *desired_direction;
	}
	else {
		m_target.m_use_desired_direction	= false;
#ifdef DEBUG
		m_target.m_desired_direction		= Fvector().set(_sqr(flt_max),_sqr(flt_max),_sqr(flt_max));
#endif
	}
}

IC	void CStalkerMovementManager::add_velocity		(int mask, float linear, float compute_angular)
{
	add_velocity				(mask,linear,compute_angular,compute_angular);
}

IC	bool CStalkerMovementManager::turn_in_place			() const
{
	return						(!path_completed() && fis_zero(speed()) && (angle_difference(body_orientation().current.yaw,body_orientation().target.yaw) > EPS_L));
}

IC	void CStalkerMovementManager::set_body_state(EBodyState body_state)
{
	THROW						((body_state != eBodyStateCrouch) || (m_target.m_mental_state != eMentalStateFree));
	m_target.m_body_state		= body_state;
}

IC	void CStalkerMovementManager::set_movement_type(EMovementType movement_type)
{
	m_target.m_movement_type	= movement_type;
}

IC	void CStalkerMovementManager::set_mental_state(EMentalState mental_state)
{
	THROW						((m_target.m_body_state != eBodyStateCrouch) || (mental_state != eMentalStateFree));
	m_target.m_mental_state		= mental_state;
}

IC	void CStalkerMovementManager::set_path_type(EPathType path_type)
{
	m_target.m_path_type		= path_type;
}

IC	void CStalkerMovementManager::set_detail_path_type(EDetailPathType detail_path_type)
{
	m_target.m_detail_path_type	= detail_path_type;
}

IC	void CStalkerMovementManager::set_node_evaluator(CAbstractVertexEvaluator *node_evaluator)
{
	m_target.m_node_evaluator	= node_evaluator;
}

IC	void CStalkerMovementManager::set_path_evaluator(CAbstractVertexEvaluator *path_evaluator)
{
	m_target.m_path_evaluator	= path_evaluator;
}

IC	const MonsterSpace::EBodyState CStalkerMovementManager::body_state() const
{
	return						(m_current.m_body_state);
}

IC	const MonsterSpace::EBodyState CStalkerMovementManager::target_body_state() const
{
	return						(m_target.m_body_state);
}

IC const MonsterSpace::EMovementType CStalkerMovementManager::movement_type() const
{
	return						(m_current.m_movement_type);
}

IC const MonsterSpace::EMentalState CStalkerMovementManager::mental_state() const
{
	return						(m_current.m_mental_state);
}

IC const MonsterSpace::EMentalState CStalkerMovementManager::target_mental_state() const
{
	return						(m_target.m_mental_state);
}

IC	const MovementManager::EPathType CStalkerMovementManager::path_type	() const
{
	return						(m_current.m_path_type);
}

IC	const DetailPathManager::EDetailPathType	CStalkerMovementManager::detail_path_type	() const
{
	return						(m_current.m_detail_path_type);
}

IC	CAbstractVertexEvaluator *CStalkerMovementManager::node_evaluator	() const
{
	return						(m_current.m_node_evaluator);
}

IC	CAbstractVertexEvaluator *CStalkerMovementManager::path_evaluator	() const
{
	return						(m_current.m_path_evaluator);
}

IC	const Fvector &CStalkerMovementManager::desired_position			() const
{
	VERIFY						(use_desired_position());
	return						(m_current.m_desired_position);
}

IC	const Fvector &CStalkerMovementManager::desired_direction			() const
{
	VERIFY						(use_desired_direction());
	return						(m_current.m_desired_direction);
}

IC	bool CStalkerMovementManager::use_desired_position					() const
{
	return						(m_current.m_use_desired_position);
}

IC	bool CStalkerMovementManager::use_desired_direction					() const
{
	return						(m_current.m_use_desired_direction);
}

IC	CAI_Stalker &CStalkerMovementManager::object						() const
{
	VERIFY						(m_object);
	return						(*m_object);
}

#ifdef OLD_VELOCITIES
IC	float CStalkerMovementManager::crouch_factor			() const
{
	return						(m_crouch_factor);
}

IC	float CStalkerMovementManager::walk_factor				() const
{
	return						(m_walk_factor);
}

IC	float CStalkerMovementManager::walk_back_factor			() const
{
	return						(m_walk_back_factor);
}

IC	float CStalkerMovementManager::run_factor				() const
{
	return						(m_run_factor);
}

IC	float CStalkerMovementManager::run_back_factor			() const
{
	return						(m_run_back_factor);
}

IC	float CStalkerMovementManager::walk_free_factor			() const
{
	return						(m_walk_free_factor);
}

IC	float CStalkerMovementManager::run_free_factor			() const
{
	return						(m_run_free_factor);
}

IC	float CStalkerMovementManager::panic_factor				() const
{
	return						(m_panic_factor);
}

IC	float CStalkerMovementManager::damaged_walk_factor		() const
{
	return						(m_damaged_walk_factor);
}

IC	float CStalkerMovementManager::damaged_run_factor		() const
{
	return						(m_damaged_run_factor);
}

IC	float CStalkerMovementManager::damaged_walk_free_factor	() const
{
	return						(m_damaged_walk_free_factor);
}

IC	float CStalkerMovementManager::damaged_run_free_factor	() const
{
	return						(m_damaged_run_free_factor);
}

IC	float CStalkerMovementManager::damaged_panic_factor		() const
{
	return						(m_damaged_panic_factor);
}
#endif

IC	const MonsterSpace::EMovementType CStalkerMovementManager::target_movement_type	() const
{
	return						(m_target.m_movement_type);
}
