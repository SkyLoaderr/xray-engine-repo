////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier.cpp
//	Created 	: 25.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\..\\3dsound.h"
#include "ai_soldier.h"
#include "ai_soldier_selectors.h"
#include "..\\..\\..\\xr_trims.h"
#include "..\\..\\hudmanager.h"
#include "..\\ai_monsters_misc.h"

#define COMPUTE_DISTANCE_2D(t,p) (sqrtf(_sqr((t).x - (p).x) + _sqr((t).z - (p).z)))

void CAI_Soldier::AttackFire()
{
	WRITE_TO_LOG("Shooting enemy...");

	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		
		return;
	}
	
	SelectEnemy(Enemy);
	
	DWORD dwCurTime = Level().timeServer();
	
	if ((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime) && (m_cBodyState != BODY_STATE_LIE)) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierLyingDown;
		m_dwLastRangeSearch = 0;
		
		return;
	}
	
	if (!(Enemy.Enemy)) {
		if (((tSavedEnemy) && (tSavedEnemy->g_Health() <= 0)) || (!tSavedEnemy)) {
			eCurrentState = tStateStack.top();
			tStateStack.pop();
			q_action.setup(AI::AIC_Action::FireEnd);
			m_dwLastRangeSearch = 0;
		}
		else {
			dwLostEnemyTime = Level().timeServer();
			eCurrentState = aiSoldierPursuit;
			q_action.setup(AI::AIC_Action::FireEnd);
			m_dwLastRangeSearch = 0;
		}
		
		return;
	}
	
	if (!(Enemy.bVisible)) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierFindEnemy;
		m_dwLastRangeSearch = 0;
		
		return;
	}
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),Enemy.Enemy->Position());

	/**
	if ((tDistance.square_magnitude() >= 25.f) && ((Group.m_dwFiring > 1) || ((Group.m_dwFiring == 1) && (!m_bFiring)))) {
		eCurrentState = aiSoldierAttackRun;
		m_dwLastRangeSearch = 0;
		
		return;
	}
	/**/

	if ((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() == 0)) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierReload;
		m_dwLastRangeSearch = 0;
		
		return;
	}

	vfSaveEnemy();

	AI_Path.TravelPath.clear();
	
	if (!m_bFiring)
		vfAimAtEnemy();
	
	vfSetFire(true,Group);
	
	vfSetMovementType(m_cBodyState,0);
	
	
}

void CAI_Soldier::AttackRun()
{
	WRITE_TO_LOG("Attack enemy");

	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		
		return;
	}
	
	SelectEnemy(Enemy);
	
	DWORD dwCurTime = Level().timeServer();
	
	if ((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime)) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierLyingDown;
		m_dwLastRangeSearch = 0;
		
		return;
	}
	
	if (!(Enemy.Enemy)) {
		if (((tSavedEnemy) && (tSavedEnemy->g_Health() <= 0)) || (!tSavedEnemy)) {
			eCurrentState = tStateStack.top();
			tStateStack.pop();
			q_action.setup(AI::AIC_Action::FireEnd);
			m_dwLastRangeSearch = 0;
		}
		else {
			dwLostEnemyTime = Level().timeServer();
			eCurrentState = aiSoldierPursuit;
			q_action.setup(AI::AIC_Action::FireEnd);
			m_dwLastRangeSearch = 0;
		}
		
		return;
	}
	
	if (!(Enemy.bVisible)) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierFindEnemy;
		m_dwLastRangeSearch = 0;
		
		return;
	}
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),Enemy.Enemy->Position());
	
	if ((tDistance.square_magnitude() < 25.f) || ((!(Group.m_dwFiring)) && (Weapons->ActiveWeapon() && (Weapons->ActiveWeapon()->GetAmmoElapsed() > 0)))) {
	//if ((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() > 0)){
		eCurrentState = aiSoldierAttackFire;
		m_dwLastRangeSearch = 0;
		
		return;
	}

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

void CAI_Soldier::Defend()
{
	WRITE_TO_LOG("Defend from enemy");
}

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

void CAI_Soldier::FindEnemy()
{
	WRITE_TO_LOG("Looking for enemy");

	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		
		return;
	}
	
	SelectEnemy(Enemy);
	
	if (!(Enemy.Enemy)) {
		if (((tSavedEnemy) && (tSavedEnemy->g_Health() <= 0)) || (!tSavedEnemy)) {
			eCurrentState = tStateStack.top();
			tStateStack.pop();
			q_action.setup(AI::AIC_Action::FireEnd);
			m_dwLastRangeSearch = 0;
		}
		else {
			dwLostEnemyTime = Level().timeServer();
			eCurrentState = aiSoldierPursuit;
			q_action.setup(AI::AIC_Action::FireEnd);
			m_dwLastRangeSearch = 0;
		}
		
		return;
	}
	
	if (Enemy.bVisible) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		m_dwLastRangeSearch = 0;
		
		return;
	}
		
	if ((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() < AMMO_NEED_RELOAD)) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierReload;
		m_dwLastRangeSearch = 0;
		
		return;
	}

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

void CAI_Soldier::FollowLeader()
{
	WRITE_TO_LOG("Following leader");

	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		
		return;
	}
		
	SelectEnemy(Enemy);
	// do I see the enemies?
	if (Enemy.Enemy)		{
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierAttackFire;
		m_dwLastRangeSearch = 0;
		
		return;
	}
	
	DWORD dwCurTime = Level().timeServer();
	
	if ((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime)) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierUnderFire;
		m_dwLastRangeSearch = 0;
		
		return;
	}
	
	if (dwCurTime - dwSenseTime < SENSE_JUMP_TIME) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierSenseSomething;
		m_dwLastRangeSearch = 0;
		
		return;
	}
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	if ((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime)) {
		//tHitDir = Group.m_tLastHitDirection;
		//dwHitTime = Group.m_dwLastHitTime;
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierUnderFire;
		m_dwLastRangeSearch = 0;
		
		return;
	}

	if (Leader == this) {
		if (m_tpaPatrolPoints.size())
			eCurrentState = aiSoldierPatrolRoute;
		else
			eCurrentState = aiSoldierFreeHunting;
		
		return;
	}
	else
		if (m_tpaPatrolPoints.size()) {
			eCurrentState = aiSoldierFollowLeaderPatrol;
			
			return;
		}

	vfInitSelector(SelectorFollowLeader,Squad,Leader);

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		vfSearchForBetterPosition(SelectorFollowLeader,Squad,Leader);
		
	SetLessCoverLook(AI_Node);

	vfSetFire(false,Group);

	vfSetMovementType(BODY_STATE_STAND,m_fMinSpeed);
	// stop processing more rules
	
}

void CAI_Soldier::FreeHunting()
{
	WRITE_TO_LOG("Free hunting");
	
	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		
		return;
	}
		
	SelectEnemy(Enemy);
	
	if (Enemy.Enemy) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierAttackFire;
		m_dwLastRangeSearch = 0;
		
		return;
	}
	
	DWORD dwCurTime = Level().timeServer();
	
	if ((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime)) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierUnderFire;
		m_dwLastRangeSearch = 0;
		
		return;
	}
	
	if (dwCurTime - dwSenseTime < SENSE_JUMP_TIME) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierSenseSomething;
		m_dwLastRangeSearch = 0;
		
		return;
	}
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	if ((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime)) {
		//tHitDir = Group.m_tLastHitDirection;
		//dwHitTime = Group.m_dwLastHitTime;
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierUnderFire;
		m_dwLastRangeSearch = 0;
		
		return;
	}

	if (m_tpaPatrolPoints.size()) {
		eCurrentState = aiSoldierPatrolRoute;
		
		return;
	}

	vfInitSelector(SelectorFreeHunting,Squad,Leader);

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		vfSearchForBetterPosition(SelectorFreeHunting,Squad,Leader);

	SetLessCoverLook(AI_Node);

	vfSetFire(false,Group);

	vfSetMovementType(BODY_STATE_STAND,m_fMinSpeed);
	// stop processing more rules
	
}

void CAI_Soldier::Injuring()
{
	WRITE_TO_LOG("Feeling pain...");
	
}

void CAI_Soldier::Jumping()
{
	WRITE_TO_LOG("Jumping...");
	
	if (m_bStateChanged) {
		m_dwLastRangeSearch = Level().timeServer();
		m_bActionStarted = true;
	}
	else {
		switch (m_cBodyState) {
			case BODY_STATE_STAND : {
				if ((m_tpCurrentLegsAnimation == tSoldierAnimations.tNormal.tGlobal.tpJumpIdle) && (m_tpCurrentLegsBlend)) {
				//if (Level().timeServer() - m_dwLastRangeSearch > 1000) {
					eCurrentState = tStateStack.top();
					tStateStack.pop();
				}
				break;
			}
			case BODY_STATE_CROUCH : {
				if ((m_tpCurrentLegsAnimation == tSoldierAnimations.tCrouch.tGlobal.tpJumpIdle) && (m_tpCurrentLegsBlend)) {
				//if (Level().timeServer() - m_dwLastRangeSearch > 1000) {
					eCurrentState = tStateStack.top();
					tStateStack.pop();
				}
				break;
			}
			case BODY_STATE_LIE : {
				eCurrentState = tStateStack.top();
				tStateStack.pop();
				break;
			}
		}
	}

	return;
}

void CAI_Soldier::StandingUp()
{
	WRITE_TO_LOG("Standing up...");
	
	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		return;
	}

	//if (m_cBodyState == BODY_STATE_LIE)
	//	m_tpCurrentGlobalAnimation = m_tpCurrentTorsoAnimation = m_tpCurrentHandsAnimation = m_tpCurrentLegsAnimation = 0;

	vfSetMovementType(BODY_STATE_STAND,0);
	
	AI_Path.TravelPath.clear();
	
	//if ((m_tpCurrentGlobalAnimation == tSoldierAnimations.tLie.tGlobal.tpLieDown) && (Level().timeServer() - dwHitTime > 500)) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		
		return;
	//}
}

void CAI_Soldier::Sitting()
{
	WRITE_TO_LOG("Sitting...");
	
	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		return;
	}

	vfSetMovementType(BODY_STATE_CROUCH,0);
	
	AI_Path.TravelPath.clear();
	
	//if ((m_tpCurrentGlobalAnimation == tSoldierAnimations.tLie.tGlobal.tpLieDown) && (Level().timeServer() - dwHitTime > 500)) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		
		return;
	//}
}

void CAI_Soldier::LyingDown()
{
	WRITE_TO_LOG("Lying down...");
	
	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		return;
	}

	vfSetMovementType(BODY_STATE_LIE,0);
	
	//if ((m_tpCurrentGlobalAnimation == tSoldierAnimations.tLie.tGlobal.tpLieDown) && (m_tpCurrentGlobalBlend) && (!(m_tpCurrentGlobalBlend->playing))) {
	AI_Path.TravelPath.clear();
	
	if ((m_tpCurrentGlobalAnimation == tSoldierAnimations.tLie.tGlobal.tpLieDown) && (Level().timeServer() - dwHitTime > 500)) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		
		return;
	}
}

void CAI_Soldier::MoreDeadThanAlive()
{
	WRITE_TO_LOG("More dead than alive");
}

void CAI_Soldier::NoWeapon()
{
	WRITE_TO_LOG("Searching for weapon");

	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		
		return;
	}
		
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
	// stop processing more rules
	
}

void CAI_Soldier::PatrolUnderFire()
{
	WRITE_TO_LOG("Patrol under fire");

	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		
		return;
	}

	SelectEnemy(Enemy);

	/**/
	if (Enemy.Enemy)		{
		eCurrentState = aiSoldierAttackFire;
		m_dwLastRangeSearch = 0;
		
		return;
	}
	/**/

	DWORD dwCurTime = Level().timeServer();

	/**/
	if (dwCurTime - dwHitTime > HIT_REACTION_TIME) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		m_dwLastRangeSearch = 0;
		
		return;
	}
	/**/
	
	if (dwCurTime - dwHitTime < m_dwPatrolShock) {
		eCurrentState = aiSoldierPatrolHurt;
		m_dwLastRangeSearch = 0;
		
		return;
	}
	
	/**
	if (m_cBodyState != BODY_STATE_LIE) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierLyingDown;
		
		return;
	}
	/**/

	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	if ((dwCurTime - Group.m_dwLastHitTime > HIT_REACTION_TIME) && (Group.m_dwLastHitTime)) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		m_dwLastRangeSearch = 0;
		
		return;
	}
	
	vfInitSelector(SelectorUnderFire,Squad,Leader);

	Fvector tTemp = tHitDir;
	tTemp.mul(40.f);
	SelectorUnderFire.m_tEnemyPosition.sub(Group.m_tHitPosition,tTemp);

	/**
	if (AI_Path.TravelStart >= AI_Path.TravelPath.size() - 1) {
		mk_rotation(Group.m_tLastHitDirection,r_torso_target);
		r_target.yaw = r_torso_target.yaw;
		r_torso_target.yaw = r_torso_target.yaw - EYE_WEAPON_DELTA;
	}
	else
		SetLessCoverLook(AI_Node);
	/**/

	/**/
	if (dwCurTime - Group.m_dwLastHitTime >= m_dwUnderFireShock) {
		if (AI_Path.bNeedRebuild)
			vfBuildPathToDestinationPoint(0);
		else
			if (m_dwLastSuccessfullSearch <= Group.m_dwLastHitTime)
				vfSearchForBetterPosition(SelectorUnderFire,Squad,Leader);
			
		//mk_rotation(Group.m_tLastHitDirection,r_torso_target);
		//r_target.yaw = r_torso_target.yaw + 0*PI_DIV_6;
		//r_torso_target.yaw = r_torso_target.yaw - EYE_WEAPON_DELTA;
		/**
		tWatchDirection.sub(Group.m_tHitPosition,eye_matrix.c);
		mk_rotation(tWatchDirection,r_torso_target);
		r_target.yaw = r_torso_target.yaw;
		r_torso_target.yaw = r_torso_target.yaw - EYE_WEAPON_DELTA;
		//r_torso_target.pitch = r_target.pitch = 0;
		/**/
		SetLessCoverLook(AI_Node);
		if (AI_Path.fSpeed)
			vfSetMovementType(BODY_STATE_CROUCH,m_fMaxSpeed);
		else
			vfSetMovementType(BODY_STATE_CROUCH,m_fMaxSpeed);
	}
	else {
		SetLessCoverLook(AI_Node);
		AI_Path.TravelPath.clear();
		vfSetMovementType(BODY_STATE_CROUCH,0);
	}
	/**/
	
	vfSetFire(false,Group);
	
	// stop processing more rules
	
}

void CAI_Soldier::PatrolHurtAggressiveUnderFire()
{
	// if no more health then soldier is dead
	WRITE_TO_LOG("Patrol hurt aggressive under fire");

	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		
		return;
	}

	SelectEnemy(Enemy);

	DWORD dwCurTime = Level().timeServer();
	
	if ((Enemy.Enemy) && (dwCurTime - dwHitTime >= m_dwPatrolShock + m_dwUnderFireShock)) {
		eCurrentState = aiSoldierAttackFire;
		m_dwLastRangeSearch = 0;
		
		return;
	}
	
	if (dwCurTime - dwHitTime >= m_dwPatrolShock + m_dwUnderFireShock) {
		eCurrentState = aiSoldierPatrolUnderFire;
		m_dwLastRangeSearch = 0;
		
		return;
	}
	
	if (dwCurTime - dwHitTime < m_dwPatrolShock) {
		eCurrentState = aiSoldierPatrolHurt;
		m_dwLastRangeSearch = 0;
		
		return;
	}
	
	if (m_cBodyState != BODY_STATE_LIE) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierLyingDown;
		
		return;
	}

	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	tWatchDirection.sub(Group.m_tHitPosition,vPosition);
	mk_rotation(tWatchDirection,r_torso_target);
	r_target.yaw = r_torso_target.yaw;
	r_torso_target.yaw = r_torso_target.yaw - EYE_WEAPON_DELTA;
	//r_torso_target.pitch = r_target.pitch = 0;
	vfSetMovementType(BODY_STATE_LIE,0);
	
	if ((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() <= 0)) {
		vfSetFire(Enemy.Enemy ? true : false,Group);
		Weapons->ActiveWeapon()->Reload();
	}
	else
		vfSetFire(Enemy.Enemy ? fabsf(r_torso_current.yaw - r_torso_target.yaw) < PI/30.f : false,Group);
	
	
}

void CAI_Soldier::PatrolHurtNonAggressiveUnderFire()
{
	WRITE_TO_LOG("Patrol hurt non-aggressive under fire");
	
	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		
		return;
	}

	SelectEnemy(Enemy);

	/**/
	if (Enemy.Enemy)		{
		eCurrentState = aiSoldierAttackFire;
		m_dwLastRangeSearch = 0;
		
		return;
	}
	/**/

	DWORD dwCurTime = Level().timeServer();

	/**/
	if (dwCurTime - dwHitTime > HIT_REACTION_TIME) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		m_dwLastRangeSearch = 0;
		
		return;
	}
	/**/
	
	/**
	if (m_cBodyState != BODY_STATE_LIE) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierLyingDown;
		
		return;
	}
	/**/

	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	if ((dwCurTime - Group.m_dwLastHitTime > HIT_REACTION_TIME) && (Group.m_dwLastHitTime)) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		m_dwLastRangeSearch = 0;
		
		return;
	}
	
	vfInitSelector(SelectorUnderFire,Squad,Leader);

	Fvector tTemp = tHitDir;
	tTemp.mul(40.f);
	SelectorUnderFire.m_tEnemyPosition.sub(Position(),tTemp);

	/**
	if (AI_Path.TravelStart >= AI_Path.TravelPath.size() - 1) {
		mk_rotation(Group.m_tLastHitDirection,r_torso_target);
		r_target.yaw = r_torso_target.yaw;
		r_torso_target.yaw = r_torso_target.yaw - EYE_WEAPON_DELTA;
	}
	else
		SetLessCoverLook(AI_Node);
	/**/

	/**/
	if (dwCurTime - dwHitTime >= m_dwUnderFireShock) {
		if (AI_Path.bNeedRebuild)
			vfBuildPathToDestinationPoint(0);
		else
			if (m_dwLastSuccessfullSearch <= Group.m_dwLastHitTime)
				vfSearchForBetterPosition(SelectorUnderFire,Squad,Leader);
			
		//mk_rotation(Group.m_tLastHitDirection,r_torso_target);
		//r_target.yaw = r_torso_target.yaw + 0*PI_DIV_6;
		//r_torso_target.yaw = r_torso_target.yaw - EYE_WEAPON_DELTA;
		/**
		tWatchDirection.sub(Group.m_tHitPosition,eye_matrix.c);
		mk_rotation(tWatchDirection,r_torso_target);
		r_target.yaw = r_torso_target.yaw;
		r_torso_target.yaw = r_torso_target.yaw - EYE_WEAPON_DELTA;
		//r_torso_target.pitch = r_target.pitch = 0;
		/**/
		SetLessCoverLook(AI_Node);
		vfSetMovementType(BODY_STATE_CROUCH,m_fMaxSpeed);
	}
	else {
		SetLessCoverLook(AI_Node);
		AI_Path.TravelPath.clear();
		vfSetMovementType(BODY_STATE_CROUCH,0);
	}
	/**/
	
	vfSetFire(false,Group);
	
	// stop processing more rules
	
}

void CAI_Soldier::PatrolHurt()
{
	WRITE_TO_LOG("Patrol hurt");

	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		
		return;
	}

	if (m_cBodyState != BODY_STATE_LIE) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierLyingDown;
		return;
	}
	
	INIT_SQUAD_AND_LEADER;
	CGroup &Group = Squad.Groups[g_Group()];
	
	DWORD dwCurTime = Level().timeServer();

	if (dwCurTime - dwHitTime >= m_dwPatrolShock) {
		if (iHealth < 0)
			eCurrentState = aiSoldierPatrolHurtNonAggressiveUnderFire;
		else
			eCurrentState = aiSoldierPatrolHurtAggressiveUnderFire;
		m_dwLastRangeSearch = 0;
		
		return;
	}

	AI_Path.TravelPath.clear();

	vfSetFire(false,Group);

	vfSetMovementType(BODY_STATE_LIE,0);

	
}

void CAI_Soldier::FollowLeaderPatrol()
{
	CHECK_FOR_STATE_TRANSITIONS("Following leader patrol");
	
	if (Leader == this) {
		eCurrentState = aiSoldierPatrolRoute;
		
		return;
	}

	if (m_bStateChanged) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierPatrolReturnToRoute;
		m_dwLastRangeSearch = 0;
		
		return;
	}
	else
		if ((!(AI_Path.fSpeed)) || (AI_Path.TravelStart >= AI_Path.TravelPath.size() - 4)) {
			CAI_Soldier *SoldierLeader = dynamic_cast<CAI_Soldier *>(Leader);
			if ((Level().timeServer() - SoldierLeader->m_dwLastRangeSearch < 30000) && (SoldierLeader->m_dwLastRangeSearch)) {
				AI_Path.TravelPath.clear();
				AI_Path.TravelPath.resize(SoldierLeader->AI_Path.TravelPath.size());
				for (int i=0, j=0; i<SoldierLeader->AI_Path.TravelPath.size(); i++, j++) {
					AI_Path.TravelPath[j].floating = FALSE;
					AI_Path.TravelPath[j].P = SoldierLeader->AI_Path.TravelPath[i].P;
					Fvector tTemp;
					tTemp.sub(SoldierLeader->AI_Path.TravelPath[i < SoldierLeader->AI_Path.TravelPath.size() - 1 ? i + 1 : 0].P, SoldierLeader->AI_Path.TravelPath[i].P);
					if ((tTemp.magnitude() < .1f) || ((j > 0) && (COMPUTE_DISTANCE_2D(SoldierLeader->AI_Path.TravelPath[i - 1].P,SoldierLeader->AI_Path.TravelPath[i].P) >= COMPUTE_DISTANCE_2D(SoldierLeader->AI_Path.TravelPath[i - 1].P,SoldierLeader->AI_Path.TravelPath[i < SoldierLeader->AI_Path.TravelPath.size() - 1 ? i + 1 : 0].P)))) {
						j--;
						continue;
					}
					tTemp.normalize();
					if (Group.Members[0] == this)
						tTemp.set(tTemp.z,0,-tTemp.x);
					else
						tTemp.set(-tTemp.z,0,tTemp.x);
					
					AI_Path.TravelPath[j].P.add(tTemp);
				}
				AI_Path.TravelPath.resize(j);
				AI_Path.TravelStart = 0;
				/**
				float fTemp, fDistance = AI_Path.TravelPath[AI_Path.TravelStart = 0].P.distance_to(Position());
				for ( i=1; i<AI_Path.TravelPath.size(); i++)
					if ((fTemp = AI_Path.TravelPath[i].P.distance_to(Position())) < fDistance) {
						fDistance = fTemp;
						AI_Path.TravelStart = i;
					}
				/**/
				m_dwLastRangeSearch = Level().timeServer();
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
}

void CAI_Soldier::Patrol()
{
	CHECK_FOR_STATE_TRANSITIONS("Patrol detour...");
	
	if (m_bStateChanged) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierPatrolReturnToRoute;
		m_dwLastRangeSearch = 0;
		
		return;
	}
	else
		if ((!(AI_Path.fSpeed)) || (AI_Path.TravelStart >= AI_Path.TravelPath.size() - 4)) {
			vfCreateFastRealisticPath(m_tpaPatrolPoints, m_dwStartPatrolNode, m_tpaPointDeviations, AI_Path.TravelPath);
			AI_Path.TravelStart = 0;
			m_dwLastRangeSearch = Level().timeServer();
		}
	
	SET_LOOK_FIRE_MOVEMENT(false, BODY_STATE_STAND,m_fMinSpeed)
}

void CAI_Soldier::PatrolReturn()
{
	CHECK_FOR_STATE_TRANSITIONS("Patrol return to route...");
	
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
		SelectorPatrol.m_tEnemyPosition = m_tpaPatrolPoints[0];
		float fTemp, fDistance;
		if (Leader == this) {
			fDistance = m_tpaPatrolPoints[0].distance_to(Position());
			/**
			for (int i=1; i<m_tpaPatrolPoints.size(); i++)
				if ((fTemp = m_tpaPatrolPoints[i].distance_to(Position())) < fDistance) {
					fDistance = fTemp;
					SelectorPatrol.m_tEnemyPosition = m_tpaPatrolPoints[i];
				}
			/**/
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
					if (fDistance < (fTemp = tTemp.magnitude())) {
						fDistance = fTemp;
					}
				}
				if (fDistance < 5.f) {
					m_ePreviousState = eCurrentState = tStateStack.top();
					tStateStack.pop();
					m_dwLastRangeSearch = 0;
					return;
				}
			}
			else {
				m_ePreviousState = eCurrentState = tStateStack.top();
				tStateStack.pop();
				m_dwLastRangeSearch = 0;
				return;
			}
		}
		else {
			AI_Path.bNeedRebuild = TRUE;
		}
	}

	if (Leader != this) {
		SET_LOOK_FIRE_MOVEMENT(false, BODY_STATE_STAND,m_fMaxSpeed)
	}
	else {
		SET_LOOK_FIRE_MOVEMENT(false, BODY_STATE_STAND,m_fMinSpeed)
	}
}

void CAI_Soldier::Pursuit()
{
	WRITE_TO_LOG("Pursuiting");

	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		
		return;
	}

	SelectEnemy(Enemy);
	
	if (Enemy.Enemy) {
		eCurrentState = aiSoldierAttackFire;
		m_dwLastRangeSearch = 0;
		
		return;
	}

	DWORD dwCurTime = Level().timeServer();
	
	if (dwCurTime - dwLostEnemyTime > LOST_ENEMY_REACTION_TIME) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		q_action.setup(AI::AIC_Action::FireEnd);
		m_fCurSpeed = m_fMaxSpeed;
		
		return;
	}
				
	if ((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime)) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierPatrolHurt;
		m_dwLastRangeSearch = 0;
		
		return;
	}
	
	if (dwCurTime - dwSenseTime < SENSE_JUMP_TIME) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierSenseSomething;
		m_dwLastRangeSearch = 0;
		
		return;
	}
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	if ((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime)) {
		//tHitDir = Group.m_tLastHitDirection;
		//dwHitTime = Group.m_dwLastHitTime;
		//tHitPosition = Group.m_tHitPosition;
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierUnderFire;
		m_dwLastRangeSearch = 0;
		
		return;
	}

	if (!tSavedEnemy) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		
		return;
	}

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

	vfSetMovementType(BODY_STATE_STAND,m_fMinSpeed);
	
	
}

void CAI_Soldier::Reload()
{
	WRITE_TO_LOG("Recharging...");
	
	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		
		return;
	}

	if ((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() == Weapons->ActiveWeapon()->GetAmmoMagSize())) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		m_dwLastRangeSearch = 0;
		
		return;
	}
	
	if ((Weapons->ActiveWeapon()) && (!(Weapons->ActiveWeapon()->GetAmmoCurrent()))) {
		eCurrentState = aiSoldierNoWeapon;
		m_dwLastRangeSearch = 0;
		
		return;
	}
	
	SelectEnemy(Enemy);

	/**
	if (!Enemy.Enemy) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		q_action.setup(AI::AIC_Action::FireEnd);
		m_dwLastRangeSearch = 0;
		
		return;
	}
	/**/
	
	DWORD dwCurTime = Level().timeServer();
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	vfInitSelector(SelectorReload,Squad,Leader);

	if (Enemy.Enemy) {
		if (AI_Path.bNeedRebuild)
			vfBuildPathToDestinationPoint(0);
		else
			vfSearchForBetterPosition(SelectorReload,Squad,Leader);
		
		tWatchDirection.sub(Enemy.Enemy->Position(),Position());
		
		if (tWatchDirection.magnitude() > 0.0001f)
			SetSmartLook(AI_Node,tWatchDirection);
		else
			SetLessCoverLook(AI_Node);
	}
	else {
		//SetLessCoverLook(AI_Node);
	}
	
	vfSetFire(false,Group);
	
	if (Weapons->ActiveWeapon())
		Weapons->ActiveWeapon()->Reload();

	//if (AI_Path.TravelPath.size() <= AI_Path.TravelStart)
		vfSetMovementType(m_cBodyState,m_fMinSpeed);
	//else
	//	vfSetMovementType(BODY_STATE_CROUCH,m_fMinSpeed);
	// stop processing more rules
	
}

void CAI_Soldier::Retreat()
{
	WRITE_TO_LOG("Retreating...");
}

void CAI_Soldier::SenseSomething()
{
	WRITE_TO_LOG("Sense something...");
	//
	//dwSenseTime = 0;
	// setting up a look to watch at the least safe direction
	q_look.setup(AI::AIC_Look::Look,AI::t_Direction,&tSenseDir,1000);
	// setting up look speed
	//q_look.o_look_speed=_FB_look_speed;

	eCurrentState = tStateStack.top();
	tStateStack.pop();
	
}

void CAI_Soldier::UnderFire()
{
	WRITE_TO_LOG("Under fire...");

	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		
		return;
	}

	SelectEnemy(Enemy);

	if (Enemy.Enemy)		{
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierAttackFire;
		m_dwLastRangeSearch = 0;
		
		return;
	}

	DWORD dwCurTime = Level().timeServer();

	/**
	if (dwCurTime - dwHitTime > HIT_REACTION_TIME) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		m_dwLastRangeSearch = 0;
		
		return;
	}
	/**/
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	Group.m_dwLastHitTime = dwHitTime;
	Group.m_tLastHitDirection = tHitDir;
	Group.m_tHitPosition = tHitPosition;
	
	if ((dwCurTime - Group.m_dwLastHitTime > HIT_REACTION_TIME) && (Group.m_dwLastHitTime)) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		m_dwLastRangeSearch = 0;
		
		return;
	}
	
	vfInitSelector(SelectorUnderFire,Squad,Leader);
	
	SelectorUnderFire.m_tEnemyPosition = tHitPosition;
	
	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		if (m_dwLastSuccessfullSearch <= Group.m_dwLastHitTime)
			vfSearchForBetterPosition(SelectorUnderFire,Squad,Leader);
		
		mk_rotation(tHitDir,r_torso_target);
		r_target.yaw = r_torso_target.yaw;
		ASSIGN_SPINE_BONE;
		r_target.yaw += PI_DIV_6;
		r_torso_target.yaw = r_torso_target.yaw - EYE_WEAPON_DELTA;
		//r_target.pitch = 0;
		//r_torso_target.pitch = 0;
		
		//q_look.o_look_speed=8*_FB_look_speed;
		
		vfSetFire(dwCurTime - dwHitTime < 1000,Group);
		
		vfSetMovementType(BODY_STATE_STAND,m_fMaxSpeed);
		// stop processing more rules
		
}

void CAI_Soldier::TestMicroActions()
{
	bStopThinking = true;
	
	if (Level().iGetKeyState(DIK_LCONTROL) || Level().iGetKeyState(DIK_RCONTROL)) {
		TEST_MICRO_ACTION(A);
		TEST_MICRO_ACTION(D);
		TEST_MICRO_ACTION(Q);
		TEST_MICRO_ACTION(E);
		TEST_MICRO_ACTION(Z);
		TEST_MICRO_ACTION(C);
		TEST_MICRO_ACTION(W);
		TEST_MICRO_ACTION(S);
		TEST_MICRO_ACTION(X);
		TEST_MICRO_ACTION(R);
		TEST_MICRO_ACTION(F);
		TEST_MICRO_ACTION(V);
		TEST_MICRO_ACTION(T);
		TEST_MICRO_ACTION(G);
		TEST_MICRO_ACTION(B);
		TEST_MICRO_ACTION(Y);
		TEST_MICRO_ACTION(H);
		TEST_MICRO_ACTION(N);
		TEST_MICRO_ACTION(U);
		TEST_MICRO_ACTION(J);
		TEST_MICRO_ACTION(M);
	}
}

void CAI_Soldier::TestA()
{
	WRITE_TO_LOG("model left turn");
	
	SUB_ANGLE(r_torso_target.yaw,PI_DIV_2);
	SUB_ANGLE(r_target.yaw,PI_DIV_2);
	SUB_ANGLE(r_spine_target.yaw,PI_DIV_2);
	ADJUST_BONE_ANGLES;

	q_look.o_look_speed = r_spine_speed = r_torso_speed;
		
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Soldier::TestD()
{
	WRITE_TO_LOG("model right turn");
	
	ADD_ANGLE(r_torso_target.yaw,PI_DIV_2);
	ADD_ANGLE(r_target.yaw,PI_DIV_2);
	ADD_ANGLE(r_spine_target.yaw,PI_DIV_2);
	ADJUST_BONE_ANGLES;
	q_look.o_look_speed = r_spine_speed = r_torso_speed;
		
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Soldier::TestQ()
{
	WRITE_TO_LOG("left torso + head turn");
	
	SUB_ANGLE(r_spine_target.yaw,PI_DIV_4);
	SUB_ANGLE(r_target.yaw,PI_DIV_2 - PI_DIV_4);
	ADJUST_BONE_ANGLES;
		
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Soldier::TestE()
{
	WRITE_TO_LOG("right torso + head turn");
	
	ADD_ANGLE(r_spine_target.yaw,PI_DIV_4);
	ADD_ANGLE(r_target.yaw,PI_DIV_2 - PI_DIV_4);
	ADJUST_BONE_ANGLES;
	
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Soldier::TestZ()
{
	WRITE_TO_LOG("left head turn");
	
	SUB_ANGLE(r_target.yaw,PI_DIV_2);
	ADJUST_BONE_ANGLES;
	
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Soldier::TestC()
{
	WRITE_TO_LOG("right head turn");
	
	ADD_ANGLE(r_target.yaw,PI_DIV_2);
	ADJUST_BONE_ANGLES;
	
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Soldier::TestW()
{
	WRITE_TO_LOG("stand up");
	
	if (m_cBodyState != BODY_STATE_STAND)
		eCurrentState = aiSoldierStandingUp;
	else {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
	}

	return;
}

void CAI_Soldier::TestS()
{
	WRITE_TO_LOG("sit down");
	
	if (m_cBodyState != BODY_STATE_CROUCH)
		eCurrentState = aiSoldierSitting;
	else {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
	}

	return;
}

void CAI_Soldier::TestX()
{
	WRITE_TO_LOG("lie down");
	
	if (m_cBodyState != BODY_STATE_LIE)
		eCurrentState = aiSoldierLyingDown;
	else {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
	}

	return;
}

void CAI_Soldier::TestR()
{
	WRITE_TO_LOG("recharge");
	
	if (Weapons->ActiveWeapon()) {
		if (m_bStateChanged) {
			if (Weapons->ActiveWeapon()->GetAmmoElapsed() == Weapons->ActiveWeapon()->GetAmmoMagSize())
				Weapons->ActiveWeapon()->AddAmmo(-1);
			Weapons->ActiveWeapon()->Reload();
		}
		else {
			if (Weapons->ActiveWeapon()->GetAmmoElapsed() == Weapons->ActiveWeapon()->GetAmmoMagSize()) {
				eCurrentState = tStateStack.top();
				tStateStack.pop();
			}
		}
	}
	else {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
	}
	
	return;
}

void CAI_Soldier::TestF()
{
	WRITE_TO_LOG("fire one shot");
	
	if (m_bStateChanged)
		q_action.setup(AI::AIC_Action::FireBegin);
	else {
		q_action.setup(AI::AIC_Action::FireEnd);
		eCurrentState = tStateStack.top();
		tStateStack.pop();
	}
				
	return;
}

void CAI_Soldier::TestV()
{
	WRITE_TO_LOG("aim 0");
	
	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
			m_tpCurrentTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpaAim[0];
			break;
		}
	}
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	
	return;
}

void CAI_Soldier::TestT()
{
	WRITE_TO_LOG("aim 1");
	
	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
			m_tpCurrentTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpaAim[1];
			break;
		}
	}
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	
	return;
}

void CAI_Soldier::TestG()
{
	WRITE_TO_LOG("attack 0");
	
	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
			m_tpCurrentTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpaAttack[0];
			break;
		}
	}
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	
	return;
}

void CAI_Soldier::TestB()
{
	WRITE_TO_LOG("attack 1");
	
	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
			m_tpCurrentTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpaAttack[1];
			break;
		}
	}
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	
	return;
}

void CAI_Soldier::TestY()
{
	WRITE_TO_LOG("idle 0");
	
	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
			m_tpCurrentTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpaIdle[0];
			break;
		}
	}
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	
	return;
}

void CAI_Soldier::TestH()
{
	WRITE_TO_LOG("idle 1");
	
	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
			m_tpCurrentTorsoAnimation = tSoldierAnimations.tNormal.tTorso.tpaIdle[1];
			break;
		}
	}
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	
	return;
}

void CAI_Soldier::TestN()
{
	WRITE_TO_LOG("jump");
	
	eCurrentState = aiSoldierJumping;
	
	return;
}

void CAI_Soldier::TestU()
{
	WRITE_TO_LOG("lie down");
	
	vfSetMovementType(BODY_STATE_LIE,0);
	eCurrentState = aiSoldierLyingDown;
	
	return;
}

void CAI_Soldier::TestJ()
{
	WRITE_TO_LOG("lie down");
	
	vfSetMovementType(BODY_STATE_LIE,0);
	eCurrentState = aiSoldierLyingDown;
	
	return;
}

void CAI_Soldier::TestM()
{
	WRITE_TO_LOG("lie down");
	
	vfSetMovementType(BODY_STATE_LIE,0);
	eCurrentState = aiSoldierLyingDown;
	
	return;
}

void CAI_Soldier::Think()
{
	bStopThinking = false;
	do {
		m_ePreviousState = eCurrentState;
		switch(eCurrentState) {
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
			case aiSoldierDie : {
				Die();
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
			case aiSoldierJumping : {
				Jumping();
				break;
			}
			case aiSoldierStandingUp : {
				StandingUp();
				break;
			}
			case aiSoldierSitting : {
				Sitting();
				break;
			}
			case aiSoldierLyingDown : {
				LyingDown();
				break;
			}
			case aiSoldierMoreDeadThanAlive : {
				MoreDeadThanAlive();
				break;
			}
			case aiSoldierNoWeapon : {
				NoWeapon();
				break;
			}
			case aiSoldierPatrolRoute : {
				Patrol();
				break;
			}
			case aiSoldierPatrolReturnToRoute : {
				PatrolReturn();
				break;
			}
			case aiSoldierFollowLeaderPatrol : {
				FollowLeaderPatrol();
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
			case aiSoldierReload : {
				Reload();
				break;
			}
			case aiSoldierRetreat : {
				Retreat();
				break;
			}
			case aiSoldierSenseSomething : {
				SenseSomething();
				break;
			}
			case aiSoldierUnderFire : {
				UnderFire();
				break;
			}
			case aiSoldierTestMicroActions : {
				TestMicroActions();
				break;
			}
			CASE_MICRO_ACTION(A);
			CASE_MICRO_ACTION(D);
			CASE_MICRO_ACTION(Q);
			CASE_MICRO_ACTION(E);
			CASE_MICRO_ACTION(Z);
			CASE_MICRO_ACTION(C);
			CASE_MICRO_ACTION(W);
			CASE_MICRO_ACTION(S);
			CASE_MICRO_ACTION(X);
			CASE_MICRO_ACTION(R);
			CASE_MICRO_ACTION(F);
			CASE_MICRO_ACTION(V);
			CASE_MICRO_ACTION(T);
			CASE_MICRO_ACTION(G);
			CASE_MICRO_ACTION(B);
			CASE_MICRO_ACTION(Y);
			CASE_MICRO_ACTION(H);
			CASE_MICRO_ACTION(N);
			CASE_MICRO_ACTION(U);
			CASE_MICRO_ACTION(J);
			CASE_MICRO_ACTION(M);
		}
		m_bStateChanged = m_ePreviousState != eCurrentState;
	}
	while (!bStopThinking);
}
