#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterHideFromPointAbstract CStateMonsterHideFromPoint<_Object>
#define DIST_TO_PATH_END		1.5f

TEMPLATE_SPECIALIZATION
void CStateMonsterHideFromPointAbstract::initialize()
{
	inherited::initialize();

	select_target_point();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterHideFromPointAbstract::execute()
{
	// проверить на завершение пути
	if (object->CDetailPathManager::time_path_built() > time_state_started) {
		if (object->IsPathEnd(DIST_TO_PATH_END)) select_target_point();
	}

	if (target.node != u32(-1)) {
		object->MoveToTarget			(target.position, target.node);
	} else {
		object->MoveAwayFromTarget		(data.point);
	}

	object->MotionMan.m_tAction				= data.action.action;
	object->MotionMan.SetSpecParams			(data.action.spec_params);

	if (data.accelerated) {
		object->MotionMan.accel_activate	(EAccelType(data.accel_type));
		object->MotionMan.accel_set_braking (data.braking);
	}

	if (data.action.sound_type != u32(-1)) {
		if (data.action.sound_delay != u32(-1))
			object->CSoundPlayer::play(data.action.sound_type, 0,0,data.action.sound_delay);
		else 
			object->CSoundPlayer::play(data.action.sound_type);
	}
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterHideFromPointAbstract::check_completion()
{	
	if (data.action.time_out !=0) {
		if (time_state_started + data.action.time_out < object->m_current_update) return true;
	} 

	if (!fis_zero(data.distance)) {
		if (object->Position().distance_to(data.point) > data.distance) return true;
	}
		
	return false;
}

TEMPLATE_SPECIALIZATION
void CStateMonsterHideFromPointAbstract::select_target_point()
{
	if (!object->GetCoverFromPoint(data.point,target.position, target.node, data.cover_min_dist,data.cover_max_dist,data.cover_search_radius)) 
		target.node	= u32(-1);
	else if (target.position.distance_to(object->Position()) < 2.f) 
		target.node	= u32(-1);
}

#undef DIST_TO_PATH_END
#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterHideFromPointAbstract
