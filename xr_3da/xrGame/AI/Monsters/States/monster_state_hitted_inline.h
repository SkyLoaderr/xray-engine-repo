#pragma once

#include "monster_state_hitted_hide.h"
#include "monster_state_hitted_moveout.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterHittedAbstract CStateMonsterHitted<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterHittedAbstract::CStateMonsterHitted(_Object *obj) : inherited(obj)
{
	add_state	(eStateHide,	xr_new<CStateMonsterHittedHide<_Object> >(obj));
	add_state	(eStateMoveOut,	xr_new<CStateMonsterHittedMoveOut<_Object> >(obj));
}

TEMPLATE_SPECIALIZATION
CStateMonsterHittedAbstract::~CStateMonsterHitted()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterHittedAbstract::reselect_state()
{
	if (prev_substate == u32(-1)) {
		select_state(eStateHide);
		return;
	}
	
	if (prev_substate == eStateHide) { 
		select_state(eStateMoveOut);
		return;
	}

	select_state(eStateHide);
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterHittedAbstract
