#include "stdafx.h"
#include "pseudo_gigant.h"
#include "../ai_monster_utils.h"
#include "pseudo_gigant_step_effector.h"
#include "../../../actor.h"
#include "../../../ActorEffector.h"
#include "pseudogigant_state_manager.h"

CPseudoGigant::CPseudoGigant()
{
	controlled::init_external(this);

	StateMan = xr_new<CStateManagerGigant>(this);
}

CPseudoGigant::~CPseudoGigant()
{
	xr_delete(StateMan);
}


void CPseudoGigant::Load(LPCSTR section)
{
	inherited::Load	(section);

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	MotionMan.accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

	step_effector.time			= pSettings->r_float(section,	"step_effector_time");
	step_effector.amplitude		= pSettings->r_float(section,	"step_effector_amplitude");
	step_effector.period_number	= pSettings->r_float(section,	"step_effector_period_number");

	if (MotionMan.start_load_shared(SUB_CLS_ID)) {

		MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimLieIdle,			"stand_sleep_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimSitIdle,			"sit_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_SIT,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimSleep,			"stand_sleep_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdNormal,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_fwd_dmg_",	-1, &inherited::get_sd()->m_fsVelocityWalkFwdDamaged,	PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimRun,				"stand_run_fwd_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdDamaged,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,			PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimLookAround,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &inherited::get_sd()->m_fsVelocitySteal,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimDie,				"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
		MotionMan.AddAnim(eAnimStandLieDown,	"stand_lie_down_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");	
		MotionMan.AddAnim(eAnimLieToSleep,		"lie_to_sleep_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

		MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
		MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimLieIdle);
		MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimSitIdle);
		MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
		MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkFwd);
		MotionMan.LinkAction(ACT_RUN,			eAnimRun);
		MotionMan.LinkAction(ACT_EAT,			eAnimEat);
		MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
		MotionMan.LinkAction(ACT_REST,			eAnimSleep);
		MotionMan.LinkAction(ACT_DRAG,			eAnimWalkFwd);
		MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack);
		MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);
		MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimStandIdle);
		MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle); 

		// define transitions																											
		MotionMan.AddTransition(eAnimStandLieDown,	eAnimSleep,		eAnimLieToSleep,		false);										
		MotionMan.AddTransition(PS_STAND,			eAnimSleep,		eAnimStandLieDown,		true);
		MotionMan.AddTransition(PS_STAND,			PS_LIE,			eAnimStandLieDown,		false);

		MotionMan.AA_Load(pSettings->r_string(section, "attack_params"));
		//MotionMan.STEPS_Load(pSettings->r_string(section, "step_params"), get_legs_number());

		MotionMan.finish_load_shared();
	}

#ifdef DEBUG	
	MotionMan.accel_chain_test		();
#endif

}

#define MAX_STEP_RADIUS 60.f

void CPseudoGigant::event_on_step()
{
	//////////////////////////////////////////////////////////////////////////
	// Earthquake Effector	//////////////
	CActor* pActor =  smart_cast<CActor*>(Level().CurrentEntity());
	if(pActor)
	{
		float dist_to_actor = pActor->Position().distance_to(Position());
		float max_dist		= MAX_STEP_RADIUS;
		if (dist_to_actor < max_dist) 
			pActor->EffectorManager().AddEffector(xr_new<CPseudogigantStepEffector>(
				step_effector.time, 
				step_effector.amplitude, 
				step_effector.period_number, 
				(max_dist - dist_to_actor) / (1.2f * max_dist))
			);
	}
	//////////////////////////////////
}
