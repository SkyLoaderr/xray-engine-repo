#include "stdafx.h"
#include "ai_dog.h"
#include "../ai_monster_utils.h"

CAI_Dog::CAI_Dog()
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

CAI_Dog::~CAI_Dog()
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


void CAI_Dog::Init()
{
	inherited::Init();

	CurrentState					= stateRest;
	CurrentState->Reset				();

	Bones.Reset();

	CJumping::Init(this);

	strike_in_jump					= false;
}

void CAI_Dog::Load(LPCSTR section)
{
	inherited::Load	(section);
	CJumping::Load	(section);


	CJumping::AddState(PSkeletonAnimated(Visual())->ID_Cycle_Safe("jump_glide_0"), JT_GLIDE,	false,	0.f, inherited::_sd->m_fsRunAngular);

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	BEGIN_LOAD_SHARED_MOTION_DATA();

	// define animation set
	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, 0,									0,										PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, 0,									inherited::_sd->m_fsTurnNormalAngular,	PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, 0,									inherited::_sd->m_fsTurnNormalAngular,	PS_STAND);
	MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, 0,									0,										PS_STAND);
	MotionMan.AddAnim(eAnimSleep,			"lie_sleep_",			-1, 0,									0,										PS_LIE);
	MotionMan.AddAnim(eAnimLieIdle,			"lie_idle_",			-1, 0,									0,										PS_LIE);
	MotionMan.AddAnim(eAnimSitIdle,			"sit_idle_",			-1, 0,									0,										PS_SIT);
	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, 0,									inherited::_sd->m_fsRunAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, inherited::_sd->m_fsWalkFwdNormal,	inherited::_sd->m_fsWalkAngular,		PS_STAND);
	MotionMan.AddAnim(eAnimWalkBkwd,		"stand_walk_bkwd_",		-1, inherited::_sd->m_fsWalkBkwdNormal,	inherited::_sd->m_fsWalkAngular,		PS_STAND);
	MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_dmg_",		-1, inherited::_sd->m_fsWalkFwdDamaged,	inherited::_sd->m_fsWalkAngular,		PS_STAND);
	MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	inherited::_sd->m_fsRunFwdNormal,	inherited::_sd->m_fsRunAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1, inherited::_sd->m_fsRunFwdDamaged,	inherited::_sd->m_fsRunAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	0,									0,										PS_STAND);
	MotionMan.AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, inherited::_sd->m_fsDrag,			inherited::_sd->m_fsWalkAngular,		PS_STAND);
	MotionMan.AddAnim(eAnimSniff,			"stand_sniff_",			-1, 0,									0,										PS_STAND);
	MotionMan.AddAnim(eAnimHowling,			"stand_howling_",		-1,	0,									0,										PS_STAND);
	MotionMan.AddAnim(eAnimJumpGlide,		"jump_glide_",			-1, 0,									0,										PS_STAND);
	MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, inherited::_sd->m_fsSteal,			inherited::_sd->m_fsWalkAngular,		PS_STAND);
	MotionMan.AddAnim(eAnimThreaten,		"stand_threaten_",		-1, 0,									0,										PS_STAND);
	MotionMan.AddAnim(eAnimDie,				"stand_die_",			-1, 0,									0,										PS_STAND);

	MotionMan.AddAnim(eAnimSitLieDown,		"sit_lie_down_",		-1, 0,									0,										PS_SIT);
	MotionMan.AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, 0,									0,										PS_STAND);	
	MotionMan.AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, 0,									0,										PS_SIT);
	MotionMan.AddAnim(eAnimLieToSleep,		"lie_to_sleep_",		-1,	0,									0,										PS_LIE);
	MotionMan.AddAnim(eAnimSleepStandUp,	"lie_to_stand_up_",		-1, 0,									0,										PS_LIE);

	MotionMan.AddAnim(eAnimJumpLeft,		"stand_jump_left_",		-1, 0,									inherited::_sd->m_fsRunAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimJumpRight,		"stand_jump_right_",	-1, 0,									inherited::_sd->m_fsRunAngular,			PS_STAND);


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
	MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);	
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimSniff);

	MotionMan.AA_PushAttackAnimTest(eAnimAttack, 0, 400, 600, -PI_DIV_6,PI_DIV_6,-PI_DIV_6,PI_DIV_6,2.5f, inherited::_sd->m_fHitPower,Fvector().set(0.f,0.f,3.f));
	MotionMan.AA_PushAttackAnimTest(eAnimAttack, 1, 400, 600, -PI_DIV_6,PI_DIV_6,-PI_DIV_6,PI_DIV_6,2.5f, inherited::_sd->m_fHitPower,Fvector().set(0.f,0.f,3.f));


	END_LOAD_SHARED_MOTION_DATA();
}



void CAI_Dog::StateSelector()
{	
	VisionElem ve;

	if (C)						SetState(statePanic);
	else if (D || E || F)		SetState(stateAttack);
	else if (A && !K)			SetState(stateExploreNDE);		//SetState(stateExploreDNE);	//SetState(stateExploreDE);	// ����� ������� ����, �� �� ����, ���� ��������			(ExploreDE)		
	else if (B && !K)			SetState(stateExploreNDE);	// ����� �� ������� ����, �� �� ����, ���� ��������		(ExploreNDE)
	else if (GetCorpse(ve) && (ve.obj->m_fFood > 1) && ((GetSatiety() < _sd->m_fMinSatiety) || flagEatNow))	
		SetState(stateEat);
	else						SetState(stateRest); 

	EMotionAnim anim = MotionMan.Seq_CurAnim();
	if ((anim == eAnimCheckCorpse) && K) MotionMan.Seq_Finish();
	
	BonesInMotion(); 

	// Temp
	ChangeEntityMorale(-0.5f);

}


void __stdcall CAI_Dog::BoneCallback(CBoneInstance *B)
{
	CAI_Dog*	this_class = dynamic_cast<CAI_Dog*> (static_cast<CObject*>(B->Callback_Param));
	//this_class->Bones.Update(B, Level().timeServer());
}

BOOL CAI_Dog::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	// Bones settings
	PKinematics(Visual())->LL_GetBoneInstance(PKinematics(Visual())->LL_BoneID("bone01")).set_callback(BoneCallback,this);
	PKinematics(Visual())->LL_GetBoneInstance(PKinematics(Visual())->LL_BoneID("bip01_spine1")).set_callback(BoneCallback,this);
	PKinematics(Visual())->LL_GetBoneInstance(PKinematics(Visual())->LL_BoneID("bip01_spine2")).set_callback(BoneCallback,this);
	PKinematics(Visual())->LL_GetBoneInstance(PKinematics(Visual())->LL_BoneID("bip01_head")).set_callback(BoneCallback,this);

	Bones.Reset();
	Bones.AddBone(GetBoneInstance("bone01"), AXIS_Y); 
	
	Bones.AddBone(GetBoneInstance("bip01_spine1"), AXIS_Z); 
	Bones.AddBone(GetBoneInstance("bip01_spine2"), AXIS_Z); 
	Bones.AddBone(GetBoneInstance("bip01_head"), AXIS_X); 
	Bones.AddBone(GetBoneInstance("bip01_head"), AXIS_Y); 
	Bones.AddBone(GetBoneInstance("bip01_head"), AXIS_Z); 

	return TRUE;
}

void CAI_Dog::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		MotionMan.Seq_Add(eAnimCheckCorpse);
		MotionMan.Seq_Switch();
	}
	
	if ((spec_params & ASP_THREATEN) == ASP_THREATEN) {
		MotionMan.SetCurAnim(eAnimThreaten);
	}

	if ((spec_params & ASP_ROTATION_JUMP) == ASP_ROTATION_JUMP) {
		float yaw, pitch;
		Fvector().sub(m_tEnemy.obj->Position(), Position()).getHP(yaw,pitch);
		yaw *= -1;

		EMotionAnim anim = eAnimJumpLeft;
		yaw = angle_normalize(yaw - PI / 20);

		if (from_right(yaw,m_body.current.yaw)) {
			anim = eAnimJumpRight;
			yaw = angle_normalize(yaw + PI / 20);	
		} 

		MotionMan.Seq_Add(anim);
		MotionMan.Seq_Switch();

		
		CMovementManager::m_body.target.yaw = yaw;

//		// calculate angular speed
//		float new_angular_velocity = 4.0f;
//		float delta_yaw = angle_difference(yaw,m_body.current.yaw);
//		float time = MotionMan.GetAnimTime(anim, 0);
//		//new_angular_velocity = 2.5f * delta_yaw / time; 

//		MotionMan.ForceAngularSpeed(new_angular_velocity);

	}

}

void CAI_Dog::OnSoundPlay()
{
	//if (!Bones.IsActive()) Bones.SetMotion(GetBoneInstance("bip01_head"),AXIS_Y, PI_DIV_6, PI_MUL_2, 1);
}

void CAI_Dog::LookPosition(Fvector /**pos/**/, float angular_speed)
{
}

#define TURN_HEAD_ANGLE PI_DIV_4

void CAI_Dog::BonesInMotion() 
{
	EMotionAnim anim = MotionMan.GetCurAnim();
	bool b_enable_motions = false;

	switch (anim) {
	case eAnimSitIdle:
	case eAnimWalkFwd:
		b_enable_motions = true;
		break;
	}
	
	if (!Bones.IsActive() && b_enable_motions) {
		float	x,y,z;
		u8		side_to_look = 0; // 0 - front, 1 - left, 2 - right
		
		u8 selector = u8(Random.randI(100)); 
		if (selector < 20)		side_to_look = 1;
		else if (selector < 40)	side_to_look = 2;
		
		if (side_to_look == 1)	{	x = -TURN_HEAD_ANGLE;	y = TURN_HEAD_ANGLE;	z = TURN_HEAD_ANGLE;} 
		else if (side_to_look == 2)	{	x = TURN_HEAD_ANGLE; 	y = TURN_HEAD_ANGLE;	z = -TURN_HEAD_ANGLE;}
		else {x = 0.f; y = 0.f; z = 0.f;}

		Bones.SetMotion(GetBoneInstance("bip01_head"),AXIS_X, x, PI_DIV_2, 3000);
		Bones.SetMotion(GetBoneInstance("bip01_head"),AXIS_Y, y, PI_DIV_2, 3000);
		Bones.SetMotion(GetBoneInstance("bip01_head"),AXIS_Z, z, PI_DIV_2, 3000);
	}
}


void CAI_Dog::UpdateCL()
{
	inherited::UpdateCL();

	CJumping::Update();
	float trace_dist = 1.0f;

	// ��������� �� ��������� ���� �� ����� ������
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

		// !!!
		LookPosition(pE->Position());
	}
}

void CAI_Dog::OnJumpStop()
{
	MotionMan.ProcessAction();
}



