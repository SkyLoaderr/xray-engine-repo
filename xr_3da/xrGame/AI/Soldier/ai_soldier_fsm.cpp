////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier_fsm.cpp
//	Created 	: 25.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : Fuzzy State Machine for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_soldier.h"
#include "ai_soldier_selectors.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\hudmanager.h"
#include "..\\..\\..\\xr_trims.h"

#define MAX_PATROL_DISTANCE				6.f
#define MIN_PATROL_DISTANCE				1.f
#define	AMMO_NEED_RELOAD				6
#define MIN_COVER_MOVE					120
#define MIN_SPINE_TURN_ANGLE			PI_DIV_6
#define EYE_WEAPON_DELTA				(0*PI/30.f)
#define TORSO_START_SPEED				PI_DIV_4
#define DISTANCE_TO_REACT				2.14f
#define RECHARGE_MEDIAN					(2.f/3.f)
#define RECHARGE_EPSILON				(0.f/6.f)

/**
void CAI_Soldier::Test()
{
	WRITE_TO_LOG("Temporary test state");

	CKinematics* tpVisualObject = PKinematics(pVisual);

	if (Level().iGetKeyState(DIK_LSHIFT)) {
		if (Level().iGetKeyState(DIK_Q))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaRunForward[0]);
		else if (Level().iGetKeyState(DIK_W))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaRunForward[1]);
		else if (Level().iGetKeyState(DIK_E))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaRunForward[2]);
		else if (Level().iGetKeyState(DIK_R))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaRunForward[3]);
		else if (Level().iGetKeyState(DIK_T))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[0]);
		else if (Level().iGetKeyState(DIK_Y))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[1]);
		else if (Level().iGetKeyState(DIK_U))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[2]);
		else if (Level().iGetKeyState(DIK_I))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaWalkBack[0]);
		else if (Level().iGetKeyState(DIK_O))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaWalkBack[1]);
		else if (Level().iGetKeyState(DIK_P))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaWalkBack[2]);
		else if (Level().iGetKeyState(DIK_A))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpWalkLeft);
		else if (Level().iGetKeyState(DIK_S))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpWalkRight);
		else if (Level().iGetKeyState(DIK_D))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpTurnLeft);
		else if (Level().iGetKeyState(DIK_F))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpTurnRight);
		else if (Level().iGetKeyState(DIK_G))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpIdle);
		else if (Level().iGetKeyState(DIK_H))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaLieDown[0]);
		else if (Level().iGetKeyState(DIK_J))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpPointSign);
		else if (Level().iGetKeyState(DIK_K))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaDeath[0]);
		else if (Level().iGetKeyState(DIK_L))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaDeath[1]);
		else if (Level().iGetKeyState(DIK_Z))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaDeath[2]);
		else if (Level().iGetKeyState(DIK_X))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaDeath[3]);
		else if (Level().iGetKeyState(DIK_C))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaDeath[4]);
		else if (Level().iGetKeyState(DIK_V))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpAim);
		else if (Level().iGetKeyState(DIK_B))
			m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpAttack);
		else if (Level().iGetKeyState(DIK_N))
			tpVisualObject->PlayFX(tSoldierAnimations.tNormal.tTorso.tpDamageLeft);
		else if (Level().iGetKeyState(DIK_M))
			tpVisualObject->PlayFX(tSoldierAnimations.tNormal.tTorso.tpDamageRight);
	}
	else
		if (Level().iGetKeyState(DIK_RSHIFT)) {
			if (Level().iGetKeyState(DIK_Q))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tTorso.tpGoAheadSign);
			else if (Level().iGetKeyState(DIK_W))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tTorso.tpRaiseHandSign);
			else if (Level().iGetKeyState(DIK_E))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpReload);
			else if (Level().iGetKeyState(DIK_R))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tLegs.tpJumpBegin);
			else if (Level().iGetKeyState(DIK_T))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tLegs.tpJumpIdle);
			else if (Level().iGetKeyState(DIK_Y))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpaWalkForward[0]);
			else if (Level().iGetKeyState(DIK_U))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpaWalkForward[1]);
			else if (Level().iGetKeyState(DIK_O))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpaWalkBack[0]);
			else if (Level().iGetKeyState(DIK_P))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpaWalkBack[1]);
			else if (Level().iGetKeyState(DIK_S))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpDeath);
			else if (Level().iGetKeyState(DIK_D))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpIdle);
			else if (Level().iGetKeyState(DIK_F))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpaLieDown[0]);
			else if (Level().iGetKeyState(DIK_G))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpPointSign);
			else if (Level().iGetKeyState(DIK_H))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpTurnLeft);
			else if (Level().iGetKeyState(DIK_J))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpTurnRight);
			else if (Level().iGetKeyState(DIK_K))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpWalkLeft);
			else if (Level().iGetKeyState(DIK_L))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpWalkRight);
			else if (Level().iGetKeyState(DIK_Z))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpAim);
			else if (Level().iGetKeyState(DIK_X))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpAttack);
			else if (Level().iGetKeyState(DIK_C))
				tpVisualObject->PlayFX(tSoldierAnimations.tCrouch.tTorso.tpDamageLeft);
			else if (Level().iGetKeyState(DIK_V))
				tpVisualObject->PlayFX(tSoldierAnimations.tCrouch.tTorso.tpDamageRight);
			else if (Level().iGetKeyState(DIK_B))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tTorso.tpGoAheadSign);
			else if (Level().iGetKeyState(DIK_N))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tTorso.tpRaiseHandSign);
			else if (Level().iGetKeyState(DIK_M))
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpReload);
		}
		else 
			if (Level().iGetKeyState(DIK_LALT)) {
				if (Level().iGetKeyState(DIK_Q))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tLegs.tpJumpBegin);
				else if (Level().iGetKeyState(DIK_W))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tLegs.tpJumpIdle);
				else if (Level().iGetKeyState(DIK_E))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpAttack);
				else if (Level().iGetKeyState(DIK_R))
					tpVisualObject->PlayFX(tSoldierAnimations.tLie.tGlobal.tpDamage);
				else if (Level().iGetKeyState(DIK_T))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpDeath);
				else if (Level().iGetKeyState(DIK_U))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpIdle);
				else if (Level().iGetKeyState(DIK_P))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpReload);
				else if (Level().iGetKeyState(DIK_A))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpStandUp);
				else if (Level().iGetKeyState(DIK_S))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpTurnLeft);
				else if (Level().iGetKeyState(DIK_D))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpTurnRight);
				else if (Level().iGetKeyState(DIK_F))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpWalkBack);
				else if (Level().iGetKeyState(DIK_G))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpWalkForward);
				else if (Level().iGetKeyState(DIK_H))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpWalkLeft);
				else if (Level().iGetKeyState(DIK_J))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tLie.tGlobal.tpWalkRight);
				else if (Level().iGetKeyState(DIK_K))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[3]);
				else if (Level().iGetKeyState(DIK_L))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[4]);
				else if (Level().iGetKeyState(DIK_Z))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaLieDown[1]);
				else if (Level().iGetKeyState(DIK_X))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tNormal.tGlobal.tpaLieDown[2]);
				else if (Level().iGetKeyState(DIK_C))
					m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tSoldierAnimations.tCrouch.tGlobal.tpaLieDown[1]);
			}
}

/**
void CAI_Soldier::OnAttackFire()
{
	WRITE_TO_LOG("Shooting enemy...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
	
	SelectEnemy(Enemy);
	
	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime) && (m_cBodyState != BODY_STATE_LIE),aiSoldierLyingDown)
	
	if (!(Enemy.Enemy)) {
		vfSetFire(false,getGroup());
		CHECK_IF_GO_TO_PREV_STATE(((tSavedEnemy) && (tSavedEnemy->g_Health() <= 0)) || (!tSavedEnemy))
		dwLostEnemyTime = Level().timeServer();
		GO_TO_NEW_STATE(aiSoldierPursuit);
	}
	
	CHECK_IF_SWITCH_TO_NEW_STATE(!(Enemy.bVisible),aiSoldierFindEnemy)
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),Enemy.Enemy->Position());

	CHECK_IF_GO_TO_NEW_STATE((tDistance.square_magnitude() >= 25.f) && ((Group.m_dwFiring > 1) || ((Group.m_dwFiring == 1) && (!m_bFiring))),aiSoldierAttackRun)
	
	CHECK_IF_SWITCH_TO_NEW_STATE((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() == 0),aiSoldierRecharge)

	AI_Path.TravelPath.clear();
	
	vfSaveEnemy();

	if (!m_bFiring)
		vfAimAtEnemy();
	
	vfSetFire(true,Group);
	
	//vfSetMovementType(m_cBodyState,0);
	if (m_cBodyState != BODY_STATE_STAND)
		vfSetMovementType(m_cBodyState,0);
	else
		vfSetMovementType(BODY_STATE_CROUCH,0);
}

void CAI_Soldier::OnAttackRun()
{
	WRITE_TO_LOG("Attack enemy");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
	
	SelectEnemy(Enemy);
	
	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiSoldierLyingDown)
	
	if (!(Enemy.Enemy)) {
		vfSetFire(false,getGroup());
		CHECK_IF_GO_TO_PREV_STATE(((tSavedEnemy) && (tSavedEnemy->g_Health() <= 0)) || (!tSavedEnemy))
		dwLostEnemyTime = Level().timeServer();
		GO_TO_NEW_STATE(aiSoldierPursuit);
	}
	
	CHECK_IF_SWITCH_TO_NEW_STATE(!(Enemy.bVisible),aiSoldierFindEnemy)
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),Enemy.Enemy->Position());
	
	CHECK_IF_GO_TO_NEW_STATE((tDistance.square_magnitude() < 25.f) || ((!(Group.m_dwFiring)) && (Weapons->ActiveWeapon() && (Weapons->ActiveWeapon()->GetAmmoElapsed() > 0))),aiSoldierAttackFire)

	INIT_SQUAD_AND_LEADER;
	
	vfInitSelector(SelectorAttack,Squad,Leader);
	
	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(&SelectorAttack);
	else
		vfSearchForBetterPosition(SelectorAttack,Squad,Leader);
	
	vfAimAtEnemy();
	
	vfSetFire(false,Group);

	if ((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() <= Weapons->ActiveWeapon()->GetAmmoMagSize()*0.2f))
		Weapons->ActiveWeapon()->Reload();
	
	vfSetMovementType(m_cBodyState,m_fMaxSpeed);
}

void CAI_Soldier::OnDefend()
{
	WRITE_TO_LOG("Defend from enemy");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
}

void CAI_Soldier::OnFindEnemy()
{
	WRITE_TO_LOG("Looking for enemy");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
	
	SelectEnemy(Enemy);
	
	if (!(Enemy.Enemy)) {
		vfSetFire(false,getGroup());
		CHECK_IF_GO_TO_PREV_STATE(((tSavedEnemy) && (tSavedEnemy->g_Health() <= 0)) || (!tSavedEnemy))
		dwLostEnemyTime = Level().timeServer();
		GO_TO_NEW_STATE(aiSoldierPursuit);
	}
	
	CHECK_IF_GO_TO_PREV_STATE(Enemy.bVisible)
	CHECK_IF_SWITCH_TO_NEW_STATE((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() < AMMO_NEED_RELOAD),aiSoldierRecharge)
		
	INIT_SQUAD_AND_LEADER;
	
	vfInitSelector(SelectorFindEnemy,Squad,Leader);
	
	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		vfSearchForBetterPosition(SelectorFindEnemy,Squad,Leader);
	
	vfAimAtEnemy();
	
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	vfSetFire(false,Group);
	
	vfSetMovementType(BODY_STATE_STAND,m_fMaxSpeed);
}

void CAI_Soldier::OnFollowLeader()
{
	WRITE_TO_LOG("Following leader");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
		
	SelectEnemy(Enemy);
	
	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiSoldierAttackFire)
	
	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiSoldierUnderFire)
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiSoldierUnderFire)

	if (Leader == this) {
		CHECK_IF_SWITCH_TO_NEW_STATE(m_tpaPatrolPoints.size(),aiSoldierPatrolRoute)
		GO_TO_NEW_STATE(aiSoldierFreeHunting)
	}
	else
		CHECK_IF_SWITCH_TO_NEW_STATE(m_tpaPatrolPoints.size(),aiSoldierFollowLeaderPatrol)

	vfInitSelector(SelectorFollowLeader,Squad,Leader);

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		vfSearchForBetterPosition(SelectorFollowLeader,Squad,Leader);
		
	SetLessCoverLook(AI_Node);

	vfSetFire(false,Group);

	vfSetMovementType(BODY_STATE_STAND,m_fMinSpeed);
}

void CAI_Soldier::OnFreeHunting()
{
	WRITE_TO_LOG("Free hunting");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
		
	SelectEnemy(Enemy);
	
	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiSoldierAttackFire)
	
	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiSoldierUnderFire)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiSoldierUnderFire)
	
	CHECK_IF_SWITCH_TO_NEW_STATE(m_tpaPatrolPoints.size(),aiSoldierPatrolRoute)

	vfInitSelector(SelectorFreeHunting,Squad,Leader);

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		vfSearchForBetterPosition(SelectorFreeHunting,Squad,Leader);

	SetLessCoverLook(AI_Node);

	vfSetFire(false,Group);

	vfSetMovementType(BODY_STATE_STAND,m_fMinSpeed);
}

void CAI_Soldier::OnInjuring()
{
	WRITE_TO_LOG("Feeling pain...");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

}

void CAI_Soldier::OnJumping()
{
	if (Movement.Environment() == CMovementControl::peInAir)
		WRITE_TO_LOG("Jumping(air)...")
	else
		if (Movement.Environment() == CMovementControl::peOnGround)
			WRITE_TO_LOG("Jumping(ground)...")
		else
			WRITE_TO_LOG("Jumping(unknown)...")

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

	if (m_bStateChanged) {
		m_dwLastRangeSearch = Level().timeServer();
		m_bActionStarted = true;
		m_bJumping = false;
	}
	else {
		switch (m_cBodyState) {
			case BODY_STATE_STAND : ;
			case BODY_STATE_CROUCH : {
				if ((Movement.Environment() == CMovementControl::peInAir) && (!m_bJumping))
					m_bJumping = true;
				else
					if ((m_bJumping) && (Movement.Environment() == CMovementControl::peOnGround)) {
						m_bJumping = false;
						eCurrentState = tStateStack.top();
						tStateStack.pop();
					}
				break;
			}
			case BODY_STATE_LIE : 
				GO_TO_PREV_STATE
		}
	}
}

void CAI_Soldier::OnSitting()
{
	WRITE_TO_LOG("Sitting...");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

	vfSetMovementType(BODY_STATE_CROUCH,0);
	
	AI_Path.TravelPath.clear();
	
	CHECK_IF_GO_TO_PREV_STATE(true)
	//if ((m_tpCurrentGlobalAnimation == tSoldierAnimations.tLie.tGlobal.tpLieDown) && (Level().timeServer() - dwHitTime > 500)) {
	//}
}

void CAI_Soldier::OnMoreDeadThanAlive()
{
	WRITE_TO_LOG("More dead than alive");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

}

void CAI_Soldier::OnNoWeapon()
{
	WRITE_TO_LOG("Searching for weapon");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
		
	SelectEnemy(Enemy);
	
	DWORD dwCurTime = Level().timeServer();
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	vfInitSelector(SelectorNoWeapon,Squad,Leader);

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else {
		m_dwLastRangeSearch = 0;
		vfSearchForBetterPosition(SelectorNoWeapon,Squad,Leader);
	}

	if (Enemy.Enemy)
		vfAimAtEnemy();
	else
		SetLessCoverLook(AI_Node);

	vfSetFire(false,Group);

	vfSetMovementType(BODY_STATE_STAND,m_fMaxSpeed);
}

void CAI_Soldier::OnPatrolUnderFire()
{
	WRITE_TO_LOG("Patrol under fire");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

	SelectEnemy(Enemy);

	CHECK_IF_GO_TO_NEW_STATE(Enemy.Enemy,aiSoldierAttackFire)
	
	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_GO_TO_PREV_STATE((dwCurTime - dwHitTime > HIT_REACTION_TIME) && (dwHitTime))
	
	CHECK_IF_SWITCH_TO_NEW_STATE(dwCurTime - dwHitTime < m_dwPatrolShock,aiSoldierPatrolHurt)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
	CHECK_IF_GO_TO_PREV_STATE((dwCurTime - Group.m_dwLastHitTime > HIT_REACTION_TIME) && (Group.m_dwLastHitTime))
	
	//CHECK_IF_SWITCH_TO_NEW_STATE(m_cBodyState != BODY_STATE_LIE,aiSoldierLyingDown)
	
	vfInitSelector(SelectorUnderFire,Squad,Leader);

	Fvector tTemp = tHitDir;
	tTemp.mul(40.f);
	
	SelectorUnderFire.m_tEnemyPosition.sub(Group.m_tHitPosition,tTemp);

	if (dwCurTime - Group.m_dwLastHitTime >= m_dwUnderFireShock) {
		if (AI_Path.bNeedRebuild)
			vfBuildPathToDestinationPoint(0);
		else
			if (m_dwLastSuccessfullSearch <= Group.m_dwLastHitTime)
				vfSearchForBetterPosition(SelectorUnderFire,Squad,Leader);
			
		SetLessCoverLook(AI_Node);
		if (AI_Path.fSpeed)
			if (_min(AI_Node->cover[0],_min(AI_Node->cover[1],_min(AI_Node->cover[2],AI_Node->cover[3]))) > MIN_COVER_MOVE)
				vfSetMovementType(BODY_STATE_STAND,m_fMaxSpeed);
			else
				vfSetMovementType(BODY_STATE_CROUCH,m_fMinSpeed);
		else
			if (_min(AI_Node->cover[0],_min(AI_Node->cover[1],_min(AI_Node->cover[2],AI_Node->cover[3]))) > MIN_COVER_MOVE)
				vfSetMovementType(BODY_STATE_STAND,m_fMaxSpeed);
			else
				vfSetMovementType(BODY_STATE_CROUCH,m_fMinSpeed);
	}
	else {
		SetLessCoverLook(AI_Node);

		AI_Path.TravelPath.clear();

		if (_min(AI_Node->cover[0],_min(AI_Node->cover[1],_min(AI_Node->cover[2],AI_Node->cover[3]))) > MIN_COVER_MOVE)
			vfSetMovementType(BODY_STATE_STAND,m_fMaxSpeed);
		else
			vfSetMovementType(BODY_STATE_CROUCH,m_fMinSpeed);
	}
	
	vfSetFire(false,Group);
}

void CAI_Soldier::OnPatrolHurtAggressiveUnderFire()
{
	// if no more health then soldier is dead
	WRITE_TO_LOG("Patrol hurt aggressive under fire");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

	SelectEnemy(Enemy);

	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_GO_TO_NEW_STATE((Enemy.Enemy) && (dwCurTime - dwHitTime >= m_dwPatrolShock + m_dwUnderFireShock),aiSoldierAttackFire)
	
	CHECK_IF_GO_TO_NEW_STATE(dwCurTime - dwHitTime >= m_dwPatrolShock + m_dwUnderFireShock,aiSoldierPatrolUnderFire)
	
	CHECK_IF_GO_TO_NEW_STATE(dwCurTime - dwHitTime < m_dwPatrolShock,aiSoldierPatrolHurt)

	CHECK_IF_SWITCH_TO_NEW_STATE(m_cBodyState != BODY_STATE_LIE,aiSoldierLyingDown)

	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
	tWatchDirection.sub(Group.m_tHitPosition,vPosition);
	LOOK_AT_DIRECTION(tWatchDirection)
	SUB_ANGLE(r_torso_target.yaw,EYE_WEAPON_DELTA)
	//r_torso_target.pitch = r_target.pitch = 0;

	vfSetMovementType(BODY_STATE_LIE,0);
	
	if ((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() <= 0)) {
		vfSetFire(Enemy.Enemy ? true : false,Group);
		Weapons->ActiveWeapon()->Reload();
	}
	else
		vfSetFire(Enemy.Enemy ? fabsf(r_torso_current.yaw - r_torso_target.yaw) < PI/30.f : false,Group);
}

void CAI_Soldier::OnPatrolHurtNonAggressiveUnderFire()
{
	WRITE_TO_LOG("Patrol hurt non-aggressive under fire");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

	SelectEnemy(Enemy);

	CHECK_IF_GO_TO_NEW_STATE(Enemy.Enemy,aiSoldierAttackFire)

	DWORD dwCurTime = Level().timeServer();

	CHECK_IF_GO_TO_PREV_STATE(dwCurTime - dwHitTime > HIT_REACTION_TIME)
	
	CHECK_IF_SWITCH_TO_NEW_STATE(m_cBodyState != BODY_STATE_LIE,aiSoldierLyingDown)
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	CHECK_IF_GO_TO_PREV_STATE((dwCurTime - Group.m_dwLastHitTime > HIT_REACTION_TIME) && (Group.m_dwLastHitTime))

	vfInitSelector(SelectorUnderFire,Squad,Leader);

	Fvector tTemp = tHitDir;
	tTemp.mul(40.f);
	SelectorUnderFire.m_tEnemyPosition.sub(Position(),tTemp);

	if (dwCurTime - dwHitTime >= m_dwUnderFireShock) {
		if (AI_Path.bNeedRebuild)
			vfBuildPathToDestinationPoint(0);
		else
			if (m_dwLastSuccessfullSearch <= Group.m_dwLastHitTime)
				vfSearchForBetterPosition(SelectorUnderFire,Squad,Leader);
			
		SetLessCoverLook(AI_Node);
		vfSetMovementType(BODY_STATE_CROUCH,m_fMaxSpeed);
	}
	else {
		SetLessCoverLook(AI_Node);
		AI_Path.TravelPath.clear();
		vfSetMovementType(BODY_STATE_CROUCH,0);
	}
	
	vfSetFire(false,Group);
}

void CAI_Soldier::OnPursuit()
{
	WRITE_TO_LOG("Pursuiting");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

	SelectEnemy(Enemy);
	
	CHECK_IF_GO_TO_NEW_STATE(Enemy.Enemy,aiSoldierAttackFire)

	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_GO_TO_PREV_STATE(dwCurTime - dwLostEnemyTime > LOST_ENEMY_REACTION_TIME)
				
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiSoldierPatrolHurt)
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiSoldierUnderFire)

	CHECK_IF_GO_TO_PREV_STATE(!tSavedEnemy)

	vfInitSelector(SelectorPursuit,Squad,Leader);

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		vfSearchForBetterPosition(SelectorPursuit,Squad,Leader);

	tWatchDirection.sub(tSavedEnemyPosition,Position());
	
	if (tWatchDirection.magnitude() > 0.0001f)
		SetSmartLook(AI_Node,tWatchDirection);
	else
		SetLessCoverLook(AI_Node);

	vfSetFire(false,Group);

	if (_min(AI_Node->cover[0],_min(AI_Node->cover[1],_min(AI_Node->cover[2],AI_Node->cover[3]))) > MIN_COVER_MOVE)
		vfSetMovementType(BODY_STATE_STAND,m_fMaxSpeed);
	else
		vfSetMovementType(BODY_STATE_CROUCH,m_fMinSpeed);
}

void CAI_Soldier::OnRetreat()
{
	WRITE_TO_LOG("Retreating...");
}

void CAI_Soldier::OnUnderFire()
{
	WRITE_TO_LOG("Under fire...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

	SelectEnemy(Enemy);

	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiSoldierAttackFire)

	DWORD dwCurTime = Level().timeServer();

	CHECK_IF_GO_TO_PREV_STATE(dwCurTime - dwHitTime > HIT_REACTION_TIME)
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	Group.m_dwLastHitTime = dwHitTime;
	Group.m_tLastHitDirection = tHitDir;
	Group.m_tHitPosition = tHitPosition;
	
	CHECK_IF_GO_TO_PREV_STATE((dwCurTime - Group.m_dwLastHitTime > HIT_REACTION_TIME) && (Group.m_dwLastHitTime))
	
	vfInitSelector(SelectorUnderFire,Squad,Leader);
	
	SelectorUnderFire.m_tEnemyPosition = tHitPosition;
	
	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		if (m_dwLastSuccessfullSearch <= Group.m_dwLastHitTime)
			vfSearchForBetterPosition(SelectorUnderFire,Squad,Leader);
		
	mk_rotation(tHitDir,r_torso_target);
	r_target.yaw = r_torso_target.yaw;
	r_target.pitch = 0;
	r_torso_target.pitch = 0;
	ASSIGN_SPINE_BONE;
	//r_target.yaw += PI_DIV_6;
	r_torso_target.yaw = r_torso_target.yaw - EYE_WEAPON_DELTA;
	
	//q_look.o_look_speed=8*_FB_look_speed;
	
	vfSetFire(dwCurTime - dwHitTime < 1000,Group);
	
	vfSetMovementType(BODY_STATE_STAND,m_fMaxSpeed);
}
/**/

void CAI_Soldier::Die()
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

void CAI_Soldier::OnTurnOver()
{
	WRITE_TO_LOG("Turning over...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
		
	SelectEnemy(Enemy);
	
	//CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiSoldierAttackFire)
	
	DWORD dwCurTime = Level().timeServer();
	
	//CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiSoldierUnderFire)
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	//CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiSoldierUnderFire)

	if ((fabsf(r_torso_target.yaw - r_torso_current.yaw) < PI_DIV_6) || ((fabsf(fabsf(r_torso_target.yaw - r_torso_current.yaw) - PI_MUL_2) < PI_DIV_6))) {
		m_ePreviousState = tStateStack.top();
		GO_TO_PREV_STATE
	}

	vfSetFire(m_bFiring = false,Group);

	if (!AI_Path.TravelPath.empty())
		AI_Path.TravelStart = AI_Path.TravelPath.size() - 1;
	else
		AI_Path.TravelStart = 0;
	
	vfSetMovementType(WALK_NO);

	r_torso_speed = PI_DIV_2/1;
	r_spine_speed = PI_DIV_2/1;
	q_look.o_look_speed = PI_DIV_2/1;
}

void CAI_Soldier::OnRecharge()
{
	
	WRITE_TO_LOG("Recharging...");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

	CHECK_IF_GO_TO_PREV_STATE((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() == Weapons->ActiveWeapon()->GetAmmoMagSize()))
	
	CHECK_IF_SWITCH_TO_NEW_STATE((Weapons->ActiveWeapon()) && (!(Weapons->ActiveWeapon()->GetAmmoCurrent())),aiSoldierNoWeapon)
	//CHECK_IF_GO_TO_PREV_STATE((Weapons->ActiveWeapon()) && (!(Weapons->ActiveWeapon()->GetAmmoCurrent())))
	
	SelectEnemy(Enemy);

	//CHECK_IF_GO_TO_PREV_STATE(!Enemy.Enemy)
	
	DWORD dwCurTime = Level().timeServer();
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	/**
	vfInitSelector(SelectorReload,Squad,Leader);

	if (Enemy.Enemy) {
		if (AI_Path.bNeedRebuild)
			vfBuildPathToDestinationPoint(0);
		else
			vfSearchForBetterPosition(SelectorReload,Squad,Leader);
	}
	/**/
	
	//SetDirectionLook();
	
	vfSetFire(false,Group);
	
	if (Weapons->ActiveWeapon())
		Weapons->ActiveWeapon()->Reload();

	//StandUp();
	//vfSetMovementType(BODY_STATE_STAND,m_fMinSpeed);
	if (m_cBodyState != BODY_STATE_STAND)
        vfSetMovementType(m_cMovementType);
	else {
		Squat();
		vfSetMovementType(m_cMovementType);
	}
}

void CAI_Soldier::OnNoWeapon()
{
	WRITE_TO_LOG("Searching for weapon");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
		
	SelectEnemy(Enemy);
	
	DWORD dwCurTime = Level().timeServer();
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	vfInitSelector(SelectorNoWeapon,Squad,Leader);

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else {
		m_dwLastRangeSearch = 0;
		vfSearchForBetterPosition(SelectorNoWeapon,Squad,Leader);
	}

	if (Enemy.Enemy)
		vfAimAtEnemy();
	else
		SetLessCoverLook(AI_Node);

	vfSetFire(false,Group);

	StandUp();
	vfSetMovementType(RUN_FORWARD_3);
}

void CAI_Soldier::OnLookingOver()
{
	WRITE_TO_LOG("Looking over...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
	
	SelectEnemy(Enemy);

	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiSoldierAttackFireAlone)
	
	DWORD dwCurTime = Level().timeServer();

	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiSoldierPatrolHurt)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
	//CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiSoldierPatrolUnderFire)
	
	if (m_tpaPatrolPoints.size() > 1)
		CHECK_IF_SWITCH_TO_NEW_STATE(this == Leader,aiSoldierPatrolRoute)
	else
		SWITCH_TO_NEW_STATE(aiSoldierFollowLeaderPatrol);

	SelectSound(m_iSoundIndex);
	if (m_iSoundIndex >= 0) {
		AI_Path.TravelPath.clear();
		SWITCH_TO_NEW_STATE(aiSoldierSenseSomethingAlone);
	}

	vfSetLookAndFireMovement(false, WALK_NO,1.0f,Group,dwCurTime);

	r_torso_target.pitch = 0;
	r_torso_speed = PI_DIV_4;
}

void CAI_Soldier::OnWaitingForAnimation()
{
	WRITE_TO_LOG("Waiting for animation...");

	vfSetMovementType(WALK_NO);

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

	if (m_bStateChanged)
		m_bActionStarted = true;
		
	CHECK_IF_GO_TO_PREV_STATE(!m_bActionStarted);
}

void CAI_Soldier::OnWaitingForTime()
{
	WRITE_TO_LOG("Waiting for time...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

	CHECK_IF_SWITCH_TO_NEW_STATE((Level().timeServer() - dwHitTime < HIT_JUMP_TIME) && (dwHitTime) && (dwHitTime > m_dwLastRangeSearch),aiSoldierPatrolHurt)

	if (m_bStateChanged)
		m_dwLastRangeSearch = Level().timeServer();
		
	CHECK_IF_GO_TO_PREV_STATE(Level().timeServer() - m_dwLastRangeSearch > m_dwTimeBeingWaited);
}

void CAI_Soldier::OnPatrolReturn()
{
	WRITE_TO_LOG("Patrol return to route...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
	
	SelectEnemy(Enemy);
	
	if (Enemy.Enemy) {
		CHECK_IF_SWITCH_TO_NEW_STATE(!bfCheckForEntityVisibility(Enemy.Enemy) && (vPosition.distance_to(Enemy.Enemy->Position()) > 5.f),aiSoldierSteal);
		SWITCH_TO_NEW_STATE(aiSoldierDefendFireAlone);
	}

	DWORD dwCurTime = Level().timeServer();

//	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiSoldierPatrolHurt)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
//	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiSoldierPatrolUnderFire)
	
	if (AI_Path.bNeedRebuild) {
		Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path,0,0);
		if (AI_Path.Nodes.size() > 1) {
			AI_Path.BuildTravelLine(Position());
			CTravelNode tTemp;
			tTemp.floating = FALSE;
			tTemp.P = AI_Path.TravelPath[AI_Path.TravelPath.size() - 1].P;
		}
		else {
			AI_Path.TravelPath.clear();
			AI_Path.bNeedRebuild = FALSE;
		}
	}
	else {
		vfInitSelector(SelectorPatrol,Squad,Leader);
		//SelectorPatrol.m_tEnemyPosition = m_tpaPatrolPoints[0];
		float fDistance;
		if (Leader == this) {
			fDistance = m_tpaPatrolPoints[0].distance_to(Position());
			SelectorPatrol.m_tEnemyPosition = m_tpaPatrolPoints[0];
//			for (int i=1; i<m_tpaPatrolPoints.size(); i++)
//				if ((fTemp = m_tpaPatrolPoints[i].distance_to(Position())) < fDistance) {
//					fDistance = fTemp;
//					SelectorPatrol.m_tEnemyPosition = m_tpaPatrolPoints[i];
//				}
		}
		else {
			fDistance = vPosition.distance_to(Leader->Position());
			SelectorPatrol.m_tEnemyPosition = Leader->Position();
		}
		
		vfSearchForBetterPosition(SelectorPatrol,Squad,Leader);
		
		if ((fDistance < 1.f) || (AI_NodeID == AI_Path.DestNode)) {
			if (this == Leader) {
				float fDistance = 0.f;
				for (int i=0; i<SelectorPatrol.taMemberPositions.size(); i++) {
					Fvector tTemp;
					tTemp.sub(vPosition,SelectorPatrol.taMemberPositions[i]);
				}
				if (fDistance < 5.f) {
					m_ePreviousState = tStateStack.top();
					GO_TO_PREV_STATE
				}
			}
			else {
				m_ePreviousState = tStateStack.top();
				GO_TO_PREV_STATE
			}
		}
		else {
			AI_Path.bNeedRebuild = TRUE;
		}
	}

	StandUp();
	vfSetLookAndFireMovement(false, Leader != this ? WALK_FORWARD_3 : WALK_FORWARD_3,1.0f,Group,dwCurTime);
}

void CAI_Soldier::OnFollowLeaderPatrol()
{
	WRITE_TO_LOG("Following leader patrol");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
	
	SelectEnemy(Enemy);
	
	if (Enemy.Enemy) {
		CHECK_IF_SWITCH_TO_NEW_STATE(!bfCheckForEntityVisibility(Enemy.Enemy) && (vPosition.distance_to(Enemy.Enemy->Position()) > 5.f),aiSoldierSteal);
		SWITCH_TO_NEW_STATE(aiSoldierDefendFireAlone);
	}

	DWORD dwCurTime = Level().timeServer();

//	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiSoldierPatrolHurt)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
//	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiSoldierPatrolUnderFire)
	
	CHECK_IF_GO_TO_NEW_STATE(Leader == this,aiSoldierPatrolRoute)

	CHECK_IF_SWITCH_TO_NEW_STATE(m_bStateChanged,aiSoldierPatrolReturnToRoute)
	
	if ((!(AI_Path.fSpeed)) || (AI_Path.TravelPath.empty()) || (AI_Path.TravelPath[AI_Path.TravelStart].P.distance_to(AI_Path.TravelPath[AI_Path.TravelPath.size() - 1].P) <= .5f)) {
		CAI_Soldier *SoldierLeader = dynamic_cast<CAI_Soldier *>(Leader);
//		if (!m_bLooped) {
//			Fvector tTemp1 = m_tpaPatrolPoints[m_tpaPatrolPoints.size() - 2];
//			tTemp1.sub(m_tpaPatrolPoints[m_tpaPatrolPoints.size() - 1]);
//			vfNormalizeSafe(tTemp1);
//			SRotation sRot;
//			mk_rotation(tTemp1,sRot);
//			if (!((fabsf(r_torso_target.yaw - sRot.yaw) < PI_DIV_6) || ((fabsf(fabsf(r_torso_target.yaw - sRot.yaw) - PI_MUL_2) < PI_DIV_6)))) {
//				AI_Path.TravelStart = AI_Path.TravelPath.size() - 1;
//				r_torso_target.yaw = sRot.yaw > PI ? sRot.yaw - 2*PI : sRot.yaw;
//				r_spine_target.yaw = r_torso_target.yaw; 
//				r_target.yaw = 0;//r_torso_target.yaw; 
//				SWITCH_TO_NEW_STATE(aiSoldierTurnOver);
//			}
//		}
		
		//if ((dwCurTime - SoldierLeader->m_dwLastRangeSearch < 10000) && (SoldierLeader->AI_Path.fSpeed > EPS_L)) {
		if (SoldierLeader->AI_Path.fSpeed > EPS_L) {
			m_dwLoopCount = SoldierLeader->m_dwLoopCount;
			
			AI_Path.TravelPath.clear();
			AI_Path.TravelPath.resize(SoldierLeader->AI_Path.TravelPath.size());
			
			vector<DWORD> &dwaNodes = SoldierLeader->m_dwaNodes;
			
			CAI_Space &AI = Level().AI;
			float fHalfSubnodeSize = AI.GetHeader().size*.5f;
					
			for (int i=0, j=0, k=0; i<SoldierLeader->AI_Path.TravelPath.size(); i++, j++) {
				
				AI_Path.TravelPath[j].floating = FALSE;
				AI_Path.TravelPath[j].P = SoldierLeader->AI_Path.TravelPath[i].P;

				Fvector tTemp;
				
				if (m_bLooped)
					tTemp.sub(SoldierLeader->AI_Path.TravelPath[i < SoldierLeader->AI_Path.TravelPath.size() - 1 ? i + 1 : 0].P, SoldierLeader->AI_Path.TravelPath[i].P);
				else
					if (i < SoldierLeader->AI_Path.TravelPath.size() - 1)
						tTemp.sub(SoldierLeader->AI_Path.TravelPath[i < SoldierLeader->AI_Path.TravelPath.size() - 1 ? i + 1 : 0].P, SoldierLeader->AI_Path.TravelPath[i].P);
					else
						tTemp.sub(SoldierLeader->AI_Path.TravelPath[i].P, SoldierLeader->AI_Path.TravelPath[i - 1].P);

				if (tTemp.magnitude() < .1f) {
					j--;
					continue;
				}

				tTemp.y = 0.f;
				tTemp.normalize();
				
				if (m_bLooped)
					if (Group.Members[0] == this) 
						tTemp.set(tTemp.z,0,-tTemp.x);
					else
						tTemp.set(-tTemp.z,0,tTemp.x);
				else
					if (Group.Members[0] == this) 
						if (m_dwLoopCount % 2)
							tTemp.set(tTemp.z,0,-tTemp.x);
						else
							tTemp.set(-tTemp.z,0,tTemp.x);
					else
						if (m_dwLoopCount % 2)
							tTemp.set(-tTemp.z,0,tTemp.x);
						else
							tTemp.set(tTemp.z,0,-tTemp.x);
				
				AI_Path.TravelPath[j].P.add(tTemp);

				if ((j > 1) && (COMPUTE_DISTANCE_2D(AI_Path.TravelPath[j].P,AI_Path.TravelPath[j-2].P) <= fHalfSubnodeSize + COMPUTE_DISTANCE_2D(AI_Path.TravelPath[j-1].P,AI_Path.TravelPath[j-2].P))) {
					Fvector tPrevious = AI_Path.TravelPath[j-2].P;
					Fvector tCurrent = AI_Path.TravelPath[j-1].P;
					Fvector tNext = AI_Path.TravelPath[j].P;
					Fvector tTemp1, tTemp2;
					tTemp1.sub(tCurrent,tPrevious);
					tTemp2.sub(tNext,tCurrent);
					tTemp1.normalize_safe();
					tTemp1.y = tTemp2.y = 0;
					tTemp2.normalize_safe();
					float fAlpha = tTemp1.dotproduct(tTemp2);
					clamp(fAlpha, -.99999f, +.99999f);
					if ((acosf(fAlpha) < PI_DIV_8*.375f) || (acosf(fAlpha) > 2*PI_DIV_8*.375f)) {
					//if (acosf(fAlpha) < PI_DIV_8*.375f) {
						j--;
						continue;
					}
				}

				int m=k;
				for ( ; (k < dwaNodes.size()) && (!bfInsideNode(AI,AI.Node(dwaNodes[k]),SoldierLeader->AI_Path.TravelPath[i].P,fHalfSubnodeSize)); k++) ;

				if (k >= dwaNodes.size()) {
					k = m;
					AI_Path.TravelPath.erase(AI_Path.TravelPath.begin() + j);
					j--;
					continue;
				}
				//R_ASSERT(k < dwaNodes.size());

				DWORD dwBestNode;
				float fBestCost;
				NodePosition tNodePosition;
				AI.PackPosition(tNodePosition,AI_Path.TravelPath[j].P);
				AI.q_Range_Bit_X(dwaNodes[k],SoldierLeader->AI_Path.TravelPath[i].P,4*fHalfSubnodeSize,&tNodePosition,dwBestNode,fBestCost);
				AI_Path.TravelPath[j].P.y = ffGetY(*(AI.Node(dwBestNode)),AI_Path.TravelPath[j].P.x,AI_Path.TravelPath[j].P.z);
			}
			
			AI_Path.TravelPath.resize(j);
			AI_Path.TravelStart = 0;

//			float fTemp, fDistance = AI_Path.TravelPath[AI_Path.TravelStart = 0].P.distance_to(Position());
//			for ( i=1; i<AI_Path.TravelPath.size(); i++)
//				if ((fTemp = AI_Path.TravelPath[i].P.distance_to(Position())) < fDistance) {
//					fDistance = fTemp;
//					AI_Path.TravelStart = i;
//				}

			m_dwLastRangeSearch = Level().timeServer();
		}
		else {
			AI_Path.TravelPath.clear();
			AI_Path.TravelStart = 0;
		}
	}
	
	if ((!m_dwLastRangeSearch) || (Level().timeServer() - m_dwLastRangeSearch >= 5000)) {
		m_dwLastRangeSearch = Level().timeServer();
		m_fMinPatrolDistance = MIN_PATROL_DISTANCE;
		m_fMaxPatrolDistance = MAX_PATROL_DISTANCE - ::Random.randF(0,4);
	}

	Fvector tTemp0;
	tTemp0.sub(Leader->Position(),Position());
	tTemp0.normalize();
	tWatchDirection.normalize();
	StandUp();
	if (acosf(tWatchDirection.dotproduct(tTemp0)) < PI_DIV_2) {
		float fDistance = Leader->Position().distance_to(Position());
		if (fDistance >= m_fMaxPatrolDistance) {
			vfSetLookAndFireMovement(false, WALK_FORWARD_3,1.1f,Group,dwCurTime);
		}
		else
			if (fDistance <= m_fMinPatrolDistance) {
				vfSetLookAndFireMovement(false, WALK_FORWARD_3,0.9f,Group,dwCurTime);
			}
			else {
				vfSetLookAndFireMovement(false, WALK_FORWARD_3,float(((fDistance - (m_fMaxPatrolDistance + m_fMinPatrolDistance)*.5f)/((m_fMaxPatrolDistance - m_fMinPatrolDistance)*.5f)*.1f + m_fMinPatrolDistance)),Group,dwCurTime);
			}
	}
	else {
		vfSetLookAndFireMovement(false, WALK_FORWARD_3,0.9f,Group,dwCurTime);
	}
}

void CAI_Soldier::OnPatrol()
{
	WRITE_TO_LOG("Patrol");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
	
	SelectEnemy(Enemy);

	if (Enemy.Enemy) {
		CHECK_IF_SWITCH_TO_NEW_STATE(!bfCheckForEntityVisibility(Enemy.Enemy) && (vPosition.distance_to(Enemy.Enemy->Position()) > 5.f),aiSoldierSteal);
		SWITCH_TO_NEW_STATE(aiSoldierDefendFireAlone);
	}

	DWORD dwCurTime = Level().timeServer();

	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiSoldierPatrolHurt)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
//	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiSoldierPatrolUnderFire)
	
	/**/
	SelectSound(m_iSoundIndex);
	if (m_iSoundIndex >= 0) {
		AI_Path.TravelPath.clear();
		SWITCH_TO_NEW_STATE(aiSoldierSenseSomethingAlone);
	}
	/**/
	
	CHECK_IF_SWITCH_TO_NEW_STATE(m_bStateChanged,aiSoldierPatrolReturnToRoute)
	
	if ((!(AI_Path.fSpeed)) || (AI_Path.TravelPath.empty()) || (AI_Path.TravelPath[AI_Path.TravelStart].P.distance_to(AI_Path.TravelPath[AI_Path.TravelPath.size() - 1].P) <= .5f)) {
		
		AI_Path.TravelPath.clear();
		AI_Path.TravelStart = 0;

		m_dwLastRangeSearch = 0;
		
		if (m_bStateChanged)
			m_dwLoopCount = 0;
		
		for (int i=0; i<Group.Members.size(); i++)
			if (((CCustomMonster*)(Group.Members[i]))->AI_Path.fSpeed > EPS_L)
				return;

//		if (!m_bLooped) {
//			Fvector tTemp1 = m_tpaPatrolPoints[m_tpaPatrolPoints.size() - 2];
//			tTemp1.sub(m_tpaPatrolPoints[m_tpaPatrolPoints.size() - 1]);
//			vfNormalizeSafe(tTemp1);
//			SRotation sRot;
//			mk_rotation(tTemp1,sRot);
//			if (!((fabsf(r_torso_target.yaw - sRot.yaw) < PI_DIV_6) || ((fabsf(fabsf(r_torso_target.yaw - sRot.yaw) - PI_MUL_2) < PI_DIV_6)))) {
//				AI_Path.TravelStart = AI_Path.TravelPath.size() - 1;
//				r_torso_target.yaw = sRot.yaw > PI ? sRot.yaw - PI_MUL_2 : sRot.yaw;
//				r_spine_target.yaw = r_torso_target.yaw; 
//				r_target.yaw = 0;//r_torso_target.yaw; 
//				SWITCH_TO_NEW_STATE(aiSoldierTurnOver);
//			}
//		}

		m_dwLastRangeSearch = dwCurTime;
		
		if ((m_bLooped) || (m_dwLoopCount % 2 == 0)) {
			vector<CLevel::SPatrolPath> &tpaPatrolPaths = Level().tpaPatrolPaths;
			m_dwStartPatrolNode = tpaPatrolPaths[m_dwPatrolPathIndex].dwStartNode;
			vfCreatePointSequence(tpaPatrolPaths[m_dwPatrolPathIndex],m_tpaPatrolPoints,m_bLooped);
			m_tpaPointDeviations.resize(m_tpaPatrolPoints.size());
		}
		
		CHECK_IF_SWITCH_TO_NEW_STATE(vPosition.distance_to(m_tpaPatrolPoints[0]) > 5.f,aiSoldierPatrolReturnToRoute)
		
		vfCreateFastRealisticPath(m_tpaPatrolPoints, m_dwStartPatrolNode, m_tpaPointDeviations, AI_Path.TravelPath, m_dwaNodes, m_bLooped,false);
		
		m_dwCreatePathAttempts++;
		Msg("paths : %d",m_dwCreatePathAttempts);
		// invert path if needed
		if (AI_Path.TravelPath.size()) {
			if (!m_bLooped) {
				m_dwLoopCount++;
				if (m_dwLoopCount % 2 == 0) {
					DWORD dwCount = AI_Path.TravelPath.size();
					for ( i=0; i<dwCount / 2; i++) {
						Fvector tTemp = AI_Path.TravelPath[i].P;
						AI_Path.TravelPath[i].P = AI_Path.TravelPath[dwCount - i - 1].P;
						AI_Path.TravelPath[dwCount - i - 1].P = tTemp;
					}
				}
			}
			m_dwLastRangeSearch = Level().timeServer();
		}
	}

	if	(!m_tpSoundBeingPlayed || !m_tpSoundBeingPlayed->feedback) {
		if (m_tpSoundBeingPlayed && !m_tpSoundBeingPlayed->feedback) {
			m_tpSoundBeingPlayed = 0;
			m_dwLastRadioTalk = dwCurTime;
		}
		if ((dwCurTime - m_dwLastSoundRefresh > m_fRadioRefreshRate) && ((dwCurTime - m_dwLastRadioTalk > m_fMaxRadioIinterval) || ((dwCurTime - m_dwLastRadioTalk > m_fMinRadioIinterval) && (::Random.randF(0,1) > (dwCurTime - m_dwLastRadioTalk - m_fMinRadioIinterval)/(m_fMaxRadioIinterval - m_fMinRadioIinterval))))) {
			m_dwLastSoundRefresh = dwCurTime;
			// Play hit-sound
			m_tpSoundBeingPlayed = &(sndRadio[Random.randI(SND_RADIO_COUNT)]);
			
			if (m_tpSoundBeingPlayed->feedback)			
				return;

			pSounds->PlayAtPos(*m_tpSoundBeingPlayed,this,vPosition);
		}
	}
	
	StandUp();
	vfSetLookAndFireMovement(false, WALK_FORWARD_3,1.0f,Group,dwCurTime);
}

void CAI_Soldier::OnAttackFireAlone()
{
	WRITE_TO_LOG("Shooting enemy alone...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
	
	SelectEnemy(Enemy);
	
	DWORD dwCurTime = Level().timeServer();
	
	//CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime) && (m_cBodyState != BODY_STATE_LIE),aiSoldierLyingDown)
	
	CHECK_IF_SWITCH_TO_NEW_STATE(m_bStateChanged && (m_ePreviousState != eCurrentState),aiSoldierPointAtSmth);	
	
	if (!(Enemy.Enemy)) {
		vfSetFire(false,getGroup());
		CHECK_IF_GO_TO_PREV_STATE(((tSavedEnemy) && (tSavedEnemy->g_Health() <= 0)) || (!tSavedEnemy))
		dwLostEnemyTime = Level().timeServer();
		GO_TO_NEW_STATE(aiSoldierPursuitAlone);
		//GO_TO_PREV_STATE;
	}
	
	//CHECK_IF_SWITCH_TO_NEW_STATE(!(Enemy.bVisible),aiSoldierFindEnemy)
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),Enemy.Enemy->Position());

	//CHECK_IF_GO_TO_NEW_STATE((tDistance.square_magnitude() >= 25.f) && ((Group.m_dwFiring > 1) || ((Group.m_dwFiring == 1) && (!m_bFiring))),aiSoldierAttackRun)
	
	CHECK_IF_SWITCH_TO_NEW_STATE((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() == 0),aiSoldierRecharge)

	if (!m_bFiring || m_bStateChanged)
		vfAimAtEnemy(true);
	
	CHECK_IF_SWITCH_TO_NEW_STATE(!((fabsf(r_torso_target.yaw - r_torso_current.yaw) < PI_DIV_6) || ((fabsf(fabsf(r_torso_target.yaw - r_torso_current.yaw) - PI_MUL_2) < PI_DIV_6))),aiSoldierTurnOver)
	
	vfSaveEnemy();

	if ((fabsf(r_torso_target.yaw - r_torso_current.yaw) < PI/30.f) || (PI_MUL_2 - fabsf(r_torso_target.yaw - r_torso_current.yaw) < PI/30.f)) {
		vfSetFire(true,Group);
		CHECK_IF_GO_TO_NEW_STATE((dwCurTime - m_dwNoFireTime > 1000) && !m_bFiring && Weapons->ActiveWeapon() && (float(Weapons->ActiveWeapon()->GetAmmoElapsed()) / float(Weapons->ActiveWeapon()->GetAmmoMagSize()) < RECHARGE_MEDIAN + ::Random.randF(-RECHARGE_EPSILON,+RECHARGE_EPSILON)),aiSoldierRecharge);
		if (dwCurTime - m_dwNoFireTime > 1000) {
			INIT_SQUAD_AND_LEADER;
			vfInitSelector(SelectorAttack,Squad,Leader);
			if (AI_Path.bNeedRebuild)
				vfBuildPathToDestinationPoint(&SelectorAttack);
			else
				vfSearchForBetterPositionWTime(SelectorAttack,Squad,Leader);
			
			if (m_cBodyState != BODY_STATE_STAND)
				vfSetMovementType(WALK_FORWARD_0);
			else {
				Squat();
				vfSetMovementType(WALK_FORWARD_0);
			}
		}
		else {
			AI_Path.TravelPath.clear();
			if (m_cBodyState != BODY_STATE_STAND)
				vfSetMovementType(WALK_NO);
			else {
				Squat();
				vfSetMovementType(WALK_NO);
			}
		}
	}
	else {
		vfSetFire(false,Group);
		AI_Path.TravelPath.clear();
		if (m_cBodyState != BODY_STATE_STAND)
			vfSetMovementType(WALK_NO);
		else {
			Squat();
			vfSetMovementType(WALK_NO);
		}
	}
}

/**/
void CAI_Soldier::OnSteal()
{
	WRITE_TO_LOG("Stealing to enemy...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
	
	SelectEnemy(Enemy);
	
	DWORD dwCurTime = Level().timeServer();
	
	if (!(Enemy.Enemy)) {
		vfSetFire(false,getGroup());
		CHECK_IF_GO_TO_PREV_STATE(((tSavedEnemy) && (tSavedEnemy->g_Health() <= 0)) || (!tSavedEnemy))
		dwLostEnemyTime = Level().timeServer();
		GO_TO_NEW_STATE(aiSoldierPursuitAlone);
		//GO_TO_PREV_STATE;
	}
	
	//CHECK_IF_SWITCH_TO_NEW_STATE(!(Enemy.bVisible),aiSoldierFindEnemy)
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	if (m_bStateChanged)
		m_dwLastRangeSearch = 0;

	Fvector tDistance;
	tDistance.sub(Position(),Enemy.Enemy->Position());

	//CHECK_IF_GO_TO_NEW_STATE((tDistance.square_magnitude() >= 25.f) && ((Group.m_dwFiring > 1) || ((Group.m_dwFiring == 1) && (!m_bFiring))),aiSoldierAttackRun)
	
	CHECK_IF_SWITCH_TO_NEW_STATE((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() == 0),aiSoldierRecharge)

	//CHECK_IF_GO_TO_PREV_STATE(bfCheckForEntityVisibility(Enemy.Enemy) || (vPosition.distance_to(Enemy.Enemy->Position()) <= 5.f));
	CHECK_IF_GO_TO_NEW_STATE(bfCheckForEntityVisibility(Enemy.Enemy) || (vPosition.distance_to(Enemy.Enemy->Position()) <= 5.f),aiSoldierAttackFireAlone);

	vfSaveEnemy();

	INIT_SQUAD_AND_LEADER;
	
	vfInitSelector(SelectorPatrol,Squad,Leader);

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
//		vfSearchForBetterPositionWTime(SelectorPatrol,Squad,Leader);
		vfSearchForBetterPosition(SelectorPatrol,Squad,Leader);

	vfAimAtEnemy();
	
	vfSetFire(false,Group);
	
	Squat();
	vfSetMovementType(WALK_FORWARD_1);
}
/**/

void CAI_Soldier::OnAttackAim()
{
	WRITE_TO_LOG("Aiming...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
		
	SelectEnemy(Enemy);
	
	DWORD dwCurTime = Level().timeServer();
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	if (m_bStateChanged)
		m_dwLastRangeSearch = dwCurTime;
	else
		if (dwCurTime - m_dwLastRangeSearch >= 500)
			GO_TO_PREV_STATE;
	
	vfSetFire(m_bFiring = false,Group);

	vfSetMovementType(WALK_NO);
}

void CAI_Soldier::OnPointAtSmth()
{
	WRITE_TO_LOG("Pointing at something...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
		
	SelectEnemy(Enemy);
	
	DWORD dwCurTime = Level().timeServer();
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	if (m_bStateChanged)
		m_dwLastRangeSearch = dwCurTime;
	else
		if (dwCurTime - m_dwLastRangeSearch >= 1500)
			GO_TO_PREV_STATE;
	
	vfSetFire(m_bFiring = false,Group);

	vfSetMovementType(WALK_NO);
}

void CAI_Soldier::OnSenseSomethingAlone()
{
	WRITE_TO_LOG("Sense something");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
	
	SelectEnemy(Enemy);

	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiSoldierAttackFireAlone)
	
	DWORD dwCurTime = Level().timeServer();

	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
	int iSoundIndex;
	
	SelectSound(iSoundIndex);

	if (iSoundIndex == -1)
		iSoundIndex = m_iSoundIndex;

	if (iSoundIndex >= 0) {
		m_iSoundIndex = iSoundIndex;
		if ((tpaDynamicSounds[iSoundIndex].eSoundType & SOUND_TYPE_WEAPON) == SOUND_TYPE_WEAPON) {
			if ((tpaDynamicSounds[iSoundIndex].eSoundType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING) {
				tHitDir.sub(tpaDynamicSounds[iSoundIndex].tSavedPosition,tpaDynamicSounds[iSoundIndex].tMySavedPosition);
				Fvector tTemp;
				tTemp.sub(tpaDynamicSounds[iSoundIndex].tMySavedPosition,tpaDynamicSounds[iSoundIndex].tSavedPosition);
				SRotation sRot;
				mk_rotation(tTemp,sRot);
				float fAngle,fAngle1;
				if (fabsf(sRot.yaw - tpaDynamicSounds[iSoundIndex].tOrientation.yaw) <= PI + EPS_L)
					fAngle = fabsf(sRot.yaw - tpaDynamicSounds[iSoundIndex].tOrientation.yaw);
				else
					fAngle = PI_MUL_2 - fabsf(sRot.yaw - tpaDynamicSounds[iSoundIndex].tOrientation.yaw);
				
				if (fabsf(sRot.pitch - tpaDynamicSounds[iSoundIndex].tOrientation.pitch) <= PI + EPS_L)
					fAngle1 = fabsf(sRot.pitch - tpaDynamicSounds[iSoundIndex].tOrientation.pitch);
				else
					fAngle1 = PI_MUL_2 - fabsf(sRot.pitch - tpaDynamicSounds[iSoundIndex].tOrientation.pitch);

				float fDistance = sinf(fAngle > fAngle1 ? fAngle : fAngle1)*tpaDynamicSounds[iSoundIndex].tMySavedPosition.distance_to(tpaDynamicSounds[iSoundIndex].tSavedPosition);

				if (fDistance < DISTANCE_TO_REACT) {
					tHitPosition = tpaDynamicSounds[iSoundIndex].tSavedPosition;
					dwHitTime = tpaDynamicSounds[iSoundIndex].dwTime;
					GO_TO_NEW_STATE(aiSoldierPatrolHurt);
				}
				else {
					tHitPosition = tpaDynamicSounds[iSoundIndex].tSavedPosition;
					dwHitTime = tpaDynamicSounds[iSoundIndex].dwTime;
					m_dwLastRangeSearch = dwCurTime;
					GO_TO_NEW_STATE(aiSoldierPatrolDanger);
				}
			}
			else {
				tHitPosition = tpaDynamicSounds[iSoundIndex].tSavedPosition;
				dwHitTime = tpaDynamicSounds[iSoundIndex].dwTime;
				m_dwLastRangeSearch = dwCurTime;
				GO_TO_NEW_STATE(aiSoldierPatrolDanger);
//				m_bStateChanged = false;
//				GO_TO_PREV_STATE;
//				Fvector tCurrentPosition = vPosition;
//				tWatchDirection.sub(tpaDynamicSounds[iSoundIndex].tSavedPosition,tCurrentPosition);
//				if (tWatchDirection.magnitude() > EPS_L) {
//					tWatchDirection.normalize();
//					mk_rotation(tWatchDirection,r_torso_target);
//					r_target.yaw = r_torso_target.yaw;
//					ASSIGN_SPINE_BONE;
//				}
//				
//				CHECK_IF_SWITCH_TO_NEW_STATE(fabsf(r_torso_target.yaw - r_torso_current.yaw) > PI_DIV_6,aiSoldierTurnOver)
//
//				vfInitSelector(SelectorPatrol,Squad,Leader);
//		
//				SelectorPatrol.m_tEnemyPosition = tpaDynamicSounds[iSoundIndex].tSavedPosition;
//
//				if (AI_Path.bNeedRebuild) {
//					vfBuildPathToDestinationPoint(0);
//				}
//				else {
//					//SelectorFindEnemy.m_tLastEnemyPosition = tpaDynamicSounds[iSoundIndex].tSavedPosition;
//					vfSearchForBetterPositionWTime(SelectorPatrol,Squad,Leader);
//				}
			}
		}
		else {
			m_bStateChanged = false;
			GO_TO_NEW_STATE(aiSoldierPatrolDanger);
		}
	}
	else {
		m_bStateChanged = false;
		GO_TO_PREV_STATE;
	}

	Squat();
	vfSetMovementType(WALK_FORWARD_0);
	
	vfAimAtEnemy();
}

void CAI_Soldier::OnPatrolHurt()
{
	WRITE_TO_LOG("Patrol hurt");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

	AI_Path.TravelPath.clear();

	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	DWORD dwCurTime = Level().timeServer();

	vfSetFire(false,Group);

	m_dwLastRangeSearch = dwCurTime;
	
	tWatchDirection.sub(tHitPosition,vPosition);
	float fDistance = tWatchDirection.magnitude();
	mk_rotation(tWatchDirection,r_torso_target);
		
	r_torso_speed = 1*PI_DIV_2;
	if (m_cBodyState != BODY_STATE_LIE) {
		if (m_cBodyState == BODY_STATE_STAND)
			m_tpAnimationBeingWaited = tSoldierAnimations.tNormal.tGlobal.tpaLieDown[0];
		else
			m_tpAnimationBeingWaited = tSoldierAnimations.tCrouch.tGlobal.tpaLieDown[0];
		Lie();
		SWITCH_TO_NEW_STATE(aiSoldierWaitForAnimation);
	}
	r_torso_speed = TORSO_START_SPEED;
	r_torso_target.yaw = r_torso_current.yaw;

	vfSetMovementType(WALK_FORWARD_1);
	
	SelectEnemy(Enemy);

	if (fabsf(r_torso_target.yaw - r_torso_current.yaw) >= PI/30)
		return;
               
	CHECK_IF_GO_TO_NEW_STATE(Enemy.Enemy,aiSoldierDefendFireAlone)

	//dwHitTime = 0;
	GO_TO_NEW_STATE(aiSoldierHurtAloneDefend);
}

void CAI_Soldier::OnDefendFireAlone()
{
	WRITE_TO_LOG("Defend fire alone");

	GO_TO_NEW_STATE(aiSoldierAttackFireAlone);
}

void CAI_Soldier::OnHurtAloneDefend()
{
	WRITE_TO_LOG("Hurt alone defend");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

	SelectEnemy(Enemy);
	
	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiSoldierDefendFireAlone)

	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime) && (dwHitTime > m_dwLastRangeSearch),aiSoldierPatrolHurt)

	INIT_SQUAD_AND_LEADER
	
	m_dwLastRangeSearch = dwCurTime;
	
	CGroup &Group = Squad.Groups[g_Group()];

	tSavedEnemyPosition = tHitPosition;
	
	vfSetFire(false,Group);

	if (m_bStateChanged)
		m_dwLastRangeSearch = 0;

	CHECK_IF_GO_TO_NEW_STATE(!m_bFiring && Weapons->ActiveWeapon() && (float(Weapons->ActiveWeapon()->GetAmmoElapsed()) / float(Weapons->ActiveWeapon()->GetAmmoMagSize()) < RECHARGE_MEDIAN + ::Random.randF(-RECHARGE_EPSILON,+RECHARGE_EPSILON)),aiSoldierRecharge);

	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
			vfInitSelector(SelectorUnderFireCover,Squad,Leader);

			SelectorUnderFireCover.m_tEnemyPosition = tHitPosition;

			if (AI_Path.bNeedRebuild)
				vfBuildPathToDestinationPoint(0);
			else
//				vfSearchForBetterPositionWTime(SelectorUnderFireCover,Squad,Leader);
				vfSearchForBetterPosition(SelectorUnderFireCover,Squad,Leader);
				
			if (dwCurTime - dwHitTime > 25000) {
				if (AI_Path.fSpeed <= EPS_L)
					vfAimAtEnemy();
				else
					SetDirectionLook();
				StandUp();
				vfSetMovementType(RUN_FORWARD_3);
			}
			else {
				vfAimAtEnemy();
				vfSetMovementType(WALK_FORWARD_0);
			}

			if (dwCurTime - dwHitTime > 45000)
				GO_TO_PREV_STATE;

			break;
		}
		case BODY_STATE_CROUCH : {
			vfInitSelector(SelectorUnderFireCover,Squad,Leader);

			SelectorUnderFireCover.m_tEnemyPosition = tHitPosition;

			if (AI_Path.bNeedRebuild)
				vfBuildPathToDestinationPoint(0);
			else
//				vfSearchForBetterPositionWTime(SelectorUnderFireCover,Squad,Leader);
				vfSearchForBetterPosition(SelectorUnderFireCover,Squad,Leader);
				
			vfAimAtEnemy();
			
			if (dwCurTime - dwHitTime > 25000) {
				StandUp();
				vfSetMovementType(WALK_FORWARD_1);
			}
			else {
				Squat();
				vfSetMovementType(WALK_FORWARD_1);
			}
			
			break;
		}
		case BODY_STATE_LIE : {
			vfAimAtEnemy();
			
			if (dwCurTime - dwHitTime > 7500) {
				vfInitSelector(SelectorUnderFireCover,Squad,Leader);

				SelectorUnderFireCover.m_tEnemyPosition = tHitPosition;
				SelectorUnderFireCover.fOptEnemyDistance = vPosition.distance_to(tHitPosition) + 10.f;

				if (AI_Path.bNeedRebuild)
					vfBuildPathToDestinationPoint(0);
				else
//					vfSearchForBetterPositionWTime(SelectorUnderFireCover,Squad,Leader);
					vfSearchForBetterPosition(SelectorUnderFireCover,Squad,Leader);
					
				if ((dwCurTime - dwHitTime > 12000) && (m_cBodyState != BODY_STATE_STAND)) {
					StandUp();
					m_tpAnimationBeingWaited = tSoldierAnimations.tLie.tGlobal.tpStandUp;
					vfSetMovementType(WALK_NO);
					SWITCH_TO_NEW_STATE(aiSoldierWaitForAnimation);
				}
			}
			else {
				vfInitSelector(SelectorUnderFireLine,Squad,Leader);

				SelectorUnderFireLine.m_tEnemyPosition = tHitPosition;
				SelectorUnderFireLine.fOptEnemyDistance = vPosition.distance_to(tHitPosition) + 10.f;

				if (AI_Path.bNeedRebuild)
					vfBuildPathToDestinationPoint(0);
				else
//					vfSearchForBetterPositionWTime(SelectorUnderFireLine,Squad,Leader);
					vfSearchForBetterPosition(SelectorUnderFireLine,Squad,Leader);
			}
			
			vfSetMovementType(WALK_FORWARD_0);
			break;
		}
		default : VERIFY(false);
	}
}

void CAI_Soldier::OnDangerAlone()
{
	WRITE_TO_LOG("Danger alone");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

	SelectEnemy(Enemy);
	
	if (Enemy.Enemy) {
		CHECK_IF_SWITCH_TO_NEW_STATE(!bfCheckForEntityVisibility(Enemy.Enemy) && (vPosition.distance_to(Enemy.Enemy->Position()) > 5.f),aiSoldierSteal);
		SWITCH_TO_NEW_STATE(aiSoldierDefendFireAlone);
	}
	else
		CHECK_IF_SWITCH_TO_NEW_STATE(tSavedEnemy,aiSoldierPursuitAlone);

	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime) && (dwHitTime > m_dwLastRangeSearch),aiSoldierPatrolHurt)

	INIT_SQUAD_AND_LEADER
	
	CGroup &Group = Squad.Groups[g_Group()];

	m_dwLastRangeSearch = dwCurTime;
	
	tSavedEnemyPosition = tHitPosition;
	
	if (m_bStateChanged)
		m_dwLastRangeSearch = 0;

	vfSetFire(false,Group);

	CHECK_IF_GO_TO_NEW_STATE(!m_bFiring && Weapons->ActiveWeapon() && (float(Weapons->ActiveWeapon()->GetAmmoElapsed()) / float(Weapons->ActiveWeapon()->GetAmmoMagSize()) < RECHARGE_MEDIAN + ::Random.randF(-RECHARGE_EPSILON,+RECHARGE_EPSILON)),aiSoldierRecharge);

	vfInitSelector(SelectorPatrol,Squad,Leader);

	SelectorPatrol.m_tEnemyPosition = tHitPosition;

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
//		vfSearchForBetterPositionWTime(SelectorPatrol,Squad,Leader);
		vfSearchForBetterPosition(SelectorPatrol,Squad,Leader);
		
	if (AI_Path.fSpeed > EPS_L)
		SetDirectionLook();
	else
		vfAimAtEnemy();

	Squat();
	vfSetMovementType(WALK_FORWARD_0);
}

void CAI_Soldier::OnPursuitAlone()
{
	WRITE_TO_LOG("Pursuit alone");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

	SelectEnemy(Enemy);
	
	if (Enemy.Enemy) {
		CHECK_IF_SWITCH_TO_NEW_STATE(!bfCheckForEntityVisibility(Enemy.Enemy) && (vPosition.distance_to(Enemy.Enemy->Position()) > 5.f),aiSoldierSteal);
		SWITCH_TO_NEW_STATE(aiSoldierDefendFireAlone);
	}

	CHECK_IF_GO_TO_PREV_STATE(!tSavedEnemy);

	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime) && (dwHitTime > m_dwLastRangeSearch),aiSoldierPatrolHurt)

	INIT_SQUAD_AND_LEADER
	
	CGroup &Group = Squad.Groups[g_Group()];

	m_dwLastRangeSearch = dwCurTime;
	
	if (m_bStateChanged)
		m_dwLastRangeSearch = 0;

	vfSetFire(false,Group);

	CHECK_IF_GO_TO_NEW_STATE(!m_bFiring && Weapons->ActiveWeapon() && (float(Weapons->ActiveWeapon()->GetAmmoElapsed()) / float(Weapons->ActiveWeapon()->GetAmmoMagSize()) < RECHARGE_MEDIAN + ::Random.randF(-RECHARGE_EPSILON,+RECHARGE_EPSILON)),aiSoldierRecharge);

	vfInitSelector(SelectorPatrol,Squad,Leader);

	SelectorPatrol.m_tEnemy = tSavedEnemy;
	SelectorPatrol.m_tEnemyPosition = tSavedEnemyPosition;

	CHECK_IF_GO_TO_PREV_STATE((ps_Size() > 0) && (ps_Element(ps_Size() - 1).dwTime > 5000));

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
//		vfSearchForBetterPositionWTime(SelectorPatrol,Squad,Leader);
		vfSearchForBetterPosition(SelectorPatrol,Squad,Leader);
		
	if (AI_Path.fSpeed > EPS_L)
		SetDirectionLook();
	else
		vfAimAtEnemy();

	Squat();
	vfSetMovementType(WALK_FORWARD_0);
}

void CAI_Soldier::Think()
{
	bStopThinking = false;
	do {
		m_ePreviousState = eCurrentState;
		/**/
		switch(eCurrentState) {
			/**
			case aiSoldierAttackRun : {
				AttackRun();
				break;
			}
			case aiSoldierAttackFire : {
				AttackFire();
				break;
			}
			case aiSoldierDefend : {
				Defend();
				break;
			}
			case aiSoldierFindEnemy : {
				FindEnemy();
				break;
			}
			case aiSoldierFollowLeader : {
				FollowLeader();
				break;
			}
			case aiSoldierFreeHunting : {
				FreeHunting();
				break;
			}
			case aiSoldierInjuring : {
				Injuring();
				break;
			}
			case aiSoldierSitting : {
				Sitting();
				break;
			}
			case aiSoldierMoreDeadThanAlive : {
				MoreDeadThanAlive();
				break;
			}
			case aiSoldierPatrolUnderFire : {
				PatrolUnderFire();
				break;
			}
			case aiSoldierPatrolHurt : {
				PatrolHurt();
				break;
			}
			case aiSoldierPatrolHurtAggressiveUnderFire : {
				PatrolHurtAggressiveUnderFire();
				break;
			}
			case aiSoldierPatrolHurtNonAggressiveUnderFire : {
				PatrolHurtAggressiveUnderFire();
				break;
			}
			case aiSoldierRetreat : {
				Retreat();
				break;
			}
			case aiSoldierUnderFire : {
				UnderFire();
				break;
			}
			case aiSoldierJumping : {
				Jumping();
				break;
			}
			/**/
			case aiSoldierDie : {
				Die();
				break;
			}
			case aiSoldierWaitForAnimation : {
				OnWaitingForAnimation();
				break;
			}
			case aiSoldierWaitForTime : {
				OnWaitingForTime();
				break;
			}
			case aiSoldierTurnOver : {
				OnTurnOver();
				break;
			}
			case aiSoldierRecharge : {
				OnRecharge();
				break;
			}
			case aiSoldierLookingOver : {
				OnLookingOver();
				break;
			}
			case aiSoldierNoWeapon : {
				OnNoWeapon();
				break;
			}
			case aiSoldierPatrolRoute : {
				OnPatrol();
				break;
			}
			case aiSoldierPatrolReturnToRoute : {
				OnPatrolReturn();
				break;
			}
			case aiSoldierFollowLeaderPatrol : {
				OnFollowLeaderPatrol();
				break;
			}
			case aiSoldierSenseSomethingAlone: {
				OnSenseSomethingAlone();
				break;
			}
			case aiSoldierAttackFireAlone : {
				OnAttackFireAlone();
				break;
			}
			case aiSoldierSteal : {
				OnSteal();
				break;
			}
			case aiSoldierAttackAim : {
				OnAttackAim();
				break;
			}
			case aiSoldierPointAtSmth : {
				OnPointAtSmth();
				break;
			}
			case aiSoldierPatrolHurt : {
				OnPatrolHurt();
				break;
			}
			case aiSoldierDefendFireAlone : {
				OnDefendFireAlone();
				break;
			}
			case aiSoldierHurtAloneDefend : {
				OnHurtAloneDefend();
				break;
			}
			case aiSoldierPatrolDanger : {
				OnDangerAlone();
				break;
			}
			case aiSoldierPursuitAlone : {
				OnPursuitAlone();
				break;
			}
			/**/
			//default : Test();
		}
		m_bStateChanged = m_ePreviousState != eCurrentState;
	}
	while (!bStopThinking);
	m_dwLastUpdate = Level().timeServer() - m_fTimeUpdateDelta;
}
