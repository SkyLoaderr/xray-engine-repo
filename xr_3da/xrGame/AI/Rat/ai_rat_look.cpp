////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat_misc.cpp
//	Created 	: 23.07.2002
//  Modified 	: 23.07.2002
//	Author		: Dmitriy Iassenev
//	Description : Visibility and look for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_rat.h"
#include "..\\..\\xr_weapon_list.h"

#define MIN_SPINE_TURN_ANGLE			PI_DIV_6
#define	MAX_HEAD_TURN_ANGLE				(PI/3.f)

bool CAI_Rat::bfCheckForVisibility(CEntity* tpEntity)
{
	return(true);
//	float fResult = 0.f;
//	
//	// computing maximum viewable distance in the specified direction
//	Fvector tCurrentWatchDirection, tTemp;
//	tCurrentWatchDirection.setHP	(r_torso_current.yaw,r_torso_current.pitch);
//	tCurrentWatchDirection.normalize();
//	tTemp.sub(tpEntity->Position(),vPosition);
//	tTemp.normalize();
//	float fAlpha = tCurrentWatchDirection.dotproduct(tTemp), fEyeFov = eye_fov*PI/180.f;
//	//float fAlpha = tWatchDirection.dotproduct(tTemp), fEyeFov = eye_fov*PI/180.f;
//	clamp(fAlpha,-.99999f,+.99999f);
//	fAlpha = acosf(fAlpha);
//	float fMaxViewableDistanceInDirection = eye_range*(1 - fAlpha/(fEyeFov/m_fLateralMultiplier));
//	
//	// computing distance weight
//	tTemp.sub(vPosition,tpEntity->Position());
//	fResult += tTemp.magnitude() >= fMaxViewableDistanceInDirection ? 0.f : m_fDistanceWeight*(1.f - tTemp.magnitude()/fMaxViewableDistanceInDirection);
//	
//	// computing movement speed weight
//	if (tpEntity->ps_Size() > 1) {
//		DWORD dwTime = tpEntity->ps_Element(tpEntity->ps_Size() - 1).dwTime;
//		if (dwTime < m_dwMovementIdleTime) {
//			tTemp.sub(tpEntity->ps_Element(tpEntity->ps_Size() - 2).vPosition,tpEntity->ps_Size() - 1);
//			float fSpeed = tTemp.magnitude()/dwTime;
//			fResult += fSpeed < m_fMaxInvisibleSpeed ? m_fMovementSpeedWeight*fSpeed/m_fMaxInvisibleSpeed : m_fMovementSpeedWeight;
//		}
//	}
//	
//	// computing lightness weight
//	fResult *= m_fShadowWeight*float(tpEntity->AI_Node->light)/255.f;
//	
//	// computing enemy state
//	switch (m_cBodyState) {
//		case BODY_STATE_STAND : {
//			break;
//		}
//		case BODY_STATE_CROUCH : {
//			fResult *= m_fCrouchVisibilityMultiplier;
//			break;
//		}
//		case BODY_STATE_LIE : {
//			fResult *= m_fLieVisibilityMultiplier;
//			break;
//		}
//	}
//	
//	// computing my ability to view the enemy
//	fResult += m_fCurSpeed < m_fMaxViewableSpeed ? m_fSpeedWeight*(1.f - m_fCurSpeed/m_fMaxViewableSpeed) : m_fSpeedWeight;
//	
//	return(fResult >= m_fVisibilityThreshold);
}

void CAI_Rat::SetDirectionLook()
{
	int i = ps_Size();
	if (i > 1) {
		CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		tWatchDirection.sub(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
		if (tWatchDirection.magnitude() > EPS_L) {
			tWatchDirection.normalize();
			mk_rotation(tWatchDirection,r_torso_target);
			//r_spine_target.yaw = r_torso_target.yaw;
			//r_spine_target.yaw = r_target.yaw = 0;
			//q_look.o_look_speed=PI_DIV_4;
		}
		else
			r_torso_target.pitch = 0;
	}
	//r_target.pitch = 0;
	//r_torso_target.pitch = 0;
}

void CAI_Rat::vfAimAtEnemy()
{
	Fvector	pos1, pos2;
	Enemy.Enemy->svCenter(pos1);
	svCenter(pos2);
	tWatchDirection.sub(pos1,pos2);
	mk_rotation(tWatchDirection,r_torso_target);
	r_target.yaw = r_torso_target.yaw;
	//r_target.yaw += PI_DIV_6;
	//r_torso_target.yaw = r_torso_target.yaw - 2*PI_DIV_6;//EYE_WEAPON_DELTA;
	//q_look.o_look_speed=_FB_look_speed;
	ADJUST_BONE_ANGLES
}

static BOOL __fastcall RatQualifier(CObject* O, void* P)
{
	if (O->CLS_ID!=CLSID_ENTITY)			
		return FALSE;
	else  {
		CEntity* E = dynamic_cast<CEntity*> (O);
		if (!E) return FALSE;
		if (!E->IsVisibleForAI()) return FALSE; 
		return TRUE;
	}
}

objQualifier* CAI_Rat::GetQualifier	()
{
	return(&RatQualifier);
}

void CAI_Rat::vfUpdateDynamicObjects()
{

}