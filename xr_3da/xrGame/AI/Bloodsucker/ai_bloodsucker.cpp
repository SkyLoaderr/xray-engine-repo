#include "stdafx.h"
#include "ai_bloodsucker.h"

// temp
#include "..\\..\\hudmanager.h"

CAI_Bloodsucker::CAI_Bloodsucker()
{
	stateRest			= xr_new<CBloodsuckerRest>		(this);
	stateEat			= xr_new<CBloodsuckerEat>		(this);
	stateAttack			= xr_new<CBloodsuckerAttack>	(this);
	statePanic			= xr_new<CBitingPanic>			(this);
	stateHearDNE		= xr_new<CBloodsuckerHearDNE>	(this);

	Init();
}

CAI_Bloodsucker::~CAI_Bloodsucker()
{
	xr_delete(stateRest);
	xr_delete(stateEat);
	xr_delete(stateAttack);
	xr_delete(statePanic);
	xr_delete(stateHearDNE);
}


void CAI_Bloodsucker::Init()
{
	inherited::Init();

	CurrentState					= stateRest;
	CurrentState->Reset				();

	flagEatNow						= false;

	Bones.Reset();
}

void CAI_Bloodsucker::Load(LPCSTR section) 
{
	inherited::Load(section);

	m_tVisibility.Load(section);

	m_fInvisibilityDist = pSettings->r_float(section,"InvisibilityDist");
	m_ftrPowerDown		= pSettings->r_float(section,"PowerDownFactor");	
	m_fPowerThreshold	= pSettings->r_float(section,"PowerThreshold");	
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

	HUD().pFontSmall->OutSet	(300,400);
	HUD().pFontSmall->OutNext("Satiety = [%f]",GetSatiety());

	//	// Blink processing
	//	bool PrevVis	=	m_tVisibility.IsCurrentVisible();
	//	bool NewVis		=	m_tVisibility.Update();
	//	if (NewVis != PrevVis) setVisible(NewVis);

}

void CAI_Bloodsucker::Think()
{
	inherited::Think();
	
	if ((flagsEnemy & FLAG_ENEMY_GO_OFFLINE) == FLAG_ENEMY_GO_OFFLINE) {
		CurrentState->Reset();
		SetState(stateRest);
	}


//	int bone1		= PKinematics(Visual())->LL_BoneID("bip01_spine");
//	int bone2		= PKinematics(Visual())->LL_BoneID("bip01_head");
//	
//	static bool look_left = true;
//
//	if (!Bones.IsActive()) {
//
//		float look_k;
//		look_k = ((look_left) ? (-1.f) : 1.f);
//
//		look_left = !look_left;
//
//		// Колбасит
//		Bones.SetMotion(&PKinematics(Visual())->LL_GetInstance(bone1), AXIS_Z, look_k * PI_DIV_4 , PI, 1);
//		Bones.SetMotion(&PKinematics(Visual())->LL_GetInstance(bone1), AXIS_Y, look_k * PI_DIV_3 , PI_DIV_2 , 1);
//	}
//

	VisionElem ve;

	if (Motion.m_tSeq.isActive())	{
		Motion.m_tSeq.Cycle(m_dwCurrentUpdate);
	}else {
		
		//- FSM 1-level 
		if (C && H && I)			SetState(statePanic);
		else if (C && H && !I)		SetState(statePanic);
		else if (C && !H && I)		SetState(statePanic);
		else if (C && !H && !I) 	SetState(statePanic);
		else if (D && H && I)		SetState(stateAttack);
		else if (D && H && !I)		SetState(stateAttack);  
		else if (D && !H && I)		SetState(statePanic);
		else if (D && !H && !I) 	SetState(statePanic);	
		else if (E && H && I)		SetState(stateAttack); 
		else if (E && H && !I)  	SetState(stateAttack);  
		else if (E && !H && I) 		SetState(stateAttack); 
		else if (E && !H && !I)		SetState(stateAttack); 
		else if (F && H && I) 		SetState(stateAttack); 		
		else if (F && H && !I)  	SetState(stateAttack); 
		else if (F && !H && I)  	SetState(stateAttack); 
		else if (F && !H && !I) 	SetState(stateAttack);		
		else if (GetCorpse(ve) && (ve.obj->m_fFood > 1) && ((GetSatiety() < 0.85f) || flagEatNow))
			SetState(stateEat);	
		else SetState(stateRest);
		//-

		//SetState(stateHearDNE);

		CurrentState->Execute(m_dwCurrentUpdate);

		// проверяем на завершённость
		if (CurrentState->CheckCompletion()) SetState(stateRest, true);
	}

	Motion.SetFrameParams(this);
	ControlAnimation();		
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
	Bones.AddBone(&PKinematics(Visual())->LL_GetInstance(bone1), AXIS_X);
	Bones.AddBone(&PKinematics(Visual())->LL_GetInstance(bone1), AXIS_Z);
	Bones.AddBone(&PKinematics(Visual())->LL_GetInstance(bone1), AXIS_Y);
	Bones.AddBone(&PKinematics(Visual())->LL_GetInstance(bone2), AXIS_X);
	Bones.AddBone(&PKinematics(Visual())->LL_GetInstance(bone2), AXIS_Y);
	Bones.AddBone(&PKinematics(Visual())->LL_GetInstance(bone2), AXIS_Z);

}

void CAI_Bloodsucker::LoadAttackAnim()
{
	Fvector center;
	center.set		(0.f,0.f,0.f);

	float	impulse = 1200.f;
	
	Fvector	dir;
	Fvector tdir;
	float yaw, pitch;
	
	dir.set(XFORM().k);
	dir.getHP(yaw,pitch);
	dir.invert();

	// 1 //
	tdir = dir;		// работаем с временной копией
	tdir.setHP(angle_normalize(yaw+PI_DIV_6),pitch);
	m_tAttackAnim.PushAttackAnim(0, 9, 0, 600,	700,	center,		1.3f, m_fHitPower, tdir, impulse);

	// 2 //
	m_tAttackAnim.PushAttackAnim(0, 9, 1, 600,	700,	center,		1.3f, m_fHitPower, dir, impulse);

	// 3 // 
	tdir = dir;
	tdir.setHP(angle_normalize(yaw-PI_DIV_3),pitch);
	m_tAttackAnim.PushAttackAnim(0, 9, 2, 600,	700,	center,		1.4f, m_fHitPower, tdir, impulse);
}
