#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterMoveToPointAbstract CStateMonsterMoveToPoint<_Object>

TEMPLATE_SPECIALIZATION
void CStateMonsterMoveToPointAbstract::initialize()
{
	inherited::initialize();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterMoveToPointAbstract::execute()
{
	if (data.vertex != u32(-1)) object->MoveToTarget(data.point, data.vertex);
	else object->MoveToTarget(data.point);

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
bool CStateMonsterMoveToPointAbstract::check_completion()
{	
	if (data.action.time_out !=0) {
		if (time_state_started + data.action.time_out < object->m_current_update) return true;
	} 
	
	if (object->Position().distance_to_xz(data.point) < data.completion_dist) return true;
	return false;
}


//////////////////////////////////////////////////////////////////////////
// CStateMonsterMoveToPointEx with path rebuild options
//////////////////////////////////////////////////////////////////////////

#define CStateMonsterMoveToPointExAbstract CStateMonsterMoveToPointEx<_Object>

TEMPLATE_SPECIALIZATION
void CStateMonsterMoveToPointExAbstract::initialize()
{
	inherited::initialize();

	time_last_build = 0;
}

TEMPLATE_SPECIALIZATION
void CStateMonsterMoveToPointExAbstract::execute()
{
	// проверить на завершение пути
	bool b_build = false;
	if (object->CDetailPathManager::time_path_built() > time_state_started) {
		if (data.time_to_rebuild != u32(-1)) {
			if (time_last_build + data.time_to_rebuild < object->m_current_update)
				b_build = true;	
		}
	} else b_build = true;	

	// проверить на завершение пути, при условии что target не достигнут
	if (object->IsPathEnd(data.completion_dist) && (object->Position().distance_to_xz(data.point) > 2.f * data.completion_dist)) {
		b_build = true;
	}
	
	if (b_build) {
		if (data.vertex != u32(-1)) object->MoveToTarget(data.point, data.vertex);
		else object->MoveToTarget(data.point);
 		time_last_build = object->m_current_update;
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
bool CStateMonsterMoveToPointExAbstract::check_completion()
{	
	if (data.action.time_out !=0) {
		if (time_state_started + data.action.time_out < object->m_current_update) return true;
	} 

	if (object->Position().distance_to_xz(data.point) < data.completion_dist) return true;
	return false;
}

