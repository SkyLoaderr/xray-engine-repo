#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterPanicRunAbstract CStateMonsterPanicRun<_Object>

#define MIN_UNSEEN_TIME			15000
#define MIN_DIST_TO_ENEMY		15.f

TEMPLATE_SPECIALIZATION
void CStateMonsterPanicRunAbstract::initialize()
{
	inherited::initialize();
	
	object->CMonsterMovement::initialize_movement		();	
}

TEMPLATE_SPECIALIZATION
void CStateMonsterPanicRunAbstract::execute()
{
	object->set_action									(ACT_RUN);
	object->set_state_sound								(MonsterSpace::eMonsterSoundPanic);
	object->MotionMan.accel_activate					(eAT_Aggressive);
	object->MotionMan.accel_set_braking					(false);
	object->CMonsterMovement::set_retreat_from_point	(object->EnemyMan.get_enemy_position());
	object->CMonsterMovement::set_generic_parameters	();
	
#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		object->HDebug->M_Add(0,"Panic :: Run", D3DCOLOR_XRGB(255,0,0));
	}
#endif

}
TEMPLATE_SPECIALIZATION
bool CStateMonsterPanicRunAbstract::check_completion()
{
	float dist_to_enemy = object->Position().distance_to(object->EnemyMan.get_enemy_position());
	u32 time_delta	= object->m_current_update - object->EnemyMan.get_enemy_time_last_seen();

	if (dist_to_enemy < MIN_DIST_TO_ENEMY)  return false;
	if (time_delta	  < MIN_UNSEEN_TIME)	return false;

	return true;
}

#undef DIST_TO_PATH_END
#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterPanicRunAbstract
