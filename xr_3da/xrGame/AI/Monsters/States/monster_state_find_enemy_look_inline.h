#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterFindEnemyLookAbstract CStateMonsterFindEnemyLook<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterFindEnemyLookAbstract::CStateMonsterFindEnemyLook(_Object *obj, state_ptr state_move, state_ptr state_look_around, state_ptr state_turn) : inherited(obj)
{
	add_state	(eMoveToPoint,	state_move);
	add_state	(eLookAround,	state_look_around);
	add_state	(eTurnAround,	state_turn);
}

TEMPLATE_SPECIALIZATION
CStateMonsterFindEnemyLookAbstract::~CStateMonsterFindEnemyLook()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterFindEnemyLookAbstract::execute()
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

TEMPLATE_SPECIALIZATION
bool CStateMonsterFindEnemyLookAbstract::check_completion()
{	
	if (current_side_id < 3) return false;
	
	//if (time_state_started + 3000 > Level().timeServer()) return false;
	return true;
}

