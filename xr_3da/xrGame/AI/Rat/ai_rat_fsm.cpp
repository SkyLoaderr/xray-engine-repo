////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat_fsm.cpp
//	Created 	: 25.04.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_rat.h"
#include "..\\ai_monsters_misc.h"

#define WRITE_TO_LOG(s) bStopThinking = true;
/**
#define WRITE_TO_LOG(s) {\
	Msg("Monster %s : \n* State : %s\n* Time delta : %7.3f\n* Global time : %7.3f",cName(),s,m_fTimeUpdateDelta,float(Level().timeServer())/1000.f);\
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
				Msg("*   %s (distance %7.2fm)",m_tpaVisibleObjects[i]->cName(),vPosition.distance_to(m_tpaVisibleObjects[i]->Position()));\
		}\
	}\
	bStopThinking = true;\
}
/**/

#ifndef DEBUG
	#define WRITE_TO_LOG(s)
#endif

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
			case aiRatFreeRecoil : {
				FreeRecoil();
				break;
			}
			case aiRatReturnHome : {
				ReturnHome();
				break;   
			}
			case aiRatEatCorpse : {
				EatCorpse();
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
	Msg("%s [health : %4.1f]",cName(),m_fFood);

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
		if (vPosition.y - fY > EPS_L) {
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
	else {
		if (m_fFood <= 0) {
			if (m_dwLastRangeSearch <= m_dwDeathTime)
				m_dwLastRangeSearch = Level().timeServer();
			setVisible(false);
			if (Level().timeServer() - m_dwLastRangeSearch > 10000) {
				NET_Packet			P;
				u_EventGen			(P,GE_DESTROY,ID());
				u_EventSend			(P);
			}
		}
	}
}

void CAI_Rat::Turn()
{
	WRITE_TO_LOG("Turning...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)

	mRotate.setHPB(m_tHPB.x = -r_torso_current.yaw,m_tHPB.y,m_tHPB.z);
	UpdateTransform();

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
	
	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(((m_Enemy.Enemy) && ((m_Enemy.Enemy->Position().distance_to(m_tSafeSpawnPosition) < m_fMaxPursuitRadius) || (vPosition.distance_to(m_tSafeSpawnPosition) > m_fMaxHomeRadius))),aiRatAttackRun);

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(m_fMorale < m_fMoraleNormalValue,aiRatUnderFire);
	
	if ((m_tLastSound.dwTime >= m_dwLastUpdateTime) && ((!m_tLastSound.tpEntity) || (m_tLastSound.tpEntity->g_Team() != g_Team())) && (!m_Enemy.Enemy)) {
		if (m_tLastSound.tpEntity)
			m_tSavedEnemy = m_tLastSound.tpEntity;
		m_tSavedEnemyPosition = m_tLastSound.tSavedPosition;
		m_dwLostEnemyTime = Level().timeServer();
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatFreeRecoil);
	}
    
	SelectCorp(m_Enemy);

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(m_Enemy.Enemy,aiRatEatCorpse);
	
	m_tSpawnPosition.set(m_tSafeSpawnPosition);
	m_fGoalChangeDelta		= m_fSafeGoalChangeDelta;
	m_tVarGoal.set			(m_tGoalVariation);
	m_fASpeed				= m_fAngleSpeed;
	
	if (bfCheckIfGoalChanged()) {
		if (m_bStateChanged || (vPosition.distance_to(m_tSpawnPosition) > m_fStableDistance) || (::Random.randF(0,1) > m_fChangeActiveStateProbability))
			if (vPosition.distance_to(m_tSafeSpawnPosition) > m_fMaxHomeRadius)
				m_fSpeed = m_fSafeSpeed = m_fMaxSpeed;
			else
				vfChooseNewSpeed();
		else {
			vfRemoveActiveMember();
		}
	}

	vfUpdateTime(m_fTimeUpdateDelta);

	vfComputeNewPosition();

	SetDirectionLook();

	if (!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,PI_DIV_8) || m_bNoWay) {
		m_fSpeed = .1f;
		if (m_bNoWay) {
			float fAngle = ::Random.randF(m_fWallMinTurnValue,m_fWallMaxTurnValue);
			r_torso_target.yaw = r_torso_current.yaw + fAngle;
			r_torso_target.yaw = angle_normalize(r_torso_target.yaw);
			SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatTurn);
		}
	}
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
		u32 dwCurTime = Level().timeServer();
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

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
}

void CAI_Rat::FreeHuntingPassive()
{
	WRITE_TO_LOG("Free hunting passive");

	bStopThinking = true;
	
	if (!g_Alive()) {
		vfAddActiveMember(true);
		bStopThinking = false;
		return;
	}

	SelectEnemy(m_Enemy);
	
	if (m_Enemy.Enemy) {
		m_fGoalChangeTime = 0;
		vfAddActiveMember(true);
		bStopThinking = false;
		return;
	}

	if (m_fMorale < m_fMoraleNormalValue) {
		vfAddActiveMember(true);
		bStopThinking = false;
		return;
	}
	
	if ((m_tLastSound.dwTime >= m_dwLastUpdateTime) && ((!m_tLastSound.tpEntity) || (m_tLastSound.tpEntity->g_Team() != g_Team()))) {
		vfAddActiveMember(true);
		bStopThinking = false;
		return;
	}
	
	m_fSpeed = 0.f;

	UpdateTransform();

	vfAddStandingMember();
	vfAddActiveMember();

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
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
			tTemp.mul(m_fUnderFireDistance);
			m_tSpawnPosition.add(vPosition,tTemp);
		}
		if (m_fMorale >= m_fMoraleNormalValue - EPS_L) {
			GO_TO_PREV_STATE;
		}
	}

	m_tGoalDir = m_tSpawnPosition;
	
	vfUpdateTime(m_fTimeUpdateDelta);

	vfComputeNewPosition();

	SetDirectionLook();

	if (!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,PI_DIV_8) || m_bNoWay) {
		m_fSpeed = .1f;
		if (m_bNoWay) {
			float fAngle = ::Random.randF(m_fWallMinTurnValue,m_fWallMaxTurnValue);
			r_torso_target.yaw = r_torso_current.yaw + fAngle;
			r_torso_target.yaw = angle_normalize(r_torso_target.yaw);
			SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatTurn);
		}
	}
	else 
		m_fSafeSpeed = m_fSpeed = m_fAttackSpeed;

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
}

void CAI_Rat::AttackFire()
{
	WRITE_TO_LOG("Attacking enemy...");

	CHECK_IF_SWITCH_TO_NEW_STATE(!g_Alive(),aiRatDie)
	
	SelectEnemy(m_Enemy);
	
	ERatStates eState = ERatStates(dwfChooseAction(m_dwActionRefreshRate,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),eCurrentState,eCurrentState,aiRatRetreat));
	if (eState != eCurrentState)
		GO_TO_NEW_STATE_THIS_UPDATE(eState);

	if (m_Enemy.Enemy)
		m_dwLostEnemyTime = Level().timeServer();

	if (!(m_Enemy.Enemy) && m_tSavedEnemy && (Level().timeServer() - m_dwLostEnemyTime < m_dwLostMemoryTime))
		m_Enemy.Enemy = m_tSavedEnemy;

	CHECK_IF_GO_TO_PREV_STATE(!(m_Enemy.Enemy));
		
	CHECK_IF_GO_TO_NEW_STATE((m_Enemy.Enemy->Position().distance_to(vPosition) > m_fAttackDistance),aiRatAttackRun)

	Fvector tTemp;
	tTemp.sub(m_Enemy.Enemy->Position(),vPosition);
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);
	
	CHECK_IF_GO_TO_NEW_STATE(!Level().AI.bfTooSmallAngle(r_torso_current.yaw,sTemp.yaw,m_fAttackAngle),aiRatAttackRun)
		
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

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)
	
	if (m_Enemy.Enemy)
		m_dwLostEnemyTime = Level().timeServer();

	SelectEnemy(m_Enemy);

	ERatStates eState = ERatStates(dwfChooseAction(m_dwActionRefreshRate,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),eCurrentState,eCurrentState,aiRatRetreat));
	if (eState != eCurrentState) {
		GO_TO_NEW_STATE_THIS_UPDATE(eState);
	}

	if (!(m_Enemy.Enemy) && m_tSavedEnemy && (Level().timeServer() - m_dwLostEnemyTime < m_dwLostMemoryTime))
		m_Enemy.Enemy = m_tSavedEnemy;

	CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE(m_Enemy.Enemy && (m_tSafeSpawnPosition.distance_to(m_Enemy.Enemy->Position()) > m_fMaxPursuitRadius),aiRatReturnHome);

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(!m_Enemy.Enemy);

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),m_Enemy.Enemy->Position());
	
	Fvector tTemp;
	tTemp.sub(m_Enemy.Enemy->Position(),vPosition);
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);

	if (m_Enemy.Enemy->Position().distance_to(vPosition) <= m_fAttackDistance) {
		if (Level().AI.bfTooSmallAngle(r_torso_target.yaw, sTemp.yaw,m_fAttackAngle)) {
			GO_TO_NEW_STATE_THIS_UPDATE(aiRatAttackFire);
		}
		else {
			r_torso_target.yaw = sTemp.yaw;
			SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatTurn);
		}
	}
	else
		CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,m_fAttackAngle),aiRatTurn);

	INIT_SQUAD_AND_LEADER;
	
	m_tGoalDir.set			(m_Enemy.Enemy->Position());
	
	vfSaveEnemy();

	vfUpdateTime(m_fTimeUpdateDelta);

	if (m_Enemy.Enemy->Position().distance_to(vPosition) <= m_fAttackDistance) {
		vfAimAtEnemy();
		UpdateTransform();
	}
	else {
		vfComputeNextDirectionPosition();
		SetDirectionLook();
	}
	
	vfSetFire(false,Group);

	if (!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,PI_DIV_8) || m_bNoWay) {
		m_fSpeed = EPS_S;
		if (m_bNoWay) {
			if (!::Random.randI(4)) {
				float fAngle = ::Random.randF(m_fWallMinTurnValue,m_fWallMaxTurnValue);
				r_torso_target.yaw = r_torso_current.yaw + fAngle;
			}
			else {
				Fvector tTemp;
				tTemp.sub(m_Enemy.Enemy->Position(),vPosition);
				tTemp.normalize_safe();
				mk_rotation(tTemp,r_torso_target);
			}
			r_torso_target.yaw = angle_normalize(r_torso_target.yaw);
			SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatTurn);
		}
	}
	else 
		m_fSafeSpeed = m_fSpeed = m_fAttackSpeed;
}

void CAI_Rat::Retreat()
{
	WRITE_TO_LOG("Retreat");

	bStopThinking = true;
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)

	SelectEnemy(m_Enemy);

	if (m_Enemy.Enemy && m_Enemy.Enemy->g_Alive()) {
		vfSaveEnemy();
		ERatStates eState = ERatStates(dwfChooseAction(m_dwActionRefreshRate,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),aiRatAttackRun,aiRatAttackRun,aiRatRetreat));
		if (eState != eCurrentState)
			GO_TO_NEW_STATE_THIS_UPDATE(eState);
		m_dwLostEnemyTime = Level().timeServer();
		
		INIT_SQUAD_AND_LEADER;
		
		Fvector tTemp;
		tTemp.sub(vPosition,m_Enemy.Enemy->Position());
		tTemp.normalize_safe();
		tTemp.mul(m_fRetreatDistance);
		m_tSpawnPosition.add(vPosition,tTemp);
	}
	else
		if ((m_Enemy.Enemy && (!m_Enemy.Enemy->g_Alive())) || ((Level().timeServer() - m_dwLostEnemyTime > m_dwRetreatTime) && ((m_tLastSound.dwTime < m_dwLastUpdateTime) || !m_tLastSound.tpEntity || (m_tLastSound.tpEntity->g_Team() == g_Team()) || (!bfCheckIfSoundFrightful())))) {
			GO_TO_PREV_STATE;
		}

	m_tGoalDir = m_tSpawnPosition;
	
	vfUpdateTime(m_fTimeUpdateDelta);
	vfComputeNewPosition();
	SetDirectionLook();

	if (!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,PI_DIV_8) || m_bNoWay) {
		m_fSpeed = .1f;
		if (m_bNoWay) {
			float fAngle = ::Random.randF(m_fWallMinTurnValue,m_fWallMaxTurnValue);
			r_torso_target.yaw = r_torso_current.yaw + fAngle;
			r_torso_target.yaw = angle_normalize(r_torso_target.yaw);
			SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatTurn);
		}
	}
	else {
		m_fSafeSpeed = m_fSpeed = m_fAttackSpeed;
	}

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
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

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(Level().timeServer() - m_dwLostEnemyTime >= m_dwLostMemoryTime);
	
	if ((m_tLastSound.dwTime >= m_dwLastUpdateTime) && ((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_BULLET_RICOCHET) == SOUND_TYPE_WEAPON_BULLET_RICOCHET)) {
		if (m_tLastSound.tpEntity)
			m_tSavedEnemy = m_tLastSound.tpEntity;
		m_tSavedEnemyPosition = m_tLastSound.tSavedPosition;
		m_dwLostEnemyTime = Level().timeServer();
		GO_TO_NEW_STATE_THIS_UPDATE(aiRatFreeRecoil);
	}

	m_tGoalDir.set(m_tSavedEnemyPosition);
	
	if (bfCheckIfGoalChanged(false))
		vfChooseNewSpeed();
	
	vfUpdateTime(m_fTimeUpdateDelta);

	vfComputeNewPosition();

	SetDirectionLook();

	if (!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,PI_DIV_8) || m_bNoWay) {
		m_fSpeed = .1f;
		if (m_bNoWay) {
			float fAngle = ::Random.randF(m_fWallMinTurnValue,m_fWallMaxTurnValue);
			r_torso_target.yaw = r_torso_current.yaw + fAngle;
			r_torso_target.yaw = angle_normalize(r_torso_target.yaw);
			SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatTurn);
		}
	}
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

void CAI_Rat::FreeRecoil()
{
	WRITE_TO_LOG("Free hunting : Recoil from something");

	CHECK_IF_SWITCH_TO_NEW_STATE(!g_Alive(),aiRatDie)
	
	SelectEnemy(m_Enemy);
		
	if (m_Enemy.Enemy)
		m_dwLostEnemyTime = Level().timeServer();

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(m_Enemy.Enemy);

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(m_fMorale < m_fMoraleNormalValue);

	if ((m_tLastSound.dwTime >= m_dwLastUpdateTime) && ((!m_tLastSound.tpEntity) || (m_tLastSound.tpEntity->g_Team() != g_Team()))) {
		m_dwLostEnemyTime = Level().timeServer();
		Fvector tTemp;
		tTemp.setHP(r_torso_current.yaw,r_torso_current.pitch);
		tTemp.normalize_safe();
		tTemp.mul(m_fUnderFireDistance);
		m_tSavedEnemyPosition = m_tLastSound.tSavedPosition;
		m_tSpawnPosition.add(vPosition,tTemp);
	}
	
	CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE(Level().timeServer() - m_dwLostEnemyTime >= m_dwLostRecoilTime,aiRatPursuit);
	
	m_tGoalDir.set(m_tSpawnPosition);
	
	m_fSafeSpeed = m_fSpeed = m_fMaxSpeed;
	
	vfUpdateTime(m_fTimeUpdateDelta);

	vfComputeNewPosition();

	if (!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,PI_DIV_8) || m_bNoWay) {
		m_fSpeed = .1f;
		if (m_bNoWay) {
			float fAngle = ::Random.randF(m_fWallMinTurnValue,m_fWallMaxTurnValue);
			r_torso_target.yaw = r_torso_current.yaw + fAngle;
			r_torso_target.yaw = angle_normalize(r_torso_target.yaw);
			SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatTurn);
		}
	}
	else 
		m_fSafeSpeed = m_fSpeed = m_fMaxSpeed;

	SetDirectionLook();

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));
}

void CAI_Rat::ReturnHome()
{
	WRITE_TO_LOG("Returning home");

	bStopThinking = true;
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)

	SelectEnemy(m_Enemy);
	
	if (m_Enemy.Enemy && (m_tSafeSpawnPosition.distance_to(m_Enemy.Enemy->Position()) < m_fMaxPursuitRadius)) {
		SelectEnemy(m_Enemy);
		m_fGoalChangeTime = 0;
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatAttackRun)
	}
 
	CHECK_IF_GO_TO_PREV_STATE(!m_Enemy.Enemy || !m_Enemy.Enemy->g_Alive() || vPosition.distance_to(m_tSafeSpawnPosition) < m_fMaxHomeRadius);

	m_tSpawnPosition.set	(m_tSafeSpawnPosition);
	m_fGoalChangeDelta		= m_fSafeGoalChangeDelta;
	m_tVarGoal.set			(m_tGoalVariation);
	m_fASpeed				= m_fAngleSpeed;
	m_tGoalDir.set			(m_tSafeSpawnPosition);
	m_fSpeed = m_fSafeSpeed = m_fAttackSpeed;

	vfUpdateTime(m_fTimeUpdateDelta);

	vfComputeNewPosition();

	SetDirectionLook();

	if (!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,PI_DIV_8) || m_bNoWay) {
		m_fSpeed = .1f;
		if (m_bNoWay) {
			float fAngle = ::Random.randF(m_fWallMinTurnValue,m_fWallMaxTurnValue);
			r_torso_target.yaw = r_torso_current.yaw + fAngle;
			r_torso_target.yaw = angle_normalize(r_torso_target.yaw);
			SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatTurn);
		}
	}
	else 
		m_fSafeSpeed = m_fSpeed = m_fAttackSpeed;

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
}

void CAI_Rat::EatCorpse()
{
	WRITE_TO_LOG("Eating a corpse");

	m_bFiring = false;

	CHECK_IF_GO_TO_NEW_STATE(!g_Alive(),aiRatDie)

	SelectEnemy(m_Enemy);

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(m_Enemy.Enemy);

	SelectCorp(m_Enemy);

	if (!m_Enemy.Enemy) {
		vfChangeGoal();
		m_fGoalChangeTime = 10.f;
	}

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(!m_Enemy.Enemy || (m_fMorale < m_fMoraleNormalValue));

	vfSaveEnemy();
	
	if ((m_tLastSound.dwTime >= m_dwLastUpdateTime) && ((!m_tLastSound.tpEntity) || (m_tLastSound.tpEntity->g_Team() != g_Team())) && (!m_Enemy.Enemy)) {
		if (m_tLastSound.tpEntity)
			m_tSavedEnemy = m_tLastSound.tpEntity;
		m_tSavedEnemyPosition = m_tLastSound.tSavedPosition;
		m_dwLostEnemyTime = Level().timeServer();
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatFreeRecoil);
	}
	m_tGoalDir.set			(m_Enemy.Enemy->Position());
	
	vfUpdateTime(m_fTimeUpdateDelta);

	if (m_Enemy.Enemy->Position().distance_to(vPosition) <= m_fAttackDistance) {
		vfAimAtEnemy();
		m_fSpeed = 0;
		UpdateTransform();
	}
	else {
		vfComputeNextDirectionPosition();
		if (!feel_touch.size())
			m_fSpeed = m_fMaxSpeed;
		else
			m_fSpeed = 0;
		SetDirectionLook();
	}

	Fvector tTemp;
	tTemp.sub(m_Enemy.Enemy->Position(),vPosition);
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);

	if (!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,PI_DIV_8) || m_bNoWay) {
		m_fSpeed = .1f;
		if (m_bNoWay) {
			float fAngle = ::Random.randF(m_fWallMinTurnValue,m_fWallMaxTurnValue);
			r_torso_target.yaw = r_torso_current.yaw + fAngle;
			r_torso_target.yaw = angle_normalize(r_torso_target.yaw);
			SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatTurn);
		}
	}
	else 
		if (m_fSafeSpeed != m_fSpeed)
			m_fSafeSpeed = m_fSpeed = m_fMaxSpeed;
	
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
	
	m_Enemy.Enemy->clCenter(tTemp);
	//tTemp = m_Enemy.Enemy->Position();
	if (tTemp.distance_to(vPosition) <= m_fAttackDistance) {
		m_fSpeed = m_fSafeSpeed = 0.f;
		if (Level().AI.bfTooSmallAngle(r_torso_target.yaw, sTemp.yaw,m_fAttackAngle)) {
			if (Level().timeServer() - m_dwLastRangeSearch > m_dwHitInterval) {
				m_dwLastRangeSearch = Level().timeServer();
				m_Enemy.Enemy->m_fFood -= m_fHitPower/10.f;
			}
			m_bFiring = true;
		}
		else {
			r_torso_target.yaw = sTemp.yaw;
			SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatTurn);
		}
	}
	else {
		CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,m_fAttackAngle),aiRatTurn);
	}
}