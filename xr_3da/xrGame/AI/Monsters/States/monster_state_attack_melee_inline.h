#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterAttackMeleeAbstract CStateMonsterAttackMelee<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterAttackMeleeAbstract::CStateMonsterAttackMelee(LPCSTR state_name) : inherited(state_name)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterAttackMeleeAbstract::~CStateMonsterAttackMelee()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackMeleeAbstract::initialize()
{
	inherited::initialize();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackMeleeAbstract::execute()
{
	m_object->MotionMan.m_tAction	= ACT_ATTACK;
	m_object->FaceTarget			(m_object->EnemyMan.get_enemy());
	m_object->CSoundPlayer::play	(MonsterSpace::eMonsterSoundAttack, 0,0,m_object->_sd->m_dwAttackSndDelay);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackMeleeAbstract::finalize()
{
	inherited::finalize();
}

