#include "stdafx.h"
#include "chimera.h"
#include "chimera_state_manager.h"
#include "../ai_monster_debug.h"
#include "../states/monster_state_rest.h"
#include "../states/monster_state_attack.h"
#include "../states/monster_state_panic.h"
#include "../states/monster_state_eat.h"
#include "../states/monster_state_hear_int_sound.h"
#include "../states/monster_state_hear_danger_sound.h"
#include "../states/monster_state_hitted.h"
#include "chimera_state_threaten.h"
#include "../states/state_test_look_actor.h"

#include "../critical_action_info.h"

#include "../states/state_hit_object.h"


CStateManagerChimera::CStateManagerChimera(CChimera *obj) : inherited(obj)
{
	add_state(eStateRest,				xr_new<CStateMonsterRest<CChimera> >					(obj));
	add_state(eStatePanic,				xr_new<CStateMonsterPanic<CChimera> >					(obj));
	add_state(eStateAttack,				xr_new<CStateMonsterAttack<CChimera> >					(obj));
	add_state(eStateEat,				xr_new<CStateMonsterEat<CChimera> >						(obj));
	add_state(eStateInterestingSound,	xr_new<CStateMonsterHearInterestingSound<CChimera> >	(obj));
	add_state(eStateDangerousSound,		xr_new<CStateMonsterHearDangerousSound<CChimera> >		(obj));
	add_state(eStateHitted,				xr_new<CStateMonsterHitted<CChimera> >					(obj));
	add_state(eStateThreaten,			xr_new<CStateChimeraThreaten<CChimera> >				(obj));
	add_state(eStateHitObject,			xr_new<CStateMonsterHitObject<CChimera> >				(obj));
	
}

CStateManagerChimera::~CStateManagerChimera()
{
}

void CStateManagerChimera::execute()
{
	bool exec_hit_obj = false;
	
	//if (prev_substate == eStateHitObject) {
	//	if (!get_state_current()->check_completion())					exec_hit_obj = true;
	//} else if (get_state(eStateHitObject)->check_start_conditions())	exec_hit_obj = true;
	
	if (exec_hit_obj) {
		select_state					(eStateHitObject); 
		get_state_current()->execute	();
		prev_substate					= current_substate;
		return;
	}



	u32 state_id = u32(-1);

	const CEntityAlive* enemy	= object->EnemyMan.get_enemy();
	const CEntityAlive* corpse	= object->CorpseMan.get_corpse();

	if (enemy) {
		//if (get_state(eStateThreaten)->check_start_conditions()) {
		//	state_id = eStateThreaten;
		//} else {
			switch (object->EnemyMan.get_danger_type()) {
				case eVeryStrong:	state_id = eStatePanic; break;
				case eStrong:		
				case eNormal:
				case eWeak:			state_id = eStateAttack; break;
			}
//		}
	} else if (object->HitMemory.is_hit()) {
		state_id = eStateHitted;
	} else if (object->hear_dangerous_sound) {
		state_id = eStateDangerousSound;
	} else if (object->hear_interesting_sound) {
		state_id = eStateInterestingSound;
	} else {
		bool can_eat = false;
		if (corpse) {
			if (prev_substate == eStateEat) {
				if (!get_state_current()->check_completion())				can_eat = true;
			} else {
				if (object->GetSatiety() < object->get_sd()->m_fMinSatiety) can_eat = true;
			}
		}

		if (can_eat)	state_id = eStateEat;
		else			state_id = eStateRest;
	}

	if (state_id == eStateAttack) {
		if (!object->MotionMan.IsCriticalAction()) {
			CObject *target = const_cast<CEntityAlive *>(object->EnemyMan.get_enemy());
			if (object->CJumpingAbility::can_jump(target)) {
				object->try_to_jump();
			}
		}
	}

	if (object->CriticalActionInfo->is_fsm_locked()) return;

	


	select_state(state_id); 

	// выполнить текущее состояние
	get_state_current()->execute();

	prev_substate = current_substate;
}

