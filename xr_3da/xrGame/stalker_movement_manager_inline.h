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

IC	void CStalkerMovementManager::add_velocity		(int mask, float linear, float compute_angular, float angular)
{
	m_movement_params.insert	(std::make_pair(mask,STravelParams(linear,compute_angular,angular)));
}

IC	void CStalkerMovementManager::add_velocity		(int mask, float linear, float compute_angular)
{
	add_velocity				(mask,linear,compute_angular,compute_angular);
}

IC	float CStalkerMovementManager::path_direction_angle	()
{
	if (!path().empty() && (path().size() > curr_travel_point_index() + 1)) {
		Fvector					t;
		t.sub					(
			path()[curr_travel_point_index() + 1].position,
			path()[curr_travel_point_index()].position
		);
		float					y,p;
		t.getHP					(y,p);
		return					(angle_difference(-y,m_body.current.yaw));
	}
	return						(0.f);
}

IC	bool CStalkerMovementManager::turn_in_place			() const
{
	return						(!path_completed() && fis_zero(speed()) && (angle_difference(body_orientation().current.yaw,body_orientation().target.yaw) > EPS_L));
}

IC	void CStalkerMovementManager::set_body_state(EBodyState body_state)
{
	m_target.m_body_state		= body_state;
}

IC	void CStalkerMovementManager::set_movement_type(EMovementType movement_type)
{
	m_target.m_movement_type	= movement_type;
}

IC	void CStalkerMovementManager::set_mental_state(EMentalState mental_state)
{
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

IC const MonsterSpace::EMovementType CStalkerMovementManager::movement_type() const
{
	return						(m_current.m_movement_type);
}

IC const MonsterSpace::EMentalState CStalkerMovementManager::mental_state() const
{
	return						(m_current.m_mental_state);
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
