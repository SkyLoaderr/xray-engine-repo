#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>
#define CStateControllerHideAbstract CStateControlHide<_Object>

#define GOOD_DISTANCE_TO_ENEMY	10.f
#define GOOD_DISTANCE_IN_COVER	10.f
#define MIN_HIDE_TIME			3.f
#define DIST_TO_PATH_END		1.5f
#define MAX_DIST_TO_ENEMY		15.f

TEMPLATE_SPECIALIZATION
void CStateControllerHideAbstract::initialize()
{
	inherited::initialize();
	
	m_cover_reached		= false;
	select_target_point();
	object->CMonsterMovement::initialize_movement();
}

TEMPLATE_SPECIALIZATION
void CStateControllerHideAbstract::execute()
{
	// проверить на завершение пути
	if (object->CDetailPathManager::time_path_built() > time_state_started) {
		if (object->IsPathEnd(DIST_TO_PATH_END)) {
			m_cover_reached			= true;
			select_target_point		();
		}
	}

	if (target.node != u32(-1)) 
		object->CMonsterMovement::set_target_point		(target.position, target.node);
	else
		object->CMonsterMovement::set_retreat_from_point(object->EnemyMan.get_enemy_position());

	object->CMonsterMovement::set_generic_parameters	();

	object->MotionMan.m_tAction			= ACT_RUN;
	object->MotionMan.accel_activate	(eAT_Aggressive);
	object->MotionMan.accel_set_braking	(false);
	object->CSoundPlayer::play			(MonsterSpace::eMonsterSoundAttack, 0,0,object->get_sd()->m_dwAttackSndDelay);

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		object->HDebug->M_Add(0,"Attack :: Hide", D3DCOLOR_XRGB(255,0,0));
	}
#endif


}

TEMPLATE_SPECIALIZATION
bool CStateControllerHideAbstract::check_start_conditions()
{
	float dist = object->Position().distance_to(object->EnemyMan.get_enemy()->Position());
	if (dist < GOOD_DISTANCE_TO_ENEMY) return false;
	if (dist > MAX_DIST_TO_ENEMY) return false;

	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateControllerHideAbstract::check_completion()
{
	float dist = object->Position().distance_to(object->EnemyMan.get_enemy()->Position());

	// good dist  
	if (dist < GOOD_DISTANCE_IN_COVER) return false;
	// +enemy doesnt see me
	if (!object->EnemyMan.get_flags().is(FLAG_ENEMY_DOESNT_SEE_ME)) return false;
	// +hide more than 3 sec
	if (time_state_started + MIN_HIDE_TIME > object->m_current_update) return false;
	// +should reach at least one target cover point 
	if (!m_cover_reached) return false;

	return true;
}

TEMPLATE_SPECIALIZATION
void CStateControllerHideAbstract::select_target_point()
{
	if (!object->GetCoverFromPoint(object->EnemyMan.get_enemy_position(),target.position, target.node, 10.f,30.f,20.f)) 
		target.node	= u32(-1);
	else if (target.position.distance_to(object->Position()) < 2.f) 
		target.node	= u32(-1);
}

#undef GOOD_DISTANCE_IN_COVER
#undef DIST_TO_PATH_END
#undef TEMPLATE_SPECIALIZATION
#undef CStateControllerHideAbstract
