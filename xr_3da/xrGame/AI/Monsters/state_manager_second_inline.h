#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateManagerSecondAbstract CStateManagerSecond<_Object>

TEMPLATE_SPECIALIZATION
void CStateManagerSecondAbstract::reinit()
{
}

TEMPLATE_SPECIALIZATION
void CStateManagerSecondAbstract::update()
{
	execute();
}
TEMPLATE_SPECIALIZATION
void CStateManagerSecondAbstract::force_script_state(EGlobalStates state)
{
	// установить текущее состояние
	select_state(state);
}
TEMPLATE_SPECIALIZATION
void CStateManagerSecondAbstract::execute_script_state()
{
	// выполнить текущее состояние
	get_state_current()->execute();
}

TEMPLATE_SPECIALIZATION
bool CStateManagerSecondAbstract::can_eat()
{
	if (!object->CorpseMan.get_corpse()) return false;
	
	if (prev_substate == eStateEat) {
		if (!get_state_current()->check_completion())		return true;
	} else {
		if (get_state(eStateEat)->check_start_conditions())	return true;
	}

	return false;
}



#undef CStateManagerSecondAbstract 
#undef TEMPLATE_SPECIALIZATION