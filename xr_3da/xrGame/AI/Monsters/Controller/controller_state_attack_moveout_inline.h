#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>
#define CStateControlMoveOutAbstract CStateControlMoveOut<_Object>

#define MAX_STATE_TIME 10000

TEMPLATE_SPECIALIZATION
void CStateControlMoveOutAbstract::execute()
{
	object->MoveToTarget(object->EnemyMan.get_enemy_position(), object->EnemyMan.get_enemy_vertex());
	
	object->MotionMan.m_tAction			= ACT_STEAL;
	object->MotionMan.accel_deactivate	();
	object->MotionMan.accel_set_braking (false);

}

TEMPLATE_SPECIALIZATION
bool CStateControlMoveOutAbstract::check_start_conditions()
{
	if (object->EnemyMan.see_enemy_now()) return false;
	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateControlMoveOutAbstract::check_completion()
{
	if (object->HitMemory.get_last_hit_time() > time_state_started) return true;
	if (object->EnemyMan.see_enemy_now()) return true;
	if (time_state_started + MAX_STATE_TIME < object->m_current_update) return true;

	return false;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateControlMoveOutAbstract
