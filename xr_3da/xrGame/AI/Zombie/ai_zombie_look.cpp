////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie_misc.cpp
//	Created 	: 23.07.2002
//  Modified 	: 23.07.2002
//	Author		: Dmitriy Iassenev
//	Description : Visibility and look for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_zombie.h"
#include "..\\..\\xr_weapon_list.h"

#define MIN_SPINE_TURN_ANGLE			PI_DIV_6
#define	MAX_HEAD_TURN_ANGLE				(PI/3.f)

bool CAI_Zombie::bfCheckForVisibility(CEntity* tpEntity)
{
	float fResult = 0.f;
	
	// computing maximum viewable distance in the specified direction
	Fvector tCurrentWatchDirection, tTemp;
	tCurrentWatchDirection.setHP	(-r_torso_current.yaw,-r_torso_current.pitch);
	tCurrentWatchDirection.normalize();
	tTemp.sub(tpEntity->Position(),vPosition);
	tTemp.normalize();
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
		DWORD dwTime = tpEntity->ps_Element(tpEntity->ps_Size() - 1).dwTime;
		if (dwTime < m_dwMovementIdleTime) {
			tTemp.sub(tpEntity->ps_Element(tpEntity->ps_Size() - 2).vPosition,tpEntity->ps_Size() - 1);
			float fSpeed = tTemp.magnitude()/dwTime;
			fResult += fSpeed < m_fMaxInvisibleSpeed ? m_fMovementSpeedWeight*fSpeed/m_fMaxInvisibleSpeed : m_fMovementSpeedWeight;
		}
	}
	
	// computing lightness weight
	fResult *= m_fShadowWeight*float(tpEntity->AI_Node->light)/255.f;
	
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

void CAI_Zombie::SetDirectionLook()
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
			//q_look.o_look_speed=PI_DIV_4;
		}
	}
	//r_target.pitch = 0;
	r_torso_target.pitch = 0;
}

void CAI_Zombie::SetLessCoverLook(NodeCompressed *tNode, bool bSpine)
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
			if (bSpine) {
				ASSIGN_SPINE_BONE;
			}
			//q_look.o_look_speed = PI_DIV_4;
			//r_torso_speed = _FB_look_speed;//(r_torso_target.yaw - r_torso_current.yaw);
			//r_target.yaw += PI_DIV_6;
		}
	}
	//r_target.pitch = 0;
	r_torso_target.pitch = 0;
}

void CAI_Zombie::vfAimAtEnemy()
{
	Fvector	pos1, pos2;
	Enemy.Enemy->svCenter(pos1);
	svCenter(pos2);
	tWatchDirection.sub(pos1,pos2);
	mk_rotation(tWatchDirection,r_torso_target);
	r_target.yaw = r_torso_target.yaw;
	ASSIGN_SPINE_BONE;
	r_torso_target.pitch = 0;
	//r_torso_target.yaw = r_torso_target.yaw - 2*PI_DIV_6;//EYE_WEAPON_DELTA;
	//q_look.o_look_speed=_FB_look_speed;
}

static BOOL __fastcall ZombieQualifier(CObject* O, void* P)
{
	if (O->CLS_ID!=CLSID_ENTITY)			
		return FALSE;
	else {
		CEntity* E = dynamic_cast<CEntity*> (O);
		if (!E) return FALSE;
		if (!E->IsVisibleForAI()) return FALSE; 
		return TRUE;
	}
}

objQualifier* CAI_Zombie::GetQualifier	()
{
	return(&ZombieQualifier);
}

void CAI_Zombie::vfUpdateDynamicObjects()
{

}

void CAI_Zombie::soundEvent(CObject* who, int eType, Fvector& Position, float power)
{
	#ifdef WRITE_LOG
		//Msg("%s - sound type %x from %s at %d in (%.2f,%.2f,%.2f) with power %.2f",cName(),eType,who ? who->cName() : "world",Level().timeServer(),Position.x,Position.y,Position.z,power);
	#endif

	return;
	if (g_Health() <= 0) {
		tpaDynamicSounds.clear();
		return;
	}
	
	//if (Level().timeServer() < 17000)
	//	return;

	power *= ffGetStartVolume(ESoundTypes(eType));
	if ((eType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING)
		power = 1.f;//expf(.1f*log(power));

	DWORD dwTime = Level().timeServer();
	
	if ((power >= m_fSensetivity*m_fSoundPower) && (power >= .10f)) {
		if (this != who) {
			//Msg("%s - sound type %x from %s at %d in (%.2f,%.2f,%.2f) with power %.2f",cName(),eType,who ? who->cName() : "world",Level().timeServer(),Position.x,Position.y,Position.z,power);
			int j;
			CEntity *tpEntity = dynamic_cast<CEntity *>(who);
			if (tpEntity && (tpEntity->g_Team() != g_Team())) 
			{
				for ( j=0; j<tpaDynamicSounds.size(); j++)
					if (who == tpaDynamicSounds[j].tpEntity) {
						tpaDynamicSounds[j].eSoundType = ESoundTypes(eType);
						tpaDynamicSounds[j].dwTime = dwTime;
						tpaDynamicSounds[j].fPower = power;
						tpaDynamicSounds[j].dwUpdateCount++;
						tpaDynamicSounds[j].tSavedPosition = Position;
						tpaDynamicSounds[j].tOrientation = tfGetOrientation(tpEntity);
						tpaDynamicSounds[j].tMySavedPosition = vPosition;
						tpaDynamicSounds[j].tMyOrientation = r_torso_current;
						tpaDynamicSounds[j].tpEntity = tpEntity;
					}
				if (j >= tpaDynamicSounds.size()) {
					if (tpaDynamicSounds.size() >= m_dwMaxDynamicSoundsCount)	{
						DWORD dwBest = dwTime + 1, dwIndex = DWORD(-1);
						for (int j=0; j<tpaDynamicSounds.size(); j++)
							if (tpaDynamicSounds[j].dwTime < dwBest) {
								dwIndex = j;
								dwBest = tpaDynamicSounds[j].dwTime;
							}
						if (dwIndex < tpaDynamicSounds.size()) {
							tpaDynamicSounds[dwIndex].eSoundType = ESoundTypes(eType);
							tpaDynamicSounds[dwIndex].dwTime = dwTime;
							tpaDynamicSounds[dwIndex].fPower = power;
							tpaDynamicSounds[dwIndex].dwUpdateCount = 1;
							tpaDynamicSounds[dwIndex].tSavedPosition = Position;
							tpaDynamicSounds[dwIndex].tOrientation = tfGetOrientation(tpEntity);
							tpaDynamicSounds[dwIndex].tMySavedPosition = vPosition;
							tpaDynamicSounds[dwIndex].tMyOrientation = r_torso_current;
							tpaDynamicSounds[dwIndex].tpEntity = tpEntity;
						}
					}
					else {
						SDynamicSound tDynamicSound;
						tDynamicSound.eSoundType = ESoundTypes(eType);
						tDynamicSound.dwTime = dwTime;
						tDynamicSound.fPower = power;
						tDynamicSound.dwUpdateCount = 1;
						tDynamicSound.tSavedPosition = Position;
						tDynamicSound.tOrientation = tfGetOrientation(tpEntity);
						tDynamicSound.tMySavedPosition = vPosition;
						tDynamicSound.tMyOrientation = r_torso_current;
						tDynamicSound.tpEntity = tpEntity;
						tpaDynamicSounds.push_back(tDynamicSound);
					}
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

void CAI_Zombie::SelectSound(int &iIndex)
{
	iIndex = -1;
	return;
	float fMaxPower = 0.f;
	for (int i=0; i<tpaDynamicSounds.size(); i++)
		if ((!tpaDynamicSounds[i].tpEntity) || (tpaDynamicSounds[i].tpEntity->g_Team() != g_Team()))
			if ((tpaDynamicSounds[i].dwTime > m_dwLastUpdate) && (tpaDynamicSounds[i].fPower > fMaxPower)) {
				fMaxPower = tpaDynamicSounds[i].fPower;
				iIndex = i;
			}
}