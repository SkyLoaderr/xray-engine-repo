#pragma once

#include "monster_state_rest_sleep.h"
#include "monster_state_rest_walk_graph.h"
#include "monster_state_rest_idle.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterRestAbstract CStateMonsterRest<_Object>


TEMPLATE_SPECIALIZATION
CStateMonsterRestAbstract::CStateMonsterRest(_Object *obj) : inherited(obj)
{
	add_state	(eStateSleep,			xr_new<CStateMonsterRestSleep<_Object> >	(obj));
	add_state	(eStateWalkGraphPoint,	xr_new<CStateMonsterRestWalkGraph<_Object> >(obj));
	add_state	(eStateIdle,			xr_new<CStateMonsterRestIdle<_Object> >		(obj));
}

TEMPLATE_SPECIALIZATION
CStateMonsterRestAbstract::CStateMonsterRest(_Object *obj, state_ptr state_sleep, state_ptr state_walk) : inherited(obj)
{
	add_state	(eStateSleep,			state_sleep);
	add_state	(eStateWalkGraphPoint,	state_walk);
	add_state	(eStateIdle,			xr_new<CStateMonsterRestIdle<_Object> >		(obj));
}

TEMPLATE_SPECIALIZATION
CStateMonsterRestAbstract::~CStateMonsterRest	()
{
}


TEMPLATE_SPECIALIZATION
void CStateMonsterRestAbstract::execute()
{
	bool bNormalSatiety =	(object->GetSatiety() > object->get_sd()->m_fMinSatiety) && 
		(object->GetSatiety() < object->get_sd()->m_fMaxSatiety); 

	if (bNormalSatiety) {
		select_state	(eStateIdle);
	} else {
		select_state	(eStateWalkGraphPoint);
	}

	get_state_current()->execute();
}

