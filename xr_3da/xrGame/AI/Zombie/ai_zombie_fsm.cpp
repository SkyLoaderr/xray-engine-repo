////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie_fsm.cpp
//	Created 	: 25.04.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_zombie.h"
#include "..\\ai_monsters_misc.h"

/**
//#define WRITE_TO_LOG(s) bStopThinking = true;
#undef WRITE_TO_LOG
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

void CAI_Zombie::Think()
{
	bStopThinking = false;
	do {
		m_ePreviousState = eCurrentState;
		switch(eCurrentState) {
			case aiZombieDie : {
				Death();
				break;
			}
			case aiZombieFreeHuntingActive : {
				FreeHuntingActive();
				break;
			}
			case aiZombieFreeHuntingPassive : {
				FreeHuntingPassive();
				break;
			}
			case aiZombieAttackFire : {
				AttackFire();
				break;
			}
			case aiZombieAttackRun : {
				AttackRun();
				break;
			}
			case aiZombieTurn : {
				Turn();
				break;
			}
			case aiZombiePursuit : {
				Pursuit();
				break;
			}
			case aiZombieReturnHome : {
				ReturnHome();
				break;
			}
			case aiZombieResurrect : {
				Resurrect();
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
void CAI_Zombie::Death()
{
	WRITE_TO_LOG("Dying...");
	bStopThinking = true;

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
	
	AI_Path.TravelPath.clear();

	UpdateTransform();

	if (m_fFood <= 0) {
		if (m_dwLastRangeSearch <= m_dwDeathTime)
			m_dwLastRangeSearch = Level().timeServer();
		setVisible(false);
		if (Level().timeServer() - m_dwLastRangeSearch > m_dwToWaitBeforeDestroy) {
			NET_Packet			P;
			u_EventGen			(P,GE_DESTROY,ID());
			u_EventSend			(P);
		}
	}
	else {
		if (Level().timeServer() - m_dwDeathTime > m_dwTimeToLie) {
			//m_fFood = Movement.GetMass()*100;
			fHealth = m_fMaxHealthValue;
			m_tpSoundBeingPlayed = 0;
			m_dwLastRangeSearch = Level().timeServer();
			GO_TO_NEW_STATE(aiZombieResurrect);
		}
	}

	SelectAnimation(clTransform.k,mRotate.k,0);
}

void CAI_Zombie::Turn()
{
	WRITE_TO_LOG("Turning...");

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

void CAI_Zombie::FreeHuntingActive()
{
	WRITE_TO_LOG("Free hunting active");

	bStopThinking = true;
	
	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	SelectEnemy(m_Enemy);
	
	if (m_Enemy.Enemy) {
		m_tpSoundBeingPlayed = &(m_tpaSoundNotice[::Random.randI(SND_NOTICE_COUNT)]);
		::Sound->PlayAtPos		(*m_tpSoundBeingPlayed,this,eye_matrix.c);
		m_fGoalChangeTime = 0;
		if ((m_Enemy.Enemy->Position().distance_to(m_tSafeSpawnPosition) < m_fMaxPursuitRadius) || (vPosition.distance_to(m_tSafeSpawnPosition) > m_fMaxHomeRadius))
			SWITCH_TO_NEW_STATE_THIS_UPDATE(aiZombieAttackRun)
	}

	if ((m_tLastSound.dwTime >= m_dwLastUpdateTime) && (m_tLastSound.tpEntity) && (m_tLastSound.tpEntity->g_Team() != g_Team())) {
		if (m_tLastSound.tpEntity)
			m_tSavedEnemy = m_tLastSound.tpEntity;
		m_tSavedEnemyPosition = m_tLastSound.tSavedPosition;
		m_dwLostEnemyTime = Level().timeServer();
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiZombiePursuit);
	}
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

	if (bfComputeNewPosition(false))
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiZombieTurn);

	SetDirectionLook();

//	if (Level().timeServer() - m_dwLastRangeSearch > 5000) {
//		m_dwLastRangeSearch = Level().timeServer();
//		PKinematics(pVisual)->PlayFX(m_tZombieAnimations.tNormal.tTorso.tpBlaBlaBla0);
//	}
	
	if	(!m_tpSoundBeingPlayed || !m_tpSoundBeingPlayed->feedback) {
		u32 dwCurTime = Level().timeServer();
		if (m_tpSoundBeingPlayed && !m_tpSoundBeingPlayed->feedback) {
			m_tpSoundBeingPlayed = 0;
			m_dwLastVoiceTalk = dwCurTime;
		}
		if ((dwCurTime - m_dwLastSoundRefresh > m_fVoiceRefreshRate) && ((dwCurTime - m_dwLastVoiceTalk > m_fMaxVoiceIinterval) || ((dwCurTime - m_dwLastVoiceTalk > m_fMinVoiceIinterval) && (::Random.randF(0,1) > (dwCurTime - m_dwLastVoiceTalk - m_fMinVoiceIinterval)/(m_fMaxVoiceIinterval - m_fMinVoiceIinterval))))) {
			m_dwLastSoundRefresh = dwCurTime;
			// Play voice-sound
			m_tpSoundBeingPlayed = &(m_tpaSoundIdle[Random.randI(SND_IDLE_COUNT)]);
			
			if (!m_tpSoundBeingPlayed->feedback)
				::Sound->PlayAtPos(*m_tpSoundBeingPlayed,this,eye_matrix.c);
			else
				m_tpSoundBeingPlayed->feedback->SetPosition(eye_matrix.c);
		}
	}
	else
		if (m_tpSoundBeingPlayed && m_tpSoundBeingPlayed->feedback)
			m_tpSoundBeingPlayed->feedback->SetPosition(eye_matrix.c);

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
}

void CAI_Zombie::FreeHuntingPassive()
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

	if ((m_tLastSound.dwTime >= m_dwLastUpdateTime) && (m_tLastSound.tpEntity) && (m_tLastSound.tpEntity->g_Team() != g_Team())) {
		vfAddActiveMember(true);
		bStopThinking = false;
		return;
	}
	
	m_fSpeed = 0.f;

	UpdateTransform();

	vfAddActiveMember();

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
}

void CAI_Zombie::AttackFire()
{
	WRITE_TO_LOG("Attacking enemy...");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	SelectEnemy(m_Enemy);
	
	if (m_Enemy.Enemy)
		m_dwLostEnemyTime = Level().timeServer();

	if (!(m_Enemy.Enemy) && m_tSavedEnemy && (Level().timeServer() - m_dwLostEnemyTime < m_dwLostMemoryTime))
		m_Enemy.Enemy = m_tSavedEnemy;

	CHECK_IF_GO_TO_PREV_STATE(!(m_Enemy.Enemy) || !m_Enemy.Enemy->g_Alive());
		
	CHECK_IF_GO_TO_NEW_STATE((m_Enemy.Enemy->Position().distance_to(vPosition) > m_fAttackDistance),aiZombieAttackRun)

	Fvector tTemp;
	tTemp.sub(m_Enemy.Enemy->Position(),vPosition);
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);
	
	CHECK_IF_GO_TO_NEW_STATE(!Level().AI.bfTooSmallAngle(r_torso_current.yaw,sTemp.yaw,m_fAttackAngle),aiZombieAttackRun)
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),m_Enemy.Enemy->Position());

	AI_Path.TravelPath.clear();
	
	vfSaveEnemy();

	vfAimAtEnemy();

	vfSetFire(true,Group);

	m_fSpeed = 0.f;
}

void CAI_Zombie::AttackRun()
{
	WRITE_TO_LOG("Attack enemy");
	bStopThinking = true;

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	if (m_Enemy.Enemy)
		m_dwLostEnemyTime = Level().timeServer();

	SelectEnemy(m_Enemy);

	if (!(m_Enemy.Enemy) && m_tSavedEnemy && (Level().timeServer() - m_dwLostEnemyTime < m_dwLostMemoryTime))
		m_Enemy.Enemy = m_tSavedEnemy;

	CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE(m_Enemy.Enemy && (m_tSafeSpawnPosition.distance_to(m_Enemy.Enemy->Position()) > m_fMaxPursuitRadius),aiZombieReturnHome);

	CHECK_IF_GO_TO_PREV_STATE(!(m_Enemy.Enemy) || !m_Enemy.Enemy->g_Alive());

	Fvector tDistance;
	tDistance.sub(Position(),m_Enemy.Enemy->Position());
	
	Fvector tTemp;
	tTemp.sub(m_Enemy.Enemy->Position(),vPosition);
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);

	if (m_Enemy.Enemy->Position().distance_to(vPosition) <= m_fAttackDistance) {
		if (Level().AI.bfTooSmallAngle(r_torso_target.yaw, sTemp.yaw,m_fAttackAngle)) {
			GO_TO_NEW_STATE_THIS_UPDATE(aiZombieAttackFire);
		}
		else {
			r_torso_target.yaw = sTemp.yaw;
			SWITCH_TO_NEW_STATE_THIS_UPDATE(aiZombieTurn);
		}
	}
	else
		CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,m_fAttackAngle),aiZombieTurn);

	INIT_SQUAD_AND_LEADER;
	
	m_tGoalDir.set			(m_Enemy.Enemy->Position());
	
	vfSaveEnemy();

	vfUpdateTime(m_fTimeUpdateDelta);

	m_fSafeSpeed = m_fSpeed = m_fAttackSpeed;

	if (bfComputeNextDirectionPosition())
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiZombieTurn);

	SetDirectionLook();

	if	(!m_tpSoundBeingPlayed || !m_tpSoundBeingPlayed->feedback) {
		u32 dwCurTime = Level().timeServer();
		if (m_tpSoundBeingPlayed && !m_tpSoundBeingPlayed->feedback) {
			m_tpSoundBeingPlayed = 0;
			m_dwLastPursuitTalk = dwCurTime;
		}
		if ((dwCurTime - m_dwLastSoundRefresh > m_fPursuitRefreshRate) && ((dwCurTime - m_dwLastPursuitTalk > m_fMaxPursuitIinterval) || ((dwCurTime - m_dwLastPursuitTalk > m_fMinPursuitIinterval) && (::Random.randF(0,1) > (dwCurTime - m_dwLastPursuitTalk - m_fMinPursuitIinterval)/(m_fMaxPursuitIinterval - m_fMinPursuitIinterval))))) {
			m_dwLastSoundRefresh = dwCurTime;
			// Play pursuit-sound
			m_tpSoundBeingPlayed = &(m_tpaSoundPursuit[Random.randI(SND_PURSUIT_COUNT)]);
			
			if (!m_tpSoundBeingPlayed->feedback)
				::Sound->PlayAtPos(*m_tpSoundBeingPlayed,this,eye_matrix.c);
			else
				m_tpSoundBeingPlayed->feedback->SetPosition(eye_matrix.c);
		}
	}
	else
		if (m_tpSoundBeingPlayed && m_tpSoundBeingPlayed->feedback)
			m_tpSoundBeingPlayed->feedback->SetPosition(eye_matrix.c);
}

void CAI_Zombie::Pursuit()
{
	WRITE_TO_LOG("Pursuit something");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	if (m_Enemy.Enemy)
		m_dwLostEnemyTime = Level().timeServer();

	SelectEnemy(m_Enemy);

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(m_Enemy.Enemy,aiZombieAttackRun);

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(Level().timeServer() - (int)m_tLastSound.dwTime >= m_dwLostMemoryTime);
	
	if (m_tLastSound.dwTime >= m_dwLastUpdateTime) {
		if (m_tLastSound.tpEntity)
			m_tSavedEnemy = m_tLastSound.tpEntity;
		m_tSavedEnemyPosition = m_tLastSound.tSavedPosition;
		m_dwLostEnemyTime = Level().timeServer();
	}

	m_tGoalDir.set(m_tSavedEnemyPosition);
	
	if (bfCheckIfGoalChanged(false))
		vfChooseNewSpeed();
	
	vfUpdateTime(m_fTimeUpdateDelta);

	if (bfComputeNewPosition())
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiZombieTurn);

	SetDirectionLook();
}

void CAI_Zombie::ReturnHome()
{
	WRITE_TO_LOG("Returning home");

	bStopThinking = true;
	
	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	SelectEnemy(m_Enemy);
	
	if (m_Enemy.Enemy && (m_tSafeSpawnPosition.distance_to(m_Enemy.Enemy->Position()) < m_fMaxPursuitRadius)) {
		SelectEnemy(m_Enemy);
		m_fGoalChangeTime = 0;
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiZombieAttackRun)
	}
 
	CHECK_IF_GO_TO_PREV_STATE(vPosition.distance_to(m_tSafeSpawnPosition) < m_fMaxHomeRadius);

	m_tSpawnPosition.set	(m_tSafeSpawnPosition);
	m_fGoalChangeDelta		= m_fSafeGoalChangeDelta;
	m_tVarGoal.set			(m_tGoalVariation);
	m_fASpeed				= m_fAngleSpeed;
	m_tGoalDir.set			(m_tSafeSpawnPosition);
	m_fSpeed = m_fSafeSpeed = m_fAttackSpeed;

	vfUpdateTime(m_fTimeUpdateDelta);

	if (bfComputeNewPosition())
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiZombieTurn);

	SetDirectionLook();

	if (!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,PI_DIV_8) || m_bNoWay) {
		m_fSpeed = .1f;
		if (m_bNoWay) {
			float fAngle = ::Random.randF(m_fWallMinTurnValue,m_fWallMaxTurnValue);
			r_torso_target.yaw = r_torso_current.yaw + fAngle;
			r_torso_target.yaw = angle_normalize(r_torso_target.yaw);
			SWITCH_TO_NEW_STATE_THIS_UPDATE(aiZombieTurn);
		}
	}
	else 
		m_fSafeSpeed = m_fSpeed = m_fAttackSpeed;

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
}

void CAI_Zombie::Resurrect()
{
	WRITE_TO_LOG("Resurrecting");

	bStopThinking = true;

	if (!g_Alive())
		return;
	
	m_fSpeed = 0.f;

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);

	if	(!m_tpSoundBeingPlayed || !m_tpSoundBeingPlayed->feedback) {
		m_tpSoundBeingPlayed = &(m_tpaSoundResurrect[Random.randI(SND_RESURRECT_COUNT)]);

		if (!m_tpSoundBeingPlayed->feedback)
			::Sound->PlayAtPos(*m_tpSoundBeingPlayed,this,eye_matrix.c);
		else
			m_tpSoundBeingPlayed->feedback->SetPosition(eye_matrix.c);
	}
	else
		if (m_tpSoundBeingPlayed && m_tpSoundBeingPlayed->feedback)
			m_tpSoundBeingPlayed->feedback->SetPosition(eye_matrix.c);
	
	for (int i=0; i<3; i++)
		if (m_tpCurrentGlobalAnimation == m_tZombieAnimations.tNormal.tGlobal.tpaStandUp[i]) {
			bool bOk = false;
			switch (i) {
				case 0 : {
					bOk = (Level().timeServer() - m_dwLastRangeSearch) > 60*1000/30;
					break;
				}
				case 1 : {
					bOk = (Level().timeServer() - m_dwLastRangeSearch) > 80*1000/30;
					break;
				}
				case 2 : {
					bOk = (Level().timeServer() - m_dwLastRangeSearch) > 50*1000/30;
					break;
				}
				default : NODEFAULT;
			}
			if (bOk) {
				m_fGoalChangeTime = 0.f;
				GO_TO_NEW_STATE_THIS_UPDATE(aiZombieFreeHuntingActive);
			}
			break;
		}
}