#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterPanicRunAbstract CStateMonsterPanicRun<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterPanicRunAbstract::CStateMonsterPanicRun(LPCSTR state_name) : inherited(state_name)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterPanicRunAbstract::~CStateMonsterPanicRun()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterPanicRunAbstract::initialize()
{
	inherited::initialize();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterPanicRunAbstract::execute()
{
	//Msg("Chim State Panic Executed!!! time = [%u] obj = [%u] K = [%u]", Level().timeServer(), m_object->m_tEnemy.obj,m_object->K);

	const CEntity *enemy = m_object->m_tEnemy.obj;
	//float dist = enemy->Position().distance_to(m_object->Position());

	m_object->MotionMan.m_tAction			= ACT_RUN;

	m_object->MoveAwayFromTarget			(enemy->Position());
	m_object->MotionMan.accel_activate		(eAT_Aggressive);
	m_object->MotionMan.accel_set_braking	(false);

	m_object->CSoundPlayer::play(MonsterSpace::eMonsterSoundPanic, 0,0,m_object->_sd->m_dwAttackSndDelay);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterPanicRunAbstract::finalize()
{
	inherited::finalize();
}

