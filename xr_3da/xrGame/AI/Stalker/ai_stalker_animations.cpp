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
#include "..\..\..\motion.h"

static const float y_spin_factor			= 0.25f;
static const float y_shoulder_factor		= 0.25f;
static const float y_head_factor			= 0.5f;
static const float p_spin_factor			= 0.2f;
static const float p_shoulder_factor		= 0.7f;
static const float p_head_factor			= 0.1f;

static const float y_spin_fire_factor		= 0.5f;
static const float y_shoulder_fire_factor	= 0.5f;
static const float y_head_fire_factor		= 0.0f;
static const float p_spin_fire_factor		= 0.2f;
static const float p_shoulder_fire_factor	= 0.7f;
static const float p_head_fire_factor		= 0.1f;

float faTurnAngles			[] = {
	0.f,
	M_PI,
	PI_DIV_6,
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
	"4_",
	0
};

LPCSTR caWeaponActionNames	[] = {
	"draw_",
	"attack_",
	"drop_",
	"holster_",
	"reload_",
	"pick_",
	"aim_",
	"walk_",
	"run_",
	"idle_",
	"escape_",
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
	"ls_",
	"rs_",
	0
};

LPCSTR caInPlaceNames		[] = {
	"idle_1",
	"turn",
	"jump_begin",
	"jump_idle",
	"jump_end",
	"jump_end_1",
	0
};

LPCSTR caGlobalNames		[] = {
//	"death_",
	"damage_",
	"escape_",
	0
};

void CAI_Stalker::vfAssignBones(CInifile *ini, const char *section)
{
	int head_bone		= PKinematics(pVisual)->LL_BoneID(ini->r_string(section,"bone_head"));
	PKinematics(pVisual)->LL_GetInstance(head_bone).set_callback(HeadCallback,this);
	
	int shoulder_bone	= PKinematics(pVisual)->LL_BoneID(ini->r_string(section,"bone_shoulder"));
	PKinematics(pVisual)->LL_GetInstance(shoulder_bone).set_callback(ShoulderCallback,this);
	
	int spin_bone		= PKinematics(pVisual)->LL_BoneID(ini->r_string(section,"bone_spin"));
	PKinematics(pVisual)->LL_GetInstance(spin_bone).set_callback(SpinCallback,this);
}

void __stdcall CAI_Stalker::HeadCallback(CBoneInstance *B)
{
	CAI_Stalker*			A = dynamic_cast<CAI_Stalker*> (static_cast<CObject*>(B->Callback_Param));
	Fvector c				= B->mTransform.c;
	Fmatrix					spin;
	float					yaw_factor = 0, pitch_factor = 0;
	switch(A->m_tLookType) {
		case eLookTypeLookFireOver :
		case eLookTypeFirePoint : {
			yaw_factor		= y_head_fire_factor;
			pitch_factor	= p_head_fire_factor;
			break;
		}
		case eLookTypeLookOver :
		case eLookTypeDirection :
		case eLookTypeSearch :
		case eLookTypeDanger :
		case eLookTypePoint : {
			yaw_factor		= y_head_factor;
			pitch_factor	= p_head_factor;
			break;
		}
		default : NODEFAULT;
	}
	float					yaw		= angle_normalize_signed(-yaw_factor * angle_normalize_signed(A->NET_Last.o_torso.yaw - A->NET_Last.o_model));
	float					pitch	= angle_normalize_signed(-pitch_factor * (A->NET_Last.o_torso.pitch));
	spin.setXYZ				(pitch, yaw, 0);
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

void __stdcall CAI_Stalker::ShoulderCallback(CBoneInstance *B)
{
	CAI_Stalker*			A = dynamic_cast<CAI_Stalker*> (static_cast<CObject*>(B->Callback_Param));
	Fvector c				= B->mTransform.c;
	Fmatrix					spin;
	float					yaw_factor = 0, pitch_factor = 0;
	switch(A->m_tLookType) {
		case eLookTypeLookFireOver :
		case eLookTypeFirePoint : {
			yaw_factor		= y_shoulder_fire_factor;
			pitch_factor	= p_shoulder_fire_factor;
			break;
		}
		case eLookTypeLookOver :
		case eLookTypeDirection :
		case eLookTypeSearch :
		case eLookTypeDanger :
		case eLookTypePoint : {
			yaw_factor		= y_shoulder_factor;
			pitch_factor	= p_shoulder_factor;
			break;
		}
		default : NODEFAULT;
	}
	float					yaw		= angle_normalize_signed(-yaw_factor * angle_normalize_signed(A->NET_Last.o_torso.yaw - A->NET_Last.o_model));
	float					pitch	= angle_normalize_signed(-pitch_factor * (A->NET_Last.o_torso.pitch));
	spin.setXYZ				(pitch, yaw, 0);
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

void __stdcall CAI_Stalker::SpinCallback(CBoneInstance *B)
{
	CAI_Stalker*			A = dynamic_cast<CAI_Stalker*> (static_cast<CObject*>(B->Callback_Param));
	Fvector c				= B->mTransform.c;
	Fmatrix					spin;
	float					yaw_factor = 0, pitch_factor = 0;
	switch(A->m_tLookType) {
		case eLookTypeLookFireOver :
		case eLookTypeFirePoint : {
			yaw_factor		= y_spin_fire_factor;
			pitch_factor	= p_spin_fire_factor;
			break;
		}
		case eLookTypeLookOver :
		case eLookTypeDirection :
		case eLookTypeSearch :
		case eLookTypeDanger :
		case eLookTypePoint : {
			yaw_factor		= y_spin_factor;
			pitch_factor	= p_spin_factor;
			break;
		}
		default : NODEFAULT;
	}
	float					yaw		= angle_normalize_signed(-yaw_factor * angle_normalize_signed(A->NET_Last.o_torso.yaw - A->NET_Last.o_model));
	float					pitch	= angle_normalize_signed(-pitch_factor * (A->NET_Last.o_torso.pitch));
	spin.setXYZ				(pitch, yaw, 0);
	B->mTransform.mulA_43	(spin);
	B->mTransform.c			= c;
}

void CAI_Stalker::vfAssignGlobalAnimation(CMotionDef *&tpGlobalAnimation)
{
	if (g_Alive() && (m_tStateType == eStateTypePanic) && (AI_Path.fSpeed > EPS_L))
		tpGlobalAnimation = m_tAnims.A[m_tBodyState].m_tGlobal.A[2].A[0];
}

void CAI_Stalker::vfAssignTorsoAnimation(CMotionDef *&tpTorsoAnimation)
{
	if (!g_Alive())
		return;
	
	CWeapon *tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
	u32		dwCurrentAniSlot = 0;
	if (tpWeapon)
		switch (tpWeapon->SUB_CLS_ID) {
			case CLSID_OBJECT_W_M134		:
			case CLSID_OBJECT_W_FN2000		:
			case CLSID_OBJECT_W_AK74		:
			case CLSID_OBJECT_W_LR300		: 
			case CLSID_OBJECT_W_SVU			:
			case CLSID_OBJECT_W_SVD			: {
				dwCurrentAniSlot = 2;
				break;
			}
			case CLSID_OBJECT_W_HPSA		:
			case CLSID_OBJECT_W_PM			:
			case CLSID_OBJECT_W_FORT		: {
				dwCurrentAniSlot = 1;
				break;
			}
			case CLSID_OBJECT_W_BINOCULAR	: {
				dwCurrentAniSlot = 5;
				break;
			}
			case CLSID_OBJECT_W_SHOTGUN		: {
				dwCurrentAniSlot = 3;
				break;
			}
			case CLSID_OBJECT_W_RPG7		: {
				dwCurrentAniSlot = 4;
				break;
			}
		}
	if (m_tStateType == eStateTypeNormal) {
		tpTorsoAnimation = 0;
		VERIFY(m_tBodyState == eBodyStateStand);
		if (m_tMovementType == eMovementTypeStand)
			tpTorsoAnimation = m_tAnims.A[m_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[9].A[0];
		else
			tpTorsoAnimation = m_tAnims.A[m_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[7 + m_tMovementType].A[1];
		return;
	}
	if (m_inventory.ActiveItem()) {
		CWeapon *tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
		if (tpWeapon) {
			switch (tpWeapon->STATE) {
				case CWeapon::eIdle : {
					tpTorsoAnimation = m_tAnims.A[m_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[6].A[m_tStateType];
					break;
				}
				case CWeapon::eReload : {
					tpTorsoAnimation = m_tAnims.A[m_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[4].A[0];
					break;
				}
				case CWeapon::eShowing : {
					tpTorsoAnimation = m_tAnims.A[m_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[0].A[0];
					break;
				}
				case CWeapon::eHiding : {
					tpTorsoAnimation = m_tAnims.A[m_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[3].A[0];
					break;
				}
				case CWeapon::eFire : {
					tpTorsoAnimation = m_tAnims.A[m_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[1].A[0];
					break;
				}
				case CWeapon::eFire2 : {
					tpTorsoAnimation = m_tAnims.A[m_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[1].A[0];
					break;
				}
				default :
					tpTorsoAnimation = m_tAnims.A[m_tBodyState].m_tTorso.A[dwCurrentAniSlot].A[6].A[0];
			}
		}
	}
	else
		tpTorsoAnimation = m_tAnims.A[m_tBodyState].m_tTorso.A[0].A[6].A[0];
}

void CAI_Stalker::vfAssignLegsAnimation(CMotionDef *&tpLegsAnimation)
{
	if (!g_Alive())
		return;
	if ((AI_Path.fSpeed < EPS_L) || (m_tMovementType == eMovementTypeStand)) {
		// standing
		if (getAI().bfTooSmallAngle(r_torso_target.yaw,r_torso_current.yaw,PI_DIV_6)) {
			tpLegsAnimation		= m_tAnims.A[m_tBodyState].m_tInPlace.A[0];
		}
		else {
			tpLegsAnimation		= m_tAnims.A[m_tBodyState].m_tInPlace.A[1];
		}
		return;
	}
	if (m_tStateType != eStateTypeDanger) {
		tpLegsAnimation = m_tAnims.A[m_tBodyState].m_tMoves.A[m_tMovementType].A[eMovementDirectionForward].A[m_tStateType];
		return;
	}
	// moving
	float					yaw, pitch;
	GetDirectionAngles		(yaw,pitch);
	yaw						= angle_normalize_signed(-yaw);
//	float					fAngleDifference = _abs(angle_normalize_signed(r_torso_current.yaw - r_current.yaw));
	float					fAnimationSwitchFactor = 1.f;//fAngleDifference < PI_DIV_2 ? 1.f : 1.f - (fAngleDifference - PI_DIV_2)/(PI - PI_DIV_2);
	if	(((m_tDesirableDirection == eMovementDirectionForward) && (m_tMovementDirection == eMovementDirectionBack))	||	((m_tDesirableDirection == eMovementDirectionBack) && (m_tMovementDirection == eMovementDirectionForward)))
		fAnimationSwitchFactor = .0f;
	if	(((m_tDesirableDirection == eMovementDirectionLeft) && (m_tMovementDirection == eMovementDirectionRight))	||	((m_tDesirableDirection == eMovementDirectionLeft) && (m_tMovementDirection == eMovementDirectionRight)))
		fAnimationSwitchFactor = .0f;

	if (getAI().bfTooSmallAngle(yaw,r_current.yaw,MAX_HEAD_TURN_ANGLE)) {
		// moving forward
		if (m_tMovementDirection == eMovementDirectionForward)
			m_dwDirectionStartTime	= Level().timeServer();
		else
			if (m_tDesirableDirection != eMovementDirectionForward) {
				m_dwDirectionStartTime	= Level().timeServer();
				m_tDesirableDirection	= eMovementDirectionForward;
			}
			else
				if ((Level().timeServer() - m_dwDirectionStartTime) > m_dwAnimationSwitchInterval*fAnimationSwitchFactor) {
					m_dwDirectionStartTime	= Level().timeServer();
					m_tMovementDirection	= eMovementDirectionForward;
				}
	}
	else {
		if (getAI().bfTooSmallAngle(yaw,r_current.yaw,4*PI_DIV_6))
			// moving left|right
			if (getAI().bfTooSmallAngle(yaw,angle_normalize_signed(r_current.yaw + PI_DIV_2),PI_DIV_4)) {
				// moving right, looking left
				if (m_tMovementDirection == eMovementDirectionRight)
					m_dwDirectionStartTime	= Level().timeServer();
				else
					if (m_tDesirableDirection != eMovementDirectionRight) {
						m_dwDirectionStartTime	= Level().timeServer();
						m_tDesirableDirection	= eMovementDirectionRight;
					}
					else
						if ((Level().timeServer() - m_dwDirectionStartTime) > m_dwAnimationSwitchInterval*fAnimationSwitchFactor) {
							m_dwDirectionStartTime	= Level().timeServer();
							m_tMovementDirection	= eMovementDirectionRight;
						}
			}
			else {
				// moving left, looking right
				if (m_tMovementDirection == eMovementDirectionLeft)
					m_dwDirectionStartTime	= Level().timeServer();
				else
					if (m_tDesirableDirection != eMovementDirectionLeft) {
						m_dwDirectionStartTime	= Level().timeServer();
						m_tDesirableDirection	= eMovementDirectionLeft;
					}
					else
						if ((Level().timeServer() - m_dwDirectionStartTime) > m_dwAnimationSwitchInterval*fAnimationSwitchFactor) {
							m_dwDirectionStartTime	= Level().timeServer();
							m_tMovementDirection	= eMovementDirectionLeft;
						}
			}
		else {
			// moving back
			if (m_tMovementDirection == eMovementDirectionBack)
				m_dwDirectionStartTime	= Level().timeServer();
			else
				if (m_tDesirableDirection != eMovementDirectionBack) {
					m_dwDirectionStartTime	= Level().timeServer();
					m_tDesirableDirection	= eMovementDirectionBack;
				}
				else
					if ((Level().timeServer() - m_dwDirectionStartTime) > m_dwAnimationSwitchInterval*fAnimationSwitchFactor) {
						m_dwDirectionStartTime	= Level().timeServer();
						m_tMovementDirection	= eMovementDirectionBack;
					}
		}
	}
//	Msg("----%d\n[W=%7.2f][TT=%7.2f][TC=%7.2f][T=%7.2f][C=%7.2f]",Level().timeServer(),yaw,r_torso_target.yaw,r_torso_current.yaw,r_target.yaw,r_current.yaw);
//	Msg("Trying %s\nMoving %s",caMovementActionNames[m_tDesirableDirection],caMovementActionNames[m_tMovementDirection]);
	tpLegsAnimation	= m_tAnims.A[m_tBodyState].m_tMoves.A[m_tMovementType].A[m_tMovementDirection].A[0];
	r_torso_target.yaw		= angle_normalize_signed(yaw + faTurnAngles[m_tMovementDirection]);
//	Msg("[W=%7.2f][TT=%7.2f][TC=%7.2f][T=%7.2f][C=%7.2f]",yaw,r_torso_target.yaw,r_torso_current.yaw,r_target.yaw,r_current.yaw);
}

void CAI_Stalker::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
	CKinematics				&tVisualObject		=	*(PKinematics(pVisual));

	if (m_tAnims.A.empty())
		return;

	CMotionDef				*tpGlobalAnimation	=	0;
	vfAssignGlobalAnimation	(tpGlobalAnimation);

	if (tpGlobalAnimation) {
		if (m_tpCurrentGlobalAnimation != tpGlobalAnimation)
			m_tpCurrentGlobalBlend = tVisualObject.PlayCycle(m_tpCurrentGlobalAnimation = tpGlobalAnimation);
	}
	else
		if (g_Alive()) {
			CMotionDef				*tpTorsoAnimation	=	0;
			CMotionDef				*tpLegsAnimation	=	0;
			m_tpCurrentGlobalBlend	= 0;
			vfAssignTorsoAnimation	(tpTorsoAnimation);
			vfAssignLegsAnimation	(tpLegsAnimation);
			
			if (tpTorsoAnimation && (m_tpCurrentTorsoAnimation != tpTorsoAnimation))
				m_tpCurrentTorsoBlend	= tVisualObject.PlayCycle(m_tpCurrentTorsoAnimation = tpTorsoAnimation);
			if (tpLegsAnimation && (m_tpCurrentLegsAnimation != tpLegsAnimation))
				m_tpCurrentLegsBlend	= tVisualObject.PlayCycle(m_tpCurrentLegsAnimation = tpLegsAnimation);
			
			if (tpTorsoAnimation && tpLegsAnimation){
				if ((tpTorsoAnimation->flags & esmSyncPart) && (tpLegsAnimation->flags & esmSyncPart))
					if (m_tpCurrentTorsoBlend && m_tpCurrentLegsBlend)
						m_tpCurrentTorsoBlend->timeCurrent = m_tpCurrentLegsBlend->timeCurrent;
			}
		}
		else {
//			tVisualObject.PlayCycle(m_tAnims.A[2]);
		}
}
