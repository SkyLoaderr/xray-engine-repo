////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat.cpp
//	Created 	: 25.04.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_rat.h"
#include "..\\ai_monsters_misc.h"

using namespace NAI_Rat_Constants;

//Msg("Monster %s : \n* State : %s\n* Time delta : %7.3f",cName(),s,m_fTimeUpdateDelta);\

#define WRITE_TO_LOG(s) {\
	Msg("Monster %s : \n* Time delta : %7.3f",cName(),m_fTimeUpdateDelta);\
	if (!feel_visible.size())\
		Msg("* No objects in frustum",feel_visible.size());\
	else {\
		Msg("* Objects in frustum (%d) :",feel_visible.size());\
		for (int i=0; i<(int)feel_visible.size(); i++)\
			Msg("*   %s",feel_visible[i].O->cName());\
		feel_vision_get(m_tpaVisibleObjects);\
		if (!m_tpaVisibleObjects.size())\
			Msg("* No visible objects");\
		else {\
			Msg("* Visible objects (%d) :",m_tpaVisibleObjects.size());\
			for (int i=0; i<(int)m_tpaVisibleObjects.size(); i++)\
				Msg("*   %s",m_tpaVisibleObjects[i]->cName());\
		}\
	}\
}

void CAI_Rat::Think()
{
	vfUpdateMorale();
	bStopThinking = false;
	do {
		m_ePreviousState = eCurrentState;
		switch(eCurrentState) {
			case aiRatDie : {
				Death();
				break;
			}
			case aiRatFreeHuntingActive : {
				FreeHuntingActive();
				break;
			}
			case aiRatFreeHuntingPassive : {
				FreeHuntingPassive();
				break;
			}
			case aiRatAttackFire : {
				AttackFire();
				break;
			}
			case aiRatAttackRun : {
				AttackRun();
				break;
			}
			case aiRatTurn : {
				Turn();
				break;
			}
			case aiRatUnderFire : {
				UnderFire();
				break;
			}
			case aiRatRetreat : {
				Retreat();
				break;
			}
			case aiRatPursuit : {
				Pursuit();
				break;
			}
			case aiRatRecoil : {
				Recoil();
				break;
			}
		}
		m_bStateChanged = m_ePreviousState != eCurrentState;
	}
	while (!bStopThinking);
//	if (m_fSpeed > EPS_L) {
//		AI_Path.TravelPath.resize(3);
//		AI_Path.TravelPath[0].floating = AI_Path.TravelPath[1].floating = AI_Path.TravelPath[2].floating = FALSE;
//		AI_Path.TravelPath[0].P = m_tOldPosition;
//		AI_Path.TravelPath[1].P = vPosition;
//		Fvector tTemp;
//		tTemp.setHP(r_torso_current.yaw,r_torso_current.pitch);
//		tTemp.normalize_safe();
//		tTemp.mul(10.f);
//		AI_Path.TravelPath[2].P.add(vPosition,tTemp);
//		AI_Path.TravelStart = 0;
//		vPosition = m_tOldPosition;
//	}
//	else {
//		AI_Path.TravelPath.clear();
//		AI_Path.TravelStart = 0;
//	}
}
void CAI_Rat::Death()
{
	//WRITE_TO_LOG("Dying...");
	bStopThinking = true;

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);
	AI_Path.TravelPath.clear();

	//setEnabled	(false);
	
	if (m_bFiring) {
		AI_Path.Calculate(this,vPosition,vPosition,m_fCurSpeed,.1f);
		float fY = ffGetY(*AI_Node,vPosition.x,vPosition.z);
		if (vPosition.y - fY > 0.01f) {
			Fvector tAcceleration;
			tAcceleration.set(0,m_fJumpSpeed,0);
			Movement.SetPosition(vPosition);
			Movement.Calculate	(tAcceleration,0,0,.1f,false);
			Movement.GetPosition(vPosition);
		}
		else
			vPosition.set(vPosition.x,fY,vPosition.z);
		UpdateTransform();
	}
}

void CAI_Rat::Turn()
{
	WRITE_TO_LOG("Turning...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)

	//mRotate.setHPB(m_tHPB.x = -r_torso_target.yaw,m_tHPB.y,m_tHPB.z);
	//UpdateTransform();
	CHECK_IF_GO_TO_PREV_STATE(Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw, PI_DIV_6))
	
	INIT_SQUAD_AND_LEADER
	
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	vfSetFire(false,Group);

	float fTurnAngle = _min(_abs(r_torso_target.yaw - r_torso_current.yaw), PI_MUL_2 - _abs(r_torso_target.yaw - r_torso_current.yaw));
	r_torso_speed = 3*fTurnAngle;

	vfSetMovementType(BODY_STATE_STAND,0);
}

void CAI_Rat::FreeHuntingActive()
{
	WRITE_TO_LOG("Free hunting active");

	bStopThinking = true;
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)

	SelectEnemy(m_Enemy);
	
	if (m_Enemy.Enemy) {
		SelectEnemy(m_Enemy);
		m_fGoalChangeTime = 0;
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatAttackRun)
	}

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(m_fMorale < m_fMoraleNormalValue,aiRatUnderFire);
	
	if ((m_tLastSound.dwTime >= m_dwLastUpdateTime) && ((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_BULLET_RICOCHET) == SOUND_TYPE_WEAPON_BULLET_RICOCHET)) {
		if (m_tLastSound.tpEntity)
			m_tSavedEnemy = m_tLastSound.tpEntity;
		m_tSavedEnemyPosition = m_tLastSound.tSavedPosition;
		m_dwLostEnemyTime = Level().timeServer();
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatRecoil);
	}
    m_tSpawnPosition.set(m_tSafeSpawnPosition);
	m_fGoalChangeDelta		= 10.f;
	m_tVarGoal.set			(10.0,0.0,20.0);
	m_fASpeed				= .2f;
	
	if (bfCheckIfGoalChanged()) {
		if (m_bStateChanged || (vPosition.distance_to(m_tSpawnPosition) > MAX_STABLE_DISTANCE) || (::Random.randF(0,1) > m_fChangeActiveStateProbability))
			if (vPosition.distance_to(m_tSafeSpawnPosition) > m_fMoraleNullRadius)
				m_fSpeed = m_fSafeSpeed = m_fMaxSpeed;
			else
				vfChooseNewSpeed();
		else {
			vfRemoveActiveMember();
		}
	}

	vfUpdateTime(m_fTimeUpdateDelta);

	if (m_fSpeed > EPS_L)
		vfComputeNewPosition();
	else
		UpdateTransform();

	SetDirectionLook();

	if (!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,PI_DIV_8))
		m_fSpeed = EPS_S;
	else 
		if (m_fSafeSpeed != m_fSpeed) {
			int iRandom = ::Random.randI(0,2);
			switch (iRandom) {
				case 0 : {
					m_fSpeed = m_fMaxSpeed;
					break;
				}
				case 1 : {
					m_fSpeed = m_fMinSpeed;
					break;
				}
				case 2 : {
					if (::Random.randI(0,4) == 0)
						m_fSpeed = EPS_S;
					break;
				}
			}
			m_fSafeSpeed = m_fSpeed;
		}
	
	if	(!m_tpSoundBeingPlayed || !m_tpSoundBeingPlayed->feedback) {
		DWORD dwCurTime = Level().timeServer();
		if (m_tpSoundBeingPlayed && !m_tpSoundBeingPlayed->feedback) {
			m_tpSoundBeingPlayed = 0;
			m_dwLastVoiceTalk = dwCurTime;
		}
		if ((dwCurTime - m_dwLastSoundRefresh > m_fVoiceRefreshRate) && ((dwCurTime - m_dwLastVoiceTalk > m_fMaxVoiceIinterval) || ((dwCurTime - m_dwLastVoiceTalk > m_fMinVoiceIinterval) && (::Random.randF(0,1) > (dwCurTime - m_dwLastVoiceTalk - m_fMinVoiceIinterval)/(m_fMaxVoiceIinterval - m_fMinVoiceIinterval))))) {
			m_dwLastSoundRefresh = dwCurTime;
			// Play voice-sound
			m_tpSoundBeingPlayed = &(m_tpaSoundVoice[Random.randI(SND_VOICE_COUNT)]);
			
			if (m_tpSoundBeingPlayed->feedback)			
				return;

			pSounds->PlayAtPos(*m_tpSoundBeingPlayed,this,eye_matrix.c);
		}
	}
	else
		if (m_tpSoundBeingPlayed && m_tpSoundBeingPlayed->feedback)
			m_tpSoundBeingPlayed->feedback->SetPosition(eye_matrix.c);
}

void CAI_Rat::FreeHuntingPassive()
{
	WRITE_TO_LOG("Free hunting passive");

	bStopThinking = true;
	
	if (!g_Alive()) {
		vfAddActiveMember(true);
		SWITCH_TO_NEW_STATE(aiRatDie);
	}

	SelectEnemy(m_Enemy);
	
	if (m_Enemy.Enemy) {
		SelectEnemy(m_Enemy);
		m_fGoalChangeTime = 0;
		vfAddActiveMember(true);
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatAttackRun)
	}

	if (m_fMorale < m_fMoraleNormalValue) {
		vfAddActiveMember(true);
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatUnderFire);
	}
	
	if ((m_tLastSound.dwTime >= m_dwLastUpdateTime) && ((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_BULLET_RICOCHET) == SOUND_TYPE_WEAPON_BULLET_RICOCHET)) {
		if (m_tLastSound.tpEntity)
			m_tSavedEnemy = m_tLastSound.tpEntity;
		m_tSavedEnemyPosition = m_tLastSound.tSavedPosition;
		m_dwLostEnemyTime = Level().timeServer();
		vfAddActiveMember(true);
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatRecoil);
	}
	
	m_fSpeed = 0.f;

	UpdateTransform();

	vfAddStandingMember();
	vfAddActiveMember();
}

void CAI_Rat::UnderFire()
{
	WRITE_TO_LOG("UnderFire");

	bStopThinking = true;
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)

	SelectEnemy(m_Enemy);
	
	if (m_Enemy.Enemy) {
		GO_TO_NEW_STATE_THIS_UPDATE(aiRatAttackRun);
	}
	else {
		if (m_tLastSound.dwTime >= m_dwLastUpdateTime) {
			if (m_tLastSound.tpEntity && (m_tLastSound.tpEntity->g_Team() != g_Team()) && (!bfCheckIfSoundFrightful())) {
				m_tSavedEnemy = m_tLastSound.tpEntity;
				m_dwLostEnemyTime = Level().timeServer();
				SWITCH_TO_NEW_STATE(aiRatAttackRun);
			}
			m_dwLastRangeSearch = Level().timeServer();
			Fvector tTemp;
			tTemp.setHP(r_torso_current.yaw,r_torso_current.pitch);
			tTemp.normalize_safe();
			tTemp.mul(UNDER_FIRE_DISTANCE);
			m_tSpawnPosition.add(vPosition,tTemp);
		}
		if (m_fMorale >= m_fMoraleNormalValue - EPS_L) {
			GO_TO_PREV_STATE;
		}
	}

	m_tGoalDir = m_tSpawnPosition;
	
	vfUpdateTime(m_fTimeUpdateDelta);

	if ((!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,PI_DIV_8)) || m_bNoWay)
		m_fSpeed = EPS_S;
	else 
		m_fSafeSpeed = m_fSpeed = m_fAttackSpeed;

	//if (m_fSpeed > EPS_L)
		vfComputeNewPosition();
	//else
	//	UpdateTransform();

	SetDirectionLook();
}

void CAI_Rat::AttackFire()
{
	WRITE_TO_LOG("Attacking enemy...");

	CHECK_IF_SWITCH_TO_NEW_STATE(!g_Alive(),aiRatDie)
	
	SelectEnemy(m_Enemy);
	
	ERatStates eState = ERatStates(dwfChooseAction(ACTION_REFRESH_RATE,MIN_PROBABILITY,g_Team(),g_Squad(),g_Group(),eCurrentState,eCurrentState,aiRatRetreat));
	if (eState != eCurrentState)
		GO_TO_NEW_STATE_THIS_UPDATE(eState);

	if (m_Enemy.Enemy)
		m_dwLostEnemyTime = Level().timeServer();

	if (!(m_Enemy.Enemy) && m_tSavedEnemy && (Level().timeServer() - m_dwLostEnemyTime < LOST_MEMORY_TIME))
		m_Enemy.Enemy = m_tSavedEnemy;

	CHECK_IF_GO_TO_PREV_STATE(!(m_Enemy.Enemy));// || !m_Enemy.Enemy->g_Alive())
		
	CHECK_IF_GO_TO_NEW_STATE((m_Enemy.Enemy->Position().distance_to(vPosition) > ATTACK_DISTANCE),aiRatAttackRun)

	Fvector tTemp;
	tTemp.sub(m_Enemy.Enemy->Position(),vPosition);
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);
	
	CHECK_IF_GO_TO_NEW_STATE(!Level().AI.bfTooSmallAngle(r_torso_current.yaw,sTemp.yaw,ATTACK_ANGLE),aiRatAttackRun)
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),m_Enemy.Enemy->Position());

	AI_Path.TravelPath.clear();
	
	vfSaveEnemy();

	vfAimAtEnemy();

	vfSetFire(true,Group);

	vfSetMovementType(m_cBodyState,0);
}

void CAI_Rat::AttackRun()
{
	WRITE_TO_LOG("Attack enemy");
	bStopThinking = true;

	//Msg("%d : %d",Level().timeServer(),m_dwLostEnemyTime);

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)
	
	if (m_Enemy.Enemy)
		m_dwLostEnemyTime = Level().timeServer();

	SelectEnemy(m_Enemy);

	ERatStates eState = ERatStates(dwfChooseAction(ACTION_REFRESH_RATE,MIN_PROBABILITY,g_Team(),g_Squad(),g_Group(),eCurrentState,eCurrentState,aiRatRetreat));
	if (eState != eCurrentState) {
		GO_TO_NEW_STATE_THIS_UPDATE(eState);
	}

	if (!(m_Enemy.Enemy) && m_tSavedEnemy && (Level().timeServer() - m_dwLostEnemyTime < LOST_MEMORY_TIME))
		m_Enemy.Enemy = m_tSavedEnemy;

	CHECK_IF_GO_TO_PREV_STATE(!m_Enemy.Enemy);// || !m_Enemy.Enemy->g_Alive())
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),m_Enemy.Enemy->Position());
	
	Fvector tTemp;
	tTemp.sub(m_Enemy.Enemy->Position(),vPosition);
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);

	if (m_Enemy.Enemy->Position().distance_to(vPosition) <= ATTACK_DISTANCE) {
		if (Level().AI.bfTooSmallAngle(r_torso_target.yaw, sTemp.yaw,ATTACK_ANGLE)) {
			GO_TO_NEW_STATE_THIS_UPDATE(aiRatAttackFire);
		}
		else {
			r_torso_target.yaw = sTemp.yaw;
			SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatTurn);
		}
	}
	else
		CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,ATTACK_ANGLE),aiRatTurn);

	INIT_SQUAD_AND_LEADER;
	
	m_tGoalDir.set			(m_Enemy.Enemy->Position());
	m_fASpeed				= .3f;
	//m_tSpawnPosition.set	(m_Enemy.Enemy->Position());
	//m_tVarGoal.set			(0,0,0);
	//m_fGoalChangeDelta		= 0.f;
	//vfChangeGoal();
	
	vfSaveEnemy();

	vfUpdateTime(m_fTimeUpdateDelta);

	if (m_Enemy.Enemy->Position().distance_to(vPosition) <= ATTACK_DISTANCE) {
		vfAimAtEnemy();
		r_torso_target.pitch = 0;
		UpdateTransform();
	}
	else {
		vfComputeNextDirectionPosition();
		SetDirectionLook();
	}
	
	vfSetFire(false,Group);

	vfSetMovementType(m_cBodyState,m_fAttackSpeed);
	
	if ((!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,PI_DIV_8)) || m_bNoWay)
		m_fSpeed = EPS_S;
	else 
		m_fSafeSpeed = m_fSpeed = m_fAttackSpeed;

}

void CAI_Rat::Retreat()
{
	WRITE_TO_LOG("Retreat");

	bStopThinking = true;
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)

	SelectEnemy(m_Enemy);
	
	if (m_Enemy.Enemy) {
		vfSaveEnemy();
		ERatStates eState = ERatStates(dwfChooseAction(ACTION_REFRESH_RATE,MIN_PROBABILITY,g_Team(),g_Squad(),g_Group(),aiRatAttackRun,aiRatAttackRun,aiRatRetreat));
		if (eState != eCurrentState)
			GO_TO_NEW_STATE_THIS_UPDATE(eState);
		m_dwLostEnemyTime = Level().timeServer();
		
		INIT_SQUAD_AND_LEADER;
		
		Fvector tTemp;
		tTemp.sub(vPosition,m_Enemy.Enemy->Position());
		tTemp.normalize_safe();
		tTemp.mul(RETREAT_DISTANCE);
		m_tSpawnPosition.add(vPosition,tTemp);
	}
	else
		if ((Level().timeServer() - m_dwLostEnemyTime > RETREAT_TIME) && ((m_tLastSound.dwTime > m_dwLastUpdateTime) || (m_tLastSound.tpEntity && ((m_tLastSound.tpEntity->g_Team() == g_Team()) || bfCheckIfSoundFrightful())))) {
			GO_TO_PREV_STATE;
		}

	m_tGoalDir = m_tSpawnPosition;
	
	if ((!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,PI_DIV_8)) || m_bNoWay) {
		m_fSpeed = 0.f;
		m_fASpeed = .2f;
		m_fGoalChangeDelta		= 10.f;
		vfUpdateTime(m_fTimeUpdateDelta);
//		UpdateTransform();
		vfComputeNewPosition();
	}
	else {
		if (m_fSpeed < EPS_L)
			m_fSafeSpeed = m_fSpeed = m_fMinSpeed;
		else
			m_fSafeSpeed = m_fSpeed = m_fAttackSpeed;
		vfUpdateTime(m_fTimeUpdateDelta);
		vfComputeNewPosition();
		SetDirectionLook();
	}
}

void CAI_Rat::Pursuit()
{
	WRITE_TO_LOG("Pursuit something");

	CHECK_IF_SWITCH_TO_NEW_STATE(!g_Alive(),aiRatDie)
	
	if (m_Enemy.Enemy)
		m_dwLostEnemyTime = Level().timeServer();

	SelectEnemy(m_Enemy);

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(m_Enemy.Enemy,aiRatAttackRun);

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE((m_fMorale < m_fMoraleNormalValue),aiRatUnderFire);

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(Level().timeServer() - m_dwLostEnemyTime >= LOST_MEMORY_TIME);
	
	if ((m_tLastSound.dwTime >= m_dwLastUpdateTime) && ((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_BULLET_RICOCHET) == SOUND_TYPE_WEAPON_BULLET_RICOCHET)) {
		if (m_tLastSound.tpEntity)
			m_tSavedEnemy = m_tLastSound.tpEntity;
		m_tSavedEnemyPosition = m_tLastSound.tSavedPosition;
		m_dwLostEnemyTime = Level().timeServer();
		GO_TO_NEW_STATE_THIS_UPDATE(aiRatRecoil);
	}

	m_tGoalDir.set(m_tSavedEnemyPosition);
	
	if (bfCheckIfGoalChanged(false))
		vfChooseNewSpeed();
	
	vfUpdateTime(m_fTimeUpdateDelta);

	if (m_fSpeed > EPS_L)
		vfComputeNewPosition();
	else
		UpdateTransform();

	SetDirectionLook();

	if (!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,PI_DIV_8) || m_bNoWay)
		m_fSpeed = EPS_S;
	else 
		if (m_fSafeSpeed != m_fSpeed) {
			int iRandom = ::Random.randI(0,2);
			switch (iRandom) {
				case 0 : {
					m_fSpeed = m_fMaxSpeed;
					break;
				}
				case 1 : {
					m_fSpeed = m_fMinSpeed;
					break;
				}
				case 2 : {
					if (::Random.randI(0,4) == 0)
						m_fSpeed = EPS_S;
					break;
				}
			}
			m_fSafeSpeed = m_fSpeed;
		}
	
	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));
}

void CAI_Rat::Recoil()
{
	WRITE_TO_LOG("Recoil from something");

	CHECK_IF_SWITCH_TO_NEW_STATE(!g_Alive(),aiRatDie)
	
	if (m_Enemy.Enemy)
		m_dwLostEnemyTime = Level().timeServer();

	SelectEnemy(m_Enemy);

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(m_Enemy.Enemy,aiRatAttackRun);

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE((m_fMorale < m_fMoraleNormalValue),aiRatUnderFire);

	if (m_tLastSound.dwTime >= m_dwLastUpdateTime) {
		if (m_tLastSound.tpEntity && (m_tLastSound.tpEntity->g_Team() != g_Team()) && (!bfCheckIfSoundFrightful())) {
			m_tSavedEnemy = m_tLastSound.tpEntity;
			m_dwLostEnemyTime = Level().timeServer();
			SWITCH_TO_NEW_STATE(aiRatAttackRun);
		}
		m_dwLostEnemyTime = Level().timeServer();
		Fvector tTemp;
		tTemp.setHP(r_torso_current.yaw,r_torso_current.pitch);
		tTemp.normalize_safe();
		tTemp.mul(UNDER_FIRE_DISTANCE);
		m_tSavedEnemyPosition = m_tLastSound.tSavedPosition;
		m_tSpawnPosition.add(vPosition,tTemp);
	}
	CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE(Level().timeServer() - m_dwLostEnemyTime >= LOST_RECOIL_TIME,aiRatPursuit);
	
	m_tGoalDir.set(m_tSpawnPosition);
	
	m_fSafeSpeed = m_fSpeed = m_fMaxSpeed;
	
	vfUpdateTime(m_fTimeUpdateDelta);

	if (m_fSpeed > EPS_L)
		vfComputeNewPosition();
	else
		UpdateTransform();

	if (!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,PI_DIV_8) || m_bNoWay)
		m_fSpeed = EPS_S;
	else 
		m_fSafeSpeed = m_fSpeed = m_fMaxSpeed;

	SetDirectionLook();

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));
}