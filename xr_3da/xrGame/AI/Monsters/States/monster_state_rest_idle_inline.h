#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterRestIdleAbstract CStateMonsterRestIdle<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterRestIdleAbstract::CStateMonsterRestIdle(_Object *obj) : inherited(obj)
{
}


TEMPLATE_SPECIALIZATION
void CStateMonsterRestIdleAbstract::execute()
{
	object->set_action				(ACT_REST);
	object->set_state_sound			(MonsterSpace::eMonsterSoundIdle);	
}
