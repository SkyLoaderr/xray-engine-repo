#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterFindEnemyWalkAbstract CStateMonsterFindEnemyWalkAround<_Object>

TEMPLATE_SPECIALIZATION
void CStateMonsterFindEnemyWalkAbstract::execute()
{
	object->set_action			(ACT_STAND_IDLE);
	object->set_state_sound		(MonsterSpace::eMonsterSoundAttack);

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		DBG().object_info(object,object).remove_item	(u32(0));
		DBG().object_info(object,object).add_item		("Find Enemy :: Walk around", D3DCOLOR_XRGB(255,0,0), 0);
	}
#endif
}

