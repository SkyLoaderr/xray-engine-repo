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
		DBG().object_info(object,object).remove_item	(u32(0));
		DBG().object_info(object,object).add_item		("Rest :: Sleep", D3DCOLOR_XRGB(255,0,0), 0);
	}
#endif
}
