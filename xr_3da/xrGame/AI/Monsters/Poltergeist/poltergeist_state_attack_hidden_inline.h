#pragma once

#include "../../../sound_player.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStatePoltergeistAttackHiddenAbstract CStatePoltergeistAttackHidden<_Object>

#define FLY_AROUND_DIST		15.f
#define DIST_TO_PATH_END	1.5f


TEMPLATE_SPECIALIZATION
void CStatePoltergeistAttackHiddenAbstract::initialize()
{
	inherited::initialize();
	select_target_point();
	object->movement().initialize_movement();
}

TEMPLATE_SPECIALIZATION
void CStatePoltergeistAttackHiddenAbstract::execute()
{
	// проверить на завершение пути
	if (object->movement().detail().time_path_built() > time_state_started) {
		if (object->movement().IsPathEnd(DIST_TO_PATH_END)) select_target_point();
	}
	
	object->movement().set_target_point		(m_target.point, m_target.node);
	object->movement().set_rebuild_time		(5000);
	object->movement().set_distance_to_end	(3.f);
	object->movement().set_use_covers		(false);

	object->MotionMan.m_tAction			= ACT_RUN;
	object->MotionMan.accel_activate	(eAT_Aggressive);
	object->MotionMan.accel_set_braking (false);
	object->sound().play				(MonsterSpace::eMonsterSoundAttack, 0,0,object->get_sd()->m_dwAttackSndDelay);
}

TEMPLATE_SPECIALIZATION
void CStatePoltergeistAttackHiddenAbstract::select_target_point()
{
	float dist = object->Position().distance_to(object->EnemyMan.get_enemy_position());
	if (dist > FLY_AROUND_DIST) {
		m_target.point	= object->EnemyMan.get_enemy_position();
		m_target.node	= object->EnemyMan.get_enemy_vertex();
	} else {
		m_target.point	= random_position(object->EnemyMan.get_enemy_position(), FLY_AROUND_DIST / 2);
		m_target.node	= u32(-1);
	}
}

#undef TEMPLATE_SPECIALIZATION
#undef CStatePoltergeistAttackHiddenAbstract