////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_property_evaluators.cpp
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker property evaluators classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_property_evaluators.h"
#include "ai/stalker/ai_stalker.h"
#include "stalker_decision_space.h"
using namespace StalkerDecisionSpace;

typedef CStalkerPropertyEvaluator::_value_type _value_type;

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorALife
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorALife::evaluate	()
{
	return			(!!ai().get_alife());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorAlive
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorAlive::evaluate	()
{
	return			(!!m_object->g_Alive());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorItems
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorItems::evaluate	()
{
	return			(!!m_object->item());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorEnemies
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorEnemies::evaluate	()
{
	return			(!!m_object->enemy());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorSeeEnemy
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorSeeEnemy::evaluate	()
{
	return				(m_object->enemy() ? m_object->visible(m_object->enemy()) : false);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorItemToKill
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorItemToKill::evaluate	()
{
	return				(!!m_object->item_to_kill());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorItemCanKill
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorItemCanKill::evaluate	()
{
	return				(m_object->item_can_kill());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorFoundItemToKill
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorFoundItemToKill::evaluate	()
{
	return				(m_object->remember_item_to_kill());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorFoundAmmo
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorFoundAmmo::evaluate	()
{
	return				(m_object->remember_ammo());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorReadyToKill
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorReadyToKill::evaluate	()
{
	return				(m_object->ready_to_kill());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorKillDistance
//////////////////////////////////////////////////////////////////////////

_value_type CStalkerPropertyEvaluatorKillDistance::evaluate	()
{
	return				(m_object->kill_distance());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorSafeToKill
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorSafeToKill::CStalkerPropertyEvaluatorSafeToKill(CPropertyStorage *storage)
{
	m_storage			= storage;
	m_last_cover_time	= 0;
}

_value_type CStalkerPropertyEvaluatorSafeToKill::evaluate	()
{
	if (!m_object->enemy())
		return			(false);

	CMemoryInfo			mem_object = m_object->memory(m_object->enemy());
	if (!mem_object.m_object)
		return			(false);

	if (!m_object->visible(m_object->enemy())) {
		Fvector			direction;
		float			y,p;
		direction.sub	(mem_object.m_object_params.m_position,m_object->Position());
		direction.getHP	(y,p);
		float			cover = ai().level_graph().cover_in_direction(y,m_object->level_vertex_id());
		if (cover <= .2f)
			m_last_cover_time = Level().timeServer();
		else
			if ((Level().timeServer() <= mem_object.m_level_time + 10000) &&
				(Level().timeServer() > m_last_cover_time + 10000))
				return	(false);
	}
	else
		if (Level().timeServer() >= m_last_cover_time + 10000)
			return		(false);
		else
			return		(true);

	if (m_storage->property(eWorldPropertyFireEnough))
		return			(false);

	return				(true);
}

