#include "stdafx.h"
#include "ai_bloodsucker.h"
#include "ai_bloodsucker_effector.h"
#include "..\\ai_monsters_misc.h"

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

	m_fEffectDist					= 0.f;

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
	m_fEffectDist		= pSettings->r_float(section,"EffectDistance");	

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);
	
	BEGIN_LOAD_SHARED_MOTION_DATA();

	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, 0,										0,										PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, 0,										inherited::_sd->m_fsTurnNormalAngular,	PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, 0,										inherited::_sd->m_fsTurnNormalAngular,	PS_STAND);
	MotionMan.AddAnim(eAnimSleep,			"lie_sleep_",			-1, 0,										0,										PS_LIE);
	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, inherited::_sd->m_fsWalkFwdNormal,		inherited::_sd->m_fsWalkAngular,		PS_STAND);
	MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_fwd_dmg_",	-1, inherited::_sd->m_fsWalkFwdDamaged,		inherited::_sd->m_fsWalkAngular,		PS_STAND);
	MotionMan.AddAnim(eAnimWalkBkwd,		"stand_walk_bkwd_",		-1, inherited::_sd->m_fsWalkBkwdNormal,		inherited::_sd->m_fsWalkAngular,		PS_STAND);
	MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	inherited::_sd->m_fsRunFwdNormal,		inherited::_sd->m_fsRunAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	inherited::_sd->m_fsRunFwdDamaged,		inherited::_sd->m_fsRunAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	0,										0,										PS_STAND);
	MotionMan.AddAnim(eAnimEat,				"sit_eat_",				-1, 0,										0,										PS_SIT);
	MotionMan.AddAnim(eAnimDie,				"stand_idle_",			-1, 0,										0,										PS_STAND);
	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, 0,										inherited::_sd->m_fsRunAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimLookAround,		"stand_look_around_",	-1, 0,										0,										PS_STAND);
	MotionMan.AddAnim(eAnimSitIdle,			"sit_idle_",			-1, 0,										0,										PS_SIT);
	MotionMan.AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, 0,										0,										PS_SIT);
	MotionMan.AddAnim(eAnimSitToSleep,		"sit_sleep_down_",		-1, 0,										0,										PS_SIT);
	MotionMan.AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, 0,										0,										PS_STAND);
	MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, inherited::_sd->m_fsSteal,				inherited::_sd->m_fsWalkAngular,		PS_STAND);

	// define transitions
	MotionMan.AddTransition(eAnimStandSitDown,	eAnimSleep,	eAnimSitToSleep,	false);
	MotionMan.AddTransition(PS_STAND,			eAnimSleep,	eAnimStandSitDown,	true);
	MotionMan.AddTransition(PS_STAND,			PS_SIT,		eAnimStandSitDown,	false);
	MotionMan.AddTransition(PS_STAND,			PS_LIE,		eAnimStandSitDown,	false);
	MotionMan.AddTransition(PS_SIT,				PS_STAND,	eAnimSitStandUp,	false);
	MotionMan.AddTransition(PS_LIE,				PS_STAND,	eAnimSitStandUp,	false);

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
	MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimLookAround); 

	Fvector center;
	center.set		(0.f,0.f,0.f);

	MotionMan.AA_PushAttackAnim(eAnimAttack, 0, 500,	600,	center,		1.3f, inherited::_sd->m_fHitPower, -PI_DIV_6,	PI_DIV_6);
	MotionMan.AA_PushAttackAnim(eAnimAttack, 1, 600,	700,	center,		1.3f, inherited::_sd->m_fHitPower, 0.f,			PI_DIV_6);
	MotionMan.AA_PushAttackAnim(eAnimAttack, 2, 500,	600,	center,		1.4f, inherited::_sd->m_fHitPower, PI_DIV_3,	PI_DIV_6);

	MotionMan.FX_LoadMap(section); 

	END_LOAD_SHARED_MOTION_DATA();
}


BOOL CAI_Bloodsucker::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	vfAssignBones	();

	return(TRUE);
}

void CAI_Bloodsucker::UpdateCL()
{
	
	inherited::UpdateCL();

	// Blink processing
	bool PrevVis	=	IsCurrentVisible();
	bool NewVis		=	CMonsterInvisibility::Update();
	if (NewVis != PrevVis) setVisible(NewVis);

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
	else if (A && !K)			SetState(stateExploreNDE); 
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
	bone_spine =	&PKinematics(Visual())->LL_GetBoneInstance(PKinematics(Visual())->LL_BoneID("bip01_spine"));
	bone_head =		&PKinematics(Visual())->LL_GetBoneInstance(PKinematics(Visual())->LL_BoneID("bip01_head"));
	bone_spine->set_callback(BoneCallback,this);
	bone_head->set_callback(BoneCallback,this);

	// Bones settings
	Bones.Reset();
	Bones.AddBone(bone_spine, AXIS_X);	Bones.AddBone(bone_spine, AXIS_Y);
	Bones.AddBone(bone_head, AXIS_X);	Bones.AddBone(bone_head, AXIS_Y);
}


#define MAX_BONE_ANGLE PI_DIV_2

void CAI_Bloodsucker::LookDirection(Fvector to_dir, float bone_turn_speed)
{
	// получаем вектор направления к источнику звука и его мировые углы
	float		yaw,pitch;
	to_dir.getHP(yaw,pitch);

	// установить параметры вращения по yaw
	float cur_yaw = -r_torso_current.yaw;						// текущий мировой угол монстра
	float bone_angle;											// угол для боны	

	float dy = _abs(angle_normalize_signed(yaw - cur_yaw));		// дельта, на которую нужно поворачиваться

	if (getAI().bfTooSmallAngle(cur_yaw,yaw, MAX_BONE_ANGLE)) {	// bone turn only
		bone_angle = dy;
	} else {													// torso & bone turn 
		if (AI_Path.TravelPath.empty()) r_torso_target.yaw = angle_normalize(-yaw);
		if (dy / 2 < MAX_BONE_ANGLE) bone_angle = dy / 2;
		else bone_angle = MAX_BONE_ANGLE;
	}

	bone_angle /= 2;
	if (IsRightSide(yaw,cur_yaw)) bone_angle *= -1.f;

	Bones.SetMotion(bone_spine, AXIS_X, bone_angle, bone_turn_speed, 100);
	Bones.SetMotion(bone_head,	AXIS_X, bone_angle, bone_turn_speed, 100);

	// установить параметры вращения по pitch
	clamp(pitch, -MAX_BONE_ANGLE, MAX_BONE_ANGLE);
	pitch /= 2; 

	Bones.SetMotion(bone_spine, AXIS_Y, pitch, bone_turn_speed, 100);
	Bones.SetMotion(bone_head,	AXIS_Y, pitch, bone_turn_speed, 100);	
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
