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
		object->HDebug->M_Add(0,"Find Enemy :: Walk around", D3DCOLOR_XRGB(255,0,0));
	}
#endif

}

