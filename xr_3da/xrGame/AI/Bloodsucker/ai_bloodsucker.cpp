#include "stdafx.h"
#include "ai_bloodsucker.h"
#include "ai_bloodsucker_effector.h"


CAI_Bloodsucker::CAI_Bloodsucker()
{
	stateRest			= xr_new<CBloodsuckerRest>		(this);
	stateEat			= xr_new<CBloodsuckerEat>		(this);
	stateAttack			= xr_new<CBloodsuckerAttack>	(this);
	statePanic			= xr_new<CBloodsuckerPanic>		(this);
	stateHearDNE		= xr_new<CBloodsuckerHearDNE>	(this);
	stateHearNDE		= xr_new<CBloodsuckerHearNDE>	(this);

	Init();
}

CAI_Bloodsucker::~CAI_Bloodsucker()
{
	xr_delete(stateRest);
	xr_delete(stateEat);
	xr_delete(stateAttack);
	xr_delete(statePanic);
	xr_delete(stateHearDNE);
	xr_delete(stateHearNDE);
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

	m_tVisibility.Load(section);

	m_fInvisibilityDist = pSettings->r_float(section,"InvisibilityDist");
	m_ftrPowerDown		= pSettings->r_float(section,"PowerDownFactor");	
	m_fPowerThreshold	= pSettings->r_float(section,"PowerThreshold");	

	m_tSelectorHearSnd.Load(section,"selector_hear_sound");				// like _free hunting

	m_dwDayTimeBegin	= pSettings->r_u32	(section,"DayTime_Begin");
	m_dwDayTimeEnd		= pSettings->r_u32	(section,"DayTime_End");		
	m_fMinSatiety		= pSettings->r_float(section,"Min_Satiety");
	m_fMaxSatiety		= pSettings->r_float(section,"Max_Satiety");
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
	bool PrevVis	=	m_tVisibility.IsCurrentVisible();
	bool NewVis		=	m_tVisibility.Update();
	if (NewVis != PrevVis) setVisible(NewVis);
}

void CAI_Bloodsucker::StateSelector()
{

	VisionElem ve;

	if (C && H && I)			SetState(statePanic);
	else if (C && H && !I)		SetState(statePanic);
	else if (C && !H && I)		SetState(statePanic);
	else if (C && !H && !I) 	SetState(statePanic);
	else if (D && H && I)		SetState(statePanic);
	else if (D && !H && I)		SetState(statePanic);
	else if (D && !H && !I) 	SetState(statePanic);			// :: Hide
	else if (D && H && !I)		SetState(stateAttack); 
	else if (E && H && I)		SetState(stateAttack); 
	else if (E && H && !I)  	SetState(stateAttack);  
	else if (E && !H && I) 		SetState(stateAttack);			// :: Detour
	else if (E && !H && !I)		SetState(stateAttack);			// :: Detour 
	else if (F && H && I) 		SetState(stateAttack); 		
	else if (F && H && !I)  	SetState(stateAttack); 
	else if (F && !H && I)  	SetState(stateAttack); 
	else if (F && !H && !I) 	SetState(stateAttack);		
	else if (A && !K)			SetState(stateHearDNE); 
	else if (B && !K)			SetState(stateHearNDE); 
	else if (GetCorpse(ve) && (ve.obj->m_fFood > 1) && ((GetSatiety() < 0.85f) || flagEatNow))
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
	PKinematics(Visual())->LL_GetInstance(bone1).set_callback(BoneCallback,this);
	int bone2	= PKinematics(Visual())->LL_BoneID("bip01_head");
	PKinematics(Visual())->LL_GetInstance(bone2).set_callback(BoneCallback,this);

	// Bones settings
	Bones.Reset();
	Bones.AddBone(GetBone(bone1), AXIS_X); Bones.AddBone(GetBone(bone1), AXIS_Y); Bones.AddBone(GetBone(bone1), AXIS_Z);
	Bones.AddBone(GetBone(bone2), AXIS_X); Bones.AddBone(GetBone(bone2), AXIS_Y); Bones.AddBone(GetBone(bone2), AXIS_Z);
}

CBoneInstance *CAI_Bloodsucker::GetBone(LPCTSTR bone_name)
{
	int bone = PKinematics(Visual())->LL_BoneID(bone_name);
	return (&PKinematics(Visual())->LL_GetInstance(bone));
}
CBoneInstance *CAI_Bloodsucker::GetBone(int bone_id)
{
	return (&PKinematics(Visual())->LL_GetInstance(bone_id));
}

//void CAI_Bloodsucker::LoadAttackAnim()
//{
//	Fvector center;
//	center.set		(0.f,0.f,0.f);
//
//	// 1 //
//	m_tAttackAnim.PushAttackAnim(0, 10, 0, 500,	600,	center,		1.3f, m_fHitPower, -PI_DIV_6, PI_DIV_6);
//
//	// 2 //
//	m_tAttackAnim.PushAttackAnim(0, 10, 1, 600,	700,	center,		1.3f, m_fHitPower, 0.f, PI_DIV_6);
//
//	// 3 // 
//	m_tAttackAnim.PushAttackAnim(0, 10, 2, 500,	600,	center,		1.4f, m_fHitPower, PI_DIV_3, PI_DIV_6);
//}


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

void CAI_Bloodsucker::LookPosition(Fvector to_point, float bone_turn_speed)
{
	Fvector	dir;
	dir.set(to_point);
	dir.sub(Position());
	LookDirection(dir,bone_turn_speed);
}

void CAI_Bloodsucker::ActivateEffector(float life_time)
{
	Level().Cameras.AddEffector(xr_new<CBloodsuckerEffector>(life_time));
}

