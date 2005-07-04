#include "stdafx.h"
#include "cat.h"
#include "cat_state_manager.h"

#include "../control_animation_base.h"
#include "../control_direction_base.h"
#include "../control_movement_base.h"
#include "../control_path_builder_base.h"

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
#include "../../../entitycondition.h"

CStateManagerCat::CStateManagerCat(CCat *obj) : inherited(obj)
{
	add_state(eStateRest,				xr_new<CStateMonsterRest<CCat> >					(obj));
	add_state(eStatePanic,				xr_new<CStateMonsterPanic<CCat> >					(obj));
	add_state(eStateAttack,				xr_new<CStateMonsterAttack<CCat> >					(obj));
	add_state(eStateEat,				xr_new<CStateMonsterEat<CCat> >						(obj));
	add_state(eStateHearInterestingSound,	xr_new<CStateMonsterHearInterestingSound<CCat> >	(obj));
	add_state(eStateHearDangerousSound,		xr_new<CStateMonsterHearDangerousSound<CCat> >		(obj));
	add_state(eStateHitted,				xr_new<CStateMonsterHitted<CCat> >					(obj));
	add_state(eStateAttack_AttackRat,	xr_new<CStateCatAttackRat<CCat> >					(obj));

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
			state_id = eStateAttack_AttackRat;
		} else {
			switch (object->EnemyMan.get_danger_type()) {
				case eStrong:	state_id = eStatePanic; break;
				case eWeak:		state_id = eStateAttack; break;
			}
		}
	} else if (object->HitMemory.is_hit()) {
		state_id = eStateHitted;
	} else if (object->hear_dangerous_sound) {
		state_id = eStateHearDangerousSound;
	} else if (object->hear_interesting_sound) {
		state_id = eStateHearInterestingSound;
	} else {
		bool can_eat = false;
		if (corpse) {
			if (prev_substate == eStateEat) {
				if (!get_state_current()->check_completion())				can_eat = true;
			} else {
				if (object->conditions().GetSatiety() < object->db().m_fMinSatiety) can_eat = true;
			}
		}

		if (can_eat)	state_id = eStateEat;
		else			state_id = eStateRest;
	}

	//if (state_id == eStateAttack_AttackRat) {
	//	if (!object->anim().IsCriticalAction()) {
	//		CObject *target = const_cast<CEntityAlive *>(object->EnemyMan.get_enemy());
	//		if (object->CJumpingAbility::can_jump(target)) {
	//			object->try_to_jump();
	//		}
	//	}
	//}

	
	//if (state_id == eStateAttack_AttackRat) {
	//	// check angle
	//	float yaw, pitch;
	//	Fvector().sub(enemy->Position(), object->Position()).getHP(yaw,pitch);
	//	if (angle_difference(-yaw,object->control().path_builder().m_body.current.yaw) > 2*PI_DIV_3) {
	//		
	//		if (m_rot_jump_last_time + ROTATION_JUMP_DELAY < Device.dwTimeGlobal) {
	//			object->anim().SetSpecParams(ASP_ROTATION_JUMP);
	//			m_rot_jump_last_time = Device.dwTimeGlobal;
	//			return;
	//		}
	//		
	//	}
	//}

	select_state(state_id); 

	// выполнить текущее состояние
	get_state_current()->execute();

	prev_substate = current_substate;
}

