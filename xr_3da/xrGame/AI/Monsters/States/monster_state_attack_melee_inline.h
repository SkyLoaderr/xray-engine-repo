#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterAttackMeleeAbstract CStateMonsterAttackMelee<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterAttackMeleeAbstract::CStateMonsterAttackMelee(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterAttackMeleeAbstract::~CStateMonsterAttackMelee()
{
}


TEMPLATE_SPECIALIZATION
void CStateMonsterAttackMeleeAbstract::execute()
{
	object->MotionMan.m_tAction	= ACT_ATTACK;
	object->FaceTarget			(object->EnemyMan.get_enemy());
	object->CSoundPlayer::play	(MonsterSpace::eMonsterSoundAttack, 0,0,object->get_sd()->m_dwAttackSndDelay);
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackMeleeAbstract::check_start_conditions()
{
	float m_fDistMin, m_fDistMax, dist;
	dist = object->GetEnemyDistances(m_fDistMin, m_fDistMax);
	
	if (dist < m_fDistMax)	return true;
	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackMeleeAbstract::check_completion()
{
	float m_fDistMin, m_fDistMax, dist;
	dist = object->GetEnemyDistances(m_fDistMin, m_fDistMax);

	if (dist > m_fDistMax)	return true;
	return false;
}


