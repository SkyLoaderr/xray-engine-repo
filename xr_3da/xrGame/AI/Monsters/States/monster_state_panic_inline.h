#pragma once

#include "state_data.h"
#include "state_move_to_point.h"
#include "state_custom_action.h"
#include "state_look_unprotected_area.h"
#include "monster_state_panic_run.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterPanicAbstract CStateMonsterPanic<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterPanicAbstract::CStateMonsterPanic(_Object *obj) : inherited(obj)
{
	add_state(eStateRun,					xr_new<CStateMonsterPanicRun<_Object> >(obj));
	add_state(eStateFaceUnprotectedArea,	xr_new<CStateMonsterLookToUnprotectedArea<_Object> >(obj));
}

TEMPLATE_SPECIALIZATION
CStateMonsterPanicAbstract::~CStateMonsterPanic()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterPanicAbstract::initialize()
{
	inherited::initialize();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterPanicAbstract::reselect_state()
{
	if (prev_substate == u32(-1)) {
		select_state(eStateRun);
	}
	
	if (prev_substate == eStateRun) select_state(eStateFaceUnprotectedArea);
	else select_state(eStateRun);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterPanicAbstract::setup_substates()
{
	state_ptr state = get_state_current();

	if (current_substate == eStateFaceUnprotectedArea) {
		SStateDataAction data;
		
		data.action			= ACT_STAND_IDLE;
		data.spec_params	= ASP_STAND_SCARED;
		data.time_out		= 3000;		
		data.sound_type		= MonsterSpace::eMonsterSoundPanic;
		data.sound_delay	= object->get_sd()->m_dwAttackSndDelay;

		state->fill_data_with(&data, sizeof(SStateDataAction));

#ifdef DEBUG
		if (psAI_Flags.test(aiMonsterDebug)) {
			object->HDebug->M_Add(0,"Panic :: Face Unprotected Area", D3DCOLOR_XRGB(255,0,0));
		}
#endif

		return;
	}

}

TEMPLATE_SPECIALIZATION
void CStateMonsterPanicAbstract::check_force_state()
{
	if ((current_substate == eStateFaceUnprotectedArea)){
		// если видит врага
		if (object->EnemyMan.get_enemy_time_last_seen() == object->m_current_update) {
			select_state(eStateRun);
			return;
		}
		// если получил hit
		if (object->HitMemory.get_last_hit_time() + 5000 > object->m_current_update) {
			select_state(eStateRun);
			return;
		}
	}
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterPanicAbstract
