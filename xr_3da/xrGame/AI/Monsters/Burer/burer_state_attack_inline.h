#pragma once

#include "burer_state_attack_tele.h"
#include "burer_state_attack_gravi.h"
#include "burer_state_attack_melee.h"
#include "../states/state_look_point.h"
#include "burer_state_attack_run_around.h"

#define GRAVI_PERCENT		80
#define TELE_PERCENT		50
#define RUN_AROUND_PERCENT	20

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateBurerAttackAbstract CStateBurerAttack<_Object>

TEMPLATE_SPECIALIZATION
CStateBurerAttackAbstract::CStateBurerAttack(_Object *obj) : inherited(obj)
{
	add_state(eStateBurerAttack_Tele,		xr_new<CStateBurerAttackTele<_Object> >		(obj));
	add_state(eStateBurerAttack_Gravi,		xr_new<CStateBurerAttackGravi<_Object> >	(obj));
	add_state(eStateBurerAttack_Melee,		xr_new<CStateBurerAttackMelee<_Object> >	(obj));
	
	add_state(eStateBurerAttack_FaceEnemy,	xr_new<CStateMonsterLookToPoint<_Object> >	(obj));
	add_state(eStateBurerAttack_RunAround,	xr_new<CStateBurerAttackRunAround<_Object> >(obj));
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackAbstract::reselect_state()
{
	if (get_state(eStateBurerAttack_Melee)->check_start_conditions()) {
		select_state(eStateBurerAttack_Melee);
		return;
	}

	bool enable_gravi	= get_state(eStateBurerAttack_Gravi)->check_start_conditions	();
	bool enable_tele	= get_state(eStateBurerAttack_Tele)->check_start_conditions		();

	if (!enable_gravi && !enable_tele) {
		if (prev_substate == eStateBurerAttack_RunAround) 
			select_state(eStateBurerAttack_FaceEnemy);
		else 	
			select_state(eStateBurerAttack_RunAround);
		return;
	}

	if (enable_gravi && enable_tele) {

		u32 rnd_val = ::Random.randI(GRAVI_PERCENT + TELE_PERCENT + RUN_AROUND_PERCENT);
		u32 cur_val = GRAVI_PERCENT;

		if (rnd_val < cur_val) {
			select_state(eStateBurerAttack_Gravi);
			return;
		}

		cur_val += TELE_PERCENT;
		if (rnd_val < cur_val) {
			select_state(eStateBurerAttack_Tele);
			return;
		}

		select_state(eStateBurerAttack_RunAround);
		return;
	}

	if ((prev_substate == eStateBurerAttack_RunAround) || (prev_substate == eStateBurerAttack_FaceEnemy)) {
		if (enable_gravi) select_state(eStateBurerAttack_Gravi);
		else select_state(eStateBurerAttack_Tele);
	} else {
		select_state(eStateBurerAttack_RunAround);
	}
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackAbstract::setup_substates()
{
	state_ptr state = get_state_current();

	if (current_substate == eStateBurerAttack_FaceEnemy) {
		SStateDataLookToPoint data;
		
		data.point				= object->EnemyMan.get_enemy()->Position(); 
		data.action.action		= ACT_STAND_IDLE;
		data.action.sound_type	= MonsterSpace::eMonsterSoundAttack;
		data.action.sound_delay = object->db().m_dwAttackSndDelay;

		state->fill_data_with	(&data, sizeof(SStateDataLookToPoint));
		return;
	}

}

#undef TEMPLATE_SPECIALIZATION
#undef CStateBurerAttackAbstract
