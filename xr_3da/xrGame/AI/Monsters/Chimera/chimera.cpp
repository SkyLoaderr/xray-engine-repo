#include "stdafx.h"
#include "chimera.h"
#include "../../ai_monster_utils.h"
#include "chimera_state_manager.h"
#include "../../ai_monster_debug.h"

CChimera::CChimera()
{
	init();
	
	StateMan = xr_new<CStateManagerChimera>(this);
}

CChimera::~CChimera()
{
	xr_delete(StateMan);
}


void CChimera::init()
{
}

void CChimera::reinit()
{
	inherited::reinit();

	b_upper_state					= false;
}

void CChimera::Load(LPCSTR section)
{
	inherited::Load	(section);

	MotionMan.AddReplacedAnim(&b_upper_state, eAnimAttack,			eAnimUpperAttack);
	MotionMan.AddReplacedAnim(&b_upper_state, eAnimWalkFwd,			eAnimUpperWalkFwd);
	MotionMan.AddReplacedAnim(&b_upper_state, eAnimStandTurnLeft,	eAnimUpperStandTurnLeft);
	MotionMan.AddReplacedAnim(&b_upper_state, eAnimStandTurnRight,	eAnimUpperStandTurnRight);
	MotionMan.AddReplacedAnim(&b_upper_state, eAnimThreaten,		eAnimUpperThreaten);

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	MotionMan.accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

	//*****************************************************************************
	// load shared motion data
	//*****************************************************************************

	if (!MotionMan.start_load_shared(SUB_CLS_ID)) return;

	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND);

	MotionMan.AddAnim(eAnimLieIdle,			"lie_sleep_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE);
	MotionMan.AddAnim(eAnimSleep,			"lie_sleep_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE);

	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdNormal,		PS_STAND);
	MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_fwd_dmg_",	-1, &inherited::get_sd()->m_fsVelocityWalkFwdDamaged,	PS_STAND);
	MotionMan.AddAnim(eAnimRun,				"stand_run_fwd_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND);
	MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdDamaged,		PS_STAND);
	MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND);

	MotionMan.AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, &inherited::get_sd()->m_fsVelocityDrag,				PS_STAND);
	MotionMan.AddAnim(eAnimLookAround,		"stand_idle_",			 2, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &inherited::get_sd()->m_fsVelocitySteal,				PS_STAND);
	MotionMan.AddAnim(eAnimDie,				"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimThreaten,		"stand_threaten_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);

	//////////////////////////////////////////////////////////////////////////

	MotionMan.AddAnim(eAnimUpperStandIdle,		"stand_up_idle_",		-1, &inherited::get_sd()->m_fsVelocityDrag,			PS_STAND_UPPER);
	MotionMan.AddAnim(eAnimUpperStandTurnLeft,	"stand_up_turn_ls_",	-1, &inherited::get_sd()->m_fsVelocityNone,			PS_STAND_UPPER);
	MotionMan.AddAnim(eAnimUpperStandTurnRight,	"stand_up_turn_rs_",	-1, &inherited::get_sd()->m_fsVelocitySteal,			PS_STAND_UPPER);

	MotionMan.AddAnim(eAnimStandToUpperStand,	"stand_upper_",			-1, &inherited::get_sd()->m_fsVelocityNone,			PS_STAND);
	MotionMan.AddAnim(eAnimUppperStandToStand,	"stand_up_to_down_",	-1, &inherited::get_sd()->m_fsVelocityNone,			PS_STAND_UPPER);

	MotionMan.AddAnim(eAnimUpperWalkFwd,		"stand_up_walk_fwd_",	-1, &inherited::get_sd()->m_fsVelocityDrag,			PS_STAND_UPPER);
	MotionMan.AddAnim(eAnimUpperThreaten,		"stand_up_threaten_",	-1, &inherited::get_sd()->m_fsVelocityNone,			PS_STAND_UPPER);
	MotionMan.AddAnim(eAnimUpperAttack,			"stand_up_attack_",		-1, &inherited::get_sd()->m_fsVelocitySteal,			PS_STAND_UPPER);

	//////////////////////////////////////////////////////////////////////////
	// define transitions
	MotionMan.AddTransition(PS_STAND,			PS_STAND_UPPER,		eAnimStandToUpperStand,		false);
	MotionMan.AddTransition(PS_STAND_UPPER,		PS_STAND,			eAnimUppperStandToStand,	false);

	// link action
	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimDragCorpse);
	MotionMan.LinkAction(ACT_RUN,			eAnimRun);
	MotionMan.LinkAction(ACT_EAT,			eAnimEat);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
	MotionMan.LinkAction(ACT_REST,			eAnimLieIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimDragCorpse);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack);
	MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimLookAround);
	MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle); 

	MotionMan.finish_load_shared();
	
	//*****************************************************************************
}

void CChimera::StateSelector()
{	
}

void CChimera::ProcessTurn()
{
	float delta_yaw = angle_difference(m_body.target.yaw, m_body.current.yaw);
	if (delta_yaw < deg(1)) return;
	
	EMotionAnim anim = MotionMan.GetCurAnim();
	
	bool turn_left = true;
	if (from_right(m_body.target.yaw, m_body.current.yaw)) turn_left = false; 
 
	switch (anim) {
		case eAnimStandIdle: 
			(turn_left) ? MotionMan.SetCurAnim(eAnimStandTurnLeft) : MotionMan.SetCurAnim(eAnimStandTurnRight);
			return;
		default:
			if (delta_yaw > deg(30)) {
				(turn_left) ? MotionMan.SetCurAnim(eAnimStandTurnLeft) : MotionMan.SetCurAnim(eAnimStandTurnRight);
			}
			return;
	}

}

bool CChimera::UpdateStateManager()
{
	StateMan->execute();
	return true;
}

void CChimera::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_THREATEN) == ASP_THREATEN) {
		MotionMan.SetCurAnim(eAnimUpperThreaten);
	}
}
