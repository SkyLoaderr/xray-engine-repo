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

#define MIN_SOUND_VOLUME				.05f
#define SOUND_UPDATE_DELAY				3000

static BOOL __fastcall StalkerQualifier(CObject* O, void* P)
{
//	if (O->CLS_ID!=CLSID_ENTITY)			
//		return FALSE;
//	else 
	{
		CEntityAlive*	E = dynamic_cast<CEntityAlive*>	(O);
		CWeapon*		W = dynamic_cast<CWeapon*>		(O);
		if (!E && !W)
			return(FALSE);
		//if (!E->IsVisibleForAI())
		//	return(FALSE); 
		return(TRUE);
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
	return(true);
//	if (Level().iGetKeyState(DIK_RCONTROL))
//		return(false);
//
//	float fResult = 0.f;
//	
//	// computing maximum viewable distance in the specified direction
//	Fvector tCurrentWatchDirection, tTemp;
//	tCurrentWatchDirection.setHP	(-r_current.yaw,-r_current.pitch);
//	tCurrentWatchDirection.normalize();
//	tTemp.sub(tpEntity->Position(),vPosition);
//	if (tTemp.magnitude() > EPS_L)
//		tTemp.normalize();
//	else
//		return(true);
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
//		u32 dwTime = tpEntity->ps_Element(tpEntity->ps_Size() - 1).dwTime;
//		if (dwTime < m_dwMovementIdleTime) {
//			tTemp.sub(tpEntity->ps_Element(tpEntity->ps_Size() - 2).vPosition,tpEntity->ps_Element(tpEntity->ps_Size() - 1).vPosition);
//			float fSpeed = tTemp.magnitude()/dwTime;
//			fResult += fSpeed < m_fMaxInvisibleSpeed ? m_fMovementSpeedWeight*fSpeed/m_fMaxInvisibleSpeed : m_fMovementSpeedWeight;
//		}
//	}
//	
//	// computing my ability to view the enemy
//	fResult += m_fCurSpeed < m_fMaxViewableSpeed ? m_fSpeedWeight*(1.f - m_fCurSpeed/m_fMaxViewableSpeed) : m_fSpeedWeight;
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
	
	// computing lightness weight
//	float fTemp = float(tpEntity->AI_Node->light)/255.f;
//	if (fTemp < .05f)
//		fResult = 0.f;
//	else
//		fResult += m_fShadowWeight*fTemp;
//	
//	return(fResult >= m_fVisibilityThreshold);
}

void CAI_Stalker::SetDirectionLook()
{
	int i = ps_Size	();
	if (i > 1) {
		CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		tWatchDirection.sub(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
		if (tWatchDirection.magnitude() > EPS_L) {
			tWatchDirection.normalize();
			mk_rotation(tWatchDirection,r_target);
		}
	}
}

void CAI_Stalker::SetLook(Fvector tPosition)
{
	Fvector tCurrentPosition = vPosition;
	tWatchDirection.sub(tPosition,tCurrentPosition);
	if (tWatchDirection.magnitude() > EPS_L) {
		tWatchDirection.normalize();
		mk_rotation(tWatchDirection,r_target);
	}
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
					r_target.yaw = fIncrement;
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
			mk_rotation(tWatchDirection,r_target);
			
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
	//float					fAngleDifference = _abs(angle_normalize_signed(r_torso_current.yaw - r_torso_target.yaw));
	float					fSpeedFactor = 1.f;//fAngleDifference < PI_DIV_2 ? 1.f/6.f : 1.f;
	angle_lerp_bounds		(r_torso_current.yaw,r_torso_target.yaw,fSpeedFactor*r_torso_speed,dt);
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
	
	// updating rotation matrix
	mRotate.setHPB			(-NET_Last.o_model,0,0);
	
	// checking if we have to switch onto another task
	Engine.Sheduler.Slice	();
}

void CAI_Stalker::vfUpdateDynamicObjects()
{
	feel_vision_get(m_tpaVisibleObjects);
	u32 dwTime = m_dwCurrentUpdate;
	for (int i=0; i<(int)m_tpaVisibleObjects.size(); i++) {
		
		CEntity *tpEntity = dynamic_cast<CEntity *>(m_tpaVisibleObjects[i]);
		
		if (!tpEntity || !bfCheckForVisibility(tpEntity))
			continue;
		
		for (int j=0; j<(int)m_tpaDynamicObjects.size(); j++)
			if (tpEntity == m_tpaDynamicObjects[j].tpEntity) {
				m_tpaDynamicObjects[j].dwTime = dwTime;
				m_tpaDynamicObjects[j].dwUpdateCount++;
				m_tpaDynamicObjects[j].dwNodeID = tpEntity->AI_NodeID;
				m_tpaDynamicObjects[j].tSavedPosition = tpEntity->Position();
				m_tpaDynamicObjects[j].tOrientation = tfGetOrientation(tpEntity);
				m_tpaDynamicObjects[j].dwMyNodeID = AI_NodeID;
				m_tpaDynamicObjects[j].tMySavedPosition = Position();
				m_tpaDynamicObjects[j].tMyOrientation = r_torso_current;
				break;
			}
		
		if (j >= (int)m_tpaDynamicObjects.size()) {
			if ((int)m_tpaDynamicObjects.size() >= m_dwMaxDynamicObjectsCount)	{
				u32 dwBest = dwTime + 1, dwIndex = u32(-1);
				for (int j=0; j<(int)m_tpaDynamicObjects.size(); j++)
					if (m_tpaDynamicObjects[j].dwTime < dwBest) {
						dwIndex = i;
						dwBest = m_tpaDynamicObjects[j].dwTime;
					}
				if (dwIndex < m_tpaDynamicObjects.size()) {
					m_tpaDynamicObjects[dwIndex].dwTime = dwTime;
					m_tpaDynamicObjects[dwIndex].dwUpdateCount = 1;
					m_tpaDynamicObjects[dwIndex].dwNodeID = tpEntity->AI_NodeID;
					m_tpaDynamicObjects[dwIndex].tSavedPosition = tpEntity->Position();
					m_tpaDynamicObjects[dwIndex].tOrientation = tfGetOrientation(tpEntity);
					m_tpaDynamicObjects[dwIndex].dwMyNodeID = AI_NodeID;
					m_tpaDynamicObjects[dwIndex].tMySavedPosition = Position();
					m_tpaDynamicObjects[dwIndex].tMyOrientation = r_torso_current;
					m_tpaDynamicObjects[dwIndex].tpEntity = tpEntity;
				}
			}
			else {
				SDynamicObject tDynamicObject;
				tDynamicObject.dwTime = dwTime;
				tDynamicObject.dwUpdateCount = 1;
				tDynamicObject.dwNodeID = tpEntity->AI_NodeID;
				tDynamicObject.tSavedPosition = tpEntity->Position();
				tDynamicObject.tOrientation = tfGetOrientation(tpEntity);
				tDynamicObject.dwMyNodeID = AI_NodeID;
				tDynamicObject.tMySavedPosition = Position();
				tDynamicObject.tMyOrientation = r_torso_current;
				tDynamicObject.tpEntity = tpEntity;
				m_tpaDynamicObjects.push_back(tDynamicObject);
			}
		}
	}
}

int	 CAI_Stalker::ifFindDynamicObject(CEntity *tpEntity)
{
	for (int i=0; i<(int)m_tpaDynamicObjects.size(); i++)
		if (m_tpaDynamicObjects[i].tpEntity == tpEntity)
			return(i);
	return(-1);
}

bool CAI_Stalker::bfAddEnemyToDynamicObjects(CAI_Stalker *tpStalker)
{
	for (int i=0; i<(int)tpStalker->m_tpaDynamicObjects.size(); i++) {
		if (tpStalker->m_tpaDynamicObjects[i].tpEntity == m_tSavedEnemy) {
			u32 dwTime = m_dwCurrentUpdate;
			CEntity *tpEntity = m_tSavedEnemy;
			for (int j=0; j<(int)m_tpaDynamicObjects.size(); j++)
				if (m_tSavedEnemy == m_tpaDynamicObjects[j].tpEntity) {
					m_tpaDynamicObjects[j].dwTime = tpStalker->m_tpaDynamicObjects[i].dwTime;
					m_tpaDynamicObjects[j].dwUpdateCount++;
					m_tpaDynamicObjects[j].dwNodeID = tpStalker->m_tpaDynamicObjects[i].dwNodeID;
					m_tpaDynamicObjects[j].tSavedPosition = tpStalker->m_tpaDynamicObjects[i].tSavedPosition;
					m_tpaDynamicObjects[j].tOrientation = tpStalker->m_tpaDynamicObjects[i].tOrientation;
					m_tpaDynamicObjects[j].dwMyNodeID = tpStalker->m_tpaDynamicObjects[i].dwMyNodeID;
					m_tpaDynamicObjects[j].tMySavedPosition = tpStalker->m_tpaDynamicObjects[i].tMySavedPosition;
					m_tpaDynamicObjects[j].tMyOrientation = tpStalker->m_tpaDynamicObjects[i].tMyOrientation;
					m_tpaDynamicObjects[j].tpEntity = tpEntity;
					return(true);
				}
			
			if (m_tpaDynamicObjects.size() >= m_dwMaxDynamicObjectsCount)	{
				u32 dwBest = dwTime + 1, dwIndex = u32(-1);
				for (int j=0; j<(int)m_tpaDynamicObjects.size(); j++)
					if (m_tpaDynamicObjects[j].dwTime < dwBest) {
						dwIndex = i;
						dwBest = m_tpaDynamicObjects[j].dwTime;
					}
				if (dwIndex < m_tpaDynamicObjects.size()) {
					m_tpaDynamicObjects[dwIndex].dwTime = dwTime;
					m_tpaDynamicObjects[dwIndex].dwUpdateCount = 1;
					m_tpaDynamicObjects[dwIndex].dwNodeID = tpStalker->m_tpaDynamicObjects[i].dwNodeID;
					m_tpaDynamicObjects[dwIndex].tSavedPosition = tpStalker->m_tpaDynamicObjects[i].tSavedPosition;
					m_tpaDynamicObjects[dwIndex].tOrientation = tpStalker->m_tpaDynamicObjects[i].tOrientation;
					m_tpaDynamicObjects[dwIndex].dwMyNodeID = tpStalker->m_tpaDynamicObjects[i].dwMyNodeID;
					m_tpaDynamicObjects[dwIndex].tMySavedPosition = tpStalker->m_tpaDynamicObjects[i].tMySavedPosition;
					m_tpaDynamicObjects[dwIndex].tMyOrientation = tpStalker->m_tpaDynamicObjects[i].tMyOrientation;
					m_tpaDynamicObjects[dwIndex].tpEntity = tpEntity;
					return(true);
				}
			}
			else {
				SDynamicObject tDynamicObject;
				tDynamicObject.dwTime = dwTime;
				tDynamicObject.dwUpdateCount = 1;
				tDynamicObject.dwNodeID = tpStalker->m_tpaDynamicObjects[i].dwNodeID;
				tDynamicObject.tSavedPosition = tpStalker->m_tpaDynamicObjects[i].tSavedPosition;
				tDynamicObject.tOrientation = tpStalker->m_tpaDynamicObjects[i].tOrientation;
				tDynamicObject.dwMyNodeID = tpStalker->m_tpaDynamicObjects[i].dwMyNodeID;
				tDynamicObject.tMySavedPosition = tpStalker->m_tpaDynamicObjects[i].tMySavedPosition;
				tDynamicObject.tMyOrientation = tpStalker->m_tpaDynamicObjects[i].tMyOrientation;
				tDynamicObject.tpEntity = tpEntity;
				m_tpaDynamicObjects.push_back(tDynamicObject);
				return(true);
			}
		}
	}
	return(false);
}


bool CAI_Stalker::bfCheckIfSound()
{
	int iIndex;
	SelectSound(iIndex);
	if ((iIndex > 0) && m_tpaDynamicSounds[iIndex].tpEntity && ((iIndex != m_iSoundIndex) || (m_tpaDynamicSounds[iIndex].dwTime > m_dwSoundTime))) {
		m_iSoundIndex = iIndex;
		m_dwSoundTime = m_tpaDynamicSounds[iIndex].dwTime;
		return(true);
	}
	else
		return(false);
}

void CAI_Stalker::SelectSound(int &iIndex)
{
	iIndex = -1;
	float fMaxPower = 0.f;
	for (int i=0; i<(int)m_tpaDynamicSounds.size(); i++)
		if ((!m_tpaDynamicSounds[i].tpEntity) || (m_tpaDynamicSounds[i].tpEntity->g_Team() != g_Team()))
			if ((m_tpaDynamicSounds[i].dwTime + SOUND_UPDATE_DELAY > m_dwCurrentUpdate) && (m_tpaDynamicSounds[i].fPower > fMaxPower)) {
				fMaxPower = m_tpaDynamicSounds[i].fPower;
				iIndex = i;
			}
}

void CAI_Stalker::feel_sound_new(CObject* who, int eType, Fvector& Position, float power)
{
	#ifdef WRITE_LOG
//		Msg("%s - sound type %x from %s at %d in (%.2f,%.2f,%.2f) with power %.2f",cName(),eType,who ? who->cName() : "world",Level().timeServer(),Position.x,Position.y,Position.z,power);
	#endif

	if (g_Health() <= 0) {
		m_tpaDynamicSounds.clear();
		return;
	}
	
	power *= ffGetStartVolume(ESoundTypes(eType));
	if ((eType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING)
		power = 1.f;//expf(.1f*log(power));

	u32 dwTime = m_dwCurrentUpdate;
	
	if ((power >= 0*m_fSensetivity*m_fSoundPower) && (power >= MIN_SOUND_VOLUME)) {
		if (this != who) {
			int j;
			CEntity *tpEntity = dynamic_cast<CEntity *>(who);
			for ( j=0; j<(int)m_tpaDynamicSounds.size(); j++)
				if (who == m_tpaDynamicSounds[j].tpEntity) {
					m_tpaDynamicSounds[j].eSoundType		= ESoundTypes(eType);
					m_tpaDynamicSounds[j].dwTime			= dwTime;
					m_tpaDynamicSounds[j].fPower			= power;
					m_tpaDynamicSounds[j].dwUpdateCount++;
					m_tpaDynamicSounds[j].tSavedPosition	= Position;
					m_tpaDynamicSounds[j].tOrientation		= tfGetOrientation(tpEntity);
					m_tpaDynamicSounds[j].tMySavedPosition	= vPosition;
					m_tpaDynamicSounds[j].tMyOrientation	= r_torso_current;
					m_tpaDynamicSounds[j].tpEntity			= tpEntity;
					m_tpaDynamicSounds[j].dwNodeID			= tpEntity ? tpEntity->AI_NodeID : 0;
					m_tpaDynamicSounds[j].dwMyNodeID		= AI_NodeID;
				}
			if (j >= (int)m_tpaDynamicSounds.size()) {
				if ((int)m_tpaDynamicSounds.size() >= m_dwMaxDynamicSoundsCount)	{
					u32 dwBest = dwTime + 1, dwIndex = u32(-1);
					for (int j=0; j<(int)m_tpaDynamicSounds.size(); j++)
						if (m_tpaDynamicSounds[j].dwTime < dwBest) {
							dwIndex = j;
							dwBest = m_tpaDynamicSounds[j].dwTime;
						}
					if (dwIndex < (int)m_tpaDynamicSounds.size()) {
						m_tpaDynamicSounds[dwIndex].eSoundType			= ESoundTypes(eType);
						m_tpaDynamicSounds[dwIndex].dwTime				= dwTime;
						m_tpaDynamicSounds[dwIndex].fPower				= power;
						m_tpaDynamicSounds[dwIndex].dwUpdateCount		= 1;
						m_tpaDynamicSounds[dwIndex].tSavedPosition		= Position;
						m_tpaDynamicSounds[dwIndex].tOrientation		= tfGetOrientation(tpEntity);
						m_tpaDynamicSounds[dwIndex].tMySavedPosition	= vPosition;
						m_tpaDynamicSounds[dwIndex].tMyOrientation		= r_torso_current;
						m_tpaDynamicSounds[dwIndex].tpEntity			= tpEntity;
						m_tpaDynamicSounds[dwIndex].dwNodeID			= tpEntity ? tpEntity->AI_NodeID : 0;
						m_tpaDynamicSounds[dwIndex].dwMyNodeID			= AI_NodeID;
					}
				}
				else {
					SDynamicSound					tDynamicSound;
					tDynamicSound.eSoundType		= ESoundTypes(eType);
					tDynamicSound.dwTime			= dwTime;
					tDynamicSound.fPower			= power;
					tDynamicSound.dwUpdateCount		= 1;
					tDynamicSound.tSavedPosition	= Position;
					tDynamicSound.tOrientation		= tfGetOrientation(tpEntity);
					tDynamicSound.tMySavedPosition	= vPosition;
					tDynamicSound.tMyOrientation	= r_torso_current;
					tDynamicSound.tpEntity			= tpEntity;
					tDynamicSound.dwNodeID			= tpEntity ? tpEntity->AI_NodeID : 0;
					tDynamicSound.dwMyNodeID		= AI_NodeID;
					m_tpaDynamicSounds.push_back	(tDynamicSound);
				}
			}
		}
	}
	// computing total power of my own sounds for computing tha ability to hear the others
	if (m_fSoundPower < power) {
		m_fSoundPower = m_fStartPower = power;
		m_dwSoundUpdate = dwTime;
	}
}