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
	CControlledEntityBase *entity = smart_cast<CControlledEntityBase *>(object);
	VERIFY(entity);

	switch (entity->get_data().m_task) {
		case eTaskFollow:	select_state(eStateFollow);	break;
		case eTaskAttack:	select_state(eStateAttack);	break;
		default:			NODEFAULT;
	} 
	
	get_state_current()->execute();

	prev_substate = current_substate;

}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterControlledAbstract
