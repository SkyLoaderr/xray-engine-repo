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
}


void CAI_Dog::Init()
{
	inherited::Init();

	CurrentState					= stateRest;
	CurrentState->Reset				();

	Bones.Reset();

	CJumping::Init(this);
}

void CAI_Dog::Load(LPCSTR section)
{
	inherited::Load	(section);
	CJumping::Load	(section);

	// define animation set
	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, 0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimLieIdle,			"lie_idle_",			-1, 0,						0,							PS_LIE);
	MotionMan.AddAnim(eAnimSleep,			"lie_idle_",			-1, 0,						0,							PS_LIE);
	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, m_fsWalkFwdNormal,		m_fsWalkAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	m_fsRunFwdNormal,		m_fsRunAngular,				PS_STAND);
	//...
	MotionMan.AddAnim(eAnimCheckCorpse,		"stand_idle_",			-1,	0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, 0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimStandLieDown,	"stand_lie_down_",		-1, 0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimLieStandUp,		"lie_stand_up_",		-1, 0,						0,							PS_LIE);
	//...
	//MotionMan.AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, 0,						0,							PS_SIT);
	//MotionMan.AddAnim(eAnimSitLieDown,		"sit_lie_down_",		-1, 0,						0,							PS_SIT);
	MotionMan.AddAnim(eAnimLieSitUp,		"lie_sit_up_",			-1, 0,						0,							PS_LIE);
	MotionMan.AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, 0,						0,							PS_STAND);	
	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		 1, 0,						m_fsRunAngular,				PS_STAND);
	MotionMan.AddAnim(eAnimStandDamaged,	"stand_damaged_",		-1, 0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, m_fsDrag,				m_fsWalkAngular,			PS_STAND);
	//...
	MotionMan.AddAnim(eAnimSteal,			"stand_drag_",			-1, m_fsSteal,				m_fsWalkAngular,			PS_STAND);

	//	MotionMan.AddAnim(eAnimJumpStart,		"jump1_",				 0, 0,						m_fsWalkAngular,			PS_STAND);
	//	MotionMan.AddAnim(eAnimJumpFly,			"jump1_",				 1, 0,						m_fsWalkAngular,			PS_STAND);
	//	MotionMan.AddAnim(eAnimJumpFinish,		"jump1_",				 2, 0,						m_fsWalkAngular,			PS_STAND);

	MotionMan.AddAnim(eAnimJumpStart,		"run_jump_",			 0, m_fsRunFwdNormal,	m_fsWalkAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimJumpFly,			"run_jump_",			 1, m_fsRunFwdNormal,	m_fsWalkAngular,			PS_STAND);

	MotionMan.AddAnim(eAnimJumpLeft,		"jump_left_",			-1,	m_fsWalkFwdNormal,		m_fsRunAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimJumpRight,		"jump_right_",			-1,	m_fsWalkFwdNormal,		m_fsRunAngular,			PS_STAND);


	// define transitions
	// order : 1. [anim -> anim]	2. [anim->state]	3. [state -> anim]		4. [state -> state]
	MotionMan.AddTransition_S2S(PS_STAND,	PS_LIE,		eAnimStandLieDown,		false);
	MotionMan.AddTransition_S2S(PS_LIE,		PS_STAND,	eAnimLieStandUp,		false);
	//MotionMan.AddTransition_S2S(PS_SIT,		PS_STAND,	eAnimSitStandUp,		false);
	//MotionMan.AddTransition_S2S(PS_SIT,		PS_LIE,		eAnimSitLieDown,		false);
	MotionMan.AddTransition_S2S(PS_LIE,		PS_SIT,		eAnimLieSitUp,			false);
	MotionMan.AddTransition_S2S(PS_STAND,	PS_SIT,		eAnimStandSitDown,		false);

	// define links from Action to animations
	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_RUN,			eAnimRun,	eAnimJumpLeft, eAnimJumpRight, PI_DIV_4);
	MotionMan.LinkAction(ACT_EAT,			eAnimEat);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
	MotionMan.LinkAction(ACT_REST,			eAnimLieIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimDragCorpse);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack);
	MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimStandIdle);

	Fvector center;
	center.set		(0.f,0.f,0.f);

	MotionMan.AA_PushAttackAnim(eAnimAttack, 0, 700,	800,	center,		2.0f, m_fHitPower, 0.f, 0.f);
	MotionMan.AA_PushAttackAnim(eAnimAttack, 1, 600,	800,	center,		2.5f, m_fHitPower, 0.f, 0.f);
	MotionMan.AA_PushAttackAnim(eAnimAttack, 2, 600,	700,	center,		1.5f, m_fHitPower, 0.f, 0.f);

	CJumping::AddState(PSkeletonAnimated(Visual())->ID_Cycle_Safe("run_jump_0"), JT_CUSTOM,	true,	0.f, 0.f);
	CJumping::AddState(PSkeletonAnimated(Visual())->ID_Cycle_Safe("run_jump_1"), JT_GLIDE,	false,	0.f, 0.f);
	CJumping::AddState(PSkeletonAnimated(Visual())->ID_Cycle_Safe("run_jump_0"), JT_CUSTOM,	true,	0.f, 0.f);
}

void CAI_Dog::StateSelector()
{	
	VisionElem ve;
	
	if (C || D || E || F) SetBkgndSound();
	else SetBkgndSound(false);

	if (C && H && I)			SetState(statePanic);
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

void CAI_Dog::LookPosition(Fvector pos)
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
//	// получаем вектор направления к источнику звука и его мировые углы
//	float		yaw,p;
//	to_dir.getHP(yaw,p); // yaw - угол на который нужно повернуть монстра
//
//	float cur_yaw = -r_torso_current.yaw;				// текущий мировой угол монстра
//	float dy = _abs(angle_normalize_signed(yaw - cur_yaw));		// дельта, на которую нужно поворачиваться
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
//	float k;													// знаковый коэф. для боны (лево/право)
//	if (angle_normalize_signed(yaw - cur_yaw) > 0) k = 1.f;		// right side
//	else k = -1.f;												// left side
//	
//	y1 *= k;
//	y2 *= k;
//	y3 *= k;
//	
//	// Вычислить скорость вращения кости (с учётов возврата в исходное положение)
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
}

///////////////////////////////////////////////////////

