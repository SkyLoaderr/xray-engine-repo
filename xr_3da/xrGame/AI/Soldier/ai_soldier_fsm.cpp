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
#include "..\\..\\..\\3dsound.h"
#include "..\\..\\..\\xr_trims.h"

#define MAX_PATROL_DISTANCE				6.f
#define MIN_PATROL_DISTANCE				1.f
#define	AMMO_NEED_RELOAD				6
#define MIN_COVER_MOVE					120
#define MIN_SPINE_TURN_ANGLE			PI_DIV_6
#define EYE_WEAPON_DELTA				(0*PI/30.f)
#define TORSO_START_SPEED				PI_DIV_4
#define DISTANCE_NEAR					0.f

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
	
	vfSetMovementType(m_cBodyState,0);

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

	//vfSetMovementType(BODY_STATE_STAND,m_fMinSpeed);
	if (m_cBodyState != BODY_STATE_STAND)
        vfSetMovementType(m_cBodyState,m_fMinSpeed);
	else
        vfSetMovementType(BODY_STATE_CROUCH,m_fMinSpeed);
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

void CAI_Soldier::OnLookingOver()
{
	WRITE_TO_LOG("Looking over...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
	
	SelectEnemy(Enemy);

//	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiSoldierAttackFire)
	
	DWORD dwCurTime = Level().timeServer();

//	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiSoldierPatrolHurt)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
//	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiSoldierPatrolUnderFire)
	
	CHECK_IF_SWITCH_TO_NEW_STATE(m_tpaPatrolPoints.size() > 1,aiSoldierPatrolRoute)
	/**
	for (int i=0; i<tpaDynamicSounds.size(); i++)
		if (tpaDynamicSounds[i].dwTime > m_dwLastUpdate) {
			SelectSound(m_iSoundIndex);
			AI_Path.TravelPath.clear();
			SWITCH_TO_NEW_STATE(aiSoldierSenseSomethingAlone);
		}
	/**/

	SET_LOOK_FIRE_MOVEMENT(false, BODY_STATE_STAND,m_fMinSpeed)

	//r_torso_target.yaw = r_torso_target.yaw + PI - PI/180;
	r_torso_target.pitch = -PI_DIV_4*0;
	r_torso_speed = PI_DIV_4;
}

void CAI_Soldier::OnWaitingForAnimation()
{
	WRITE_TO_LOG("Waiting for animation...");

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
	
//	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiSoldierAttackFire)
	
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
		
		if ((fDistance < 2.f) || (AI_NodeID == AI_Path.DestNode)) {
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

	SET_LOOK_FIRE_MOVEMENT(false, BODY_STATE_STAND,Leader != this ? m_fMaxSpeed : m_fMinSpeed)
	
	SetDirectionLook();
}

void CAI_Soldier::OnFollowLeaderPatrol()
{
	WRITE_TO_LOG("Following leader patrol");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
	
	SelectEnemy(Enemy);
	
//	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiSoldierAttackFire)
	
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
	if (acosf(tWatchDirection.dotproduct(tTemp0)) < PI_DIV_2) {
		float fDistance = Leader->Position().distance_to(Position());
		if (fDistance >= m_fMaxPatrolDistance) {
			SET_LOOK_FIRE_MOVEMENT(false, BODY_STATE_STAND,1.1f*m_fMinSpeed);
		}
		else
			if (fDistance <= m_fMinPatrolDistance) {
				SET_LOOK_FIRE_MOVEMENT(false, BODY_STATE_STAND,.9f*m_fMinSpeed);
			}
			else {
				SET_LOOK_FIRE_MOVEMENT(false, BODY_STATE_STAND,((fDistance - (m_fMaxPatrolDistance + m_fMinPatrolDistance)*.5f)/((m_fMaxPatrolDistance - m_fMinPatrolDistance)*.5f)*.1f + m_fMinPatrolDistance)*m_fMinSpeed);
			}
	}
	else {
		SET_LOOK_FIRE_MOVEMENT(false, BODY_STATE_STAND,.9f*m_fMinSpeed);
	}

	SetDirectionLook();
}

void CAI_Soldier::OnPatrol()
{
	WRITE_TO_LOG("Patrol");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
	
	SelectEnemy(Enemy);

//	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiSoldierAttackFire)
	
	DWORD dwCurTime = Level().timeServer();

	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiSoldierPatrolHurt)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
//	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiSoldierPatrolUnderFire)
	
	/**/
	for (int i=0; i<tpaDynamicSounds.size(); i++)
		if (tpaDynamicSounds[i].dwTime > m_dwLastUpdate) {
			SelectSound(m_iSoundIndex);
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
		
		vfCreateFastRealisticPath(m_tpaPatrolPoints, m_dwStartPatrolNode, m_tpaPointDeviations, AI_Path.TravelPath, m_dwaNodes, m_bLooped,true);
		
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
	
	SET_LOOK_FIRE_MOVEMENT(false, BODY_STATE_STAND,m_fMinSpeed)

	SetDirectionLook();
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
				tHitPosition = tpaDynamicSounds[iSoundIndex].tSavedPosition;
				dwHitTime = tpaDynamicSounds[iSoundIndex].dwTime;
				GO_TO_NEW_STATE(aiSoldierPatrolHurt);
			}
			else {
				Fvector tCurrentPosition = vPosition;
				tWatchDirection.sub(tpaDynamicSounds[iSoundIndex].tSavedPosition,tCurrentPosition);
				if (tWatchDirection.magnitude() > EPS_L) {
					tWatchDirection.normalize();
					mk_rotation(tWatchDirection,r_torso_target);
					r_target.yaw = r_torso_target.yaw;
					ASSIGN_SPINE_BONE;
				}
				
				CHECK_IF_SWITCH_TO_NEW_STATE(fabsf(r_torso_target.yaw - r_torso_current.yaw) > PI_DIV_6,aiSoldierTurnOver)

				vfInitSelector(SelectorPatrol,Squad,Leader);
		
				SelectorPatrol.m_tEnemyPosition = tpaDynamicSounds[iSoundIndex].tSavedPosition;

				if (AI_Path.bNeedRebuild) {
					vfBuildPathToDestinationPoint(0);
				}
				else {
					//SelectorFindEnemy.m_tLastEnemyPosition = tpaDynamicSounds[iSoundIndex].tSavedPosition;
					vfSearchForBetterPositionWTime(SelectorPatrol,Squad,Leader);
				}
			}
		}
		else {
			m_bStateChanged = false;
			GO_TO_PREV_STATE;
		}
		/**
		SOUND_TYPE_WEAPON_RECHARGING
		SOUND_TYPE_WEAPON_TAKING			
		SOUND_TYPE_WEAPON_HIDING			
		SOUND_TYPE_WEAPON_CHANGING			
		SOUND_TYPE_WEAPON_EMPTY_CLICKING	
		SOUND_TYPE_WEAPON_BULLET_RICOCHET	

		SOUND_TYPE_MONSTER_DYING			
		SOUND_TYPE_MONSTER_INJURING			
		SOUND_TYPE_MONSTER_WALKING_NORMAL	
		SOUND_TYPE_MONSTER_WALKING_CROUCH	
		SOUND_TYPE_MONSTER_WALKING_LIE		
		SOUND_TYPE_MONSTER_RUNNING_NORMAL	
		SOUND_TYPE_MONSTER_RUNNING_CROUCH	
		SOUND_TYPE_MONSTER_RUNNING_LIE		
		SOUND_TYPE_MONSTER_JUMPING_NORMAL	
		SOUND_TYPE_MONSTER_JUMPING_CROUCH	
		SOUND_TYPE_MONSTER_FALLING			
		SOUND_TYPE_MONSTER_TALKING			
		/**/
	}
	else {
		m_bStateChanged = false;
		GO_TO_PREV_STATE;
	}

	vfSetMovementType(BODY_STATE_CROUCH,m_fMinSpeed);
	
	vfAimAtEnemy();
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
		//GO_TO_NEW_STATE(aiSoldierPursuit);
		GO_TO_PREV_STATE;
	}
	
	//CHECK_IF_SWITCH_TO_NEW_STATE(!(Enemy.bVisible),aiSoldierFindEnemy)
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),Enemy.Enemy->Position());

	//CHECK_IF_GO_TO_NEW_STATE((tDistance.square_magnitude() >= 25.f) && ((Group.m_dwFiring > 1) || ((Group.m_dwFiring == 1) && (!m_bFiring))),aiSoldierAttackRun)
	
	CHECK_IF_SWITCH_TO_NEW_STATE((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() == 0),aiSoldierRecharge)

	if (!m_bFiring)
		vfAimAtEnemy();
	
	CHECK_IF_SWITCH_TO_NEW_STATE(fabsf(r_torso_target.yaw - r_torso_current.yaw) > PI_DIV_6,aiSoldierTurnOver)
	
	//CHECK_IF_SWITCH_TO_NEW_STATE(!bfCheckForEntityVisibility(Enemy.Enemy) && (vPosition.distance_to(Enemy.Enemy->Position()) > 5.f),aiSoldierSteal);

	AI_Path.TravelPath.clear();
	
	vfSaveEnemy();

	if (fabsf(r_torso_target.yaw - r_torso_current.yaw) < PI/30.f)
		vfSetFire(true,Group);
	else
		vfSetFire(false,Group);
	
	if (m_cBodyState != BODY_STATE_STAND)
		vfSetMovementType(m_cBodyState,0);
	else
		vfSetMovementType(BODY_STATE_CROUCH,0);
	
	//vfSetMovementType(BODY_STATE_STAND,m_fMinSpeed);
}

void CAI_Soldier::OnSteal()
{
	WRITE_TO_LOG("Stealing to enemy...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)
	
	SelectEnemy(Enemy);
	
	DWORD dwCurTime = Level().timeServer();
	
	//CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime) && (m_cBodyState != BODY_STATE_LIE),aiSoldierLyingDown)
	
	if (!(Enemy.Enemy)) {
		vfSetFire(false,getGroup());
		CHECK_IF_GO_TO_PREV_STATE(((tSavedEnemy) && (tSavedEnemy->g_Health() <= 0)) || (!tSavedEnemy))
		dwLostEnemyTime = Level().timeServer();
		//GO_TO_NEW_STATE(aiSoldierPursuit);
		GO_TO_PREV_STATE;
	}
	
	//CHECK_IF_SWITCH_TO_NEW_STATE(!(Enemy.bVisible),aiSoldierFindEnemy)
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),Enemy.Enemy->Position());

	//CHECK_IF_GO_TO_NEW_STATE((tDistance.square_magnitude() >= 25.f) && ((Group.m_dwFiring > 1) || ((Group.m_dwFiring == 1) && (!m_bFiring))),aiSoldierAttackRun)
	
	CHECK_IF_SWITCH_TO_NEW_STATE((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() == 0),aiSoldierRecharge)

	CHECK_IF_GO_TO_PREV_STATE(bfCheckForEntityVisibility(Enemy.Enemy) || (vPosition.distance_to(Enemy.Enemy->Position()) <= 5.f));

	vfSaveEnemy();

	INIT_SQUAD_AND_LEADER;
	
	vfInitSelector(SelectorPatrol,Squad,Leader);

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		vfSearchForBetterPositionWTime(SelectorPatrol,Squad,Leader);

	vfAimAtEnemy();
	
	vfSetFire(false,Group);
	
	vfSetMovementType(BODY_STATE_STAND,m_fMinSpeed);
}

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

	vfSetMovementType(m_cBodyState,0);
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

	vfSetMovementType(m_cBodyState,0);
}

/**
void CAI_Soldier::OnLyingDown()
{
	WRITE_TO_LOG("Lying down...");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

	DWORD dwCurTime = Level().timeServer();
	
	if (m_bStateChanged)
		m_dwLastRangeSearch = dwCurTime;

	vfSetMovementType(BODY_STATE_LIE,0);
	vfSetFire(false,Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()]);
	
	//if ((m_tpCurrentGlobalAnimation == tSoldierAnimations.tLie.tGlobal.tpLieDown) && (m_tpCurrentGlobalBlend) && (!(m_tpCurrentGlobalBlend->playing))) {
	AI_Path.TravelPath.clear();
	
	//CHECK_IF_GO_TO_PREV_STATE(((m_tpCurrentGlobalAnimation == tSoldierAnimations.tNormal.tGlobal.tpaLieDown[0]) || (m_tpCurrentGlobalAnimation == tSoldierAnimations.tCrouch.tGlobal.tpaLieDown[0])) && (Level().timeServer() - dwHitTime > 2500))
	CHECK_IF_GO_TO_PREV_STATE(dwCurTime - m_dwLastRangeSearch > 2500)
}

void CAI_Soldier::OnStandingUp()
{
	WRITE_TO_LOG("Standing up...");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiSoldierDie)

	DWORD dwCurTime = Level().timeServer();
	
	if (m_bStateChanged)
		m_dwLastRangeSearch = dwCurTime;

	vfSetFire(false,Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()]);
	
	AI_Path.TravelPath.clear();
	
	if (dwCurTime - m_dwLastRangeSearch > 2500) {
		vfSetMovementType(BODY_STATE_STAND,m_fMinSpeed);
		GO_TO_PREV_STATE;
	}
}
/**/

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
		
	if (fDistance < DISTANCE_NEAR) {
		r_torso_speed = TORSO_START_SPEED;
		vfSetMovementType(BODY_STATE_CROUCH,0);
	}
	else {
		r_torso_speed = 1*PI_DIV_2;
		if (m_cBodyState != BODY_STATE_LIE) {
			Lie();
			if (m_cBodyState == BODY_STATE_STAND)
				m_tpAnimationBeingWaited = tSoldierAnimations.tNormal.tGlobal.tpaLieDown[0];
			else
				m_tpAnimationBeingWaited = tSoldierAnimations.tCrouch.tGlobal.tpaLieDown[0];
			SWITCH_TO_NEW_STATE(aiSoldierWaitForAnimation);
		}
		r_torso_speed = TORSO_START_SPEED;
		r_torso_target.yaw = r_torso_current.yaw;
	}

	SelectEnemy(Enemy);

	if (fabsf(r_torso_target.yaw - r_torso_current.yaw) >= PI/30)
		return;
               
	if (fDistance < DISTANCE_NEAR)
		CHECK_IF_SWITCH_TO_NEW_STATE(!m_bStateChanged,aiSoldierAttackAim);

	CHECK_IF_GO_TO_NEW_STATE(Enemy.Enemy,aiSoldierDefendFireAlone)

	//dwHitTime = 0;
	GO_TO_NEW_STATE(aiSoldierHurtAloneDefend);
}

void CAI_Soldier::OnDefendFireAlone()
{
	WRITE_TO_LOG("Defend fire alone");
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

	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
			vfInitSelector(SelectorUnderFireCover,Squad,Leader);

			SelectorUnderFireCover.m_tEnemyPosition = tHitPosition;

			if (AI_Path.bNeedRebuild)
				vfBuildPathToDestinationPoint(0);
			else
				vfSearchForBetterPositionWTime(SelectorUnderFireCover,Squad,Leader);
				
			if (AI_Path.fSpeed > EPS_L)
				SetDirectionLook();
			else
				vfAimAtEnemy();

			if (dwCurTime - dwHitTime > 25000)
				vfSetMovementType(BODY_STATE_STAND,m_fMaxSpeed);
			else
				vfSetMovementType(m_cBodyState,m_fMinSpeed);

			if (dwCurTime - dwHitTime > 45000) {
				GO_TO_PREV_STATE;
			}

			break;
		}
		case BODY_STATE_CROUCH : {
			vfInitSelector(SelectorUnderFireCover,Squad,Leader);

			SelectorUnderFireCover.m_tEnemyPosition = tHitPosition;

			if (AI_Path.bNeedRebuild)
				vfBuildPathToDestinationPoint(0);
			else
				vfSearchForBetterPositionWTime(SelectorUnderFireCover,Squad,Leader);
				
			vfAimAtEnemy();
			
			if (dwCurTime - dwHitTime > 25000)
				vfSetMovementType(BODY_STATE_STAND,m_fMinSpeed);
			else
				vfSetMovementType(BODY_STATE_CROUCH,m_fMinSpeed);
			
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
					vfSearchForBetterPositionWTime(SelectorUnderFireCover,Squad,Leader);
					
				if ((dwCurTime - dwHitTime > 12000) && (m_cBodyState != BODY_STATE_STAND)) {
					StandUp();
					m_tpAnimationBeingWaited = tSoldierAnimations.tLie.tGlobal.tpStandUp;
					vfSetMovementType(m_cBodyState,0);
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
					vfSearchForBetterPositionWTime(SelectorUnderFireLine,Squad,Leader);
			}
			
			vfSetMovementType(m_cBodyState,m_fMinSpeed);
			break;
		}
	}
}

void CAI_Soldier::Think()
{
	bStopThinking = false;
	do {
		m_ePreviousState = eCurrentState;
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
			case aiSoldierPursuit : {
				Pursuit();
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
			case aiSoldierTurnOver : {
				OnTurnOver();
				break;
			}
			case aiSoldierLookingOver : {
				OnLookingOver();
				break;
			}
			case aiSoldierAttackFireAlone : {
				OnAttackFireAlone();
				break;
			}
			case aiSoldierRecharge : {
				OnRecharge();
				break;
			}
			case aiSoldierNoWeapon : {
				OnNoWeapon();
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
			case aiSoldierWaitForAnimation : {
				OnWaitingForAnimation();
				break;
			}
			case aiSoldierWaitForTime : {
				OnWaitingForTime();
				break;
			}
			/**/
		}
		m_bStateChanged = m_ePreviousState != eCurrentState;
	}
	while (!bStopThinking);
	m_dwLastUpdate = Level().timeServer() - m_fTimeUpdateDelta;
}
