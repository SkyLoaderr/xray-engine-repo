#include "stdafx.h"
#include "snork.h"
#include "snork_state_manager.h"

CSnork::CSnork()
{
	StateMan = xr_new<CStateManagerSnork>(this);
}

CSnork::~CSnork()
{
	xr_delete		(StateMan);
}

void CSnork::Load(LPCSTR section)
{
	inherited::Load	(section);

	MotionMan.accel_load			(section);

	if (MotionMan.start_load_shared(SUB_CLS_ID)) {
		MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
		MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	&inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND);
		MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
		MotionMan.AddAnim(eAnimDie,				"stand_die_",			0,  &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);

		MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
		MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimStandIdle);
		MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimStandIdle);
		MotionMan.LinkAction(ACT_WALK_FWD,		eAnimRun);
		MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimRun);
		MotionMan.LinkAction(ACT_RUN,			eAnimRun);
		MotionMan.LinkAction(ACT_EAT,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_SLEEP,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_REST,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_DRAG,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack);
		MotionMan.LinkAction(ACT_STEAL,			eAnimRun);
		MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimStandIdle);
		MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle); 

		MotionMan.finish_load_shared();
	}

#ifdef DEBUG	
	MotionMan.accel_chain_test		();
#endif

}
