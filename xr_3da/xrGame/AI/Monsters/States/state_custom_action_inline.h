#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterCustomActionAbstract CStateMonsterCustomAction<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterCustomActionAbstract::CStateMonsterCustomAction(_Object *obj) : inherited(obj, ST_CustomAction, &data)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterCustomActionAbstract::~CStateMonsterCustomAction()
{
}


TEMPLATE_SPECIALIZATION
void CStateMonsterCustomActionAbstract::execute()
{
	object->MotionMan.m_tAction		= data.action;
	object->MotionMan.SetSpecParams(data.spec_params);

	if (data.sound_type != u32(-1)) {
		if (data.sound_delay != u32(-1))
			object->CSoundPlayer::play(data.sound_type, 0,0,data.sound_delay);
		else 
			object->CSoundPlayer::play(data.sound_type);
	}

}

TEMPLATE_SPECIALIZATION
bool CStateMonsterCustomActionAbstract::check_completion()
{	
	if (time_state_started + data.time_out > object->m_current_update) return false;
	return true;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterCustomActionAbstract
