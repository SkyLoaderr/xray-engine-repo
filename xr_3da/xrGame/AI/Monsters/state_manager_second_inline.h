#pragma once

#include "../../level.h"
#include "../../level_debug.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateManagerSecondAbstract CStateManagerSecond<_Object>

TEMPLATE_SPECIALIZATION
void CStateManagerSecondAbstract::reinit()
{
	inherited::reinit();
}

TEMPLATE_SPECIALIZATION
void CStateManagerSecondAbstract::update()
{
	execute();
}
TEMPLATE_SPECIALIZATION
void CStateManagerSecondAbstract::force_script_state(EGlobalStates state)
{
	// ���������� ������� ���������
	select_state(state);
}
TEMPLATE_SPECIALIZATION
void CStateManagerSecondAbstract::execute_script_state()
{
	// ��������� ������� ���������
	get_state_current()->execute();
}

TEMPLATE_SPECIALIZATION
bool CStateManagerSecondAbstract::can_eat()
{
	if (!object->CorpseMan.get_corpse()) return false;
	
	return check_state(eStateEat);
}

TEMPLATE_SPECIALIZATION
bool CStateManagerSecondAbstract::check_state(u32 state_id) 
{
	if (prev_substate == state_id) {
		if (!get_state_current()->check_completion())		return true;
	} else {
		if (get_state(state_id)->check_start_conditions())	return true;
	}

	return false;
}

TEMPLATE_SPECIALIZATION
void CStateManagerSecondAbstract::critical_finalize()
{
	inherited::critical_finalize();
	
#ifdef DEBUG
	DBG().object_info(object,object).remove_item(u32(0));
#endif
}

#undef CStateManagerSecondAbstract 
#undef TEMPLATE_SPECIALIZATION