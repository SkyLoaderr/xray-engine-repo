#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterThreatenThreatenAbstract CStateMonsterThreatenThreaten<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterThreatenThreatenAbstract::CStateMonsterThreatenThreaten(LPCSTR state_name) : inherited(state_name)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterThreatenThreatenAbstract::~CStateMonsterThreatenThreaten()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterThreatenThreatenAbstract::initialize()
{
	inherited::initialize();

	time_faced_enemy = 0;
}

TEMPLATE_SPECIALIZATION
void CStateMonsterThreatenThreatenAbstract::execute()
{
	m_object->MotionMan.m_tAction	= ACT_STAND_IDLE;

	const CEntity *enemy = m_object->m_tEnemy.obj;

	// Смотреть на врага 
	TTime time_current = Level().timeServer();
	if (time_faced_enemy + 1200 < time_current) {
		time_faced_enemy = time_current;
		m_object->FaceTarget(enemy);
	}

	m_object->MotionMan.SetSpecParams	(ASP_THREATEN);
	m_object->CSoundPlayer::play		(MonsterSpace::eMonsterSoundThreaten);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterThreatenThreatenAbstract::finalize()
{
	inherited::finalize();
}

