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
#include "..\\..\\actor.h"

#define	MAX_HEAD_TURN_ANGLE				(PI/2.f)
#define MIN_SPINE_TURN_ANGLE			PI_DIV_6
#define EYE_WEAPON_DELTA				(0*PI/30.f)
#define WEAPON_DISTANCE					(.35f)
#define SQUARE_WEAPON_DISTANCE			(WEAPON_DISTANCE*WEAPON_DISTANCE)
#define ASSIGN_PROPORTIONAL_POWER(a,b)	if ((eType & a) == a) power*=b;

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
	float fMaxViewableDistanceInDirection = eye_range*(1 - fAlpha/(fEyeFov/m_fLateralMutliplier));
	
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

void CAI_Soldier::SetDirectionLook()
{
	int i = ps_Size();
	if (i > 1) {
		CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		tWatchDirection.sub(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
		if (tWatchDirection.magnitude() > EPS_L) {
			tWatchDirection.normalize();
			mk_rotation(tWatchDirection,r_torso_target);

//			r_torso_target.yaw += m_fAddAngle;
//			while (r_torso_target.yaw > PI_MUL_2)
//				r_torso_target.yaw -= PI_MUL_2;

			r_target.yaw = r_torso_target.yaw;
			ASSIGN_SPINE_BONE;
			q_look.o_look_speed=PI_DIV_4;
		}
	}
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
		r_target.yaw = r_torso_target.yaw;
		ASSIGN_SPINE_BONE;
		q_look.o_look_speed=PI_DIV_4;
	}
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
			if (bSpine) {
				ASSIGN_SPINE_BONE;
			}
			q_look.o_look_speed = PI_DIV_4;
			//r_torso_speed = _FB_look_speed;//(r_torso_target.yaw - r_torso_current.yaw);
			//r_target.yaw += PI_DIV_6;
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
	ASSIGN_SPINE_BONE;
	r_torso_target.yaw = r_torso_target.yaw - EYE_WEAPON_DELTA;
	//r_target.pitch *= -1;
	q_look.o_look_speed=8*_FB_look_speed;
	/**
	}
	else {
	SetLessCoverLook(tNode);
	q_look.o_look_speed=8*_FB_look_speed;
	}
	/**/
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

SRotation CAI_Soldier::tfGetOrientation(CEntity *tpEntity)
{
	CCustomMonster *tpCustomMonster = dynamic_cast<CCustomMonster *>(tpEntity);
	if (tpCustomMonster)
		return(tpCustomMonster->r_torso_current);
	else {
		SRotation tTemp;
		CActor *tpActor = dynamic_cast<CActor *>(tpEntity);
		if (tpActor)
			return(tpActor->Orientation());
		else
			return(tTemp);
	}
}

void CAI_Soldier::vfUpdateDynamicObjects()
{
	ai_Track.o_get(tpaVisibleObjects);
	DWORD dwTime = Level().timeServer();
	for (int i=0; i<tpaVisibleObjects.size(); i++) {
		
		CEntity *tpEntity = dynamic_cast<CEntity *>(tpaVisibleObjects[i]);
		
		if (!tpEntity || !bfCheckForVisibility(tpEntity))
			continue;
		
		for (int j=0; j<tpaDynamicObjects.size(); j++)
			if (tpEntity == tpaDynamicObjects[j].tpEntity) {
				tpaDynamicObjects[j].dwTime = dwTime;
				tpaDynamicObjects[j].dwUpdateCount++;
				tpaDynamicObjects[j].tSavedPosition = tpEntity->Position();
				tpaDynamicObjects[j].tOrientation = tfGetOrientation(tpEntity);
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
					tpaDynamicObjects[dwIndex].tSavedPosition = tpEntity->Position();
					tpaDynamicObjects[dwIndex].tOrientation = tfGetOrientation(tpEntity);
					tpaDynamicObjects[dwIndex].tMySavedPosition = Position();
					tpaDynamicObjects[dwIndex].tMyOrientation = r_torso_current;
					tpaDynamicObjects[dwIndex].tpEntity = tpEntity;
				}
			}
			else {
				SDynamicObject tDynamicObject;
				tDynamicObject.dwTime = dwTime;
				tDynamicObject.dwUpdateCount = 1;
				tDynamicObject.tSavedPosition = tpEntity->Position();
				tDynamicObject.tOrientation = tfGetOrientation(tpEntity);
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
		//m_fSoundPower = m_fStartPower/(10*float(Level().timeServer() - m_dwSoundUpdate)/1000.f + 1);
		m_fSoundPower -= m_fStartPower*float(Level().timeServer() - m_dwSoundUpdate)/1000.f/2.f;
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
		Msg("%s - sound type %x from %s at %d in (%.2f,%.2f,%.2f) with power %.2f",cName(),eType,who ? who->cName() : "world",Level().timeServer(),Position.x,Position.y,Position.z,power);
	#endif

	if (Level().timeServer() < 15000)
		return;

	power *= ffGetStartVolume(ESoundTypes(eType));

	ASSIGN_PROPORTIONAL_POWER(SOUND_TYPE_WEAPON_RECHARGING		,0.15f);
	ASSIGN_PROPORTIONAL_POWER(SOUND_TYPE_WEAPON_SHOOTING		,1.00f); 
	ASSIGN_PROPORTIONAL_POWER(SOUND_TYPE_WEAPON_TAKING			,0.05f); 
	ASSIGN_PROPORTIONAL_POWER(SOUND_TYPE_WEAPON_HIDING			,0.05f); 
	ASSIGN_PROPORTIONAL_POWER(SOUND_TYPE_WEAPON_CHANGING		,0.05f); 
	ASSIGN_PROPORTIONAL_POWER(SOUND_TYPE_WEAPON_EMPTY_CLICKING	,0.10f); 
	ASSIGN_PROPORTIONAL_POWER(SOUND_TYPE_WEAPON_BULLET_RICOCHET	,0.50f); 
	ASSIGN_PROPORTIONAL_POWER(SOUND_TYPE_MONSTER_DYING			,0.50f); 
	ASSIGN_PROPORTIONAL_POWER(SOUND_TYPE_MONSTER_INJURING		,0.50f);
	ASSIGN_PROPORTIONAL_POWER(SOUND_TYPE_MONSTER_WALKING_NORMAL ,0.15f); 
	ASSIGN_PROPORTIONAL_POWER(SOUND_TYPE_MONSTER_WALKING_CROUCH	,0.05f); 
	ASSIGN_PROPORTIONAL_POWER(SOUND_TYPE_MONSTER_WALKING_LIE	,0.10f); 
	ASSIGN_PROPORTIONAL_POWER(SOUND_TYPE_MONSTER_RUNNING_NORMAL	,1.00f);//.25f 
	ASSIGN_PROPORTIONAL_POWER(SOUND_TYPE_MONSTER_RUNNING_CROUCH	,1.00f);//.20f 
	ASSIGN_PROPORTIONAL_POWER(SOUND_TYPE_MONSTER_RUNNING_LIE	,0.20f); 
	ASSIGN_PROPORTIONAL_POWER(SOUND_TYPE_MONSTER_JUMPING_NORMAL	,0.20f); 
	ASSIGN_PROPORTIONAL_POWER(SOUND_TYPE_MONSTER_JUMPING_CROUCH	,0.15f); 
	ASSIGN_PROPORTIONAL_POWER(SOUND_TYPE_MONSTER_FALLING		,0.20f); 
	ASSIGN_PROPORTIONAL_POWER(SOUND_TYPE_MONSTER_TALKING		,0.25f); 

	DWORD dwTime = Level().timeServer();
	
	if ((power >= m_fSensetivity*m_fSoundPower) && ((eType & SOUND_TYPE_MONSTER) != SOUND_TYPE_MONSTER)) {
		if (this != who) {
			int j = tpaDynamicObjects.size();
			CEntity *tpEntity = dynamic_cast<CEntity *>(who);
			if (!tpEntity || !bfCheckForVisibility(tpEntity)) {
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
			if ((tpaDynamicSounds[i].dwTime > m_dwLastUpdate) && (tpaDynamicSounds[i].fPower > fMaxPower)) {
				fMaxPower = tpaDynamicSounds[i].fPower;
				iIndex = i;
			}
}

/**/
bool CAI_Soldier::bfCheckForEntityVisibility(CEntity *tpEntity)
{
	Fvector tMonsterDirection, tDirection;
	float fEyeFov, fEyeRange;
	
	CCustomMonster *tpCustomMonster = dynamic_cast<CCustomMonster *>(tpEntity);
	if (tpCustomMonster) {
		tMonsterDirection.setHP(tpCustomMonster->r_torso_current.yaw,0);
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

void CAI_Soldier::vfAimAtEnemy()
{
	Fvector	pos1, pos2;
	
	if (Enemy.Enemy)
		Enemy.Enemy->svCenter(pos1);
	else
		pos1 = tSavedEnemyPosition;

	svCenter(pos2);
	tWatchDirection.sub(pos1,pos2);
	float fDistance = tWatchDirection.magnitude();
	mk_rotation(tWatchDirection,r_torso_target);
	r_target.yaw = r_torso_target.yaw;
	
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
				
				m_fAddWeaponAngle = (WEAPON_DISTANCE + .15f*fabsf(PI - m_fAddWeaponAngle)/PI)/fDistance;
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
	ASSIGN_SPINE_BONE;
}
