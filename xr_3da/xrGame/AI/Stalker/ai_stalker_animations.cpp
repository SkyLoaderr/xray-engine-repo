////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_animations.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Animations, Bone transformations and Sounds for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "ai_stalker_animations.h"

static const float y_spin_factor		= 0.4f;
static const float y_shoulder_factor	= 0.4f;
static const float y_head_factor		= 0.2f;
static const float p_spin_factor		= 0.2f;
static const float p_shoulder_factor	= 0.7f;
static const float p_head_factor		= 0.1f;

LPCSTR caStateNames			[dwStateCount] = {
	"cr_",
	"norm_",
};

LPCSTR caWeaponNames		[dwWeaponCount] = {
	"0_",
	"1_",
	"2_",
	"3_",
};

LPCSTR caWeaponActionNames	[dwWeaponActionCount] = {
	"aim_",
	"attack_",
	"draw_",
	"drop_",
	"holster_",
	"reload_",
	"pick_",
};

LPCSTR caMovementNames		[dwMovementCount] = {
	"run_",
	"walk_",
};

LPCSTR caMovementActionNames[dwMovementActionCount] = {
	"fwd_",
	"back_",
	"ls_",
	"rs_",
};

LPCSTR caInPlaceNames		[dwInPlaceCount] = {
	"idle",
	"turn",
	"jump_begin",
	"jump_idle",
	"jump_end",
	"jump_end_1",
};

LPCSTR caGlobalNames		[dwGlobalCount] = {
	"death_",
	"damage_",
};


#define TORSO_ANGLE_DELTA		(PI/30.f)

void CAI_Stalker::vfAssignBones(CInifile *ini, const char *section)
{
	int head_bone		= PKinematics(pVisual)->LL_BoneID(ini->ReadSTRING(section,"bone_head"));
	PKinematics(pVisual)->LL_GetInstance(head_bone).set_callback(HeadCallback,this);
	
	int shoulder_bone	= PKinematics(pVisual)->LL_BoneID(ini->ReadSTRING(section,"bone_shoulder"));
	PKinematics(pVisual)->LL_GetInstance(shoulder_bone).set_callback(ShoulderCallback,this);
	
	int spin_bone		= PKinematics(pVisual)->LL_BoneID(ini->ReadSTRING(section,"bone_spin"));
	PKinematics(pVisual)->LL_GetInstance(spin_bone).set_callback(SpinCallback,this);
}

void __stdcall CAI_Stalker::HeadCallback(CBoneInstance *B)
{
	CAI_Stalker*			A = dynamic_cast<CAI_Stalker*> (static_cast<CObject*>(B->Callback_Param));
	Fvector c				= B->mTransform.c;
	Fmatrix					spin;
	spin.setXYZ				(angle_normalize_signed(A->r_current.pitch - A->r_torso_current.pitch)*p_head_factor, angle_normalize_signed(-(A->r_current.yaw - A->r_torso_current.yaw)*y_head_factor), 0);
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

void __stdcall CAI_Stalker::ShoulderCallback(CBoneInstance *B)
{
	CAI_Stalker*			A = dynamic_cast<CAI_Stalker*> (static_cast<CObject*>(B->Callback_Param));
	Fvector c				= B->mTransform.c;
	Fmatrix					spin;
	spin.setXYZ				(angle_normalize_signed(A->r_current.pitch - A->r_torso_current.pitch)*p_shoulder_factor, angle_normalize_signed(-(A->r_current.yaw - A->r_torso_current.yaw)*y_shoulder_factor), 0);
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

void __stdcall CAI_Stalker::SpinCallback(CBoneInstance *B)
{
	CAI_Stalker*			A = dynamic_cast<CAI_Stalker*> (static_cast<CObject*>(B->Callback_Param));
	Fvector c				= B->mTransform.c;
	Fmatrix					spin;
	spin.setXYZ				(angle_normalize_signed(A->r_current.pitch - A->r_torso_current.pitch)*p_spin_factor, angle_normalize_signed(-(A->r_current.yaw - A->r_torso_current.yaw)*y_spin_factor), 0);
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

void __stdcall CAI_Stalker::LegsCallback(CBoneInstance *B)
{
//	CAI_Stalker*		A = dynamic_cast<CAI_Stalker*> (static_cast<CObject*>(B->Callback_Param));
	
//	Fmatrix				spin;
//	float				bone_yaw	= A->r_torso_current.yaw - A->r_model_yaw - A->r_model_yaw_delta;
//	float				bone_pitch	= A->r_torso_current.pitch;
//	clamp				(bone_pitch,-PI_DIV_8,PI_DIV_4);
//	spin.setXYZ			(bone_yaw,bone_pitch,0);
//	B->mTransform.mulB_43(spin);
}

void CAI_Stalker::vfAssignGlobalAnimation(CMotionDef *&tpGlobalAnimation)
{
	if (g_Health() <= 0)
		tpGlobalAnimation = m_tAnims.A[1]->m_tGlobal.A[0]->A[0];
}

void CAI_Stalker::vfAssignTorsoAnimation(CMotionDef *&tpTorsoAnimation)
{
	if (Weapons->ActiveWeapon())
		if (m_eCurrentState == eStalkerStateRecharge) {
			switch (Weapons->ActiveWeaponID()) {
				case 0 : {
					tpTorsoAnimation = m_tAnims.A[1]->m_tTorso.A[3]->A[5]->A[0];
					break;
				}
				case 1 : {
					tpTorsoAnimation = m_tAnims.A[1]->m_tTorso.A[1]->A[5]->A[0];
					break;
				}
				case 2 : {
					tpTorsoAnimation = m_tAnims.A[1]->m_tTorso.A[2]->A[5]->A[0];
					break;
				}
			}
		}
		else {
			switch (Weapons->ActiveWeaponID()) {
				case 0 : {
					tpTorsoAnimation = m_tAnims.A[1]->m_tTorso.A[3]->A[0]->A[0];
					break;
				}
				case 1 : {
					tpTorsoAnimation = m_tAnims.A[1]->m_tTorso.A[1]->A[0]->A[0];
					break;
				}
				case 2 : {
					tpTorsoAnimation = m_tAnims.A[1]->m_tTorso.A[2]->A[0]->A[0];
					break;
				}
			}
		}
	else 
		tpTorsoAnimation = m_tAnims.A[1]->m_tTorso.A[0]->A[0]->A[0];
}

void CAI_Stalker::vfAssignLegsAnimation(CMotionDef *&tpLegsAnimation)
{
	if (m_fCurSpeed > EPS_L)
		tpLegsAnimation		= m_tAnims.A[m_tBodyState]->m_tMoves.A[m_tMovementType]->A[0]->A[0];
	else
		tpLegsAnimation		= m_tAnims.A[1]->m_tInPlace.A[0];
}

void CAI_Stalker::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
	CKinematics				&tVisualObject		=	*(PKinematics(pVisual));
	CMotionDef				*tpGlobalAnimation	=	0;
	CMotionDef				*tpTorsoAnimation	=	0;
	CMotionDef				*tpLegsAnimation	=	0;

	vfAssignGlobalAnimation	(tpGlobalAnimation);
	vfAssignTorsoAnimation	(tpTorsoAnimation);
	vfAssignLegsAnimation	(tpLegsAnimation);

	if ((tpGlobalAnimation) && (m_tpCurrentGlobalAnimation != tpGlobalAnimation))
		tVisualObject.PlayCycle(m_tpCurrentGlobalAnimation = tpGlobalAnimation);
	else {
		if ((tpTorsoAnimation) && (m_tpCurrentTorsoAnimation != tpTorsoAnimation))
			tVisualObject.PlayCycle(m_tpCurrentTorsoAnimation = tpTorsoAnimation);
		if ((tpLegsAnimation) && (m_tpCurrentLegsAnimation != tpLegsAnimation))
			tVisualObject.PlayCycle(m_tpCurrentLegsAnimation = tpLegsAnimation);
	}
}
