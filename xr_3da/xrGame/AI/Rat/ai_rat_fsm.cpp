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

void CAI_Rat::Die()
{
	WRITE_TO_LOG("Dying...");

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
	AI_Path.TravelPath.clear();
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

	setEnabled	(false);
	
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

void CAI_Rat::FreeHunting()
{
	WRITE_TO_LOG("Free hunting");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)
		
	SelectEnemy(Enemy);
	
	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiRatAttackFire)
	
	DWORD dwCurTime = Level().timeServer();
	
	//CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiSoldierUnderFire)
	
	//CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwSenseTime < SENSE_JUMP_TIME) && (dwSenseTime),aiSoldierSenseSomething)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
	//CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiSoldierUnderFire)
	
	//CHECK_IF_SWITCH_TO_NEW_STATE(m_tpaPatrolPoints.size(),aiSoldierPatrolRoute)

	/**/
	if (fabsf(r_torso_target.yaw - r_torso_current.yaw) < PI) {
		CHECK_IF_SWITCH_TO_NEW_STATE(fabsf(r_torso_target.yaw - r_torso_current.yaw) >= TORSO_ANGLE_DELTA,aiRatTurn)
	}
	else {
		CHECK_IF_SWITCH_TO_NEW_STATE(PI_MUL_2 - fabsf(r_torso_target.yaw - r_torso_current.yaw) >= TORSO_ANGLE_DELTA,aiRatTurn)
	}
	/**/

	vfInitSelector(SelectorFreeHunting,Squad,Leader);

	/**/
	if ((AI_Path.TravelPath.empty()) || (AI_Path.TravelStart >= AI_Path.TravelPath.size() - 2) || (!AI_Path.fSpeed)) {
		if (ps_Size() > 1)
			if (ps_Element(ps_Size() - 1).dwTime - ps_Element(ps_Size() - 2).dwTime < 500)
				SelectorFreeHunting.m_tDirection.sub(ps_Element(ps_Size() - 2).vPosition,ps_Element(ps_Size() - 1).vPosition);
			else {
				Fvector tDistance;
				tDistance.sub(ps_Element(ps_Size() - 1).vPosition,ps_Element(ps_Size() - 2).vPosition);
				if (tDistance.magnitude() < .01f)
					SelectorFreeHunting.m_tDirection.sub(ps_Element(ps_Size() - 2).vPosition,ps_Element(ps_Size() - 1).vPosition);
				else
					SelectorFreeHunting.m_tDirection.sub(ps_Element(ps_Size() - 1).vPosition,ps_Element(ps_Size() - 2).vPosition);
				//SelectorFreeHunting.m_tDirection.sub(ps_Element(ps_Size() - 2).vPosition,ps_Element(ps_Size() - 1).vPosition);
			}
		else
			SelectorFreeHunting.m_tDirection.set(::Random.randF(0,1),0,::Random.randF(0,1));
		SelectorFreeHunting.m_tDirection.normalize_safe();
		vfSearchForBetterPositionWTime(SelectorFreeHunting,Squad,Leader);
		vfBuildPathToDestinationPoint(0);
	}
	else
		if (ps_Size() > 1)
			if (ps_Element(ps_Size() - 1).dwTime - ps_Element(ps_Size() - 2).dwTime > 500) {
				SelectorFreeHunting.m_tDirection.sub(ps_Element(ps_Size() - 2).vPosition,ps_Element(ps_Size() - 1).vPosition);
				SelectorFreeHunting.m_tDirection.normalize_safe();
				vfSearchForBetterPositionWTime(SelectorFreeHunting,Squad,Leader);
				vfBuildPathToDestinationPoint(0);
			}
			else {
				Fvector tDistance;
				tDistance.sub(ps_Element(ps_Size() - 1).vPosition,ps_Element(ps_Size() - 2).vPosition);
				if (tDistance.magnitude() < .01f) {
					SelectorFreeHunting.m_tDirection.sub(ps_Element(ps_Size() - 2).vPosition,ps_Element(ps_Size() - 1).vPosition);
					SelectorFreeHunting.m_tDirection.normalize_safe();
					vfSearchForBetterPositionWTime(SelectorFreeHunting,Squad,Leader);
					vfBuildPathToDestinationPoint(0);
				}
			}
	/**/

	SetDirectionLook();

	if	(!m_tpSoundBeingPlayed || !m_tpSoundBeingPlayed->feedback) {
		if (m_tpSoundBeingPlayed && !m_tpSoundBeingPlayed->feedback) {
			m_tpSoundBeingPlayed = 0;
			m_dwLastVoiceTalk = dwCurTime;
		}
		if ((dwCurTime - m_dwLastSoundRefresh > m_fVoiceRefreshRate) && ((dwCurTime - m_dwLastVoiceTalk > m_fMaxVoiceIinterval) || ((dwCurTime - m_dwLastVoiceTalk > m_fMinVoiceIinterval) && (::Random.randF(0,1) > (dwCurTime - m_dwLastVoiceTalk - m_fMinVoiceIinterval)/(m_fMaxVoiceIinterval - m_fMinVoiceIinterval))))) {
			m_dwLastSoundRefresh = dwCurTime;
			// Play hit-sound
			m_tpSoundBeingPlayed = &(sndVoices[Random.randI(SND_VOICE_COUNT)]);
			
			if (m_tpSoundBeingPlayed->feedback)			
				return;

			pSounds->PlayAtPos(*m_tpSoundBeingPlayed,this,vPosition);
		}
	}

	vfSetFire(false,Group);

	vfSetMovementType(BODY_STATE_STAND,m_fMinSpeed);
}

void CAI_Rat::FollowLeader()
{
	WRITE_TO_LOG("Following leader");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)

	SelectEnemy(Enemy);
	
	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiRatAttackFire)
	
	DWORD dwCurTime = Level().timeServer();
	
	//CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiSoldierUnderFire)
	
	//CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwSenseTime < SENSE_JUMP_TIME) && (dwSenseTime),aiSoldierSenseSomething)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
	//CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiSoldierUnderFire)
	
	//CHECK_IF_SWITCH_TO_NEW_STATE(m_tpaPatrolPoints.size(),aiSoldierPatrolRoute)

	CHECK_IF_GO_TO_NEW_STATE(Leader == this,aiRatFreeHunting)

	CHECK_IF_SWITCH_TO_NEW_STATE(fabsf(r_torso_current.yaw - r_torso_target.yaw) >= PI_DIV_6,aiRatTurn)

	GoToPointViaSubnodes(Leader->Position());
	
	SetDirectionLook();

	if	(!m_tpSoundBeingPlayed || !m_tpSoundBeingPlayed->feedback) {
		if (m_tpSoundBeingPlayed && !m_tpSoundBeingPlayed->feedback) {
			m_tpSoundBeingPlayed = 0;
			m_dwLastVoiceTalk = dwCurTime;
		}
		if ((dwCurTime - m_dwLastSoundRefresh > m_fVoiceRefreshRate) && ((dwCurTime - m_dwLastVoiceTalk > m_fMaxVoiceIinterval) || ((dwCurTime - m_dwLastVoiceTalk > m_fMinVoiceIinterval) && (::Random.randF(0,1) > (dwCurTime - m_dwLastVoiceTalk - m_fMinVoiceIinterval)/(m_fMaxVoiceIinterval - m_fMinVoiceIinterval))))) {
			m_dwLastSoundRefresh = dwCurTime;
			// Play hit-sound
			m_tpSoundBeingPlayed = &(sndVoices[Random.randI(SND_VOICE_COUNT)]);
			
			if (m_tpSoundBeingPlayed->feedback)			
				return;

			pSounds->PlayAtPos(*m_tpSoundBeingPlayed,this,vPosition);
		}
	}

	vfSetFire(false,Group);

	vfSetMovementType(BODY_STATE_STAND,m_fMinSpeed);
}

void CAI_Rat::AttackFire()
{
	WRITE_TO_LOG("Attacking enemy...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)
	
	SelectEnemy(Enemy);
	
//	DWORD dwCurTime = Level().timeServer();
	
	/**
	if (!(Enemy.Enemy)) {
		CHECK_IF_GO_TO_PREV_STATE(((tSavedEnemy) && (tSavedEnemy->g_Health() <= 0)) || (!tSavedEnemy))
		dwLostEnemyTime = Level().timeServer();
		GO_TO_NEW_STATE(aiRatPursuit);
	}
	/**/
	
	//CHECK_IF_SWITCH_TO_NEW_STATE(!(Enemy.bVisible),aiRatFindEnemy)
	CHECK_IF_GO_TO_PREV_STATE(!(Enemy.Enemy) || !(Enemy.bVisible))
		
	CHECK_IF_GO_TO_NEW_STATE((Enemy.Enemy->Position().distance_to(vPosition) > 2.f),aiRatAttackRun)

	Fvector tTemp;
	tTemp.sub(Enemy.Enemy->Position(),vPosition);
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);
	
	CHECK_IF_GO_TO_NEW_STATE(fabsf(r_torso_current.yaw - sTemp.yaw) > PI_DIV_6,aiRatAttackRun)
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),Enemy.Enemy->Position());

	AI_Path.TravelPath.clear();
	
	vfSaveEnemy();

	//vfAimAtEnemy();
	SetDirectionLook();

	vfSetFire(true,Group);

	vfSetMovementType(m_cBodyState,0);
}

void CAI_Rat::AttackRun()
{
	WRITE_TO_LOG("Attack enemy");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)
	
	SelectEnemy(Enemy);
	
//	DWORD dwCurTime = Level().timeServer();
	
	//CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiRatLyingDown)
	
	/**
	if (!(Enemy.Enemy)) {
		CHECK_IF_GO_TO_PREV_STATE(((tSavedEnemy) && (tSavedEnemy->g_Health() <= 0)) || (!tSavedEnemy))
		dwLostEnemyTime = Level().timeServer();
		GO_TO_NEW_STATE(aiRatPursuit);
	}
	/**/
	
	//CHECK_IF_SWITCH_TO_NEW_STATE(!(Enemy.bVisible),aiRatFindEnemy)
	CHECK_IF_GO_TO_PREV_STATE(!(Enemy.Enemy) || !(Enemy.bVisible))
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),Enemy.Enemy->Position());
	
	Fvector tTemp;
	tTemp.sub(Enemy.Enemy->Position(),vPosition);
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);

	CHECK_IF_GO_TO_NEW_STATE((fabsf(r_torso_current.yaw - sTemp.yaw) < PI_DIV_6) && (tDistance.magnitude() <= 2.f),aiRatAttackFire);

	CHECK_IF_SWITCH_TO_NEW_STATE(fabsf(r_torso_current.yaw - sTemp.yaw) >= PI_DIV_6,aiRatTurn)

	INIT_SQUAD_AND_LEADER;
	
	/**
	vfInitSelector(SelectorAttack,Squad,Leader);
	
	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(&SelectorAttack);
	else
		vfSearchForBetterPosition(SelectorAttack,Squad,Leader);
	/**/

	GoToPointViaSubnodes(Enemy.Enemy->Position());
	
	//vfAimAtEnemy();
	SetDirectionLook();
	
	vfSetFire(false,Group);

	vfSetMovementType(m_cBodyState,m_fMaxSpeed);
}

void CAI_Rat::Turn()
{
	WRITE_TO_LOG("Turning...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)

	//CHECK_IF_GO_TO_PREV_STATE((!m_bStateChanged) && (m_tpCurrentGlobalAnimation != tRatAnimations.tNormal.tGlobal.tpTurnLeft) && (m_tpCurrentGlobalAnimation != tRatAnimations.tNormal.tGlobal.tpTurnRight))
	CHECK_IF_GO_TO_PREV_STATE(fabsf(r_torso_target.yaw - r_torso_current.yaw) < TORSO_ANGLE_DELTA)
	
	INIT_SQUAD_AND_LEADER
	
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	vfSetFire(false,Group);

	float fTurnAngle = min(fabsf(r_torso_target.yaw - r_torso_current.yaw), PI_MUL_2 - fabsf(r_torso_target.yaw - r_torso_current.yaw));
	r_torso_speed = 3*fTurnAngle;
	//AI_Path.TravelPath.clear();

	vfSetMovementType(BODY_STATE_STAND,0);
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
			case aiRatFollowLeader : {
				FollowLeader();
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
		}
		m_bStateChanged = m_ePreviousState != eCurrentState;
	}
	while (!bStopThinking);
}
