#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>
#define CStateControlMoveOutAbstract CStateControlMoveOut<_Object>

#define MAX_STATE_TIME 10000

TEMPLATE_SPECIALIZATION
void CStateControlMoveOutAbstract::execute()
{
	object->movement().set_target_point		(object->EnemyMan.get_enemy_position(), object->EnemyMan.get_enemy_vertex());
	object->movement().set_generic_parameters();

	object->MotionMan.m_tAction			= ACT_STEAL;
	object->MotionMan.accel_deactivate	();
	object->MotionMan.accel_set_braking (false);
	object->sound().play(MonsterSpace::eMonsterSoundAttack, 0,0,object->get_sd()->m_dwAttackSndDelay);

}

TEMPLATE_SPECIALIZATION
bool CStateControlMoveOutAbstract::check_start_conditions()
{
	if (object->IsVisibleObject(object->EnemyMan.get_enemy())) return false;
	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateControlMoveOutAbstract::check_completion()
{
	if (object->HitMemory.get_last_hit_time() > time_state_started) return true;
	if (object->IsVisibleObject(object->EnemyMan.get_enemy())) return true;
	if (time_state_started + MAX_STATE_TIME < object->m_current_update) return true;

	return false;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateControlMoveOutAbstract
