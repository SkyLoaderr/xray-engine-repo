#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterPanicRunAbstract CStateMonsterPanicRun<_Object>

#define MIN_UNSEEN_TIME			10000
#define MIN_DIST_TO_ENEMY		15.f
#define DIST_TO_PATH_END		2.5f


TEMPLATE_SPECIALIZATION
void CStateMonsterPanicRunAbstract::initialize()
{
	inherited::initialize();
	
	m_cover_reached				= false;
	target.node					= u32(-1);

	select_target_point			();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterPanicRunAbstract::execute()
{
	// проверить на завершение пути
	if (object->CDetailPathManager::time_path_built() > time_state_started) {
		if (object->IsPathEnd(DIST_TO_PATH_END)) {
			m_cover_reached			= true;
			select_target_point		();
		}
	}

	if (target.node != u32(-1)) 
		object->MoveToTarget			(target.position, target.node);
	else
		object->MoveAwayFromTarget		(object->EnemyMan.get_enemy_position());

	object->MotionMan.m_tAction				= ACT_RUN;
	object->MotionMan.accel_activate		(eAT_Aggressive);
	object->MotionMan.accel_set_braking		(false);
	object->CSoundPlayer::play				(MonsterSpace::eMonsterSoundPanic, 0,0,object->get_sd()->m_dwAttackSndDelay);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterPanicRunAbstract::select_target_point()
{
	if (!object->GetCoverFromPoint(object->EnemyMan.get_enemy_position(),target.position, target.node, 30.f,50.f,40.f)) {
		target.node					= u32(-1);
	} 
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterPanicRunAbstract::check_completion()
{
	if (!m_cover_reached) return false;
	
	float dist_to_enemy = object->Position().distance_to(object->EnemyMan.get_enemy_position());
	u32 time_delta	= object->m_current_update - object->EnemyMan.get_enemy_time_last_seen();

	if (dist_to_enemy < MIN_DIST_TO_ENEMY)  return false;
	if (time_delta	  < MIN_UNSEEN_TIME)	return false;

	return true;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterPanicRunAbstract
