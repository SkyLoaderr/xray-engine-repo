#include "stdafx.h"
#include "ai_chimera.h"

CAI_Chimera::CAI_Chimera()
{
	stateRest			= xr_new<CBitingRest>		(this);
	stateAttack			= xr_new<CBitingAttack>		(this, false);
	stateEat			= xr_new<CBitingEat>		(this, true);
	stateHide			= xr_new<CBitingHide>		(this);
	stateDetour			= xr_new<CBitingDetour>		(this);
	statePanic			= xr_new<CBitingPanic>		(this, false);
	stateExploreNDE		= xr_new<CBitingExploreNDE>	(this);
	CurrentState		= stateRest;

	Init();
}

CAI_Chimera::~CAI_Chimera()
{
	xr_delete(stateRest);
	xr_delete(stateAttack);
	xr_delete(stateEat);
	xr_delete(statePanic);
	xr_delete(stateExploreNDE);
	xr_delete(stateHide);
	xr_delete(stateDetour);
}


void CAI_Chimera::Init()
{
	inherited::Init();

	CurrentState			= stateRest;
	CurrentState->Reset		();

	fSpinYaw				= 0.f;
	timeLastSpin			= 0;
	fStartYaw				= 0.f;
	fFinishYaw				= 1.f;
	fPrevMty				= 0.f;

	Bones.Reset();
}

void CAI_Chimera::StateSelector()
{

//	if (!Bones.IsActive()) {
//
//		if (!getAI().bfTooSmallAngle(r_torso_current.yaw,r_torso_target.yaw, PI_DIV_6)) {
//			float k;	
//			if (angle_normalize_signed(r_torso_target.yaw - r_torso_current.yaw) > 0) k = -1.0;
//			else k = 1.0f;
//
//			int spin_bone	= PKinematics(Visual())->LL_BoneID("bip01_spine1");
//			Bones.SetMotion(&PKinematics(Visual())->LL_GetInstance(spin_bone), AXIS_Z, k * PI_DIV_6 , PI, 1);	
//		}
//
//	}
	VisionElem ve;

	if (C && H && I)		SetState(statePanic);
	else if (C && H && !I)		SetState(statePanic);
	else if (C && !H && I)		SetState(statePanic);
	else if (C && !H && !I) 	SetState(statePanic);
	else if (D && H && I)		SetState(stateAttack);
	else if (D && H && !I)		SetState(stateAttack);  //тихо подобраться и начать аттаку
	else if (D && !H && I)		SetState(statePanic);
	else if (D && !H && !I) 	SetState(stateHide);	// отход перебежками через укрытия
	else if (E && H && I)		SetState(stateAttack); 
	else if (E && H && !I)  	SetState(stateAttack);  //тихо подобраться и начать аттаку
	else if (E && !H && I) 		SetState(stateDetour); 
	else if (E && !H && !I)		SetState(stateDetour); 
	else if (F && H && I) 		SetState(stateAttack); 		
	else if (F && H && !I)  	SetState(stateAttack); 
	else if (F && !H && I)  	SetState(stateDetour); 
	else if (F && !H && !I) 	SetState(stateHide);
	else if (A && !K && !H)		SetState(stateExploreNDE);  //SetState(stateExploreDNE);  // слышу опасный звук, но не вижу, враг не выгодный		(ExploreDNE)
	else if (A && !K && H)		SetState(stateExploreNDE);  //SetState(stateExploreDNE);	//SetState(stateExploreDE);	// слышу опасный звук, но не вижу, враг выгодный			(ExploreDE)		
	else if (B && !K && !H)		SetState(stateExploreNDE);	// слышу не опасный звук, но не вижу, враг не выгодный	(ExploreNDNE)
	else if (B && !K && H)		SetState(stateExploreNDE);	// слышу не опасный звук, но не вижу, враг выгодный		(ExploreNDE)
	else if (GetCorpse(ve) && (ve.obj->m_fFood > 1) && ((GetSatiety() < 0.85f) || flagEatNow))	
		SetState(stateEat);
	else						SetState(stateRest); 

}



BOOL CAI_Chimera::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	vfAssignBones(pSettings,cNameSect());

	// define animation set
	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, 0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimLieIdle,			"lie_idle_",			-1, 0,						0,							PS_LIE);
	MotionMan.AddAnim(eAnimSleep,			"lie_idle_",			-1, 0,						0,							PS_LIE);
	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, m_ftrWalkSpeed,			m_ftrWalkRSpeed,			PS_STAND);
	MotionMan.AddAnim(eAnimWalkTurnLeft,	"stand_walk_ls_",		-1, m_ftrWalkTurningSpeed,	m_ftrWalkRSpeed,			PS_STAND);
	MotionMan.AddAnim(eAnimWalkTurnRight,	"stand_walk_rs_",		-1, m_ftrWalkTurningSpeed,	m_ftrWalkRSpeed,			PS_STAND);
	MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	m_ftrRunAttackSpeed,	m_ftrRunRSpeed,				PS_STAND);
	MotionMan.AddAnim(eAnimRunTurnLeft,		"stand_run_ls_",		-1,	m_ftrRunAttackTurnSpeed,m_ftrRunAttackTurnRSpeed,	PS_STAND);
	MotionMan.AddAnim(eAnimRunTurnRight,	"stand_run_rs_",		-1,	m_ftrRunAttackTurnSpeed,m_ftrRunAttackTurnRSpeed,	PS_STAND);
	MotionMan.AddAnim(eAnimEat,				"lie_eat_",				-1, 0,						0,							PS_LIE);
	MotionMan.AddAnim(eAnimStandLieDown,	"stand_lie_down_",		-1, 0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimLieStandUp,		"lie_stand_up_",		-1, 0,						0,							PS_LIE);
	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		 0, 0,						m_ftrRunRSpeed,				PS_STAND);


	// define transitions
	// order : 1. [anim -> anim]	2. [anim->state]	3. [state -> anim]		4. [state -> state]
	MotionMan.AddTransition(PS_STAND,	PS_LIE,		eAnimStandLieDown,		false);
	MotionMan.AddTransition(PS_LIE,		PS_STAND,	eAnimLieStandUp,		false);

	// define links from Action to animations
	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd,	eAnimWalkTurnLeft,	eAnimWalkTurnRight, PI_DIV_6);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_RUN,			eAnimRun,		eAnimRunTurnLeft,	eAnimRunTurnRight, PI_DIV_6);
	MotionMan.LinkAction(ACT_EAT,			eAnimEat);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
	MotionMan.LinkAction(ACT_REST,			eAnimLieIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimWalkFwd);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack);
	MotionMan.LinkAction(ACT_STEAL,			eAnimWalkFwd);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimStandIdle);


	Fvector center, left_side, right_side;

	center.set		(0.f,0.f,0.f);
	left_side.set	(-0.3f,0.f,0.f);
	right_side.set	(0.3f,0.f,0.f);

	MotionMan.AA_PushAttackAnim(eAnimAttack, 0, 700,	800,	center,		2.f, m_fHitPower, 0.f, 0.f);
	MotionMan.AA_PushAttackAnim(eAnimAttack, 1, 500,	600,	right_side, 2.5f, m_fHitPower, 0.f, 0.f);
	MotionMan.AA_PushAttackAnim(eAnimAttack, 2, 600,	700,	center,		2.5f, m_fHitPower, 0.f, 0.f);
	MotionMan.AA_PushAttackAnim(eAnimAttack, 3, 800,	900,	left_side,	1.0f, m_fHitPower, 0.f, 0.f);
	MotionMan.AA_PushAttackAnim(eAnimAttack, 5, 1500, 1600,	right_side, 2.0f, m_fHitPower, 0.f, 0.f);
	return TRUE;
}

void CAI_Chimera::Load (LPCSTR section)
{
	inherited::Load (section);
}

