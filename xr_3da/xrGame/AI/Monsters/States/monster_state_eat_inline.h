#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterEatAbstract CStateMonsterEat<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterEatAbstract::CStateMonsterEat(_Object *obj, state_ptr state_eat) : inherited(obj)
{
	add_state	(eStateCorpseApproachRun,	xr_new<CStateMonsterMoveToPoint<_Object> >(obj));
	add_state	(eStateCorpseApproachWalk,	xr_new<CStateMonsterMoveToPoint<_Object> >(obj));
	add_state	(eStateCheckCorpse,			xr_new<CStateMonsterCustomAction<_Object> >(obj));
	add_state	(eStateEat,					state_eat);
	add_state	(eStateWalkAway,			xr_new<CStateMonsterHideFromPoint<_Object> >(obj));
	add_state	(eStateRest,				xr_new<CStateMonsterCustomAction<_Object> >(obj));
}

TEMPLATE_SPECIALIZATION
CStateMonsterEatAbstract::~CStateMonsterEat()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterEatAbstract::initialize()
{
	inherited::initialize();
}


TEMPLATE_SPECIALIZATION
void CStateMonsterEatAbstract::reselect()
{
	if (prev_substate == u32(-1)) {
		select_state(eStateCorpseApproachRun);
		return;
	}
	
	(prev_substate != eStateRest) ?  select_state(prev_substate+1) : select_state(eStateRest);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterEatAbstract::setup_substates()
{
	state_ptr state = get_state_current();

	if ((current_substate == eStateCorpseApproachRun) || (current_substate == eStateCorpseApproachWalk)) {
		
		// Определить позицию ближайшей боны у трупа
		Fvector nearest_bone_pos;
		const CEntityAlive *corpse = object->CorpseMan.get_corpse();
		if ((corpse->m_pPhysicsShell == NULL) || (!corpse->m_pPhysicsShell->bActive)) {
			nearest_bone_pos	= corpse->Position(); 
		} else nearest_bone_pos = object->m_PhysicMovementControl->PHCaptureGetNearestElemPos(corpse);
		
		
		SStateDataMoveToPoint data;
		data.point			= nearest_bone_pos;
		data.vertex			= u32(-1);
		data.action			= ((current_substate == eStateCorpseApproachRun) ? ACT_RUN : ACT_WALK_FWD);
		data.accelerated	= true;
		data.braking		= true;
		data.accel_type 	= eAT_Calm;

		state->fill_data_with(&data, sizeof(SStateDataMoveToPoint));
		return;
	}

	if (current_substate == eStateCheckCorpse) {
		SStateDataCustomAction data;
		data.action			= ACT_STAND_IDLE;
		data.spec_params	= ASP_CHECK_CORPSE;
		data.time_out		= 2500;
		state->fill_data_with(&data, sizeof(SStateDataCustomAction));
		return;
	}
	
	if (current_substate == eStateWalkAway) {
		SStateHideFromPoint data;
		
		point			= object->CorpseMan.get_corpse_position();
		action			= ACT_WALK_FWD;
		distance		= 10.f;	
		accelerated		= true;
		braking			= true;
		accel_type		= eAT_Calm;
				
		state->fill_data_with(&data, sizeof(SStateHideFromPoint));
	}

	if (current_substate == eStateRest) {
		SStateDataCustomAction data;
		data.action			= ACT_REST;
		data.spec_params	= 0;
		data.time_out		= 2500;
		state->fill_data_with(&data, sizeof(SStateDataCustomAction));
	}


//#ifdef DEBUG
//	switch (current_substate) {
//		case eStateCorpseApproachRun:
//			break;
//		case eStateCorpseApproachWalk:
//			break;
//		case eStateCheckCorpse:
//			break;
//		case eStatePrepareDrag:
//			break;
//
//	}
//	
//	object->HDebug->M_Add(0, "Turn To Point", D3DCOLOR_XRGB(255,0,0));
//#endif


}


#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterEatAbstract
