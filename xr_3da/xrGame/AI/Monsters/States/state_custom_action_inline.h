#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterCustomActionAbstract CStateMonsterCustomAction<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterCustomActionAbstract::CStateMonsterCustomAction(_Object *obj) : inherited(obj, &data)
{
	data.action			= ACT_STAND_IDLE; 
}

TEMPLATE_SPECIALIZATION
CStateMonsterCustomActionAbstract::~CStateMonsterCustomAction()
{
}


TEMPLATE_SPECIALIZATION
void CStateMonsterCustomActionAbstract::execute()
{
	object->MotionMan.m_tAction	= data.action;
}

