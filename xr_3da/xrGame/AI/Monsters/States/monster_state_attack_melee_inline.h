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
	object->DirMan.face_target	(object->EnemyMan.get_enemy(), 1200);
	object->set_state_sound		(MonsterSpace::eMonsterSoundAttack);

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		DBG().object_info(object,object).remove_item	(u32(0));
		DBG().object_info(object,object).add_item		("Attack :: Melee", D3DCOLOR_XRGB(255,0,0), 0);
	}
#endif
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


