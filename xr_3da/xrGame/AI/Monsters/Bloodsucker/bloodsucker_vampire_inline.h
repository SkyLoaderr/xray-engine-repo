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

TEMPLATE_SPECIALIZATION
CStateBloodsuckerVampireAbstract::CStateBloodsuckerVampire(_Object *obj) : inherited(obj)
{
	add_state	(eStateVampire_ApproachEnemy,	xr_new<CStateBloodsuckerVampireApproach<_Object> >	(obj));
	add_state	(eStateVampire_Execute,			xr_new<CStateBloodsuckerVampireExecute<_Object> >	(obj));
	add_state	(eStateVampire_RunAway,			xr_new<CStateMonsterHideFromPoint<_Object> >		(obj));
	//add_state	(eStateVampire_Hide,)
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

	enemy	= object->EnemyMan.get_enemy		();

	object->sound().play						(CAI_Bloodsucker::eVampireStartHunt);
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::execute()
{
	bool state_selected = false;

	if (prev_substate == eStateVampire_RunAway) {
		select_state	(eStateVampire_RunAway);
		state_selected	= true;
	}

	if (!state_selected) {
		if (prev_substate != eStateVampire_Execute) {
			if (get_state(eStateVampire_Execute)->check_start_conditions()) state_selected = true;
		} else {
			if (!get_state_current()->check_completion())			state_selected = true;
		}

		if (state_selected) select_state(eStateVampire_Execute);
	}

	if (!state_selected) {
		if (prev_substate == eStateVampire_Execute) {
			select_state	(eStateVampire_RunAway);
			state_selected	= true;
		}
	}

	if (!state_selected) {
		select_state(eStateVampire_ApproachEnemy);
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
	if (!object->WantVampire()) return false;
	
	// является ли враг актером
	const CEntityAlive *enemy = object->EnemyMan.get_enemy();
	if (enemy->CLS_ID != CLSID_OBJECT_ACTOR)		return false;
	if (!object->EnemyMan.see_enemy_now())			return false;
	if (object->CControlledActor::is_controlling())	return false;

	const CActor *actor = smart_cast<const CActor *>(enemy);
	VERIFY(actor);
	if (actor->input_external_handler_installed()) return false;

	if (m_time_last_vampire + object->m_vampire_min_delay > Device.dwTimeGlobal) return false;

	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateBloodsuckerVampireAbstract::check_completion()
{
	// если убежал
	if ((current_substate == eStateVampire_RunAway) && 
		get_state_current()->check_completion())	return true;

	// если враг изменился
	if (enemy != object->EnemyMan.get_enemy())		return true;
	
	// если актера уже контролит другой кровосос
	if ((current_substate != eStateVampire_Execute) && 
		object->CControlledActor::is_controlling())	return true;

	return false;
}


TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireAbstract::setup_substates()
{
	state_ptr state = get_state_current();

	if (current_substate == eStateVampire_RunAway) {

		SStateHideFromPoint		data;
		data.point				= object->EnemyMan.get_enemy_position();
		data.accelerated		= true;
		data.braking			= false;
		data.accel_type			= eAT_Aggressive;
		data.distance			= RUN_AWAY_DISTANCE;
		data.action.action		= ACT_RUN;
		data.action.sound_type	= MonsterSound::eMonsterSoundAggressive;
		data.action.sound_delay = object->db().m_dwAttackSndDelay;
		data.action.time_out	= 15000;

		state->fill_data_with(&data, sizeof(SStateHideFromPoint));

		return;
	}
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateBloodsuckerVampireAbstract

