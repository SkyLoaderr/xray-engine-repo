////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier_misc.cpp
//	Created 	: 23.07.2002
//  Modified 	: 23.07.2002
//	Author		: Dmitriy Iassenev
//	Description : Visibility and look for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_soldier.h"
#include "..\\..\\xr_weapon_list.h"

bool CAI_Soldier::bfCheckForVisibility(CEntity* tpEntity)
{
	float fResult = 0.f;
	
	// computing maximum viewable distance in the specified direction
	Fvector tCurrentWatchDirection, tTemp;
	tCurrentWatchDirection.setHP	(r_current.yaw,r_current.pitch);
	tCurrentWatchDirection.normalize();
	tTemp.sub(tpEntity->Position(),vPosition);
	tTemp.normalize();
	//float fAlpha = tCurrentWatchDirection.dotproduct(tTemp), fEyeFov = eye_fov*PI/180.f;
	float fAlpha = tWatchDirection.dotproduct(tTemp), fEyeFov = eye_fov*PI/180.f;
	clamp(fAlpha,-.99999f,+.99999f);
	fAlpha = acosf(fAlpha);
	float fMaxViewableDistanceInDirection = eye_range*(1 - fAlpha/(fEyeFov/1.5f));
	
	// computing distance weight
	tTemp.sub(vPosition,tpEntity->Position());
	fResult += tTemp.magnitude() >= fMaxViewableDistanceInDirection ? 0.f : m_fDistanceWeight*(1.f - tTemp.magnitude()/fMaxViewableDistanceInDirection);
	
	// computing movement speed weight
	if (tpEntity->ps_Size() > 1) {
		DWORD dwTime = tpEntity->ps_Element(tpEntity->ps_Size() - 1).dwTime;
		if (dwTime < m_dwMovementIdleTime) {
			tTemp.sub(tpEntity->ps_Element(tpEntity->ps_Size() - 2).vPosition,tpEntity->ps_Size() - 1);
			float fSpeed = tTemp.magnitude()/dwTime;
			fResult += fSpeed < m_fMaxInvisibleSpeed ? m_fMovementSpeedWeight*fSpeed/m_fMaxInvisibleSpeed : m_fMovementSpeedWeight;
		}
	}
	
	// computing lightness weight
	fResult *= 2*float(0 + tpEntity->AI_Node->light)/(0 + 255.f);
	
	// computing enemy state
	switch (m_cBodyState) {
	case BODY_STATE_STAND : {
		break;
							}
	case BODY_STATE_CROUCH : {
		fResult *= m_fCrouchVisibilityMultiplier;
		break;
							 }
	case BODY_STATE_LIE : {
		fResult *= m_fLieVisibilityMultiplier;
		break;
						  }
	}
	
	// computing my ability to view the enemy
	fResult += m_fCurSpeed < m_fMaxViewableSpeed ? m_fSpeedWeight*(1.f - m_fCurSpeed/m_fMaxViewableSpeed) : m_fSpeedWeight;
	
	return(fResult >= m_fVisibilityThreshold);
}

static BOOL __fastcall SoldierQualifier(CObject* O, void* P)
{
	if (O->CLS_ID!=CLSID_ENTITY)			
		return FALSE;
	else
		return TRUE;
}

objQualifier* CAI_Soldier::GetQualifier	()
{
	return(&SoldierQualifier);
}

void CAI_Soldier::OnVisible()
{
	inherited::OnVisible();
	
	Weapons->OnRender(FALSE);
	//return(0);
}

void CAI_Soldier::SetDirectionLook()
{
	int i = ps_Size();
	if (i > 1) {
		CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		tWatchDirection.sub(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
		if (tWatchDirection.square_magnitude() > EPS_L) {
			tWatchDirection.normalize();
			mk_rotation(tWatchDirection,r_torso_target);
			r_target.yaw = r_torso_target.yaw;
			ASSIGN_SPINE_BONE;
			r_target.yaw += PI_DIV_6;
			q_look.o_look_speed=PI_DIV_4;
		}
	}
	//r_target.pitch = 0;
	//r_torso_target.pitch = 0;
}

float ffCalcSquare(float fAngle, float fAngleOfView, float _b0, float _b1, float _b2, float _b3)
{
	fAngle = fAngle >= PI ? 2*PI - fAngle : fAngle;
	float fSquare;
	if ((fAngle >= -PI/2 + fAngleOfView/2) && (fAngle < fAngleOfView/2))
		if (fAngle < PI/2 - fAngleOfView/2) {
			float b0 = _b0, b1 = _b1, b2 = _b2, b3 = _b3, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), fKsi = fAngleOfView - fAngle;
			fSquare = (CUBE(PI)*(_sqr(a1) + _sqr(a2))/24.f + _sqr(PI)*(a1*b1 + a2*b2)/4.f + PI*(_sqr(b1) + _sqr(b2))/2.f - (CUBE(fAngle)*_sqr(a1)/3.f + _sqr(fAngle)*a1*b1 + fAngle*_sqr(b1) + CUBE(fKsi)*_sqr(a2)/3.f + _sqr(fKsi)*a2*b2 + fKsi*_sqr(b2)))/2.f;
		}
		else {
			float b0 = _b1, b1 = _b2, b2 = _b3, b3 = _b0, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), a3 = (b1 - b3)/(PI/2.f), fKsi = PI- fAngleOfView - fAngle;
			fSquare = (CUBE(PI)*(_sqr(a3) + _sqr(a2))/24.f + _sqr(PI)*(a3*b3 + a2*b2)/4.f + PI*(_sqr(b3) + _sqr(b2))/2.f - (CUBE(fAngle)*_sqr(a1)/3.f + _sqr(fAngle)*a1*b1 + fAngle*_sqr(b1) + CUBE(fKsi)*_sqr(a3)/3.f + _sqr(fKsi)*a3*b3 + fKsi*_sqr(b3)))/2.f;
		}
		else
			if ((fAngle - PI/2 >= -PI/2 + fAngleOfView/2) && (fAngle - PI/2 < fAngleOfView/2))
				if (fAngle - PI/2 < PI/2 - fAngleOfView/2) {
					float b0 = _b1, b1 = _b2, b2 = _b3, b3 = _b0, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), fKsi = fAngleOfView - fAngle;;
					fSquare = (CUBE(PI)*(_sqr(a1) + _sqr(a2))/24.f + _sqr(PI)*(a1*b1 + a2*b2)/4.f + PI*(_sqr(b1) + _sqr(b2))/2.f - (CUBE(fAngle)*_sqr(a1)/3.f + _sqr(fAngle)*a1*b1 + fAngle*_sqr(b1) + CUBE(fKsi)*_sqr(a2)/3.f + _sqr(fKsi)*a2*b2 + fKsi*_sqr(b2)))/2.f;
				}
				else {
					float b0 = _b2, b1 = _b3, b2 = _b0, b3 = _b1, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), a3 = (b1 - b3)/(PI/2.f), fKsi = PI- fAngleOfView - fAngle;
					fSquare = (CUBE(PI)*(_sqr(a3) + _sqr(a2))/24.f + _sqr(PI)*(a3*b3 + a2*b2)/4.f + PI*(_sqr(b3) + _sqr(b2))/2.f - (CUBE(fAngle)*_sqr(a1)/3.f + _sqr(fAngle)*a1*b1 + fAngle*_sqr(b1) + CUBE(fKsi)*_sqr(a3)/3.f + _sqr(fKsi)*a3*b3 + fKsi*_sqr(b3)))/2.f;
				}
				else
					if ((fAngle - PI >= -PI/2 + fAngleOfView/2) && (fAngle - PI < fAngleOfView/2))
						if (fAngle - PI < PI/2 - fAngleOfView/2) {
							float b0 = _b2, b1 = _b3, b2 = _b0, b3 = _b1, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), fKsi = fAngleOfView - fAngle;;
							fSquare = (CUBE(PI)*(_sqr(a1) + _sqr(a2))/24.f + _sqr(PI)*(a1*b1 + a2*b2)/4.f + PI*(_sqr(b1) + _sqr(b2))/2.f - (CUBE(fAngle)*_sqr(a1)/3.f + _sqr(fAngle)*a1*b1 + fAngle*_sqr(b1) + CUBE(fKsi)*_sqr(a2)/3.f + _sqr(fKsi)*a2*b2 + fKsi*_sqr(b2)))/2.f;
						}
						else {
							float b0 = _b3, b1 = _b0, b2 = _b1, b3 = _b2, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), a3 = (b1 - b3)/(PI/2.f), fKsi = PI- fAngleOfView - fAngle;
							fSquare = (CUBE(PI)*(_sqr(a3) + _sqr(a2))/24.f + _sqr(PI)*(a3*b3 + a2*b2)/4.f + PI*(_sqr(b3) + _sqr(b2))/2.f - (CUBE(fAngle)*_sqr(a1)/3.f + _sqr(fAngle)*a1*b1 + fAngle*_sqr(b1) + CUBE(fKsi)*_sqr(a3)/3.f + _sqr(fKsi)*a3*b3 + fKsi*_sqr(b3)))/2.f;
						}
						else
							if (fAngle - 3*PI/2 < PI/2 - fAngleOfView/2) {
								float b0 = _b3, b1 = _b0, b2 = _b1, b3 = _b2, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), fKsi = fAngleOfView - fAngle;;
								fSquare = (CUBE(PI)*(_sqr(a1) + _sqr(a2))/24.f + _sqr(PI)*(a1*b1 + a2*b2)/4.f + PI*(_sqr(b1) + _sqr(b2))/2.f - (CUBE(fAngle)*_sqr(a1)/3.f + _sqr(fAngle)*a1*b1 + fAngle*_sqr(b1) + CUBE(fKsi)*_sqr(a2)/3.f + _sqr(fKsi)*a2*b2 + fKsi*_sqr(b2)))/2.f;
							}
							else {
								float b0 = _b0, b1 = _b1, b2 = _b2, b3 = _b3, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), a3 = (b1 - b3)/(PI/2.f), fKsi = PI- fAngleOfView - fAngle;
								fSquare = (CUBE(PI)*(_sqr(a3) + _sqr(a2))/24.f + _sqr(PI)*(a3*b3 + a2*b2)/4.f + PI*(_sqr(b3) + _sqr(b2))/2.f - (CUBE(fAngle)*_sqr(a1)/3.f + _sqr(fAngle)*a1*b1 + fAngle*_sqr(b1) + CUBE(fKsi)*_sqr(a3)/3.f + _sqr(fKsi)*a3*b3 + fKsi*_sqr(b3)))/2.f;
							}
							return(fSquare);
}

void CAI_Soldier::SetLessCoverLook(NodeCompressed *tNode)
{
	int i = ps_Size();
	if (i > 1) {
		CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		tWatchDirection.sub(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
		if (tWatchDirection.square_magnitude() > 0.000001) {
			tWatchDirection.normalize();
			mk_rotation(tWatchDirection,r_torso_target);
			
			float fAngleOfView = eye_fov/180.f*PI, fMaxSquare = -1.f, fBestAngle;
			
			for (float fIncrement = r_torso_current.yaw - MAX_HEAD_TURN_ANGLE; fIncrement <= r_torso_current.yaw + MAX_HEAD_TURN_ANGLE; fIncrement += 2*MAX_HEAD_TURN_ANGLE/60.f) {
				float fSquare = ffCalcSquare(fIncrement,fAngleOfView,FN(1),FN(2),FN(3),FN(0));
				if (fSquare > fMaxSquare) {
					fMaxSquare = fSquare;
					fBestAngle = fIncrement;
				}
			}
			
			r_target.yaw = fBestAngle;
			ASSIGN_SPINE_BONE;
			q_look.o_look_speed = PI_DIV_4;
			//r_torso_speed = _FB_look_speed;//(r_torso_target.yaw - r_torso_current.yaw);
			r_target.yaw += PI_DIV_6;
		}
	}
	//r_target.pitch = 0;
	//r_torso_target.pitch = 0;
}

void CAI_Soldier::SetSmartLook(NodeCompressed *tNode, Fvector &tEnemyDirection)
{
	Fvector tLeft;
	Fvector tRight;
	Fvector tFront;
	Fvector tBack;
	tLeft.set(-1,0,0);
	tRight.set(1,0,0);
	tFront.set(0,0,1);
	tBack.set(0,0,-1);

	tEnemyDirection.normalize();
	
	/**
	float fCover = 0;
	if ((tEnemyDirection.x < 0) && (tEnemyDirection.z > 0)) {
	float fAlpha = acosf(tEnemyDirection.dotproduct(tFront));
	fCover = cosf(fAlpha)*tNode->cover[0] + sinf(fAlpha)*tNode->cover[1];
	}
	else 
	if ((tEnemyDirection.x > 0) && (tEnemyDirection.z > 0)) {
	float fAlpha = acosf(tEnemyDirection.dotproduct(tFront));
	fCover = cosf(fAlpha)*tNode->cover[1] + sinf(fAlpha)*tNode->cover[2];
	}
	else 
	if ((tEnemyDirection.x > 0) && (tEnemyDirection.z < 0)) {
				float fAlpha = acosf(tEnemyDirection.dotproduct(tBack));
				fCover = cosf(fAlpha)*tNode->cover[3] + sinf(fAlpha)*tNode->cover[2];
				}
				else 
				if ((tEnemyDirection.x < 0) && (tEnemyDirection.z < 0)) {
				float fAlpha = acosf(tEnemyDirection.dotproduct(tBack));
				fCover = cosf(fAlpha)*tNode->cover[3] + sinf(fAlpha)*tNode->cover[0];
				}
				
				  
					//Msg("%8.2f",fCover);
					if (fCover > -1.0f*255.f) {
	/**/
	//q_look.setup(AI::AIC_Look::Look, AI::t_Direction, &(tEnemyDirection), 1000);
	mk_rotation(tEnemyDirection,r_torso_target);
	r_target.yaw = r_torso_target.yaw;
	r_torso_target.yaw = r_torso_target.yaw - EYE_WEAPON_DELTA;
	r_target.pitch *= -1;
	q_look.o_look_speed=8*_FB_look_speed;
	/**
	}
	else {
	SetLessCoverLook(tNode);
	q_look.o_look_speed=8*_FB_look_speed;
	}
	/**/
}

void CAI_Soldier::vfAimAtEnemy()
{
	Fvector	pos1, pos2;
	Enemy.Enemy->svCenter(pos1);
	svCenter(pos2);
	tWatchDirection.sub(pos1,pos2);
	mk_rotation(tWatchDirection,r_torso_target);
	r_target.yaw = r_torso_target.yaw;
	ASSIGN_SPINE_BONE;
	//r_target.yaw += 2*PI_DIV_6;
	//r_torso_target.yaw = r_torso_target.yaw - 2*PI_DIV_6;//EYE_WEAPON_DELTA;
	q_look.o_look_speed=_FB_look_speed;
}
