#include "stdafx.h"
#include "cat.h"
#include "cat_state_manager.h"
#include "../../../level.h"
#include "../../../level_debug.h"
#include "../states/monster_state_rest.h"
#include "../states/monster_state_attack.h"
#include "../states/monster_state_panic.h"
#include "../states/monster_state_eat.h"
#include "../states/monster_state_hear_int_sound.h"
#include "../states/monster_state_hear_danger_sound.h"
#include "../states/monster_state_hitted.h"
#include "cat_state_attack_rat.h"
#include "../../../clsid_game.h"
#include "../states/state_test_look_actor.h"
#include "../critical_action_info.h"
#include "../../../entitycondition.h"

CStateManagerCat::CStateManagerCat(CCat *obj) : inherited(obj)
{
	add_state(eStateRest,				xr_new<CStateMonsterRest<CCat> >					(obj));
	add_state(eStatePanic,				xr_new<CStateMonsterPanic<CCat> >					(obj));
	add_state(eStateAttack,				xr_new<CStateMonsterAttack<CCat> >					(obj));
	add_state(eStateEat,				xr_new<CStateMonsterEat<CCat> >						(obj));
	add_state(eStateInterestingSound,	xr_new<CStateMonsterHearInterestingSound<CCat> >	(obj));
	add_state(eStateDangerousSound,		xr_new<CStateMonsterHearDangerousSound<CCat> >		(obj));
	add_state(eStateHitted,				xr_new<CStateMonsterHitted<CCat> >					(obj));
	add_state(eStateAttackRat,			xr_new<CStateCatAttackRat<CCat> >					(obj));

	add_state(eStateThreaten,			xr_new<CStateMonsterLookActor<CCat> >				(obj));


	m_rot_jump_last_time = 0;
}

CStateManagerCat::~CStateManagerCat()
{
}

#define ROTATION_JUMP_DELAY		3000

void CStateManagerCat::execute()
{
	u32 state_id = u32(-1);

	const CEntityAlive* enemy	= object->EnemyMan.get_enemy();
	const CEntityAlive* corpse	= object->CorpseMan.get_corpse();

	if (enemy) {
		if (enemy->CLS_ID == CLSID_AI_RAT) {
			state_id = eStateAttackRat;
		} else {
			switch (object->EnemyMan.get_danger_type()) {
			case eVeryStrong:	state_id = eStatePanic; break;
			case eStrong:		
			case eNormal:
			case eWeak:			state_id = eStateAttack; break;
			}
		}
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
				if (object->conditions().GetSatiety() < object->get_sd()->m_fMinSatiety) can_eat = true;
			}
		}

		if (can_eat)	state_id = eStateEat;
		else			state_id = eStateRest;
	}

	if (state_id == eStateAttackRat) {
		if (!object->MotionMan.IsCriticalAction()) {
			CObject *target = const_cast<CEntityAlive *>(object->EnemyMan.get_enemy());
			if (object->CJumpingAbility::can_jump(target)) {
				object->try_to_jump();
			}
		}
	}

	if (object->CriticalActionInfo->is_fsm_locked()) return;
	
	if (state_id == eStateAttackRat) {
		// check angle
		float yaw, pitch;
		Fvector().sub(enemy->Position(), object->Position()).getHP(yaw,pitch);
		yaw *= -1;	yaw = angle_normalize(yaw);
		if (angle_difference(yaw,object->movement().m_body.current.yaw) > 2*PI_DIV_3) {
			
			if (m_rot_jump_last_time + ROTATION_JUMP_DELAY < Device.dwTimeGlobal) {
				object->MotionMan.SetSpecParams(ASP_ROTATION_JUMP);
				m_rot_jump_last_time = Device.dwTimeGlobal;
				return;
			}
			
		}
	}

	select_state(state_id); 

	// выполнить текущее состояние
	get_state_current()->execute();

	prev_substate = current_substate;
}

