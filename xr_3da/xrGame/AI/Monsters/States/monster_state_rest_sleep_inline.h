#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterRestSleepAbstract CStateMonsterRestSleep<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterRestSleepAbstract::CStateMonsterRestSleep(_Object *obj) : inherited(obj, ST_Rest)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterRestSleepAbstract::~CStateMonsterRestSleep	()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestSleepAbstract::initialize()
{
	inherited::initialize	();
	object->fall_asleep		();

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		DBG().object_info(object,object).remove_item	(u32(0));
		DBG().object_info(object,object).add_item		("Rest :: Sleep", D3DCOLOR_XRGB(255,0,0), 0);
	}
#endif

}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestSleepAbstract::execute()
{
	object->set_action				(ACT_SLEEP);
	object->set_state_sound			(MonsterSpace::eMonsterSoundIdle);	
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestSleepAbstract::finalize()
{
	inherited::finalize	();
	object->wake_up		();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestSleepAbstract::critical_finalize()
{
	inherited::critical_finalize	();
	object->wake_up					();
}

