#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterRestSleepAbstract CStateMonsterRestSleep<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterRestSleepAbstract::CStateMonsterRestSleep(LPCSTR state_name) : inherited(state_name)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterRestSleepAbstract::~CStateMonsterRestSleep	()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestSleepAbstract::initialize()
{
	inherited::initialize();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestSleepAbstract::execute()
{
	m_object->MotionMan.m_tAction = ACT_SLEEP;
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestSleepAbstract::finalize()
{
	inherited::finalize();
}

