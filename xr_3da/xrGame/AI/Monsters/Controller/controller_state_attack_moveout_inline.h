#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>
#define CStateControlMoveOutAbstract CStateControlMoveOut<_Object>

#define MAX_STATE_TIME 10000

TEMPLATE_SPECIALIZATION
void CStateControlMoveOutAbstract::execute()
{
	object->CMonsterMovement::set_target_point		(object->EnemyMan.get_enemy_position(), object->EnemyMan.get_enemy_vertex());
	object->CMonsterMovement::set_generic_parameters();

	object->MotionMan.m_tAction			= ACT_STEAL;
	object->MotionMan.accel_deactivate	();
	object->MotionMan.accel_set_braking (false);
	object->CSoundPlayer::play(MonsterSpace::eMonsterSoundAttack, 0,0,object->get_sd()->m_dwAttackSndDelay);

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		object->HDebug->M_Add(0,"Attack :: Move Out", D3DCOLOR_XRGB(255,0,0));
	}
#endif

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
