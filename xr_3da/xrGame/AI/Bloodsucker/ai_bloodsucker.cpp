#include "stdafx.h"
#include "ai_bloodsucker.h"

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

	Bones.Init();
}


void CAI_Bloodsucker::Think()
{
	inherited::Think();
	
	if (flagEnemyGoOffline) {
		CurrentState->Reset();
		SetState(stateRest);
	}

	VisionElem ve;

	if (Motion.m_tSeq.isActive())	{
		Motion.m_tSeq.Cycle(m_dwCurrentUpdate);
	}else {
		
		//- FSM 1-level 
		if (GetCorpse(ve) && (ve.obj->m_fFood > 1) && (GetSatiety() > 0.85f))
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
}


void CAI_Bloodsucker::MotionToAnim(EMotionAnim motion, int &index1, int &index2, int &index3)
{
	index1 = index2 = 0;		// bug protection ;) todo: find out the reason
	index3 = -1;

	switch(motion) {
		case eMotionStandIdle:		index1 = 0; index2 = 0;	 break;
		case eMotionLieIdle:		index1 = 1; index2 = 0;	 break;
		case eMotionStandTurnLeft:	index1 = 0; index2 = 0;	 break;
		case eMotionWalkFwd:		index1 = 0; index2 = 2;	 break;
		case eMotionWalkBkwd:		index1 = 0; index2 = 2;  break;
		case eMotionWalkTurnLeft:	index1 = 0; index2 = 0;  break;
		case eMotionWalkTurnRight:	index1 = 0; index2 = 0;  break;
		case eMotionRun:			index1 = 0; index2 = 6;  break;
		case eMotionRunTurnLeft:	index1 = 0; index2 = 6;  break;
		case eMotionRunTurnRight:	index1 = 0; index2 = 6;  break;
		case eMotionAttack:			index1 = 0; index2 = 9;  break;
		case eMotionAttackRat:		index1 = 0; index2 = 9;	 break;
		case eMotionFastTurnLeft:	index1 = 0; index2 = 0;  break;
		case eMotionEat:			index1 = 1; index2 = 12; break;
		case eMotionStandDamaged:	index1 = 0; index2 = 0;  break;
		case eMotionScared:			index1 = 0; index2 = 0;  break;
		case eMotionDie:			index1 = 0; index2 = 0; break;
		case eMotionLieDown:		index1 = 0; index2 = 16; break;
		case eMotionStandUp:		index1 = 1; index2 = 17; break;
		case eMotionCheckCorpse:	index1 = 0; index2 = 3;	 index3 = 0;	break;
		case eMotionLieDownEat:		index1 = 0; index2 = 16; break;
		case eMotionAttackJump:		index1 = 0; index2 = 0;  break;
			///default:					NODEFAULT;
	} 

	if (index3 == -1) index3 = ::Random.randI((int)m_tAnimations.A[index1].A[index2].A.size());

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


