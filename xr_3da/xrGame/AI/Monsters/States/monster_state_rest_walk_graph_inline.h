#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterRestWalkGraphAbstract CStateMonsterRestWalkGraph<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterRestWalkGraphAbstract::CStateMonsterRestWalkGraph(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterRestWalkGraphAbstract::~CStateMonsterRestWalkGraph	()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestWalkGraphAbstract::execute()
{
	object->MotionMan.m_tAction	= ACT_WALK_FWD;
	object->WalkNextGraphPoint	();
	object->CSoundPlayer::play	(MonsterSpace::eMonsterSoundIdle, 0,0,object->_sd->m_dwIdleSndDelay);
}

