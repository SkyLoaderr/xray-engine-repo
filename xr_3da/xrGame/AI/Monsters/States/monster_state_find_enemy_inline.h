#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterFindEnemyAbstract CStateMonsterFindEnemy<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterFindEnemyAbstract::CStateMonsterFindEnemy(_Object *obj, state_ptr state_run, state_ptr state_look_around, state_ptr state_angry, state_ptr state_walk_around) : inherited(obj)
{
	add_state	(eStateRun,			state_run);
	add_state	(eStateLookAround,	state_look_around);
	add_state	(eStateAngry,		state_angry);
	add_state	(eStateWalkAround,	state_walk_around);
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

