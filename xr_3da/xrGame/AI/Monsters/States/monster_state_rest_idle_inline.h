#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterRestIdleAbstract CStateMonsterRestIdle<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterRestIdleAbstract::CStateMonsterRestIdle(_Object *obj) : inherited(obj, ST_Rest)
{
}


TEMPLATE_SPECIALIZATION
void CStateMonsterRestIdleAbstract::execute()
{
	object->set_action				(ACT_REST);
	object->set_state_sound			(MonsterSpace::eMonsterSoundIdle);	

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		object->HDebug->M_Add(0,"Rest :: Idle", D3DCOLOR_XRGB(255,0,0));

	}
#endif
}
