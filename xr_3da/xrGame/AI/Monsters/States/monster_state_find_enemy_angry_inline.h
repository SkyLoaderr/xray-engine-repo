#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterFindEnemyAngryAbstract CStateMonsterFindEnemyAngry<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterFindEnemyAngryAbstract::CStateMonsterFindEnemyAngry(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterFindEnemyAngryAbstract::~CStateMonsterFindEnemyAngry()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterFindEnemyAngryAbstract::execute()
{
	object->MotionMan.m_tAction			= ACT_ATTACK;
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterFindEnemyAngryAbstract::check_completion()
{	
	if (time_state_started + 4000 > Level().timeServer()) return false;
	return true;
}

