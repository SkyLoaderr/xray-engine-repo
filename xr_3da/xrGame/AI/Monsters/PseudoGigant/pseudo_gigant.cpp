#include "stdafx.h"
#include "pseudo_gigant.h"
#include "../../ai_monster_utils.h"

CPseudoGigant::CPseudoGigant()
{
	stateRest			= xr_new<CBitingRest>(this);
	stateAttack			= xr_new<CBitingAttack>		(this, false);
	stateEat			= xr_new<CBitingEat>		(this, true);
	stateHide			= xr_new<CBitingHide>		(this);
	stateDetour			= xr_new<CBitingDetour>		(this);
	statePanic			= xr_new<CBitingPanic>		(this, false);
	stateExploreNDE		= xr_new<CBitingExploreNDE>	(this);
	stateExploreDNE		= xr_new<CBitingExploreDNE>	(this, false);
	stateNull			= xr_new<CBitingNull>		();

	CurrentState		= stateRest;

	Init();
}

CPseudoGigant::~CPseudoGigant()
{
	xr_delete(stateRest);
	xr_delete(stateAttack);
	xr_delete(stateEat);
	xr_delete(stateHide);
	xr_delete(stateDetour);
	xr_delete(statePanic);
	xr_delete(stateExploreNDE);
	xr_delete(stateNull);
}


void CPseudoGigant::Init()
{
	inherited::Init();

	CurrentState					= stateRest;
	CurrentState->Reset				();
}

void CPseudoGigant::Load(LPCSTR section)
{
	inherited::Load	(section);

	BEGIN_LOAD_SHARED_MOTION_DATA();

	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND);

	MotionMan.AddAnim(eAnimLieIdle,			"stand_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_LIE);
	MotionMan.AddAnim(eAnimSleep,			"stand_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_LIE);

	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::_sd->m_fsVelocityWalkFwdNormal,		PS_STAND);
	MotionMan.AddAnim(eAnimRun,				"stand_run_fwd_",		-1,	&inherited::_sd->m_fsVelocityRunFwdNormal,		PS_STAND);
	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND);

	MotionMan.AddAnim(eAnimLookAround,		"stand_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimDie,				"stand_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND);

	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_RUN,			eAnimRun);
	MotionMan.LinkAction(ACT_EAT,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
	MotionMan.LinkAction(ACT_REST,			eAnimLieIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack);
	MotionMan.LinkAction(ACT_STEAL,			eAnimWalkFwd);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimLookAround);
	MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle); 

	MotionMan.AA_PushAttackAnimTest(eAnimAttack,	0, 500,	600,	STANDART_ATTACK, inherited::_sd->m_fHitPower,Fvector().set(0.f,0.f,1.f));
	MotionMan.AA_PushAttackAnimTest(eAnimAttack,	1, 500,	600,	STANDART_ATTACK, inherited::_sd->m_fHitPower,Fvector().set(0.f,0.f,1.f));

	END_LOAD_SHARED_MOTION_DATA();

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);
}

void CPseudoGigant::StateSelector()
{	
	IState *state;
	VisionElem ve;

	if (C && H && I)			state = statePanic;
	else if (C && H && !I)		state = statePanic;
	else if (C && !H && I)		state = statePanic;
	else if (C && !H && !I) 	state = statePanic;
	else if (D && H && I)		state = stateAttack;
	else if (D && H && !I)		state = stateAttack;  //тихо подобраться и начать аттаку
	else if (D && !H && I)		state = stateAttack;
	else if (D && !H && !I)		state = statePanic;
	else if (E && H && I)		state = stateAttack; 
	else if (E && H && !I)  	state = stateAttack;  //тихо подобраться и начать аттаку
	else if (E && !H && I) 		state = stateAttack;
	else if (E && !H && !I)		state = stateAttack;
	else if (F && H && I) 		state = stateAttack; 		
	else if (F && H && !I)  	state = stateAttack; 
	else if (A && !K && !H)		state = stateExploreNDE;  //SetState(stateExploreDNE);  // слышу опасный звук, но не вижу, враг не выгодный		(ExploreDNE)
	else if (A && !K && H)		state = stateExploreNDE;  //SetState(stateExploreDNE);	//SetState(stateExploreDE);	// слышу опасный звук, но не вижу, враг выгодный			(ExploreDE)		
	else if (B && !K && !H)		state = stateExploreNDE;	// слышу не опасный звук, но не вижу, враг не выгодный	(ExploreNDNE)
	else if (B && !K && H)		state = stateExploreNDE;	// слышу не опасный звук, но не вижу, враг выгодный		(ExploreNDE)
	else if (GetCorpse(ve) && (ve.obj->m_fFood > 1) && ((GetSatiety() < _sd->m_fMinSatiety) || flagEatNow))	
		state = stateEat;
	else						state = stateRest; 

	SetState(state);
}

void CPseudoGigant::ProcessTurn()
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

