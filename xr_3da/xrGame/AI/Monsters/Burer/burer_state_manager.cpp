#include "stdafx.h"
#include "burer.h"
#include "burer_state_manager.h"

#include "../states/monster_state_rest.h"
#include "../states/monster_state_panic.h"
#include "../states/monster_state_eat.h"
#include "../states/monster_state_hear_int_sound.h"
#include "../states/monster_state_hear_danger_sound.h"
#include "../states/monster_state_hitted.h"
#include "../states/state_custom_action.h"

#include "burer_state_attack.h"


CStateManagerBurer::CStateManagerBurer(CBurer *monster) : inherited(monster)
{
	add_state(eStateRest,					xr_new<CStateMonsterRest<CBurer> >					(monster));
	add_state(eStatePanic,					xr_new<CStateMonsterPanic<CBurer> >					(monster));
	add_state(eStateAttack,					xr_new<CStateBurerAttack<CBurer> >					(monster));
	add_state(eStateEat,					xr_new<CStateMonsterEat<CBurer> >					(monster));
	add_state(eStateHearInterestingSound,	xr_new<CStateMonsterHearInterestingSound<CBurer> >	(monster));
	add_state(eStateHearDangerousSound,		xr_new<CStateMonsterHearDangerousSound<CBurer> >	(monster));
	add_state(eStateHitted,					xr_new<CStateMonsterHitted<CBurer> >				(monster));
	add_state(eStateBurerScanning,			xr_new<CStateMonsterCustomAction<CBurer> >				(monster));
}

#define SCAN_STATE_TIME 4000

void CStateManagerBurer::execute()
{
	u32 state = u32(-1);

	if (object->EnemyMan.get_enemy()) {
		switch (object->EnemyMan.get_danger_type()) {
			case eVeryStrong:				state = eStatePanic; break;
			case eStrong:		
			case eNormal:
			case eWeak:						state = eStateAttack; break;
		}
	} else if (object->HitMemory.is_hit() && (object->HitMemory.get_last_hit_time() + 10000 > object->m_current_update)) 
		state = eStateHitted;
	else if (object->hear_dangerous_sound || object->hear_interesting_sound) {
		state = eStateHearInterestingSound;
	} else if (object->time_last_scan + SCAN_STATE_TIME > object->m_current_update){
		state = eStateBurerScanning;
	} else if (can_eat()) {
			state = eStateEat;
	} else	state = eStateRest;

	state = eStateBurerScanning;
	
	select_state(state); 
	
	// ��������� ������� ���������
	get_state_current()->execute();

	prev_substate = current_substate;
}

void CStateManagerBurer::setup_substates()
{
	if (current_substate == eStateBurerScanning) {
		SStateDataAction	data;
		
		data.action			= ACT_LOOK_AROUND;
		data.sound_type		= MonsterSpace::eMonsterSoundIdle;
		data.sound_delay	= object->get_sd()->m_dwIdleSndDelay;
		
		get_state_current()->fill_data_with(&data, sizeof(SStateDataAction));
		return;
	}
}
