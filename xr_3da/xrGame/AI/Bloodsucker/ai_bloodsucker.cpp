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



//void CBones::Update()
//{
//	if (bones.empty()) return;
//
//	bool bAllReached = true;
//	for (int i=0; i<bones.size(); i++) { 
//		if ((getAI().bfTooSmallAngle(bones[i].cur_yaw,bones[i].target_yaw,EPS_L)) && 
//			fsimilar(bones[i].cur_yaw, 0, EPS_L)) {
//			bones[i] = bones.back();
//			bones.push_back();
//		}
//	}
//	
//	if (bAllReached) bones.clear();
//}
