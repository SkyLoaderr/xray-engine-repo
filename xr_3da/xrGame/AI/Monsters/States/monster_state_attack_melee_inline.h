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
	object->FaceTarget			(object->EnemyMan.get_enemy());
	object->set_state_sound		(MonsterSpace::eMonsterSoundAttack);

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		object->HDebug->M_Add(0,"Attack :: Melee", D3DCOLOR_XRGB(255,0,0));
	}
#endif

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


