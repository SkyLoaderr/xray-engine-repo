#pragma once

#include "state_data.h"
#include "state_move_to_point.h"
#include "state_hide_from_point.h"
#include "state_custom_action.h"

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
void CStateMonsterEatAbstract::reselect_state()
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

#ifdef _DEBUG
	switch (current_substate) {
		case eStateCorpseApproachRun:
			object->HDebug->M_Add(0, "Run", D3DCOLOR_XRGB(0,0,255));
			break;
		case eStateCorpseApproachWalk:
			object->HDebug->M_Add(0, "Walk", D3DCOLOR_XRGB(0,0,255));
			break;
		case eStateCheckCorpse:
			object->HDebug->M_Add(0, "CheckCorpse", D3DCOLOR_XRGB(0,0,255));
			break;
		case eStateEat:
			object->HDebug->M_Add(0, "Eat", D3DCOLOR_XRGB(0,0,255));
			break;
		case eStateWalkAway:
			object->HDebug->M_Add(0, "Walk Away", D3DCOLOR_XRGB(0,0,255));
			break;
		case eStateRest:
			object->HDebug->M_Add(0, "Rest", D3DCOLOR_XRGB(0,0,255));
			break;
	}
#endif


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
		data.completion_dist= ((current_substate == eStateCorpseApproachRun) ? 4.5f : object->get_sd()->m_fDistToCorpse);

		state->fill_data_with(&data, sizeof(SStateDataMoveToPoint));
		return;
	}

	if (current_substate == eStateCheckCorpse) {
		SStateDataCustomAction data;
		data.action			= ACT_STAND_IDLE;
		data.spec_params	= ASP_CHECK_CORPSE;
		data.time_out		= 1500;
		state->fill_data_with(&data, sizeof(SStateDataCustomAction));
		return;
	}
	
	if (current_substate == eStateWalkAway) {
		SStateHideFromPoint data;
		
		data.point			= object->CorpseMan.get_corpse_position();
		data.action			= ACT_WALK_FWD;
		data.distance		= 15.f;	
		data.accelerated	= true;
		data.braking		= true;
		data.accel_type		= eAT_Calm;
		data.cover_min_dist			= 5.f;
		data.cover_max_dist			= 10.f;
		data.cover_search_radius	= 20.f;
		state->fill_data_with(&data, sizeof(SStateHideFromPoint));
		return;
	}

	if (current_substate == eStateRest) {
		SStateDataCustomAction data;
		data.action			= ACT_REST;
		data.spec_params	= 0;
		data.time_out		= 8500;
		state->fill_data_with(&data, sizeof(SStateDataCustomAction));
		return;
	}

}


#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterEatAbstract
