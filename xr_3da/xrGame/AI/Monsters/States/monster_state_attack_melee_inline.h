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
	object->set_action			(ACT_ATTACK);
	object->dir().face_target	(object->EnemyMan.get_enemy(), 1200);
	object->set_state_sound		(MonsterSpace::eMonsterSoundAttack);
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackMeleeAbstract::check_start_conditions()
{
	float m_fDistMax	= object->MeleeChecker.get_max_distance		();
	float dist			= object->MeleeChecker.distance_to_enemy	(object->EnemyMan.get_enemy());
	
	if (dist < m_fDistMax)	return true;
	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackMeleeAbstract::check_completion()
{
	float m_fDistMax	= object->MeleeChecker.get_max_distance		();
	float dist			= object->MeleeChecker.distance_to_enemy	(object->EnemyMan.get_enemy());


	if (dist > m_fDistMax)	return true;
	return false;
}


