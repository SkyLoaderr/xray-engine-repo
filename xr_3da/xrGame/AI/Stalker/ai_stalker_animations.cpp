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

float faTurnAngles			[4] = {
	0.f,
	PI,
	2*PI_DIV_6,
	0.f,
};

LPCSTR caStateNames			[] = {
	"cr_",
	"norm_",
	0
};

LPCSTR caWeaponNames		[] = {
	"0_",
	"1_",
	"2_",
	"3_",
	0
};

LPCSTR caWeaponActionNames	[] = {
	"aim_",
	"attack_",
	"draw_",
	"drop_",
	"holster_",
	"reload_",
	"pick_",
	0
};

LPCSTR caMovementNames		[] = {
	"walk_",
	"run_",
	0
};

LPCSTR caMovementActionNames[] = {
	"fwd_",
	"back_",
	"rs_",
	"ls_",
	0
};

LPCSTR caInPlaceNames		[] = {
	"idle",
	"turn",
	"jump_begin",
	"jump_idle",
	"jump_end",
	"jump_end_1",
	0
};

LPCSTR caGlobalNames		[] = {
	"death_",
	"damage_",
	0
};

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
	float					yaw_factor, pitch_factor;
	switch(A->m_tLookType) {
		case eLookTypeView : {
			yaw_factor		= 1.f;
			pitch_factor	= p_head_factor;
			break;
		}
		case eLookTypeSearch : {
			yaw_factor		= y_head_factor;
			pitch_factor	= p_head_factor;
			break;
		}
		default : {
			yaw_factor		= y_head_factor;
			pitch_factor	= p_head_factor;
		}
	}
	float					yaw		= yaw_factor * angle_normalize_signed(-(A->r_current.yaw - A->r_torso_current.yaw));
	float					pitch	= pitch_factor * angle_normalize_signed(A->r_current.pitch - A->r_torso_current.pitch);
	spin.setXYZ				(pitch, yaw, 0);
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

void __stdcall CAI_Stalker::ShoulderCallback(CBoneInstance *B)
{
	CAI_Stalker*			A = dynamic_cast<CAI_Stalker*> (static_cast<CObject*>(B->Callback_Param));
	Fvector c				= B->mTransform.c;
	Fmatrix					spin;
	float					yaw_factor, pitch_factor;
	switch(A->m_tLookType) {
		case eLookTypeView : {
			yaw_factor		= 0.f;
			pitch_factor	= p_shoulder_factor;
			break;
		}
		case eLookTypeSearch : {
			yaw_factor		= y_shoulder_factor;
			pitch_factor	= p_shoulder_factor;
			break;
		}
		default : {
			yaw_factor		= y_shoulder_factor;
			pitch_factor	= p_shoulder_factor;
		}
	}
	float					yaw		= yaw_factor * angle_normalize_signed(-(A->r_current.yaw - A->r_torso_current.yaw));
	float					pitch	= pitch_factor * angle_normalize_signed(A->r_current.pitch - A->r_torso_current.pitch);
	spin.setXYZ				(pitch, yaw, 0);
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

void __stdcall CAI_Stalker::SpinCallback(CBoneInstance *B)
{
	CAI_Stalker*			A = dynamic_cast<CAI_Stalker*> (static_cast<CObject*>(B->Callback_Param));
	Fvector c				= B->mTransform.c;
	Fmatrix					spin;
	float					yaw_factor, pitch_factor;
	switch(A->m_tLookType) {
		case eLookTypeView : {
			yaw_factor		= 0.f;
			pitch_factor	= p_spin_factor;
			break;
		}
		case eLookTypeSearch : {
			yaw_factor		= y_spin_factor;
			pitch_factor	= p_spin_factor;
			break;
		}
		default : {
			yaw_factor		= y_spin_factor;
			pitch_factor	= p_spin_factor;
		}
	}
	float					yaw		= yaw_factor * angle_normalize_signed(-(A->r_current.yaw - A->r_torso_current.yaw));
	float					pitch	= pitch_factor * angle_normalize_signed(A->r_current.pitch - A->r_torso_current.pitch);
	spin.setXYZ				(pitch, yaw, 0);
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
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
	if (m_fCurSpeed < EPS_L) {
		// standing
		tpLegsAnimation		= m_tAnims.A[1]->m_tInPlace.A[0];
		return;
	}
	Fvector					tDirection;
	float					yaw, pitch;
	
	int						i = ps_Size	();
	if (!i)
		return;
	
	CObject::SavedPosition	tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
	tDirection.sub			(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
	if (tDirection.magnitude() < EPS_L)
		return;
	
	tDirection.getHP		(yaw,pitch);
	yaw						= angle_normalize_signed(-yaw);
	// moving
	if (getAI().bfTooSmallAngle(yaw,r_current.yaw,MAX_HEAD_TURN_ANGLE)) {
		// moving forward
		Msg					("Trying forward");
		if (m_tMovementDirection == eMovementDirectionForward)
			m_dwDirectionStartTime	= Level().timeServer();
		else
			if (m_tDesirableDirection != eMovementDirectionForward)
				m_dwDirectionStartTime	= Level().timeServer();
			else
				if ((Level().timeServer() - m_dwDirectionStartTime) > m_dwAnimationSwitchInterval) {
					m_dwDirectionStartTime	= Level().timeServer();
					m_tMovementDirection	= eMovementDirectionForward;
					Msg						("Forward");
				}
	}
	else {
		if (getAI().bfTooSmallAngle(yaw,r_current.yaw,4*PI_DIV_6))
			// moving left|right
			if (getAI().bfTooSmallAngle(yaw,r_current.yaw + PI_DIV_2,PI_DIV_4)) {
				// moving left, looking right
				Msg				("Trying left");
				if (m_tMovementDirection == eMovementDirectionLeft)
					m_dwDirectionStartTime	= Level().timeServer();
				else
					if (m_tDesirableDirection != eMovementDirectionLeft)
						m_dwDirectionStartTime	= Level().timeServer();
					else
						if ((Level().timeServer() - m_dwDirectionStartTime) > m_dwAnimationSwitchInterval) {
							m_dwDirectionStartTime	= Level().timeServer();
							m_tMovementDirection	= eMovementDirectionLeft;
							Msg						("Left");
						}
			}
			else {
				// moving right, looking left
				Msg				("Trying right");
				if (m_tMovementDirection == eMovementDirectionRight)
					m_dwDirectionStartTime	= Level().timeServer();
				else
					if (m_tDesirableDirection != eMovementDirectionRight)
						m_dwDirectionStartTime	= Level().timeServer();
					else
						if ((Level().timeServer() - m_dwDirectionStartTime) > m_dwAnimationSwitchInterval) {
							m_dwDirectionStartTime	= Level().timeServer();
							m_tMovementDirection	= eMovementDirectionRight;
							Msg						("Right");
						}
			}
		else {
			// moving back
			Msg			("Trying back");
			if (m_tMovementDirection == eMovementDirectionBack)
				m_dwDirectionStartTime	= Level().timeServer();
			else
				if (m_tDesirableDirection != eMovementDirectionBack)
					m_dwDirectionStartTime	= Level().timeServer();
				else
					if ((Level().timeServer() - m_dwDirectionStartTime) > m_dwAnimationSwitchInterval) {
						m_dwDirectionStartTime	= Level().timeServer();
						m_tMovementDirection	= eMovementDirectionBack;
						Msg						("Back");
					}
		}
	}
	
	tpLegsAnimation			= m_tAnims.A[m_tBodyState]->m_tMoves.A[m_tMovementType]->A[m_tMovementDirection]->A[0];
	r_target.yaw			= angle_normalize_signed(r_target.yaw + r_torso_target.yaw);
	r_torso_target.yaw		= angle_normalize_signed(yaw + faTurnAngles[m_tMovementDirection]);
	r_target.yaw			= angle_normalize_signed(r_target.yaw - r_torso_target.yaw);
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
