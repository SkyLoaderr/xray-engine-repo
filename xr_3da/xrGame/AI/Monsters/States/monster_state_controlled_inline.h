#pragma once

#include "monster_state_controlled_attack.h"
#include "monster_state_controlled_follow.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterControlledAbstract CStateMonsterControlled<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterControlledAbstract::CStateMonsterControlled(_Object *obj) : inherited(obj)
{
	add_state	(eStateAttack,		xr_new<CStateMonsterControlledAttack<_Object> >	(obj));
	add_state	(eStateFollow,		xr_new<CStateMonsterControlledFollow<_Object> >	(obj));	
}

TEMPLATE_SPECIALIZATION
void CStateMonsterControlledAbstract::execute()
{
	switch (object->get_data().m_task) {
		case eTaskFollow:	select_state(eStateFollow);	break;
		case eTaskAttack:	{
			// проверить валидность данных атаки
			const CEntity *enemy = object->get_data().m_object;
			if (!enemy || enemy->getDestroy() || !enemy->g_Alive()) {
				object->get_data().m_object = object->get_controller();
				select_state(eStateFollow);
			} else 
				select_state(eStateAttack);	break;
		}
		default:			NODEFAULT;
	} 
	
	get_state_current()->execute();

	prev_substate = current_substate;

}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterControlledAbstract
