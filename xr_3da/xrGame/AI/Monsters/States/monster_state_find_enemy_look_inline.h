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
void CStateMonsterFindEnemyLookAbstract::reselect_state()
{
//	if (prev_substate == u32(-1)) {
//		select_state(eStateRun);
//		return;
//	}
//
//	switch (prev_substate)	{
//		case eStateRun:			select_state(eStateLookAround);	break;
//		case eStateLookAround:	select_state(eStateAngry);		break;
//		case eStateAngry:		select_state(eStateWalkAround);	break;
//		case eStateWalkAround:	select_state(eStateWalkAround);	break;
//	}

}

TEMPLATE_SPECIALIZATION
bool CStateMonsterFindEnemyLookAbstract::check_completion()
{	
	if (current_side_id < 3) return false;
	
	//if (time_state_started + 3000 > Level().timeServer()) return false;
	return true;
}

TEMPLATE_SPECIALIZATION
void CStateMonsterFindEnemyLookAbstract::setup_substates()
{
	state_ptr state = get_state_current();

	if (current_substate == eMoveToPoint) {
		
		SStateDataMoveToPoint data;
		data.point			= Fvector().set(0.f,0.f,0.f); // !!!
		data.vertex			= u32(-1);
		data.action			= ACT_WALK_FWD;
		data.accelerated	= true;
		data.braking		= false;
		data.accel_type 	= eAT_Aggressive;

		state->fill_data_with(&data, sizeof(SStateDataMoveToPoint));
		return;
	}

	if (current_substate == eLookAround) {
		SStateDataLookToPoint data;

		data.point	= Fvector().set(0.f,0.f,0.f);
		data.action	= ACT_LOOK_AROUND;

		state->fill_data_with(&data, sizeof(SStateDataLookToPoint));
		return;
	}

	if (current_substate == eTurnAround) {
		SStateDataCustomAction data;
		
		data.action	= ACT_STAND_IDLE;
		
		state->fill_data_with(&data, sizeof(SStateDataCustomAction));
		return;
	}
}


