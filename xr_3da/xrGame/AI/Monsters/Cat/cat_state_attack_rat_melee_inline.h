#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateCatAttackRatMeleeAbstract CStateCatAttackRatMelee<_Object>

TEMPLATE_SPECIALIZATION
CStateCatAttackRatMeleeAbstract::CStateCatAttackRatMelee(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
void CStateCatAttackRatMeleeAbstract::execute()
{
	object->set_action				(ACT_ATTACK);
	object->MotionMan.SetSpecParams	(ASP_ATTACK_RAT);

	object->DirMan.face_target	(object->EnemyMan.get_enemy(), 1200);
	object->set_state_sound		(MonsterSpace::eMonsterSoundAttack);

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		DBG().object_info(object,object).remove_item	(u32(0));
		DBG().object_info(object,object).add_item		("Attack Rat :: Melee", D3DCOLOR_XRGB(255,0,0), 0);
	}
#endif


}

TEMPLATE_SPECIALIZATION
bool CStateCatAttackRatMeleeAbstract::check_start_conditions()
{
	float m_fDistMin	= 0.7f;
	float dist			= object->Position().distance_to(object->EnemyMan.get_enemy()->Position());
	
	if (dist < m_fDistMin)	return true;
	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateCatAttackRatMeleeAbstract::check_completion()
{
	float m_fDistMax	= 3.0f;
	float dist			= object->Position().distance_to(object->EnemyMan.get_enemy()->Position());
	
	if (dist > m_fDistMax)	return true;
	return false;
}


