#include "stdafx.h"
#include "ai_bloodsucker.h"

CAI_Bloodsucker::CAI_Bloodsucker()
{
	stateRest			= xr_new<CBloodsuckerRest>		(this);
	CurrentState		= stateRest;

	Init();
}

CAI_Bloodsucker::~CAI_Bloodsucker()
{
	xr_delete(stateRest);
}


void CAI_Bloodsucker::Init()
{
	inherited::Init();

	CurrentState					= stateRest;
	CurrentState->Reset				();

	Bones.Init();
}


void CAI_Bloodsucker::Think()
{
	inherited::Think();
	
	VisionElem ve;

	if (Motion.m_tSeq.isActive())	{
		Motion.m_tSeq.Cycle(m_dwCurrentUpdate);
	}else {
		//- FSM 1-level 
		SetState(stateRest); 
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
}


void CAI_Bloodsucker::MotionToAnim(EMotionAnim motion, int &index1, int &index2, int &index3)
{
	index1 = 0; index2 = 0;	 index3 = -1;
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
}


