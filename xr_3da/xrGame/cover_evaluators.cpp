////////////////////////////////////////////////////////////////////////////
//	Module 		: cover_evaluators.h
//	Created 	: 24.04.2004
//  Modified 	: 24.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Cover evaluators
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cover_evaluators.h"
#include "cover_point.h"
#include "ai_space.h"
#include "level_graph.h"

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorCloseToEnemy
//////////////////////////////////////////////////////////////////////////

void CCoverEvaluatorCloseToEnemy::evaluate			(CCoverPoint *cover_point)
{
	float					enemy_distance	= m_enemy_position.distance_to(cover_point->position());
	float					my_distance		= m_start_position.distance_to(cover_point->position());

	if (enemy_distance <= m_min_distance)
		return;

	if (enemy_distance >= m_max_distance)
		return;

	if (enemy_distance >= m_current_distance + m_deviation)
		return;

	Fvector					direction;
	float					y,p;
	direction.sub			(m_enemy_position,cover_point->position());
	direction.getHP			(y,p);
	float					cover_value = ai().level_graph().cover_in_direction(y,cover_point->level_vertex_id());
	if (cover_value >= 2.f*m_best_value)
		return;

	m_selected				= cover_point;
	m_best_value			= cover_value;
	m_best_distance			= my_distance;
}

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorFarFromEnemy
//////////////////////////////////////////////////////////////////////////

void CCoverEvaluatorFarFromEnemy::evaluate			(CCoverPoint *cover_point)
{
	float					enemy_distance	= m_enemy_position.distance_to(cover_point->position());
//	float					my_distance		= m_start_position.distance_to(cover_point->position());

	if (enemy_distance <= m_min_distance)
		return;

	if (enemy_distance >= m_max_distance)
		return;

	if (enemy_distance <= m_current_distance - m_deviation)
		return;

	Fvector					direction;
	float					y,p;
	direction.sub			(m_enemy_position,cover_point->position());
	direction.getHP			(y,p);
	float					cover_value = ai().level_graph().cover_in_direction(y,cover_point->level_vertex_id());
	if (cover_value >= 2.f*m_best_value)
		return;

	m_selected				= cover_point;
	m_best_value			= cover_value;
}

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorBest
//////////////////////////////////////////////////////////////////////////

void CCoverEvaluatorBest::evaluate			(CCoverPoint *cover_point)
{
	float					enemy_distance	= m_enemy_position.distance_to(cover_point->position());
	float					my_distance		= m_start_position.distance_to_sqr(cover_point->position());

	if (enemy_distance <= m_min_distance)
		return;

	if (enemy_distance >= m_max_distance)
		return;

	Fvector					direction;
	float					y,p;
	direction.sub			(m_enemy_position,cover_point->position());
	direction.getHP			(y,p);
	float					cover_value = ai().level_graph().cover_in_direction(y,cover_point->level_vertex_id());
	float					value = 1*_sqr(cover_value) + 1*_sqr(my_distance/300.f);
	if (value >= m_best_value)
		return;

	m_selected				= cover_point;
	m_best_value			= value;
}

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorAngle
//////////////////////////////////////////////////////////////////////////

void CCoverEvaluatorAngle::initialize		(const Fvector &start_position)
{
	inherited::initialize		(start_position);
	m_best_alpha				= 0.f;
	m_direction.sub				(m_start_position,m_enemy_position);
	m_direction.normalize_safe	();
	float						best_value = -1.f;
	float						m_best_angle = 0.f;
	for (float alpha = 0.f, step = PI_MUL_2/360.f; alpha < PI_MUL_2; alpha += step) {
		float					value = ai().level_graph().compute_square(alpha,PI,m_level_vertex_id);
		if (value > best_value) {
			best_value			= value;
			m_best_angle		= alpha;
		}
	}
	m_best_direction.setHP		(m_best_angle + 0*PI,0.f);
}

void CCoverEvaluatorAngle::evaluate			(CCoverPoint *cover_point)
{
	float					enemy_distance	= m_enemy_position.distance_to(cover_point->position());
//	float					my_distance		= m_start_position.distance_to(cover_point->position());

	if (enemy_distance <= m_min_distance)
		return;

	if (enemy_distance >= m_max_distance)
		return;

	Fvector					direction;
	float					y,p;
	direction.sub			(cover_point->position(),m_enemy_position);
	direction.getHP			(y,p);
	float					cover_value = ai().level_graph().cover_in_direction(y + PI,cover_point->level_vertex_id());
//	if (cover_value >= 2.f*m_best_value)
//		return;

	direction.normalize_safe();
	float					cos_a = direction.dotproduct(m_best_direction);
	clamp					(cos_a,-0.99999f,0.99999f);
	float					alpha = _abs(acosf(cos_a));
	if (alpha <= m_best_alpha)
		return;

	m_selected				= cover_point;
	m_best_value			= cover_value;
	m_best_alpha			= alpha;
}

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorSafe
//////////////////////////////////////////////////////////////////////////

void CCoverEvaluatorSafe::evaluate			(CCoverPoint *cover_point)
{
	if (m_start_position.distance_to(cover_point->position()) <= m_min_distance)
		return;

	float					cover_value = ai().level_graph().vertex_cover(cover_point->level_vertex_id());
	if (cover_value >= m_best_value)
		return;

	m_selected				= cover_point;
	m_best_value			= cover_value;
}

