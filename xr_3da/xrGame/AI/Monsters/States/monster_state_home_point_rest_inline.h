#pragma once

#include "../monster_home.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterRestMoveToHomePointAbstract CStateMonsterRestMoveToHomePoint<_Object>

TEMPLATE_SPECIALIZATION
void CStateMonsterRestMoveToHomePointAbstract::initialize()
{
	inherited::initialize	();
	m_target_node			= object->Home->get_place();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestMoveToHomePointAbstract::execute()
{
	object->path().set_target_point		(ai().level_graph().vertex_position(m_target_node), m_target_node);
	object->anim().accel_activate		(EAccelType(eAT_Calm));
	object->anim().accel_set_braking	(true);
	object->path().set_rebuild_time		(0);
	object->path().set_distance_to_end	(0.f);
	object->path().set_use_covers		(false);
	
	object->set_action					(ACT_WALK_FWD);
	object->set_state_sound				(MonsterSound::eMonsterSoundIdle);
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterRestMoveToHomePointAbstract::check_start_conditions()
{
	return (!object->Home->at_home());
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterRestMoveToHomePointAbstract::check_completion()
{
	return ((object->ai_location().level_vertex_id() == m_target_node) && !object->control().path_builder().is_moving_on_path());
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterRestMoveToHomePointAbstract
