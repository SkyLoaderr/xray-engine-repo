////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier_look.cpp
//	Created 	: 23.07.2002
//  Modified 	: 23.07.2002
//	Author		: Dmitriy Iassenev
//	Description : Visibility and look for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_soldier.h"
#include "..\\..\\xr_weapon_list.h"

#define	MAX_HEAD_TURN_ANGLE				(2.f*PI_DIV_6)
#define MIN_SPINE_TURN_ANGLE			PI_DIV_6
#define EYE_WEAPON_DELTA				(0*PI/30.f)
#define WEAPON_DISTANCE					(.35f)
#define SQUARE_WEAPON_DISTANCE			(WEAPON_DISTANCE*WEAPON_DISTANCE)
#define MIN_SOUND_VOLUME				.05f

bool CAI_Soldier::bfCheckForVisibility(CEntity* tpEntity)
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
		DWORD dwTime = tpEntity->ps_Element(tpEntity->ps_Size() - 1).dwTime;
		if (dwTime < m_dwMovementIdleTime) {
			tTemp.sub(tpEntity->ps_Element(tpEntity->ps_Size() - 2).vPosition,tpEntity->ps_Size() - 1);
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

void CAI_Soldier::SetDirectionLook()
{
	int i = ps_Size	();
	if (i > 1) {
		CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		tWatchDirection.sub(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
		if (tWatchDirection.magnitude() > EPS_L) {
			tWatchDirection.normalize();
			mk_rotation(tWatchDirection,r_torso_target);

//			r_torso_target.yaw += m_fAddAngle;
//			while (r_torso_target.yaw > PI_MUL_2)
//				r_torso_target.yaw -= PI_MUL_2;
			//ASSIGN_SPINE_BONE;
			//q_look.o_look_speed=PI_DIV_4;
		}
	}
	r_target.yaw = r_torso_target.yaw;
	//r_target.pitch = 0;
	//r_torso_target.pitch = 0;
}

void CAI_Soldier::SetLook(Fvector tPosition)
{
	Fvector tCurrentPosition = vPosition;
	tWatchDirection.sub(tPosition,tCurrentPosition);
	if (tWatchDirection.magnitude() > EPS_L) {
		tWatchDirection.normalize();
		mk_rotation(tWatchDirection,r_torso_target);
		//ASSIGN_SPINE_BONE;
		//q_look.o_look_speed=PI_DIV_4;
	}
	r_target.yaw = r_torso_target.yaw;
}

void CAI_Soldier::SetLessCoverLook(NodeCompressed *tNode, bool bSpine)
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
			//if (bSpine) {
				//ASSIGN_SPINE_BONE;
			//}
			//q_look.o_look_speed = PI_DIV_4;
			//r_torso_speed = _FB_look_speed;//(r_torso_target.yaw - r_torso_current.yaw);
			//r_target.yaw += PI_DIV_6;
		}
	}
	//r_target.pitch = 0;
	//r_torso_target.pitch = 0;
}

static BOOL __fastcall SoldierQualifier(CObject* O, void* P)
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

void CAI_Soldier::vfUpdateDynamicObjects()
{
	ai_Track.o_get(tpaVisibleObjects);
	DWORD dwTime = m_dwCurrentUpdate;
	for (int i=0; i<tpaVisibleObjects.size(); i++) {
		
		CEntity *tpEntity = dynamic_cast<CEntity *>(tpaVisibleObjects[i]);
		
		if (!tpEntity || !bfCheckForVisibility(tpEntity))
			continue;
		
		for (int j=0; j<tpaDynamicObjects.size(); j++)
			if (tpEntity == tpaDynamicObjects[j].tpEntity) {
				tpaDynamicObjects[j].dwTime = dwTime;
				tpaDynamicObjects[j].dwUpdateCount++;
				tpaDynamicObjects[j].dwNodeID = tpEntity->AI_NodeID;
				tpaDynamicObjects[j].tSavedPosition = tpEntity->Position();
				tpaDynamicObjects[j].tOrientation = tfGetOrientation(tpEntity);
				tpaDynamicObjects[j].dwMyNodeID = AI_NodeID;
				tpaDynamicObjects[j].tMySavedPosition = Position();
				tpaDynamicObjects[j].tMyOrientation = r_torso_current;
				break;
			}
		
		if (j >= tpaDynamicObjects.size()) {
			if (tpaDynamicObjects.size() >= m_dwMaxDynamicObjectsCount)	{
				DWORD dwBest = dwTime + 1, dwIndex = DWORD(-1);
				for (int j=0; j<tpaDynamicObjects.size(); j++)
					if (tpaDynamicObjects[j].dwTime < dwBest) {
						dwIndex = i;
						dwBest = tpaDynamicObjects[j].dwTime;
					}
				if (dwIndex < tpaDynamicObjects.size()) {
					tpaDynamicObjects[dwIndex].dwTime = dwTime;
					tpaDynamicObjects[dwIndex].dwUpdateCount = 1;
					tpaDynamicObjects[dwIndex].dwNodeID = tpEntity->AI_NodeID;
					tpaDynamicObjects[dwIndex].tSavedPosition = tpEntity->Position();
					tpaDynamicObjects[dwIndex].tOrientation = tfGetOrientation(tpEntity);
					tpaDynamicObjects[dwIndex].dwMyNodeID = AI_NodeID;
					tpaDynamicObjects[dwIndex].tMySavedPosition = Position();
					tpaDynamicObjects[dwIndex].tMyOrientation = r_torso_current;
					tpaDynamicObjects[dwIndex].tpEntity = tpEntity;
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
				tpaDynamicObjects.push_back(tDynamicObject);
			}
		}
	}
}

void CAI_Soldier::vfUpdateSounds(DWORD dwTimeDelta)
{
	/**/
	if (m_fSoundPower > EPS_L) {
		//m_fSoundPower = m_fStartPower/(10*float(m_dwLastUpdate - m_dwSoundUpdate)/1000.f + 1);
		m_fSoundPower -= m_fStartPower*float(m_dwCurrentUpdate - m_dwSoundUpdate)/1000.f/2.f;
		if (m_fSoundPower <= EPS_L)
			m_fSoundPower = 0.f;
	}
	else
		m_fSoundPower = 0.f;
	
	//Msg("%.2f",m_fSoundPower);
	/**/
}

void CAI_Soldier::soundEvent(CObject* who, int eType, Fvector& Position, float power)
{
	#ifdef WRITE_LOG
		//Msg("%s - sound type %x from %s at %d in (%.2f,%.2f,%.2f) with power %.2f",cName(),eType,who ? who->cName() : "world",Level().timeServer(),Position.x,Position.y,Position.z,power);
	#endif

	if (g_Health() <= 0) {
		tpaDynamicSounds.clear();
		return;
	}
	
	//if (Level().timeServer() < 17000)
	//	return;

	power *= ffGetStartVolume(ESoundTypes(eType));
	if ((eType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING)
		power = 1.f;//expf(.1f*log(power));

	DWORD dwTime = m_dwCurrentUpdate;
	
	if ((power >= m_fSensetivity*m_fSoundPower) && (power >= MIN_SOUND_VOLUME)) {
		if (this != who) {
			//Msg("%s - sound type %x from %s at %d in (%.2f,%.2f,%.2f) with power %.2f",cName(),eType,who ? who->cName() : "world",m_dwCurrentUpdate,Position.x,Position.y,Position.z,power);
			int j;
			CEntity *tpEntity = dynamic_cast<CEntity *>(who);
			//if (!tpEntity) 
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

void CAI_Soldier::SelectSound(int &iIndex)
{
	iIndex = -1;
	float fMaxPower = 0.f;
	for (int i=0; i<tpaDynamicSounds.size(); i++)
		if ((!tpaDynamicSounds[i].tpEntity) || (tpaDynamicSounds[i].tpEntity->g_Team() != g_Team()))
			if ((tpaDynamicSounds[i].dwTime > m_dwCurrentUpdate) && (tpaDynamicSounds[i].fPower > fMaxPower)) {
				fMaxPower = tpaDynamicSounds[i].fPower;
				iIndex = i;
			}
}

/**/
bool CAI_Soldier::bfCheckForEntityVisibility(CEntity *tpEntity)
{
	if (!tpEntity)
		return(false);

	Fvector tMonsterDirection, tDirection;
	float fEyeFov, fEyeRange;
	
	CCustomMonster *tpCustomMonster = dynamic_cast<CCustomMonster *>(tpEntity);
	if (tpCustomMonster) {
		tMonsterDirection.setHP(-tpCustomMonster->r_torso_current.yaw,0);
	}
	else {
		CActor *tpActor = dynamic_cast<CActor *>(tpEntity);
		if (tpActor)
			tMonsterDirection.setHP(tpActor->Orientation().yaw,0);
		else
			R_ASSERT(false);
	}
	CEntityAlive *tpEntityAlive = dynamic_cast<CEntityAlive *>(tpEntity);
	if (tpEntityAlive) {
		fEyeRange = tpEntityAlive->ffGetRange();
		fEyeFov = tpEntityAlive->ffGetFov() + 20.f;
	}
	
	tDirection.sub(vPosition,tpEntity->Position());
	float fDistance = tDirection.magnitude();
	if (fDistance > fEyeRange + EPS_L)
		return(false);
		//return(true);
	tDirection.normalize_safe();
	float fAlpha = tDirection.dotproduct(tMonsterDirection);
	clamp(fAlpha,-.99999f,+.99999f);
	fAlpha = acosf(fAlpha);
	bool bVisible = fAlpha <= fEyeFov/180.f/2.f*PI + EPS_L;
	return(bVisible);
	//return(!bVisible);
}
/**/

void CAI_Soldier::vfAimAtEnemy(bool bInaccuracy)
{
	Fvector	pos1, pos2;
	
	if (Enemy.Enemy)
		Enemy.Enemy->svCenter(pos1);
	else
		pos1 = tSavedEnemyPosition;

	svCenter(pos2);
	tWatchDirection.sub(pos1,pos2);
	float fDistance = tWatchDirection.magnitude();
	
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// for making soldiers not so precise :-)))
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (bInaccuracy) {
		float fMissDistance = fDistance <= m_fMinMissDistance ? m_fMinMissFactor : fDistance >= m_fMaxMissDistance ? m_fMaxMissFactor : (fDistance - m_fMinMissDistance)/(m_fMaxMissDistance - m_fMinMissDistance)*(m_fMaxMissFactor - m_fMinMissFactor) + m_fMinMissFactor;
		float fAlpha = 1.f - _sqr(fMissDistance)/(2*tWatchDirection.square_magnitude());
		clamp(fAlpha,-.99999f,+.99999f);
		fAlpha = acosf(fAlpha);

		SRotation sRot;
		mk_rotation(tWatchDirection,sRot);
		
		sRot.pitch += ::Random.randF(-fAlpha,+fAlpha);
		sRot.yaw += ::Random.randF(-fAlpha,+fAlpha);
		tWatchDirection.setHP(-sRot.yaw,-sRot.pitch);
	}
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	mk_rotation(tWatchDirection,r_torso_target);
	
	m_fAddWeaponAngle = 0;
	// turning model a bit more for precise weapon shooting
	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
			break;
		}
		case BODY_STATE_CROUCH : {
			if (fDistance > EPS_L) {
				m_fAddWeaponAngle = r_torso_target.yaw;
				if (m_fAddWeaponAngle > PI - EPS_L)
					m_fAddWeaponAngle -= PI_MUL_2;
				
				m_fAddWeaponAngle = WEAPON_DISTANCE/fDistance;//(WEAPON_DISTANCE + .15f*fabsf(PI - m_fAddWeaponAngle)/PI)/fDistance;
				clamp(m_fAddWeaponAngle,-.99999f,+.99999f);
				m_fAddWeaponAngle = asinf(m_fAddWeaponAngle);
			}
			else
				m_fAddWeaponAngle = 0;
			
			//m_fAddWeaponAngle = 0;
			break;
		}
		case BODY_STATE_LIE : {
			break;
		}
	}
	
	r_torso_target.yaw -= m_fAddWeaponAngle;
	r_target.yaw = r_torso_target.yaw;

	//ASSIGN_SPINE_BONE;
}

int	 CAI_Soldier::ifFindDynamicObject(CEntity *tpEntity)
{
	for (int i=0; i<tpaDynamicObjects.size(); i++)
		if (tpaDynamicObjects[i].tpEntity == tpEntity)
			return(i);
	return(-1);
}
