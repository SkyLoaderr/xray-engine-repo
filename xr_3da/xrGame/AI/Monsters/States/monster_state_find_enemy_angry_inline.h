#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterFindEnemyAngryAbstract CStateMonsterFindEnemyAngry<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterFindEnemyAngryAbstract::CStateMonsterFindEnemyAngry(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterFindEnemyAngryAbstract::~CStateMonsterFindEnemyAngry()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterFindEnemyAngryAbstract::execute()
{
	object->set_action					(ACT_STAND_IDLE);
	object->MotionMan.SetSpecParams		(ASP_THREATEN);
	object->set_state_sound				(MonsterSpace::eMonsterSoundAttack);

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		DBG().object_info(object,object).remove_item	(u32(0));
		DBG().object_info(object,object).add_item		("Find Enemy :: Angry", D3DCOLOR_XRGB(255,0,0), 0);

	}
#endif
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterFindEnemyAngryAbstract::check_completion()
{	
	if (time_state_started + 4000 > Device.dwTimeGlobal) return false;
	return true;
}

