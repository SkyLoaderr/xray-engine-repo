#pragma once

#include "bloodsucker_vampire_execute.h"
#include "../states/state_hide_from_point.h"
#include "bloodsucker_vampire_approach.h"
#include "../../../clsid_game.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateBloodsuckerVampireAbstract CStateBloodsuckerVampire<_Object>

#define RUN_AWAY_DISTANCE			50.f
#define TIME_VAMPIRE_STATE_DELAY	50000

TEMPLATE_SPECIALIZATION
CStateBloodsuckerVampireAbstract::CStateBloodsuckerVampire(_Object *obj) : inherited(obj)
{
	add_state	(eStateApproachEnemy,	xr_new<CStateBloodsuckerVampireApproach<_Object> >	(obj));
	add_state	(eStateExecute,			xr_new<CStateBloodsuckerVampireExecute<_Object> >	(obj));
	add_state	(eStateRunAway,			xr_new<CStateMonsterHideFromPoint<_Object> >		(obj));
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::reinit()
{
	inherited::reinit	();
	
	m_time_last_vampire	= 0;
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::initialize()
{
	inherited::initialize						();

	object->CInvisibility::set_manual_switch	();
	object->CInvisibility::manual_activate		();
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::execute()
{
	bool state_selected = false;

	if (prev_substate == eStateRunAway) {
		select_state	(eStateRunAway);
		state_selected	= true;
	}

	if (!state_selected) {
		if (prev_substate != eStateExecute) {
			if (get_state(eStateExecute)->check_start_conditions()) state_selected = true;
		} else {
			if (!get_state_current()->check_completion())			state_selected = true;
		}

		if (state_selected) select_state(eStateExecute);
	}

	if (!state_selected) {
		if (prev_substate == eStateExecute) {
			select_state	(eStateRunAway);
			state_selected	= true;
		}
	}

	if (!state_selected) {
		select_state(eStateApproachEnemy);
	}

	get_state_current()->execute();

	prev_substate = current_substate;
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::finalize()
{
	inherited::finalize();

	object->CInvisibility::set_manual_switch	(false);
	m_time_last_vampire							= Device.dwTimeGlobal;
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::critical_finalize()
{
	inherited::critical_finalize();
	
	object->CInvisibility::set_manual_switch	(false);
	m_time_last_vampire							= Device.dwTimeGlobal;
}

TEMPLATE_SPECIALIZATION
bool CStateBloodsuckerVampireAbstract::check_start_conditions()
{
	// является ли враг актером
	const CEntityAlive *enemy = object->EnemyMan.get_enemy();
	if (enemy->CLS_ID != CLSID_OBJECT_ACTOR)		return false;
	if (!object->EnemyMan.see_enemy_now())			return false;
	if (object->CControlledActor::is_controlled())	return false;
	if (m_time_last_vampire + TIME_VAMPIRE_STATE_DELAY > Device.dwTimeGlobal) return false;

	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateBloodsuckerVampireAbstract::check_completion()
{
	if ((current_substate == eStateRunAway) && 
		get_state_current()->check_completion()) return true;

	return false;
}


TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::setup_substates()
{
	state_ptr state = get_state_current();

	if (current_substate == eStateRunAway) {

		SStateHideFromPoint		data;
		data.point				= object->EnemyMan.get_enemy_position();
		data.accelerated		= true;
		data.braking			= false;
		data.accel_type			= eAT_Aggressive;
		data.distance			= RUN_AWAY_DISTANCE;
		data.action.action		= ACT_RUN;
		data.action.sound_type	= MonsterSpace::eMonsterSoundAttack;
		data.action.sound_delay = object->get_sd()->m_dwAttackSndDelay;
		data.action.time_out	= 15000;

		state->fill_data_with(&data, sizeof(SStateHideFromPoint));

#ifdef DEBUG
		if (psAI_Flags.test(aiMonsterDebug)) {
			object->HDebug->M_Add(0,"Vampire :: Run Away", D3DCOLOR_XRGB(255,0,0));
		}
#endif
		return;
	}
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateBloodsuckerVampireAbstract

