////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat.cpp
//	Created 	: 25.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_rat.h"
#include "ai_rat_selectors.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\hudmanager.h"
#include "..\\..\\..\\xr_trims.h"

#define TORSO_ANGLE_DELTA				(PI/30.f)
//#define PRE_THINK_COUNT					4
#define ATTACK_DISTANCE						.5f
#define ATTACK_ANGLE					PI_DIV_6
#define LOST_MEMORY_TIME				30000
#define UNDER_FIRE_TIME					10000
#define UNDER_FIRE_DISTACNE				12000

void CAI_Rat::Die()
{
	WRITE_TO_LOG("Dying...");

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
	AI_Path.TravelPath.clear();
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

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

	CHECK_IF_GO_TO_PREV_STATE(Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw, PI_DIV_6))
	
	INIT_SQUAD_AND_LEADER
	
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	vfSetFire(false,Group);

	float fTurnAngle = min(fabsf(r_torso_target.yaw - r_torso_current.yaw), PI_MUL_2 - fabsf(r_torso_target.yaw - r_torso_current.yaw));
	r_torso_speed = 3*fTurnAngle;

	vfSetMovementType(BODY_STATE_STAND,0);
}

void CAI_Rat::AttackFire()
{
	WRITE_TO_LOG("Attacking enemy...");

	CHECK_IF_SWITCH_TO_NEW_STATE(!g_Alive(),aiRatDie)
	
	SelectEnemy(Enemy);
	
	CHECK_IF_GO_TO_PREV_STATE(!(Enemy.Enemy) || !Enemy.Enemy->g_Alive())
		
	CHECK_IF_GO_TO_NEW_STATE((Enemy.Enemy->Position().distance_to(vPosition) > ATTACK_DISTANCE),aiRatAttackRun)

	Fvector tTemp;
	tTemp.sub(Enemy.Enemy->Position(),vPosition);
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);
	
	CHECK_IF_GO_TO_NEW_STATE(!Level().AI.bfTooSmallAngle(r_torso_current.yaw,sTemp.yaw,ATTACK_ANGLE),aiRatAttackRun)
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),Enemy.Enemy->Position());

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
	
	SelectEnemy(Enemy);
	
	if (Enemy.Enemy)
		m_dwLostEnemyTime = Level().timeServer();
	if (!(Enemy.Enemy) && tSavedEnemy && ((tSavedEnemy->Position().distance_to(vPosition) < ffGetRange()) || (Level().timeServer() - m_dwLostEnemyTime < LOST_MEMORY_TIME)))
		Enemy.Enemy = tSavedEnemy;
	CHECK_IF_GO_TO_PREV_STATE(!Enemy.Enemy || !Enemy.Enemy->g_Alive())
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),Enemy.Enemy->Position());
	
	Fvector tTemp;
	tTemp.sub(Enemy.Enemy->Position(),vPosition);
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);

	CHECK_IF_GO_TO_NEW_STATE(Level().AI.bfTooSmallAngle(r_torso_target.yaw, sTemp.yaw,ATTACK_ANGLE) && (Enemy.Enemy->Position().distance_to(vPosition) <= ATTACK_DISTANCE),aiRatAttackFire);

	CHECK_IF_SWITCH_TO_NEW_STATE(!Level().AI.bfTooSmallAngle(r_torso_target.yaw, r_torso_current.yaw,ATTACK_ANGLE),aiRatTurn)

	INIT_SQUAD_AND_LEADER;
	
	m_tGoalDir.set			(Enemy.Enemy->Position());
	m_fASpeed				= .3f;
	m_tSpawnPosition.set	(Enemy.Enemy->Position());
	m_tVarGoal.set			(0,0,0);
	if (m_fSpeed > EPS_L) {
		m_fGoalChangeDelta		= .5f;
		m_fSpeed				= m_fCurSpeed = m_fMaxSpeed;
	}
	else
		m_fGoalChangeDelta		= 3.f;

	vfSaveEnemy();

	vfUpdateTime(m_fTimeUpdateDelta);

	if (Enemy.Enemy->Position().distance_to(vPosition) <= ATTACK_DISTANCE) {
		vfAimAtEnemy();
		r_torso_target.pitch = 0;
	}
	else {
		vfComputeNewPosition();
		SetDirectionLook();
	}
	
	vfSetFire(false,Group);

	vfSetMovementType(m_cBodyState,m_fMaxSpeed);
	
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
}

void CAI_Rat::FreeHunting()
{
	WRITE_TO_LOG("Free hunting");

	bStopThinking = true;
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)

	SelectEnemy(Enemy);
	
	if (Enemy.Enemy) {
		m_fGoalChangeTime = 0;
		SWITCH_TO_NEW_STATE(aiRatAttackFire)
	}

	if (m_tLastSound.dwTime >= m_dwLastUpdateTime) {
		if ((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_SHOOTING) != SOUND_TYPE_WEAPON_SHOOTING) {
			tSavedEnemy = m_tLastSound.tpEntity;
			m_dwLostEnemyTime = Level().timeServer();
			SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatAttackRun);
		}
		m_dwLastRangeSearch = Level().timeServer();
		Fvector tTemp;
		tTemp.setHP(r_torso_current.yaw,r_torso_current.pitch);
		tTemp.normalize_safe();
		tTemp.mul(UNDER_FIRE_DISTACNE);
		m_tSpawnPosition.add(vPosition,tTemp);
		m_fGoalChangeTime = 0;
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatUnderFire);
	}
    m_tSpawnPosition.set(m_tSafeSpawnPosition);
	m_fGoalChangeDelta		= 10.f;
	m_tVarGoal.set			(10.0,0.0,20.0);
	m_fASpeed				= .2f;
	
	if (bfCheckIfGoalChanged())
		vfChooseNewSpeed();

	vfUpdateTime(m_fTimeUpdateDelta);

	vfComputeNewPosition();

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
	AI_Path.TravelPath.clear();
}

void CAI_Rat::UnderFire()
{
	WRITE_TO_LOG("UnderFire");

	bStopThinking = true;
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)

	SelectEnemy(Enemy);
	
	if (Enemy.Enemy) {
		m_fGoalChangeTime = 0;
		SWITCH_TO_NEW_STATE(aiRatAttackFire)
	}

	if (m_tLastSound.dwTime >= m_dwLastUpdateTime) {
		if ((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_SHOOTING) != SOUND_TYPE_WEAPON_SHOOTING) {
			tSavedEnemy = m_tLastSound.tpEntity;
			m_dwLostEnemyTime = Level().timeServer();
			SWITCH_TO_NEW_STATE(aiRatAttackRun);
		}
		m_dwLastRangeSearch = Level().timeServer();
		Fvector tTemp;
		tTemp.setHP(r_torso_current.yaw,r_torso_current.pitch);
		tTemp.normalize_safe();
		tTemp.mul(UNDER_FIRE_DISTACNE);
		m_tSpawnPosition.add(vPosition,tTemp);
		m_fGoalChangeTime = 0;
	}

	if (Level().timeServer() - m_dwLastRangeSearch > UNDER_FIRE_TIME) {
		m_tSafeSpawnPosition.set(Level().Teams[g_Team()].Squads[g_Squad()].Leader->Position());
		GO_TO_PREV_STATE;
	}

	m_fGoalChangeDelta		= 10.f;
	m_tVarGoal.set			(10.0,0.0,20.0);
	m_fASpeed				= .2f;
	
	bfCheckIfGoalChanged();
	
	m_fSpeed = m_fMaxSpeed;
	
	vfUpdateTime(m_fTimeUpdateDelta);

	vfComputeNewPosition();

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
	AI_Path.TravelPath.clear();
}

void CAI_Rat::Think()
{
	bStopThinking = false;
	do {
		m_ePreviousState = eCurrentState;
		switch(eCurrentState) {
			case aiRatDie : {
				Die();
				break;
			}
			case aiRatFreeHunting : {
				FreeHunting();
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
		}
		m_bStateChanged = m_ePreviousState != eCurrentState;
	}
	while (!bStopThinking);
}
