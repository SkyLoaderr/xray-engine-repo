#include "stdafx.h"
#include "cat.h"
#include "cat_state_manager.h"

CCat::CCat()
{
	StateMan = xr_new<CStateManagerCat>(this);
}

CCat::~CCat()
{
	xr_delete(StateMan);
}

void CCat::Load(LPCSTR section)
{
	inherited::Load	(section);

	MotionMan.accel_load			(section);
//	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);
//	MotionMan.accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

//	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
//	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	//*****************************************************************************
	// load shared motion data
	//*****************************************************************************

	if (MotionMan.start_load_shared(SUB_CLS_ID)) {

		MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);

		//MotionMan.AddAnim(eAnimLieIdle,			"lie_sleep_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE);
		//MotionMan.AddAnim(eAnimSleep,			"lie_sleep_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE);

		//MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdNormal,	PS_STAND);
		//MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_fwd_dmg_",	-1, &inherited::get_sd()->m_fsVelocityWalkFwdDamaged,	PS_STAND);
		//MotionMan.AddAnim(eAnimRun,				"stand_run_fwd_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND);
		//MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdDamaged,	PS_STAND);
		//MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		//MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		//MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);

		//MotionMan.AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, &inherited::get_sd()->m_fsVelocityDrag,				PS_STAND);
		//MotionMan.AddAnim(eAnimLookAround,		"stand_idle_",			 2, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		//MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &inherited::get_sd()->m_fsVelocitySteal,			PS_STAND);
		//MotionMan.AddAnim(eAnimDie,				"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		//MotionMan.AddAnim(eAnimThreaten,		"stand_threaten_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);

		// link action
		MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
		MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimStandIdle);
		MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimStandIdle);
		MotionMan.LinkAction(ACT_WALK_FWD,		eAnimStandIdle);
		MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimStandIdle);
		MotionMan.LinkAction(ACT_RUN,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_EAT,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_SLEEP,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_REST,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_DRAG,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_ATTACK,		eAnimStandIdle);
		MotionMan.LinkAction(ACT_STEAL,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimStandIdle);
		MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle); 

		//MotionMan.AA_Load(pSettings->r_string(section, "attack_params"));

		MotionMan.finish_load_shared();
	}

#ifdef DEBUG	
	MotionMan.accel_chain_test		();
#endif

	//*****************************************************************************
}
