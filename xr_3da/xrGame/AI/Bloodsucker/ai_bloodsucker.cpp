#include "stdafx.h"
#include "ai_bloodsucker.h"
#include "ai_bloodsucker_effector.h"

#include "..\\..\\hudmanager.h"

CAI_Bloodsucker::CAI_Bloodsucker()
{
	stateRest			= xr_new<CBitingRest>			(this);
	stateEat			= xr_new<CBitingEat>			(this, false);
	stateAttack			= xr_new<CBitingAttack>			(this, true);
	statePanic			= xr_new<CBitingPanic>			(this, false);
	stateExploreDNE		= xr_new<CBitingExploreDNE>		(this, true);
	stateExploreNDE		= xr_new<CBitingExploreNDE>		(this);

	Init();
}

CAI_Bloodsucker::~CAI_Bloodsucker()
{
	xr_delete(stateRest);
	xr_delete(stateEat);
	xr_delete(stateAttack);
	xr_delete(statePanic);
	xr_delete(stateExploreDNE);
	xr_delete(stateExploreNDE);
}


void CAI_Bloodsucker::Init()
{
	inherited::Init();

	CurrentState					= stateRest;
	CurrentState->Reset				();

	Bones.Reset();
}

void CAI_Bloodsucker::Load(LPCSTR section) 
{
	inherited::Load(section);

	CMonsterInvisibility::Load(section);

	m_fInvisibilityDist = pSettings->r_float(section,"InvisibilityDist");
	m_ftrPowerDown		= pSettings->r_float(section,"PowerDownFactor");	
	m_fPowerThreshold	= pSettings->r_float(section,"PowerThreshold");	

}


BOOL CAI_Bloodsucker::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	vfAssignBones	();

	// define animation set
	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, 0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_idle_ls_",		-1, 0,						m_fsTurnNormalAngular,		PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnRight,	"stand_idle_rs_",		-1, 0,						m_fsTurnNormalAngular,		PS_STAND);
	MotionMan.AddAnim(eAnimSleep,			"lie_sleep_",			-1, 0,						0,							PS_LIE);
	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, m_fsWalkFwdNormal,		m_fsWalkAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimWalkBkwd,		"stand_walk_bkwd_",		-1, m_fsWalkBkwdNormal,		m_fsWalkAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	m_fsRunFwdNormal,		m_fsRunAngular,				PS_STAND);
	MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimEat,				"sit_eat_",				-1, 0,						0,							PS_SIT);
	MotionMan.AddAnim(eAnimDie,				"stand_idle_",			-1, 0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, 0,						m_fsRunAngular,				PS_STAND);
	MotionMan.AddAnim(eAnimLookAround,		"stand_look_around_",	-1, 0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimSitIdle,			"sit_idle_",			-1, 0,						0,							PS_SIT);
	MotionMan.AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, 0,						0,							PS_SIT);
	MotionMan.AddAnim(eAnimSitToSleep,		"sit_to_sleep_",		-1, 0,						0,							PS_SIT);
	MotionMan.AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, 0,						0,							PS_STAND);

	// define transitions
	// order : 1. [anim -> anim]	2. [anim->state]	3. [state -> anim]		4. [state -> state]
	MotionMan.AddTransition_A2A(eAnimStandSitDown,	eAnimSleep,	eAnimSitToSleep,	false);
	MotionMan.AddTransition_S2A(PS_STAND,			eAnimSleep,	eAnimStandSitDown,	true);
	MotionMan.AddTransition_S2S(PS_STAND,			PS_SIT,		eAnimStandSitDown,		false);
	MotionMan.AddTransition_S2S(PS_STAND,			PS_LIE,		eAnimStandSitDown,		false);
	MotionMan.AddTransition_S2S(PS_SIT,				PS_STAND,	eAnimSitStandUp,		false);
	MotionMan.AddTransition_S2S(PS_LIE,				PS_STAND,	eAnimSitStandUp,		false);

	// define links from Action to animations
	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle,	eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_6);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimSitIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimSitIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkBkwd);
	MotionMan.LinkAction(ACT_RUN,			eAnimRun);
	MotionMan.LinkAction(ACT_EAT,			eAnimEat);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
	MotionMan.LinkAction(ACT_REST,			eAnimSitIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimWalkBkwd);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack,	eAnimRun,			eAnimRun,			PI_DIV_6);
	MotionMan.LinkAction(ACT_STEAL,			eAnimWalkFwd);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimLookAround); 

	Fvector center;
	center.set		(0.f,0.f,0.f);

	MotionMan.AA_PushAttackAnim(eAnimAttack, 0, 500,	600,	center,		1.3f, m_fHitPower, -PI_DIV_6,	PI_DIV_6);
	MotionMan.AA_PushAttackAnim(eAnimAttack, 1, 600,	700,	center,		1.3f, m_fHitPower, 0.f,			PI_DIV_6);
	MotionMan.AA_PushAttackAnim(eAnimAttack, 2, 500,	600,	center,		1.4f, m_fHitPower, PI_DIV_3,	PI_DIV_6);

	// Hit fx's
	MotionMan.AddHitFX("hit_high_0"); 
	MotionMan.AddHitFX("hit_high_1");
	MotionMan.AddHitFX("hit_low_0"); 
	MotionMan.AddHitFX("hit_low_1");

	return(TRUE);
}

void CAI_Bloodsucker::UpdateCL()
{
	
	inherited::UpdateCL();

	// Blink processing
	bool PrevVis	=	IsCurrentVisible();
	bool NewVis		=	CMonsterInvisibility::Update();
	if (NewVis != PrevVis) setVisible(NewVis);

	//----------------------------------------------------------------
	float ty,cy;

	cy = r_torso_current.yaw;
	ty = r_torso_target.yaw;

	HUD().pFontSmall->OutSet (300,420);	
	HUD().pFontSmall->OutNext("CY = [%f]   TY = [%f]", rad2deg(cy),rad2deg(ty));
	//----------------------------------------------------------------

}

void CAI_Bloodsucker::StateSelector()
{

	VisionElem ve;

	if (C && H && I)			SetState(statePanic);
	else if (C && H && !I)		SetState(statePanic);
	else if (C && !H && I)		SetState(statePanic);
	else if (C && !H && !I) 	SetState(statePanic);
	else if (D && H && I)		SetState(stateAttack);
	else if (D && !H && I)		SetState(statePanic);
	else if (D && !H && !I) 	SetState(stateAttack);			// :: Hide
	else if (D && H && !I)		SetState(stateAttack); 
	else if (E && H && I)		SetState(stateAttack); 
	else if (E && H && !I)  	SetState(stateAttack);  
	else if (E && !H && I) 		SetState(stateAttack);			// :: Detour
	else if (E && !H && !I)		SetState(stateAttack);			// :: Detour 
	else if (F && H && I) 		SetState(stateAttack); 		
	else if (F && H && !I)  	SetState(stateAttack); 
	else if (F && !H && I)  	SetState(stateAttack); 
	else if (F && !H && !I) 	SetState(stateAttack);		
	else if (A && !K)			SetState(stateExploreDNE); 
	else if (B && !K)			SetState(stateExploreNDE); 
	else if ((GetCorpse(ve) && (ve.obj->m_fFood > 1)) && ((GetSatiety() < 0.85f) || flagEatNow))
		SetState(stateEat);	
	else						SetState(stateRest);
}

void __stdcall CAI_Bloodsucker::BoneCallback(CBoneInstance *B)
{
	CAI_Bloodsucker*	this_class = dynamic_cast<CAI_Bloodsucker*> (static_cast<CObject*>(B->Callback_Param));

	this_class->Bones.Update(B, Level().timeServer());
}

void CAI_Bloodsucker::vfAssignBones()
{
	// Установка callback на кости

	int bone1		= PKinematics(Visual())->LL_BoneID("bip01_spine");
	PKinematics(Visual())->LL_GetInstance(u16(bone1)).set_callback(BoneCallback,this);
	int bone2	= PKinematics(Visual())->LL_BoneID("bip01_head");
	PKinematics(Visual())->LL_GetInstance(u16(bone2)).set_callback(BoneCallback,this);

	// Bones settings
	Bones.Reset();
	Bones.AddBone(GetBone(bone1), AXIS_X); Bones.AddBone(GetBone(bone1), AXIS_Y); Bones.AddBone(GetBone(bone1), AXIS_Z);
	Bones.AddBone(GetBone(bone2), AXIS_X); Bones.AddBone(GetBone(bone2), AXIS_Y); Bones.AddBone(GetBone(bone2), AXIS_Z);
}



void CAI_Bloodsucker::LookDirection(Fvector to_dir, float bone_turn_speed)
{
	// получаем вектор направления к источнику звука и его мировые углы
	float		yaw,p;
	to_dir.getHP(yaw,p);

	float cur_yaw = -r_torso_current.yaw;						// текущий мировой угол монстра
	float bone_angle;											// угол для боны	
	float k;													// знаковый коэф. для боны (лево/право)

	float max_bone_angle = PI_DIV_4;									
	float dy = _abs(angle_normalize_signed(yaw - cur_yaw));		// дельта, на которую нужно поворачиваться

	if (getAI().bfTooSmallAngle(cur_yaw,yaw, max_bone_angle)) {	// bone turn only
		bone_angle = dy;
	} else {													// torso & bone turn 
		r_torso_target.yaw = angle_normalize(-yaw);
		if (dy / 2 < max_bone_angle) bone_angle = dy / 2;
		else bone_angle = max_bone_angle;
	}

	if (angle_normalize_signed(yaw - cur_yaw) > 0) k = -1.f; // right side
	else k = 1.f;	 // left side

	Bones.SetMotion(GetBone("bip01_spine"), AXIS_X, bone_angle * k, bone_turn_speed, 1);
}

void CAI_Bloodsucker::LookPosition(Fvector to_point)
{
	Fvector	dir;
	dir.set(to_point);
	dir.sub(Position());
	LookDirection(dir,PI_DIV_3);
}

void CAI_Bloodsucker::ActivateEffector(float life_time)
{
	Level().Cameras.AddEffector(xr_new<CBloodsuckerEffector>(life_time));
}


void CAI_Bloodsucker::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		MotionMan.Seq_Add(eAnimCheckCorpse);
		MotionMan.Seq_Switch();
	}
}
