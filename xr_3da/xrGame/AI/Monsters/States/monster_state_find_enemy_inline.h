#pragma once

#include "monster_state_find_enemy_run.h"
#include "monster_state_find_enemy_angry.h"
#include "monster_state_find_enemy_walk.h"
#include "monster_state_find_enemy_look.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterFindEnemyAbstract CStateMonsterFindEnemy<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterFindEnemyAbstract::CStateMonsterFindEnemy(_Object *obj) : inherited(obj)
{
	add_state	(eStateRun,			xr_new<CStateMonsterFindEnemyRun<_Object> >			(obj));
	add_state	(eStateLookAround,	xr_new<CStateMonsterFindEnemyLook<_Object> >		(obj));
	add_state	(eStateAngry,		xr_new<CStateMonsterFindEnemyAngry<_Object> >		(obj));
	add_state	(eStateWalkAround,	xr_new<CStateMonsterFindEnemyWalkAround<_Object> >	(obj));
}

TEMPLATE_SPECIALIZATION
CStateMonsterFindEnemyAbstract::~CStateMonsterFindEnemy()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterFindEnemyAbstract::reselect_state()
{
	if (prev_substate == u32(-1)) {
		select_state(eStateRun);
		return;
	}
	
	switch (prev_substate)	{
		case eStateRun:			select_state(eStateLookAround);	break;
		case eStateLookAround:	select_state(eStateAngry);		break;
		case eStateAngry:		select_state(eStateWalkAround);	break;
		case eStateWalkAround:	select_state(eStateWalkAround);	break;
	}
}

