#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterCustomActionAbstract CStateMonsterCustomAction<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterCustomActionAbstract::CStateMonsterCustomAction(_Object *obj) : inherited(obj, &data)
{
	data.action			= ACT_STAND_IDLE; 
	data.spec_params	= 0;
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
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterCustomActionAbstract::check_completion()
{	
	if (time_state_started + data.time_out > object->m_current_update) return false;
	return true;
}
