////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_callbacks.cpp
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation manager : bone callbacks
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_animation_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "sight_manager.h"
#include "stalker_movement_manager.h"

static const float y_spin_factor			= 0.25f;
static const float y_shoulder_factor		= 0.25f;
static const float y_head_factor			= 0.5f;
static const float p_spin_factor			= 0.0f;
static const float p_shoulder_factor		= 0.0f;
static const float p_head_factor			= 1.0f;

static const float y_spin_fire_factor		= 0.5f;
static const float y_shoulder_fire_factor	= 0.5f;
static const float y_head_fire_factor		= 0.0f;
static const float p_spin_fire_factor		= 0.5f;
static const float p_shoulder_fire_factor	= 0.5f;
static const float p_head_fire_factor		= 0.0f;

void __stdcall head_callback	(CBoneInstance *B)
{
	CAI_Stalker*			A = static_cast<CAI_Stalker*>(B->Callback_Param);
	Fvector c				= B->mTransform.c;
	Fmatrix					spin;
	float					yaw_factor = 0, pitch_factor = 0;
	if (A->sight().use_torso_look()) {
		yaw_factor			= y_head_fire_factor;
		pitch_factor		= p_head_fire_factor;
	}
	else {
		yaw_factor			= y_head_factor;
		pitch_factor		= p_head_factor;
	}
	float					yaw		= angle_normalize_signed(-yaw_factor * angle_normalize_signed(A->movement().head_orientation().current.yaw - (A->movement().body_orientation().current.yaw)));
	float					pitch	= angle_normalize_signed(-pitch_factor * (A->NET_Last.o_torso.pitch));

	spin.setXYZ				(pitch, yaw, 0);
	VERIFY					(_valid(spin));
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

void __stdcall shoulder_callback(CBoneInstance *B)
{
	CAI_Stalker*			A = static_cast<CAI_Stalker*>(B->Callback_Param);
	Fvector c				= B->mTransform.c;
	Fmatrix					spin;
	float					yaw_factor = 0, pitch_factor = 0;
	if (A->sight().use_torso_look()) {
		yaw_factor			= y_shoulder_fire_factor;
		pitch_factor		= p_shoulder_fire_factor;
	}
	else {
		yaw_factor			= y_shoulder_factor;
		pitch_factor		= p_shoulder_factor;
	}
	float					yaw		= angle_normalize_signed(-yaw_factor * angle_normalize_signed(A->movement().head_orientation().current.yaw - (A->movement().body_orientation().current.yaw)));
	float					pitch	= angle_normalize_signed(-pitch_factor * (A->NET_Last.o_torso.pitch));
	spin.setXYZ				(pitch, yaw, 0);
	VERIFY					(_valid(spin));
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

void __stdcall spine_callback(CBoneInstance *B)
{
	CAI_Stalker*			A = static_cast<CAI_Stalker*>(B->Callback_Param);
	Fvector c				= B->mTransform.c;
	Fmatrix					spin;
	float					yaw_factor = 0, pitch_factor = 0;
	if (A->sight().use_torso_look()) {
		yaw_factor			= y_spin_fire_factor;
		pitch_factor		= p_spin_fire_factor;
	}
	else {
		yaw_factor			= y_spin_factor;
		pitch_factor		= p_spin_factor;
	}
	float					yaw		= angle_normalize_signed(-yaw_factor * angle_normalize_signed(A->movement().head_orientation().current.yaw - (A->movement().body_orientation().current.yaw)));
	float					pitch	= angle_normalize_signed(-pitch_factor * (A->NET_Last.o_torso.pitch));
	spin.setXYZ				(pitch, yaw, 0);
	VERIFY					(_valid(spin));
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

void CStalkerAnimationManager::assign_bone_callbacks	()
{
	CKinematics						*kinematics = smart_cast<CSkeletonAnimated*>(m_visual);
	VERIFY							(kinematics);
	
	LPCSTR							section = *object().cNameSect();
	
	int								head_bone = kinematics->LL_BoneID(pSettings->r_string(section,"bone_head"));
	kinematics->LL_GetBoneInstance	(u16(head_bone)).set_callback(head_callback,&object());

	int								shoulder_bone = kinematics->LL_BoneID(pSettings->r_string(section,"bone_shoulder"));
	kinematics->LL_GetBoneInstance	(u16(shoulder_bone)).set_callback(shoulder_callback,&object());

	int								spin_bone = kinematics->LL_BoneID(pSettings->r_string(section,"bone_spin"));
	kinematics->LL_GetBoneInstance	(u16(spin_bone)).set_callback(spine_callback,&object());
}
