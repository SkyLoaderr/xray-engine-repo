#include "stdafx.h"
#include "ai_dog.h"


CAI_Dog::CAI_Dog()
{
	stateRest			= xr_new<CBitingRest>		(this);
	stateAttack			= xr_new<CBitingAttack>		(this, false);
	stateEat			= xr_new<CBitingEat>		(this, true);
	stateHide			= xr_new<CBitingHide>		(this);
	stateDetour			= xr_new<CBitingDetour>		(this);
	statePanic			= xr_new<CBitingPanic>		(this, false);
	stateExploreNDE		= xr_new<CBitingExploreNDE>	(this);
	
	CurrentState		= stateRest;

	stateTest			= xr_new<CBitingNull>		(this);

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

	xr_delete(stateTest);
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
	MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, inherited::_sd->m_fsSteal,			inherited::_sd->m_fsWalkAngular,	PS_STAND);
	MotionMan.AddAnim(eAnimDie,				"stand_die_",			-1, 0,									0,										PS_STAND);

	MotionMan.AddAnim(eAnimSitLieDown,		"sit_lie_down_",		-1, 0,									0,										PS_SIT);
	MotionMan.AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, 0,									0,										PS_STAND);	
	MotionMan.AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, 0,									0,										PS_SIT);
	MotionMan.AddAnim(eAnimLieToSleep,		"lie_to_sleep_",		-1,	0,									0,										PS_LIE);
	MotionMan.AddAnim(eAnimSleepStandUp,	"lie_to_stand_up_",		-1, 0,									0,										PS_LIE);


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

	MotionMan.AA_PushAttackAnimTest(eAnimAttack, 0, 500, 800, STANDART_ATTACK, inherited::_sd->m_fHitPower,Fvector().set(0.f,0.f,3.f));

	END_LOAD_SHARED_MOTION_DATA();
}



void CAI_Dog::StateSelector()
{	
	
	//HDebug->M_Add(0,cName(),D3DCOLOR_XRGB(255,0,128));
	
	VisionElem ve;

	if (C || D || E || F) SetBkgndSound();
	else SetBkgndSound(false);

	if (C)						SetState(statePanic);
	else if (D || E || F)		SetState(stateAttack);
	else if (A && !K)			SetState(statePanic);		//SetState(stateExploreDNE);	//SetState(stateExploreDE);	// слышу опасный звук, но не вижу, враг выгодный			(ExploreDE)		
	else if (B && !K)			SetState(stateExploreNDE);	// слышу не опасный звук, но не вижу, враг выгодный		(ExploreNDE)
	else if (GetCorpse(ve) && (ve.obj->m_fFood > 1) && ((GetSatiety() < 0.85f) || flagEatNow))	
		SetState(stateEat);
	else						SetState(stateRest); 
}


void __stdcall CAI_Dog::BoneCallback(CBoneInstance *B)
{
	CAI_Dog*	this_class = dynamic_cast<CAI_Dog*> (static_cast<CObject*>(B->Callback_Param));
	this_class->Bones.Update(B, Level().timeServer());
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

	return TRUE;
}

void CAI_Dog::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		MotionMan.Seq_Add(eAnimCheckCorpse);
		MotionMan.Seq_Switch();
	}
}

void CAI_Dog::OnSoundPlay()
{
	if (!Bones.IsActive()) Bones.SetMotion(GetBoneInstance("bone01"),AXIS_Y, PI_DIV_6, PI_MUL_2, 1);
}

void CAI_Dog::LookPosition(Fvector /**pos/**/, float angular_speed)
{
//	if (GetBoneInstance.IsActive()) return;
//	TTime itime = TTime(MotionMan.jump.ph_time * 1000);
//
//	if (MotionMan.jump.started + TTime(itime / 3) > m_dwCurrentTime) return;
//	float max_bone_angle = PI_DIV_4;
//	
//	Fvector to_dir;
//	to_dir = pos;
//	to_dir.sub(Position());
//
//	// получаем вектор направлени€ к источнику звука и его мировые углы
//	float		yaw,p;
//	to_dir.getHP(yaw,p); // yaw - угол на который нужно повернуть монстра
//
//	float cur_yaw = -m_body.current.yaw;				// текущий мировой угол монстра
//	float dy = _abs(angle_normalize_signed(yaw - cur_yaw));		// дельта, на которую нужно поворачиватьс€
//
//	float y1,y2,y3;
//	y1 = y2 = dy * 0.3f;
//	y3 = dy * 0.4f;
//	
//	// normalize
//	clamp(y1,0.f,max_bone_angle);
//	clamp(y2,0.f,max_bone_angle);
//	clamp(y3,0.f,max_bone_angle);
//	
//	float k;													// знаковый коэф. дл€ боны (лево/право)
//	if (angle_normalize_signed(yaw - cur_yaw) > 0) k = 1.f;		// right side
//	else k = -1.f;												// left side
//	
//	y1 *= k;
//	y2 *= k;
//	y3 *= k;
//	
//	// ¬ычислить скорость вращени€ кости (с учЄтов возврата в исходное положение)
//	float t = MotionMan.jump.ph_time;
//	
//	GetBoneInstance.SetMotion(GetBoneInstance("bip01_spine1"), AXIS_Z, y1, y1/t * 4.f, 1);
//	GetBoneInstance.SetMotion(GetBoneInstance("bip01_spine2"), AXIS_Z, y2, y2/t * 4.f, 1);
//	GetBoneInstance.SetMotion(GetBoneInstance("bip01_head"),	 AXIS_X, y3, y3/t * 4.f, 1);
}

void CAI_Dog::UpdateCL()
{
	inherited::UpdateCL();
	CJumping::Update();

	float trace_dist = 1.0f;

	// ѕроверить на нанесение хита во врем€ прыжка
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

///////////////////////////////////////////////////////

