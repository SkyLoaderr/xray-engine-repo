#include "stdafx.h"
#include "pseudo_gigant.h"
#include "pseudo_gigant_step_effector.h"
#include "../../../actor.h"
#include "../../../ActorEffector.h"
#include "../../../level.h"
#include "pseudogigant_state_manager.h"
#include "../monster_velocity_space.h"
#include "../control_animation_base.h"
#include "../control_movement_base.h"


CPseudoGigant::CPseudoGigant()
{
	CControlled::init_external(this);

	StateMan = xr_new<CStateManagerGigant>(this);
}

CPseudoGigant::~CPseudoGigant()
{
	xr_delete(StateMan);
}


void CPseudoGigant::Load(LPCSTR section)
{
	inherited::Load	(section);

	anim().AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	anim().AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	anim().accel_load			(section);
	anim().accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	anim().accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

	step_effector.time			= pSettings->r_float(section,	"step_effector_time");
	step_effector.amplitude		= pSettings->r_float(section,	"step_effector_amplitude");
	step_effector.period_number	= pSettings->r_float(section,	"step_effector_period_number");

	SVelocityParam &velocity_none		= move().get_velocity(MonsterMovement::eVelocityParameterIdle);	
	SVelocityParam &velocity_turn		= move().get_velocity(MonsterMovement::eVelocityParameterStand);
	SVelocityParam &velocity_walk		= move().get_velocity(MonsterMovement::eVelocityParameterWalkNormal);
	SVelocityParam &velocity_run		= move().get_velocity(MonsterMovement::eVelocityParameterRunNormal);
	SVelocityParam &velocity_walk_dmg	= move().get_velocity(MonsterMovement::eVelocityParameterWalkDamaged);
	SVelocityParam &velocity_run_dmg	= move().get_velocity(MonsterMovement::eVelocityParameterRunDamaged);
	SVelocityParam &velocity_steal		= move().get_velocity(MonsterMovement::eVelocityParameterSteal);
	//SVelocityParam &velocity_drag		= move().get_velocity(MonsterMovement::eVelocityParameterDrag);

	anim().AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &velocity_none,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &velocity_turn,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &velocity_turn,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimLieIdle,			"stand_sleep_",			-1, &velocity_none,		PS_LIE,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimSitIdle,			"sit_idle_",			-1, &velocity_none,		PS_SIT,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimSleep,			"stand_sleep_",			-1, &velocity_none,		PS_LIE,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &velocity_walk,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimWalkDamaged,		"stand_walk_fwd_dmg_",	-1, &velocity_walk_dmg,	PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimRun,				"stand_run_fwd_",		-1,	&velocity_run,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&velocity_run_dmg,	PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimEat,				"stand_eat_",			-1, &velocity_none,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimAttack,			"stand_attack_",		-1, &velocity_turn,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimLookAround,		"stand_idle_",			-1, &velocity_none,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimSteal,			"stand_steal_",			-1, &velocity_steal,	PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimDie,				"stand_idle_",			-1, &velocity_none,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	anim().AddAnim(eAnimStandLieDown,	"stand_lie_down_",		-1, &velocity_none,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");	
	anim().AddAnim(eAnimLieToSleep,		"lie_to_sleep_",		-1, &velocity_none,		PS_LIE,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

	anim().LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	anim().LinkAction(ACT_SIT_IDLE,		eAnimSitIdle);
	anim().LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
	anim().LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	anim().LinkAction(ACT_WALK_BKWD,		eAnimWalkFwd);
	anim().LinkAction(ACT_RUN,			eAnimRun);
	anim().LinkAction(ACT_EAT,			eAnimEat);
	anim().LinkAction(ACT_SLEEP,			eAnimSleep);
	anim().LinkAction(ACT_REST,			eAnimSleep);
	anim().LinkAction(ACT_DRAG,			eAnimWalkFwd);
	anim().LinkAction(ACT_ATTACK,		eAnimAttack);
	anim().LinkAction(ACT_STEAL,			eAnimSteal);
	anim().LinkAction(ACT_LOOK_AROUND,	eAnimStandIdle);

	// define transitions																											
	anim().AddTransition(eAnimStandLieDown,	eAnimSleep,		eAnimLieToSleep,		false);										
	anim().AddTransition(PS_STAND,			eAnimSleep,		eAnimStandLieDown,		true);
	anim().AddTransition(PS_STAND,			PS_LIE,			eAnimStandLieDown,		false);

#ifdef DEBUG	
	anim().accel_chain_test		();
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
