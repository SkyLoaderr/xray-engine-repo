#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterFindEnemyLookAbstract CStateMonsterFindEnemyLook<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterFindEnemyLookAbstract::CStateMonsterFindEnemyLook(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterFindEnemyLookAbstract::~CStateMonsterFindEnemyLook()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterFindEnemyLookAbstract::initialize()
{

}

TEMPLATE_SPECIALIZATION
void CStateMonsterFindEnemyLookAbstract::execute()
{
	object->MotionMan.m_tAction			= ACT_LOOK_AROUND;
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterFindEnemyLookAbstract::check_completion()
{	
	if (time_state_started + 3000 > Level().timeServer()) return false;
	return true;
}

