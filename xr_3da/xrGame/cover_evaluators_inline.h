////////////////////////////////////////////////////////////////////////////
//	Module 		: cover_evaluators_inline.h
//	Created 	: 24.04.2004
//  Modified 	: 24.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Cover evaluators inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorBase
//////////////////////////////////////////////////////////////////////////
IC	CCoverEvaluatorBase::CCoverEvaluatorBase	(CRestrictedObject *object)
{
	m_inertia_time			= 0;
	m_last_update			= 0;
	m_selected				= 0;
	m_object				= object;
}

IC	CCoverPoint *CCoverEvaluatorBase::selected	() const
{
	return					(m_selected);
}

IC	void CCoverEvaluatorBase::set_inertia		(u32 inertia_time)
{
	m_inertia_time			= inertia_time;
}

IC	bool CCoverEvaluatorBase::inertia			() const
{
	return					(Level().timeServer() < m_last_update + m_inertia_time);
}

IC	void CCoverEvaluatorBase::setup				()
{
	m_initialized			= true;
}

IC	void CCoverEvaluatorBase::initialize		(const Fvector &start_position)
{
	VERIFY					(initialized());
	m_start_position		= start_position;
	m_selected				= 0;
	m_best_value			= 1000.f;
	m_last_update			= Level().timeServer();
}

IC	void CCoverEvaluatorBase::finalize			()
{
	m_initialized			= false;
}

IC	bool CCoverEvaluatorBase::initialized		() const
{
	return					(m_initialized);
}

IC	bool CCoverEvaluatorBase::accessible		(const Fvector &position)
{
	return					(m_object ? m_object->accessible(position) : true);
}
//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorCloseToEnemy
//////////////////////////////////////////////////////////////////////////

IC	CCoverEvaluatorCloseToEnemy::CCoverEvaluatorCloseToEnemy	(CRestrictedObject *object) : inherited(object)
{
}

IC	void CCoverEvaluatorCloseToEnemy::initialize(const Fvector &start_position)
{
	inherited::initialize	(start_position);
	m_current_distance		= m_start_position.distance_to(m_enemy_position);
}

IC	void CCoverEvaluatorCloseToEnemy::setup		(const Fvector &enemy_position, float min_enemy_distance, float	max_enemy_distance, float deviation)
{
	inherited::setup		();
	m_enemy_position		= enemy_position;
	m_deviation				= deviation;
	m_min_distance			= min_enemy_distance;
	m_max_distance			= max_enemy_distance;
}

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorAngle
//////////////////////////////////////////////////////////////////////////

IC	CCoverEvaluatorAngle::CCoverEvaluatorAngle	(CRestrictedObject *object) : inherited(object)
{
}

IC	void CCoverEvaluatorAngle::setup		(const Fvector &enemy_position, float min_enemy_distance, float	max_enemy_distance, u32 level_vertex_id)
{
	inherited::setup		(enemy_position,min_enemy_distance,max_enemy_distance);
	m_level_vertex_id		= level_vertex_id;
}

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorFarFromEnemy
//////////////////////////////////////////////////////////////////////////

IC	CCoverEvaluatorFarFromEnemy::CCoverEvaluatorFarFromEnemy	(CRestrictedObject *object) : inherited(object)
{
}

IC	CCoverEvaluatorBest::CCoverEvaluatorBest	(CRestrictedObject *object) : inherited(object)
{
}

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorSafe
//////////////////////////////////////////////////////////////////////////

IC	CCoverEvaluatorSafe::CCoverEvaluatorSafe	(CRestrictedObject *object) : inherited(object)
{
}

IC	void CCoverEvaluatorSafe::setup		(float min_distance)
{
	inherited::setup		();
	m_min_distance			= min_distance;
}
