////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie_fsm.cpp
//	Created 	: 25.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Fuzzy State Machine for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_zombie.h"
#include "ai_zombie_selectors.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\hudmanager.h"
#include "..\\..\\..\\3dsound.h"
#include "..\\..\\..\\xr_trims.h"

void CAI_Zombie::Die()
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

void CAI_Zombie::Jumping()
{
	if (Movement.Environment() == CMovementControl::peInAir)
		WRITE_TO_LOG("Jumping(air)...")
	else
		if (Movement.Environment() == CMovementControl::peOnGround)
			WRITE_TO_LOG("Jumping(ground)...")
		else
			WRITE_TO_LOG("Jumping(unknown)...")

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)

	if (m_bStateChanged) {
		m_dwLastRangeSearch = Level().timeServer();
		m_bActionStarted = true;
		m_bJumping = false;
	}
	else
		if ((Movement.Environment() == CMovementControl::peInAir) && (!m_bJumping))
			m_bJumping = true;
		else
			if ((m_bJumping) && (Movement.Environment() == CMovementControl::peOnGround)) {
				m_bJumping = false;
				eCurrentState = tStateStack.top();
				tStateStack.pop();
			}
}

void CAI_Zombie::FreeHunting()
{
	WRITE_TO_LOG("Free hunting");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)
		
	SelectEnemy(Enemy);
	
	//CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiSoldierAttackFire)
	
	DWORD dwCurTime = Level().timeServer();
	
	//CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiSoldierUnderFire)
	
	//CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwSenseTime < SENSE_JUMP_TIME) && (dwSenseTime),aiSoldierSenseSomething)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
	//CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiSoldierUnderFire)
	
	//CHECK_IF_SWITCH_TO_NEW_STATE(m_tpaPatrolPoints.size(),aiSoldierPatrolRoute)

	vfInitSelector(SelectorFreeHunting,Squad,Leader);

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else {
		if ((AI_Path.TravelPath.empty()) || (AI_Path.TravelStart >= AI_Path.TravelPath.size() - 2) || (!AI_Path.fSpeed)) {
			if (ps_Size() > 1) {
				SelectorFreeHunting.m_tDirection.sub(ps_Element(ps_Size() - 2).vPosition,ps_Element(ps_Size() - 1).vPosition);
				SelectorFreeHunting.m_tDirection.normalize_safe();
			}
			else
				SelectorFreeHunting.m_tDirection.set(1,0,0);
			vfSearchForBetterPositionWTime(SelectorFreeHunting,Squad,Leader);
		}
	}

	//SetLessCoverLook(AI_Node);
	SetDirectionLook();

	vfSetFire(false,Group);

	vfSetMovementType(BODY_STATE_STAND,m_fMinSpeed);
}

void CAI_Zombie::Think()
{
	bStopThinking = false;
	do {
		m_ePreviousState = eCurrentState;
		switch(eCurrentState) {
			case aiZombieDie : {
				Die();
				break;
			}
			case aiZombieJumping : {
				Jumping();
				break;
			}
			case aiZombieFreeHunting : {
				FreeHunting();
				break;
			}
		}
		m_bStateChanged = m_ePreviousState != eCurrentState;
	}
	while (!bStopThinking);
}
