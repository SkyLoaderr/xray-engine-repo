////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_fire.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Visibility and look for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "..\\ai_monsters_misc.h"

static BOOL __fastcall StalkerQualifier(CObject* O, void* P)
{
	if (O->CLS_ID!=CLSID_ENTITY)			
		return FALSE;
	else {
		CEntityAlive* E = dynamic_cast<CEntityAlive*> (O);
		if (!E) return FALSE;
		if (!E->IsVisibleForAI()) return FALSE; 
		return TRUE;
	}
}

objQualifier* CAI_Stalker::GetQualifier	()
{
	return(&StalkerQualifier);
}

void CAI_Stalker::OnVisible	()
{
	inherited::OnVisible	();

	CWeapon* W				= Weapons->ActiveWeapon();
	if (W)					W->OnVisible		();
}

bool CAI_Stalker::bfCheckForVisibility(CEntity* tpEntity)
{
	if (Level().iGetKeyState(DIK_RCONTROL))
		return(false);

	float fResult = 0.f;
	
	// computing maximum viewable distance in the specified direction
	Fvector tCurrentWatchDirection, tTemp;
	tCurrentWatchDirection.setHP	(-r_current.yaw,-r_current.pitch);
	tCurrentWatchDirection.normalize();
	tTemp.sub(tpEntity->Position(),vPosition);
	if (tTemp.magnitude() > EPS_L)
		tTemp.normalize();
	else
		return(true);
	float fAlpha = tCurrentWatchDirection.dotproduct(tTemp), fEyeFov = eye_fov*PI/180.f;
	//float fAlpha = tWatchDirection.dotproduct(tTemp), fEyeFov = eye_fov*PI/180.f;
	clamp(fAlpha,-.99999f,+.99999f);
	fAlpha = acosf(fAlpha);
	float fMaxViewableDistanceInDirection = eye_range*(1 - fAlpha/(fEyeFov/m_fLateralMultiplier));
	
	// computing distance weight
	tTemp.sub(vPosition,tpEntity->Position());
	fResult += tTemp.magnitude() >= fMaxViewableDistanceInDirection ? 0.f : m_fDistanceWeight*(1.f - tTemp.magnitude()/fMaxViewableDistanceInDirection);
	
	// computing movement speed weight
	if (tpEntity->ps_Size() > 1) {
		u32 dwTime = tpEntity->ps_Element(tpEntity->ps_Size() - 1).dwTime;
		if (dwTime < m_dwMovementIdleTime) {
			tTemp.sub(tpEntity->ps_Element(tpEntity->ps_Size() - 2).vPosition,tpEntity->ps_Element(tpEntity->ps_Size() - 1).vPosition);
			float fSpeed = tTemp.magnitude()/dwTime;
			fResult += fSpeed < m_fMaxInvisibleSpeed ? m_fMovementSpeedWeight*fSpeed/m_fMaxInvisibleSpeed : m_fMovementSpeedWeight;
		}
	}
	
	// computing my ability to view the enemy
	fResult += m_fCurSpeed < m_fMaxViewableSpeed ? m_fSpeedWeight*(1.f - m_fCurSpeed/m_fMaxViewableSpeed) : m_fSpeedWeight;
	
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
	
	// computing lightness weight
	float fTemp = float(tpEntity->AI_Node->light)/255.f;
	if (fTemp < .05f)
		fResult = 0.f;
	else
		fResult += m_fShadowWeight*fTemp;
	
	return(fResult >= m_fVisibilityThreshold);
}

void CAI_Stalker::SetDirectionLook()
{
	int i = ps_Size	();
	if (i > 1) {
		CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		tWatchDirection.sub(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
		if (tWatchDirection.magnitude() > EPS_L) {
			tWatchDirection.normalize();
			mk_rotation(tWatchDirection,r_torso_target);
		}
	}
	r_target.yaw = r_torso_target.yaw;
}

void CAI_Stalker::SetLook(Fvector tPosition)
{
	Fvector tCurrentPosition = vPosition;
	tWatchDirection.sub(tPosition,tCurrentPosition);
	if (tWatchDirection.magnitude() > EPS_L) {
		tWatchDirection.normalize();
		mk_rotation(tWatchDirection,r_torso_target);
	}
	r_target.yaw = r_torso_target.yaw;
}

void CAI_Stalker::SetLessCoverLook()
{
	if (AI_Path.TravelPath.empty() || (AI_Path.TravelStart >= AI_Path.TravelPath.size() - 1))
		if (ps_Size())
			SetLessCoverLook(AI_Node);
		else {
			float fAngleOfView = eye_fov/180.f*PI, fMaxSquare = 0.f;
			for (float fIncrement = 0.f; fIncrement < PI_MUL_2; fIncrement += 2*MAX_HEAD_TURN_ANGLE/60.f) {
				float fSquare0 = ffCalcSquare(fIncrement,fAngleOfView,AI_Node);
				if (fSquare0 > fMaxSquare) {
					fMaxSquare = fSquare0;
					r_torso_target.yaw = r_target.yaw = fIncrement;
				}
			}
		}
	else {
		SetDirectionLook();
		
		NodeCompressed *tpNextNode = 0;
		bool bOk = false;
		NodeLink *taLinks = (NodeLink *)((BYTE *)AI_Node + sizeof(NodeCompressed));
		int iCount = AI_Node->links;
		for (int i=0; i<iCount; i++) {
			tpNextNode = getAI().Node(getAI().UnpackLink(taLinks[i]));
 			if (getAI().bfInsideNode(tpNextNode,AI_Path.TravelPath[AI_Path.TravelStart + 1].P)) {
				bOk = true;
				break;
			}
		}
		if (!bOk)
			SetLessCoverLook(AI_Node);
		else {
			float fAngleOfView = eye_fov/180.f*PI, fMaxSquare = 0.f;
			
			for (float fIncrement = r_torso_current.yaw - MAX_HEAD_TURN_ANGLE; fIncrement <= r_torso_current.yaw + MAX_HEAD_TURN_ANGLE; fIncrement += 2*MAX_HEAD_TURN_ANGLE/60.f) {
				float fSquare0 = ffCalcSquare(fIncrement,fAngleOfView,AI_Node);
				float fSquare1 = ffCalcSquare(fIncrement,fAngleOfView,tpNextNode);
				if (fSquare1 - fSquare0 > fMaxSquare) {
					fMaxSquare = fSquare1 - fSquare0;
					r_target.yaw = fIncrement;
				}
			}
		}
	}
}

void CAI_Stalker::SetLessCoverLook(NodeCompressed *tpNode)
{
	SetLessCoverLook(tpNode,MAX_HEAD_TURN_ANGLE);
}

void CAI_Stalker::SetLessCoverLook(NodeCompressed *tpNode, float fMaxHeadTurnAngle)
{
	int i = ps_Size();
	if (i > 1) {
		CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		tWatchDirection.sub(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
		if (tWatchDirection.square_magnitude() > 0.000001) {
			tWatchDirection.normalize();
			mk_rotation(tWatchDirection,r_torso_target);
			
			float fAngleOfView = eye_fov/180.f*PI, fMaxSquare = -1.f, fBestAngle = r_target.yaw;
			
			for (float fIncrement = r_torso_current.yaw - fMaxHeadTurnAngle; fIncrement <= r_torso_current.yaw + fMaxHeadTurnAngle; fIncrement += 2*fMaxHeadTurnAngle/60.f) {
				float fSquare = ffCalcSquare(fIncrement,fAngleOfView,tpNode);
				if (fSquare > fMaxSquare) {
					fMaxSquare = fSquare;
					fBestAngle = fIncrement;
				}
			}
			
			r_target.yaw = fBestAngle;
		}
	}
}

void CAI_Stalker::vfValidateAngleDependency(float x1, float &x2, float x3)
{
	float	_x2	= angle_normalize_signed(x2 - x1);
	float	_x3	= angle_normalize_signed(x3 - x1);
	if ((_x2*_x3 <= 0) && (_abs(_x2) + _abs(_x3) > PI - EPS_L))
		x2  = x3;
}

void CAI_Stalker::Exec_Look(float dt)
{
	// normalizing torso angles
	r_torso_current.yaw		= angle_normalize_signed	(r_torso_current.yaw);
	r_torso_current.pitch	= angle_normalize_signed	(r_torso_current.pitch);
	r_torso_target.yaw		= angle_normalize_signed	(r_torso_target.yaw);
	r_torso_target.pitch	= angle_normalize_signed	(r_torso_target.pitch);
	
	// normalizing head angles
	r_current.yaw			= angle_normalize_signed	(r_current.yaw);
	r_current.pitch			= angle_normalize_signed	(r_current.pitch);
	r_target.yaw			= angle_normalize_signed	(r_target.yaw);
	r_target.pitch			= angle_normalize_signed	(r_target.pitch);

	// validating angles
	vfValidateAngleDependency(r_torso_current.yaw,r_torso_target.yaw,r_current.yaw);
	vfValidateAngleDependency(r_current.yaw,r_target.yaw,r_torso_current.yaw);

	// updating torso angles
	angle_lerp_bounds		(r_torso_current.yaw,r_torso_target.yaw,r_torso_speed,dt);
	angle_lerp_bounds		(r_torso_current.pitch,r_torso_target.pitch,r_torso_speed,dt);
	
	// updating head angles
	angle_lerp_bounds		(r_current.yaw,r_target.yaw,r_head_speed,dt);
	angle_lerp_bounds		(r_current.pitch,r_target.pitch,r_head_speed,dt);

	// normalizing torso angles
	r_torso_current.yaw		= angle_normalize_signed	(r_torso_current.yaw);
	r_torso_current.pitch	= angle_normalize_signed	(r_torso_current.pitch);
	
	// normalizing head angles
	r_current.yaw			= angle_normalize_signed	(r_current.yaw);
	r_current.pitch			= angle_normalize_signed	(r_current.pitch);
	
	//Msg("After  : %f, %f, %f, %f",R2D(r_torso_current.yaw),R2D(r_torso_target.yaw),R2D(r_current.yaw),R2D(r_target.yaw));
	// updating rotation matrix
	mRotate.setHPB			(-NET_Last.o_model,0,0);
	
	// checking if we have to switch onto another task
	Engine.Sheduler.Slice	();
}