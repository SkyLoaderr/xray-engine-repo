#include "stdafx.h"
#include "ai_pseudodog.h"
#include "../ai_monster_utils.h"

CAI_PseudoDog::CAI_PseudoDog()
{
	stateRest			= xr_new<CBitingRest>		(this);
	stateAttack			= xr_new<CBitingAttack>		(this, false);
	stateEat			= xr_new<CBitingEat>		(this, true);
	stateHide			= xr_new<CBitingHide>		(this);
	stateDetour			= xr_new<CBitingDetour>		(this);
	statePanic			= xr_new<CBitingPanic>		(this, false);
	stateExploreNDE		= xr_new<CBitingExploreNDE>	(this);
	stateExploreDNE		= xr_new<CBitingExploreDNE>	(this, false);

	CurrentState		= stateRest;

	Init();
}

CAI_PseudoDog::~CAI_PseudoDog()
{
	xr_delete(stateRest);
	xr_delete(stateAttack);
	xr_delete(stateEat);
	xr_delete(stateHide);
	xr_delete(stateDetour);
	xr_delete(statePanic);
	xr_delete(stateExploreNDE);
	xr_delete(stateExploreDNE);

}


void CAI_PseudoDog::Init()
{
	inherited::Init();

	CurrentState					= stateRest;
	CurrentState->Reset				();

	CJumping::Init(this);

	strike_in_jump					= false;
}

void CAI_PseudoDog::Load(LPCSTR section)
{
	inherited::Load	(section);
	CJumping::Load	(section);


	CJumping::AddState(PSkeletonAnimated(Visual())->ID_Cycle_Safe("jump_glide_0"), JT_GLIDE,	false,	0.f, inherited::_sd->m_fsVelocityRunFwdNormal.velocity.angular);

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	BEGIN_LOAD_SHARED_MOTION_DATA();

	// define animation set
	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND);
	MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimSleep,			"lie_sleep_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_LIE);
	MotionMan.AddAnim(eAnimLieIdle,			"lie_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_LIE);
	MotionMan.AddAnim(eAnimSitIdle,			"sit_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_SIT);
	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND);
	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::_sd->m_fsVelocityWalkFwdNormal,		PS_STAND);
	MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_dmg_",		-1, &inherited::_sd->m_fsVelocityWalkFwdDamaged,	PS_STAND);
	MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	&inherited::_sd->m_fsVelocityRunFwdNormal,		PS_STAND);
	MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1, &inherited::_sd->m_fsVelocityRunFwdDamaged,		PS_STAND);
	MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&inherited::_sd->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, &inherited::_sd->m_fsVelocityDrag,				PS_STAND);
	MotionMan.AddAnim(eAnimSniff,			"stand_sniff_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimHowling,			"stand_howling_",		-1,	&inherited::_sd->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimJumpGlide,		"jump_glide_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &inherited::_sd->m_fsVelocitySteal,				PS_STAND);
	MotionMan.AddAnim(eAnimDie,				"stand_die_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimSitLieDown,		"sit_lie_down_",		-1, &inherited::_sd->m_fsVelocityNone,				PS_SIT);
	MotionMan.AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND);	
	MotionMan.AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, &inherited::_sd->m_fsVelocityNone,				PS_SIT);
	MotionMan.AddAnim(eAnimLieToSleep,		"lie_to_sleep_",		-1,	&inherited::_sd->m_fsVelocityNone,				PS_LIE);
	MotionMan.AddAnim(eAnimSleepStandUp,	"lie_to_stand_up_",		-1, &inherited::_sd->m_fsVelocityNone,				PS_LIE);

	// define transitions
	// order : 1. [anim -> anim]	2. [anim->state]	3. [state -> anim]		4. [state -> state]
	MotionMan.AddTransition(eAnimLieIdle,	eAnimSleep,	eAnimLieToSleep,	false);
	MotionMan.AddTransition(eAnimSleep,		PS_STAND,	eAnimSleepStandUp,	false);
	MotionMan.AddTransition(PS_SIT,		PS_LIE,		eAnimSitLieDown,		false);
	MotionMan.AddTransition(PS_STAND,	PS_SIT,		eAnimStandSitDown,		false);
	MotionMan.AddTransition(PS_SIT,		PS_STAND,	eAnimSitStandUp,		false);

	// define links from Action to animations
	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle,	eAnimStandTurnLeft,	eAnimStandTurnRight, PI_DIV_6);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimSitIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkBkwd);
	MotionMan.LinkAction(ACT_RUN,			eAnimRun);
	MotionMan.LinkAction(ACT_EAT,			eAnimEat);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
	MotionMan.LinkAction(ACT_REST,			eAnimSitIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimDragCorpse);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack, eAnimStandTurnLeft,	eAnimStandTurnRight, PI_DIV_6);
	MotionMan.LinkAction(ACT_STEAL,			eAnimWalkFwd);	
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimSniff);
	MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle,	eAnimStandTurnLeft, eAnimStandTurnRight, EPS_S); 

	MotionMan.AA_PushAttackAnimTest(eAnimAttack, 0, 500, 800, STANDART_ATTACK, inherited::_sd->m_fHitPower,Fvector().set(0.f,0.f,3.f));

	END_LOAD_SHARED_MOTION_DATA();

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	MotionMan.accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

}

void CAI_PseudoDog::StateSelector()
{	
	VisionElem ve;

	if (C)						SetState(statePanic);
	else if (D || E || F)		SetState(stateAttack);
	else if (A && !K)			SetState(stateExploreNDE);		//SetState(stateExploreDNE);	//SetState(stateExploreDE);	// слышу опасный звук, но не вижу, враг выгодный			(ExploreDE)		
	else if (B && !K)			SetState(stateExploreNDE);	// слышу не опасный звук, но не вижу, враг выгодный		(ExploreNDE)
	else if (GetCorpse(ve) && (ve.obj->m_fFood > 1) && ((GetSatiety() < _sd->m_fMinSatiety) || flagEatNow))	
		SetState(stateEat);
	else						SetState(stateRest); 

}

void CAI_PseudoDog::UpdateCL()
{
	inherited::UpdateCL();
	CJumping::Update();

	float trace_dist = 1.0f;

	// Проверить на нанесение хита во время прыжка
	if (CJumping::IsGlide()) {

		if (strike_in_jump) return;

		const CEntity *pE = dynamic_cast<const CEntity *>(CJumping::GetEnemy());
		if (!pE) return;

		Fvector trace_from;
		Center(trace_from);
		setEnabled(false);
		Collide::rq_result	l_rq;

		if (Level().ObjectSpace.RayPick(trace_from, Direction(), trace_dist , Collide::rqtBoth, l_rq)) {
			if ((l_rq.O == CJumping::GetEnemy()) && (l_rq.range < trace_dist)) {
				HitEntity(pE, inherited::_sd->m_fHitPower,Direction());
				strike_in_jump = true;
			}
		}
		setEnabled(true);			
	}

}
void CAI_PseudoDog::OnJumpStop()
{
	//MotionMan.ProcessAction();
}


void CAI_PseudoDog::ProcessTurn()
{
	float delta_yaw = angle_difference(m_body.target.yaw, m_body.current.yaw);
	if (delta_yaw < deg(1)) {
		//m_body.current.yaw = m_body.target.yaw;
		return;
	}

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



