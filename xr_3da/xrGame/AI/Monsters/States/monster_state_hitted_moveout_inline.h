#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>
#define CStateMonsterHittedMoveOutAbstract CStateMonsterHittedMoveOut<_Object>

#define DIST_TO_PATH_END	1.5f
#define DIST_TO_HIT_POINT	3.f

TEMPLATE_SPECIALIZATION
void CStateMonsterHittedMoveOutAbstract::initialize()
{
	inherited::initialize();
	select_target();
	object->CMonsterMovement::initialize_movement	();	
}

TEMPLATE_SPECIALIZATION
void CStateMonsterHittedMoveOutAbstract::execute()
{
	// проверить на завершение пути
	if (object->CDetailPathManager::time_path_built() > time_state_started) {
		if (object->IsPathEnd(DIST_TO_PATH_END)) 
			select_target		();
	}
	
	if (target.node != u32(-1))
		object->CMonsterMovement::set_target_point	(target.position, target.node);
	else
		object->CMonsterMovement::set_target_point	(object->HitMemory.get_last_hit_position());

	float dist = object->HitMemory.get_last_hit_position().distance_to(object->Position());

	if (dist > 10.f) object->set_action	(ACT_WALK_FWD);
	else object->set_action				(ACT_STEAL);
	
	object->MotionMan.accel_deactivate	();
	object->MotionMan.accel_set_braking (false);
	object->set_state_sound				(MonsterSpace::eMonsterSoundIdle);


#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		object->HDebug->M_Add(0,"Hitted :: Move Out", D3DCOLOR_XRGB(255,0,0));
	}
#endif


}

TEMPLATE_SPECIALIZATION
bool CStateMonsterHittedMoveOutAbstract::check_completion()
{
	if (object->HitMemory.get_last_hit_time() > time_state_started) return true;

	float dist = object->HitMemory.get_last_hit_position().distance_to(object->Position());
	if (dist < DIST_TO_HIT_POINT) return true;

	return false;
}

TEMPLATE_SPECIALIZATION
void CStateMonsterHittedMoveOutAbstract::select_target()
{
	if (!object->GetCoverCloseToPoint(object->HitMemory.get_last_hit_position(), 10.f, 20.f, 0.f, 15.f, target.position, target.node)){
		target.node = u32(-1);
	}
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterHittedMoveOutAbstract
