#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>
#define CStateMonsterHittedHideAbstract CStateMonsterHittedHide<_Object>

#define GOOD_DISTANCE_TO_ENEMY	10.f
#define GOOD_DISTANCE_IN_COVER	15.f
#define MIN_HIDE_TIME			3.f
#define DIST_TO_PATH_END		1.5f

TEMPLATE_SPECIALIZATION
void CStateMonsterHittedHideAbstract::initialize()
{
	inherited::initialize();
	
	m_cover_reached		= false;
	select_target_point();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterHittedHideAbstract::execute()
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
		object->MoveAwayFromTarget		(danger_point);

	object->MotionMan.m_tAction			= ACT_RUN;
	object->MotionMan.accel_activate	(eAT_Aggressive);
	object->MotionMan.accel_set_braking	(false);
	object->CSoundPlayer::play			(MonsterSpace::eMonsterSoundAttack, 0,0,object->get_sd()->m_dwAttackSndDelay);
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterHittedHideAbstract::check_start_conditions()
{
	if (object->HitMemory.is_hit() && !object->EnemyMan.get_enemy()) return true;
	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterHittedHideAbstract::check_completion()
{
	float dist = object->Position().distance_to(danger_point);

	// good dist  
	if (dist < GOOD_DISTANCE_IN_COVER) return false;
	// +hide more than 3 sec
	if (time_state_started + MIN_HIDE_TIME > object->m_current_update) return false;
	// +should reach at least one target cover point 
	if (!m_cover_reached) return false;

	return true;
}

TEMPLATE_SPECIALIZATION
void CStateMonsterHittedHideAbstract::select_target_point()
{
	danger_point.mad(object->Position(), object->HitMemory.get_last_hit_dir(), 2.f);
	
	if (!object->GetCoverFromPoint(danger_point, target.position, target.node, 10.f,30.f,20.f)) 
		target.node	= u32(-1);
	else if (target.position.distance_to(object->Position()) < 2.f) 
		target.node	= u32(-1);
}

#undef DIST_TO_PATH_END
#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterHittedHideAbstract
