#include "stdafx.h"
#include "ai_bloodsucker.h"

// temp
#include "..\\..\\hudmanager.h"

CAI_Bloodsucker::CAI_Bloodsucker()
{
	stateRest			= xr_new<CBloodsuckerRest>		(this);
	stateEat			= xr_new<CBloodsuckerEat>		(this);

	Init();
}

CAI_Bloodsucker::~CAI_Bloodsucker()
{
	xr_delete(stateRest);
	xr_delete(stateEat);
}


void CAI_Bloodsucker::Init()
{
	inherited::Init();

	CurrentState					= stateRest;
	CurrentState->Reset				();

	flagEatNow						= false;

	Bones.Init();
}


void CAI_Bloodsucker::Think()
{
	inherited::Think();
	
	if ((flagsEnemy & FLAG_ENEMY_GO_OFFLINE) == FLAG_ENEMY_GO_OFFLINE) {
		CurrentState->Reset();
		SetState(stateRest);
	}

	VisionElem ve;

	if (Motion.m_tSeq.isActive())	{
		Motion.m_tSeq.Cycle(m_dwCurrentUpdate);
	}else {
		
		//- FSM 1-level 
		if (GetCorpse(ve) && (ve.obj->m_fFood > 1) && ((GetSatiety() < 0.85f) || flagEatNow))
			SetState(stateEat);	
		else SetState(stateRest);
		//-

		CurrentState->Execute(m_dwCurrentUpdate);

		// проверяем на завершённость
		if (CurrentState->CheckCompletion()) SetState(stateRest, true);
	}

	Motion.SetFrameParams(this);
	ControlAnimation();		
}

void CAI_Bloodsucker::UpdateCL()
{
	inherited::UpdateCL();

	HUD().pFontSmall->OutSet	(300,400);
	HUD().pFontSmall->OutNext("Satiety = [%f]",GetSatiety());

	// Blink processing
	bool PrevVis	=	m_tVisibility.IsVisible();
	bool NewVis		=	m_tVisibility.Update();
	if (NewVis != PrevVis) setVisible(NewVis);
	
// Temp /////////////////////////////////////////////////////	
	static TTime lastChange = 0;
	if (!m_tVisibility.IsActive())
		if (lastChange + 2000 < Level().timeServer()) {
			lastChange = Level().timeServer();
			m_tVisibility.Switch(!m_tVisibility.IsVisible());
		}
////////////////////////////////////////////////////////////
	
}

void CAI_Bloodsucker::Load(LPCSTR section) 
{
	inherited::Load(section);

	m_tVisibility.Load(section);
}


BOOL CAI_Bloodsucker::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);
	
	vfAssignBones	();

	return(TRUE);
}

void __stdcall CAI_Bloodsucker::BoneCallback(CBoneInstance *B)
{
	CAI_Bloodsucker*	this_class = dynamic_cast<CAI_Bloodsucker*> (static_cast<CObject*>(B->Callback_Param));

	this_class->Bones.Update(Level().timeServer());
}


void CAI_Bloodsucker::vfAssignBones()
{
	// ----
	int bone1		= PKinematics(Visual())->LL_BoneID("bip01_spine");
	PKinematics(Visual())->LL_GetInstance(bone1).set_callback(BoneCallback,this);

	// ----
	int bone2	= PKinematics(Visual())->LL_BoneID("bip01_head");
	PKinematics(Visual())->LL_GetInstance(bone2).set_callback(BoneCallback,this);

	// Bones settings
	xr_vector<bonesBone> bones;
	
	bonesBone bone;
//	bone.Setup(&PKinematics(Visual())->LL_GetInstance(bone1), AXIS_X);
//	bones.push_back(bone);
	bone.Setup(&PKinematics(Visual())->LL_GetInstance(bone2), AXIS_X);
	bones.push_back(bone);

	Bones.Init();
	Bones.SetupMotionBones(bones);
}


