#pragma once

#include "monster_state_rest_sleep.h"
#include "monster_state_rest_walk_graph.h"
#include "monster_state_rest_idle.h"
#include "monster_state_rest_fun.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterRestAbstract CStateMonsterRest<_Object>

#define TIME_DELAY_FUN	20000

TEMPLATE_SPECIALIZATION
CStateMonsterRestAbstract::CStateMonsterRest(_Object *obj) : inherited(obj)
{
	add_state	(eStateSleep,			xr_new<CStateMonsterRestSleep<_Object> >	(obj));
	add_state	(eStateWalkGraphPoint,	xr_new<CStateMonsterRestWalkGraph<_Object> >(obj));
	add_state	(eStateIdle,			xr_new<CStateMonsterRestIdle<_Object> >		(obj));
	add_state	(eStateFun,				xr_new<CStateMonsterRestFun<_Object> >		(obj));
}

TEMPLATE_SPECIALIZATION
CStateMonsterRestAbstract::CStateMonsterRest(_Object *obj, state_ptr state_sleep, state_ptr state_walk) : inherited(obj)
{
	add_state	(eStateSleep,			state_sleep);
	add_state	(eStateWalkGraphPoint,	state_walk);
	add_state	(eStateIdle,			xr_new<CStateMonsterRestIdle<_Object> >		(obj));
	add_state	(eStateFun,				xr_new<CStateMonsterRestFun<_Object> >		(obj));
}

TEMPLATE_SPECIALIZATION
CStateMonsterRestAbstract::~CStateMonsterRest	()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestAbstract::initialize()
{
	inherited::initialize	();

	time_last_fun			= 0;
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestAbstract::execute()
{
	bool bNormalSatiety =	(object->conditions().GetSatiety() > object->get_sd()->m_fMinSatiety) && 
		(object->conditions().GetSatiety() < object->get_sd()->m_fMaxSatiety); 

	bool state_fun = false;

	if (prev_substate == eStateFun) {
		if (!get_state(eStateFun)->check_completion()) 
			state_fun = true;
	} else {
		if (get_state(eStateFun)->check_start_conditions() && (time_last_fun + TIME_DELAY_FUN < Device.dwTimeGlobal)) 
			state_fun = true;
	}
	
	if (state_fun) {
		select_state	(eStateFun);
	} else {
		if (bNormalSatiety) {
			select_state	(eStateIdle);
		} else {
			select_state	(eStateWalkGraphPoint);
		}
	}
	
	if ((prev_substate == eStateFun) && (current_substate != prev_substate)) time_last_fun = Device.dwTimeGlobal;

	get_state_current()->execute();

	prev_substate = current_substate;
}

