#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterLookToPointAbstract CStateMonsterLookToPoint<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterLookToPointAbstract::CStateMonsterLookToPoint(_Object *obj) : inherited(obj, ST_LookPoint,&data)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterLookToPointAbstract::~CStateMonsterLookToPoint()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterLookToPointAbstract::initialize()
{
	inherited::initialize();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterLookToPointAbstract::execute()
{
	object->MotionMan.m_tAction				= data.action.action;
	object->MotionMan.SetSpecParams			(data.action.spec_params);
	object->FaceTarget						(data.point);

	if (data.action.sound_type != u32(-1)) {
		if (data.action.sound_delay != u32(-1))
			object->CSoundPlayer::play(data.action.sound_type, 0,0,data.action.sound_delay);
		else 
			object->CSoundPlayer::play(data.action.sound_type);
	}

}

TEMPLATE_SPECIALIZATION
bool CStateMonsterLookToPointAbstract::check_completion()
{	
	if (data.action.time_out != 0) {
		if (time_state_started + data.action.time_out < object->m_current_update) return true;
	} else if (angle_difference(object->m_body.current.yaw, object->m_body.target.yaw) < deg(1)) 
		return true;
	return false;
}

