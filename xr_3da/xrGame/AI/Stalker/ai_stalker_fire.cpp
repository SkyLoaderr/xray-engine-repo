////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_fire.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Fire and enemy parameters for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../../WeaponMagazined.h"
#include "../../ai_script_actions.h"

#define TIME_FIRE_DELTA		400

float CAI_Stalker::HitScale	(int element)
{
	CKinematics* V		= PKinematics(Visual());			VERIFY(V);
	float scale			= fis_zero(V->LL_GetBoneInstance(u16(element)).get_param(0))?1.f:V->LL_GetBoneInstance(u16(element)).get_param(0);
	return				(m_default_hit_factor*scale);
}

void CAI_Stalker::g_fireParams(Fvector& P, Fvector& D)
{
	if (g_Alive()) {
		Center(P);
		D.setHP(-m_head.current.yaw,-m_head.current.pitch);
		D.normalize_safe();
	}
}

void CAI_Stalker::g_WeaponBones	(int &L, int &R1, int &R2)
{
	CKinematics *V	= PKinematics(Visual());
	R1				= V->LL_BoneID("bip01_r_hand");
	R2				= V->LL_BoneID("bip01_r_finger2");
	if ((IsLimping() && (m_tMentalState == eMentalStateFree)) || (GetCurrentAction() && !GetCurrentAction()->m_tAnimationAction.m_bHandUsage))
		L				= R2;
	else
		L				= V->LL_BoneID("bip01_l_finger1");
}

void CAI_Stalker::HitSignal(float amount, Fvector& vLocalDir, CObject* who, s16 element)
{
	if (g_Alive()) {
		// Play hit-ref_sound
		CSoundPlayer::play	(eStalkerSoundInjuring);
		Fvector				D;
		float				yaw, pitch;
		D.getHP				(yaw,pitch);
		CSkeletonAnimated	*tpKinematics = PSkeletonAnimated(Visual());
#pragma todo("Dima to Dima : forward-back bone impulse direction has been determined incorrectly!")
		CMotionDef			*tpMotionDef = m_tAnims.A[m_tBodyState].m_tGlobal.A[0].A[iFloor(tpKinematics->LL_GetBoneInstance(element).get_param(1) + (angle_difference(m_body.current.yaw,-yaw) <= PI_DIV_2 ? 0 : 1))];
		float				power_factor = 3.f*amount/100.f; clamp(power_factor,0.f,1.f);
		tpKinematics->PlayFX(tpMotionDef,power_factor);
	}
	add_hit_object		(amount,vLocalDir,who,element);
}

bool CAI_Stalker::bfCheckForNodeVisibility(u32 dwNodeID, bool /**bIfRayPick/**/)
{
	return				(CVisualMemoryManager::visible(dwNodeID,m_head.current.yaw));
}