#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>
#define CStateControlMoveOutAbstract CStateControlMoveOut<_Object>

#define MAX_STATE_TIME 10000

TEMPLATE_SPECIALIZATION
void CStateControlMoveOutAbstract::execute()
{
	object->path().set_target_point		(object->EnemyMan.get_enemy_position(), object->EnemyMan.get_enemy_vertex());
	object->path().set_generic_parameters();

	object->anim().m_tAction			= ACT_STEAL;
	object->anim().accel_deactivate	();
	object->anim().accel_set_braking (false);
	object->sound().play(MonsterSpace::eMonsterSoundAttack, 0,0,object->db().m_dwAttackSndDelay);

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
	if (time_state_started + MAX_STATE_TIME < Device.dwTimeGlobal) return true;

	return false;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateControlMoveOutAbstract
