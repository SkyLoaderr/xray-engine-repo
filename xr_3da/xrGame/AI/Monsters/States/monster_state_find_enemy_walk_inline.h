#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterFindEnemyWalkAbstract CStateMonsterFindEnemyWalkAround<_Object>

TEMPLATE_SPECIALIZATION
void CStateMonsterFindEnemyWalkAbstract::execute()
{
	object->MotionMan.m_tAction			= ACT_STAND_IDLE;
}

