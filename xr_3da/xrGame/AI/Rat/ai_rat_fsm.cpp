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
#include "..\\..\\..\\3dsound.h"
#include "..\\..\\..\\xr_trims.h"

void CAI_Rat::Die()
{
	WRITE_TO_LOG("Dying...");

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
	AI_Path.TravelPath.clear();
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

	//bActive = false;
	bEnabled = false;
}

void CAI_Rat::Jumping()
{
	WRITE_TO_LOG("Jumping...");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)
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

	GoToPointViaSubnodes(Leader->Position());
	
	SetDirectionLook();

	vfSetFire(false,Group);

	vfSetMovementType(BODY_STATE_STAND,m_fMinSpeed);
}

void CAI_Rat::AttackFire()
{
	WRITE_TO_LOG("Attacking enemy...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiRatDie)
	
	SelectEnemy(Enemy);
	
	DWORD dwCurTime = Level().timeServer();
	
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
	
	CHECK_IF_GO_TO_NEW_STATE(fabsf(r_torso_current.yaw - sTemp.yaw) > 2*PI_DIV_6,aiRatAttackRun)
		
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
	
	DWORD dwCurTime = Level().timeServer();
	
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

	CHECK_IF_GO_TO_NEW_STATE((fabsf(r_torso_current.yaw - sTemp.yaw) < 2*PI_DIV_6) && (tDistance.magnitude() <= 2.f),aiRatAttackFire);

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
			case aiRatJumping : {
				Jumping();
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
		}
		m_bStateChanged = m_ePreviousState != eCurrentState;
	}
	while (!bStopThinking);
}
