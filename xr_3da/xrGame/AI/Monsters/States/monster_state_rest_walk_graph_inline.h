#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterRestWalkGraphAbstract CStateMonsterRestWalkGraph<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterRestWalkGraphAbstract::CStateMonsterRestWalkGraph(LPCSTR state_name) : inherited(state_name)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterRestWalkGraphAbstract::~CStateMonsterRestWalkGraph	()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestWalkGraphAbstract::initialize()
{
	inherited::initialize();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestWalkGraphAbstract::execute()
{
	m_object->MotionMan.m_tAction	= ACT_WALK_FWD;
	m_object->WalkNextGraphPoint	();
	m_object->CSoundPlayer::play	(MonsterSpace::eMonsterSoundIdle, 0,0,m_object->_sd->m_dwIdleSndDelay);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestWalkGraphAbstract::finalize()
{
	inherited::finalize();
}

