#pragma once

#include "state_data.h"
#include "state_move_to_point.h"
#include "state_hide_from_point.h"
#include "state_custom_action.h"
#include "monster_state_eat_eat.h"
#include "monster_state_eat_drag.h"
#include "../../../PhysicsShell.h"
#include "../../../PHMovementControl.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterEatAbstract CStateMonsterEat<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterEatAbstract::CStateMonsterEat(_Object *obj) : inherited(obj)
{
	add_state	(eStateEat_CorpseApproachRun,	xr_new<CStateMonsterMoveToPoint<_Object> >(obj));
	add_state	(eStateEat_CorpseApproachWalk,	xr_new<CStateMonsterMoveToPoint<_Object> >(obj));
	add_state	(eStateEat_CheckCorpse,			xr_new<CStateMonsterCustomAction<_Object> >(obj));
	add_state	(eStateEat_Eat,					xr_new<CStateMonsterEating<_Object> >(obj));
	add_state	(eStateEat_WalkAway,			xr_new<CStateMonsterHideFromPoint<_Object> >(obj));
	add_state	(eStateEat_Rest,				xr_new<CStateMonsterCustomAction<_Object> >(obj));
	add_state	(eStateEat_Drag,				xr_new<CStateMonsterDrag<_Object> >(obj));
}

TEMPLATE_SPECIALIZATION
CStateMonsterEatAbstract::CStateMonsterEat(_Object *obj, state_ptr state_eat) : inherited(obj)
{
	add_state	(eStateEat_CorpseApproachRun,	xr_new<CStateMonsterMoveToPoint<_Object> >(obj));
	add_state	(eStateEat_CorpseApproachWalk,	xr_new<CStateMonsterMoveToPoint<_Object> >(obj));
	add_state	(eStateEat_CheckCorpse,			xr_new<CStateMonsterCustomAction<_Object> >(obj));
	add_state	(eStateEat_Eat,					state_eat);
	add_state	(eStateEat_WalkAway,			xr_new<CStateMonsterHideFromPoint<_Object> >(obj));
	add_state	(eStateEat_Rest,				xr_new<CStateMonsterCustomAction<_Object> >(obj));
	add_state	(eStateEat_Drag,				xr_new<CStateMonsterDrag<_Object> >(obj));
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
	if (prev_substate == u32(-1)) {select_state(eStateEat_CorpseApproachRun);return;}
	if (prev_substate == eStateEat_CorpseApproachRun) { select_state(eStateEat_CorpseApproachWalk); return; }
	if (prev_substate == eStateEat_CorpseApproachWalk) { select_state(eStateEat_CheckCorpse); return; }
	if (prev_substate == eStateEat_CheckCorpse) { 
		if (object->ability_can_drag()) select_state(eStateEat_Drag);
		else							select_state(eStateEat_Eat);
		return; 
	}
	
	if (prev_substate == eStateEat_Drag)		{ select_state(eStateEat_Eat);		return; }
	if (prev_substate == eStateEat_Eat)			{ select_state(eStateEat_WalkAway); return; }
	if (prev_substate == eStateEat_WalkAway)	{ select_state(eStateEat_Rest);		return; }
	if (prev_substate == eStateEat_Rest)		{ select_state(eStateEat_Rest);		return; }
}

TEMPLATE_SPECIALIZATION
void CStateMonsterEatAbstract::setup_substates()
{
	state_ptr state = get_state_current();
	if ((current_substate == eStateEat_CorpseApproachRun) || (current_substate == eStateEat_CorpseApproachWalk)) {
		
		// ���������� ������� ��������� ���� � �����
		Fvector nearest_bone_pos;
		const CEntityAlive *corpse = object->CorpseMan.get_corpse();
		if ((corpse->m_pPhysicsShell == NULL) || (!corpse->m_pPhysicsShell->bActive)) {
			nearest_bone_pos	= corpse->Position(); 
		} else nearest_bone_pos = object->m_PhysicMovementControl->PHCaptureGetNearestElemPos(corpse);
		
		
		SStateDataMoveToPoint data;
		data.point			= nearest_bone_pos;
		data.vertex			= u32(-1);
		data.action.action	= ((current_substate == eStateEat_CorpseApproachRun) ? ACT_RUN : ACT_WALK_FWD);
		data.accelerated	= true;
		data.braking		= true;
		data.accel_type 	= eAT_Calm;
		data.completion_dist= ((current_substate == eStateEat_CorpseApproachRun) ? 4.5f : object->get_sd()->m_fDistToCorpse);
		data.action.sound_type	= MonsterSpace::eMonsterSoundIdle;
		data.action.sound_delay = object->get_sd()->m_dwIdleSndDelay;

		state->fill_data_with(&data, sizeof(SStateDataMoveToPoint));
		return;
	}

	if (current_substate == eStateEat_CheckCorpse) {
		SStateDataAction data;
		data.action			= ACT_STAND_IDLE;
		data.spec_params	= ASP_CHECK_CORPSE;
		data.time_out		= 1500;
		data.sound_type	= MonsterSpace::eMonsterSoundEat;
		data.sound_delay = object->get_sd()->m_dwEatSndDelay;

		state->fill_data_with(&data, sizeof(SStateDataAction));

		return;
	}
	
	if (current_substate == eStateEat_WalkAway) {
		SStateHideFromPoint data;
		
		data.point					= object->CorpseMan.get_corpse_position();
		data.action.action			= ACT_WALK_FWD;
		data.distance				= 15.f;	
		data.accelerated			= true;
		data.braking				= true;
		data.accel_type				= eAT_Calm;
		data.cover_min_dist			= 20.f;
		data.cover_max_dist			= 30.f;
		data.cover_search_radius	= 25.f;
		data.action.sound_type	= MonsterSpace::eMonsterSoundIdle;
		data.action.sound_delay = object->get_sd()->m_dwIdleSndDelay;

		state->fill_data_with(&data, sizeof(SStateHideFromPoint));

		return;
	}

	if (current_substate == eStateEat_Rest) {
		SStateDataAction data;
		data.action			= ACT_REST;
		data.spec_params	= 0;
		data.time_out		= 8500;
		data.sound_type	= MonsterSpace::eMonsterSoundIdle;
		data.sound_delay = object->get_sd()->m_dwIdleSndDelay;

		state->fill_data_with(&data, sizeof(SStateDataAction));
		return;
	}

}

TEMPLATE_SPECIALIZATION
bool CStateMonsterEatAbstract::check_completion()
{
	if (object->CorpseMan.get_corpse()) return false;
	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterEatAbstract::check_start_conditions()
{
	return (object->conditions().GetSatiety() < object->get_sd()->m_fMinSatiety);
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterEatAbstract
