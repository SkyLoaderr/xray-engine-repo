////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier.cpp
//	Created 	: 25.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_soldier.h"
#include "ai_soldier_selectors.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\hudmanager.h"
#include "..\\..\\..\\3dsound.h"
#include "..\\..\\..\\xr_trims.h"

void CAI_Soldier::AttackFire()
{
	WRITE_TO_LOG("Shooting enemy...");

	CHECK_FOR_DEATH
	
	SelectEnemy(Enemy);
	
	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_SWITCH_TO_NEW_STATE(
		(dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime) && (m_cBodyState != BODY_STATE_LIE),
		aiSoldierLyingDown)
	
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

	//CHECK_IF_GO_TO_NEW_STATE((tDistance.square_magnitude() >= 25.f) && ((Group.m_dwFiring > 1) || ((Group.m_dwFiring == 1) && (!m_bFiring))),aiSoldierAttackRun)
	
	CHECK_IF_SWITCH_TO_NEW_STATE((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() == 0),aiSoldierReload)

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

void CAI_Soldier::AttackRun()
{
	WRITE_TO_LOG("Attack enemy");

	CHECK_FOR_DEATH
	
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
	
	CHECK_IF_GO_TO_NEW_STATE(
		(tDistance.square_magnitude() < 25.f) || ((!(Group.m_dwFiring)) && (Weapons->ActiveWeapon() && (Weapons->ActiveWeapon()->GetAmmoElapsed() > 0))),
		aiSoldierAttackFire)

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
	
	CHECK_FOR_DEATH
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

	CHECK_FOR_DEATH
	
	SelectEnemy(Enemy);
	
	if (!(Enemy.Enemy)) {
		vfSetFire(false,getGroup());
		CHECK_IF_GO_TO_PREV_STATE(((tSavedEnemy) && (tSavedEnemy->g_Health() <= 0)) || (!tSavedEnemy))
		dwLostEnemyTime = Level().timeServer();
		GO_TO_NEW_STATE(aiSoldierPursuit);
	}
	
	CHECK_IF_GO_TO_PREV_STATE(Enemy.bVisible)
	CHECK_IF_SWITCH_TO_NEW_STATE(
		(Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() < AMMO_NEED_RELOAD),
		aiSoldierReload)
		
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

	CHECK_FOR_DEATH
		
	SelectEnemy(Enemy);
	
	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiSoldierAttackFire)
	
	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiSoldierUnderFire)
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwSenseTime < SENSE_JUMP_TIME) && (dwSenseTime),aiSoldierSenseSomething)
	
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

void CAI_Soldier::FreeHunting()
{
	WRITE_TO_LOG("Free hunting");
	
	CHECK_FOR_DEATH
		
	SelectEnemy(Enemy);
	
	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiSoldierAttackFire)
	
	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiSoldierUnderFire)
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwSenseTime < SENSE_JUMP_TIME) && (dwSenseTime),aiSoldierSenseSomething)
	
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

void CAI_Soldier::Injuring()
{
	WRITE_TO_LOG("Feeling pain...");
	
	CHECK_FOR_DEATH

}

void CAI_Soldier::Jumping()
{
	if (Movement.Environment() == CMovementControl::peInAir)
		WRITE_TO_LOG("Jumping(air)...")
	else
		if (Movement.Environment() == CMovementControl::peOnGround)
			WRITE_TO_LOG("Jumping(ground)...")
		else
			WRITE_TO_LOG("Jumping(unknown)...")

	CHECK_FOR_DEATH

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

void CAI_Soldier::StandingUp()
{
	WRITE_TO_LOG("Standing up...");
	
	CHECK_FOR_DEATH

	//if (m_cBodyState == BODY_STATE_LIE)
	//	m_tpCurrentGlobalAnimation = m_tpCurrentTorsoAnimation = m_tpCurrentHandsAnimation = m_tpCurrentLegsAnimation = 0;

	vfSetMovementType(BODY_STATE_STAND,0);
	
	AI_Path.TravelPath.clear();
	
	CHECK_IF_GO_TO_PREV_STATE(true)
	//if ((m_tpCurrentGlobalAnimation == tSoldierAnimations.tLie.tGlobal.tpLieDown) && (Level().timeServer() - dwHitTime > 500)) {
	//}
}

void CAI_Soldier::Sitting()
{
	WRITE_TO_LOG("Sitting...");
	
	CHECK_FOR_DEATH

	vfSetMovementType(BODY_STATE_CROUCH,0);
	
	AI_Path.TravelPath.clear();
	
	CHECK_IF_GO_TO_PREV_STATE(true)
	//if ((m_tpCurrentGlobalAnimation == tSoldierAnimations.tLie.tGlobal.tpLieDown) && (Level().timeServer() - dwHitTime > 500)) {
	//}
}

void CAI_Soldier::LyingDown()
{
	WRITE_TO_LOG("Lying down...");
	
	CHECK_FOR_DEATH

	vfSetMovementType(BODY_STATE_LIE,0);
	vfSetFire(false,Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()]);
	
	//if ((m_tpCurrentGlobalAnimation == tSoldierAnimations.tLie.tGlobal.tpLieDown) && (m_tpCurrentGlobalBlend) && (!(m_tpCurrentGlobalBlend->playing))) {
	AI_Path.TravelPath.clear();
	
	CHECK_IF_GO_TO_PREV_STATE((m_tpCurrentGlobalAnimation == tSoldierAnimations.tLie.tGlobal.tpLieDown) && (Level().timeServer() - dwHitTime > 500))
}

void CAI_Soldier::MoreDeadThanAlive()
{
	WRITE_TO_LOG("More dead than alive");
	
	CHECK_FOR_DEATH

}

void CAI_Soldier::NoWeapon()
{
	WRITE_TO_LOG("Searching for weapon");

	CHECK_FOR_DEATH
		
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

void CAI_Soldier::PatrolUnderFire()
{
	WRITE_TO_LOG("Patrol under fire");

	CHECK_FOR_DEATH

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

void CAI_Soldier::PatrolHurtAggressiveUnderFire()
{
	// if no more health then soldier is dead
	WRITE_TO_LOG("Patrol hurt aggressive under fire");

	CHECK_FOR_DEATH

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

void CAI_Soldier::PatrolHurtNonAggressiveUnderFire()
{
	WRITE_TO_LOG("Patrol hurt non-aggressive under fire");
	
	CHECK_FOR_DEATH

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

void CAI_Soldier::PatrolHurt()
{
	WRITE_TO_LOG("Patrol hurt");

	CHECK_FOR_DEATH

	CHECK_IF_SWITCH_TO_NEW_STATE(m_cBodyState != BODY_STATE_LIE,aiSoldierLyingDown)
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	DWORD dwCurTime = Level().timeServer();

	if (dwCurTime - dwHitTime >= m_dwPatrolShock) {
		CHECK_IF_GO_TO_NEW_STATE(iHealth < 0,aiSoldierPatrolHurtNonAggressiveUnderFire)
		GO_TO_NEW_STATE(aiSoldierPatrolHurtAggressiveUnderFire);
	}

	AI_Path.TravelPath.clear();

	vfSetFire(false,Group);

	vfSetMovementType(BODY_STATE_LIE,0);
}

void CAI_Soldier::FollowLeaderPatrol()
{
	WRITE_TO_LOG("Following leader patrol");

	CHECK_FOR_DEATH
	
	SelectEnemy(Enemy);
	
	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiSoldierAttackFire)
	
	DWORD dwCurTime = Level().timeServer();

	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiSoldierPatrolHurt)
	
	CHECK_IF_SWITCH_TO_NEW_STATE(dwCurTime - dwSenseTime < SENSE_JUMP_TIME,aiSoldierSenseSomething)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiSoldierPatrolUnderFire)
	
	CHECK_IF_GO_TO_NEW_STATE(Leader == this,aiSoldierPatrolRoute)

	CHECK_IF_SWITCH_TO_NEW_STATE(m_bStateChanged,aiSoldierPatrolReturnToRoute)
	
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
	WRITE_TO_LOG("Patrol detour...");

	CHECK_FOR_DEATH
	
	SelectEnemy(Enemy);
	
	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiSoldierAttackFire)
	
	DWORD dwCurTime = Level().timeServer();

	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiSoldierPatrolHurt)
	
	CHECK_IF_SWITCH_TO_NEW_STATE(dwCurTime - dwSenseTime < SENSE_JUMP_TIME,aiSoldierSenseSomething)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiSoldierPatrolUnderFire)
	
	CHECK_IF_SWITCH_TO_NEW_STATE(m_bStateChanged,aiSoldierPatrolReturnToRoute)
	
	if ((!(AI_Path.fSpeed)) || (AI_Path.TravelStart >= AI_Path.TravelPath.size() - 4) || (AI_Path.TravelPath.empty())) {
		vfCreateFastRealisticPath(m_tpaPatrolPoints, m_dwStartPatrolNode, m_tpaPointDeviations, AI_Path.TravelPath);
		if (AI_Path.TravelPath.size()) {
			AI_Path.TravelStart = 0;
			m_dwLastRangeSearch = Level().timeServer();
		}
	}
	
	SET_LOOK_FIRE_MOVEMENT(false, BODY_STATE_STAND,m_fMinSpeed)
}

void CAI_Soldier::PatrolReturn()
{
	WRITE_TO_LOG("Patrol return to route...");

	CHECK_FOR_DEATH
	
	SelectEnemy(Enemy);
	
	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiSoldierAttackFire)
	
	DWORD dwCurTime = Level().timeServer();

	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiSoldierPatrolHurt)
	
	CHECK_IF_SWITCH_TO_NEW_STATE(dwCurTime - dwSenseTime < SENSE_JUMP_TIME,aiSoldierSenseSomething)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiSoldierPatrolUnderFire)
	
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
		float fDistance;
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
}

void CAI_Soldier::Pursuit()
{
	WRITE_TO_LOG("Pursuiting");

	CHECK_FOR_DEATH

	SelectEnemy(Enemy);
	
	CHECK_IF_GO_TO_NEW_STATE(Enemy.Enemy,aiSoldierAttackFire)

	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_GO_TO_PREV_STATE(dwCurTime - dwLostEnemyTime > LOST_ENEMY_REACTION_TIME)
				
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiSoldierPatrolHurt)
	
	CHECK_IF_SWITCH_TO_NEW_STATE(dwCurTime - dwSenseTime < SENSE_JUMP_TIME,aiSoldierSenseSomething)
	
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

void CAI_Soldier::Reload()
{
	WRITE_TO_LOG("Recharging...");
	
	CHECK_FOR_DEATH

	CHECK_IF_GO_TO_PREV_STATE((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() == Weapons->ActiveWeapon()->GetAmmoMagSize()))
	
	CHECK_IF_SWITCH_TO_NEW_STATE((Weapons->ActiveWeapon()) && (!(Weapons->ActiveWeapon()->GetAmmoCurrent())),aiSoldierNoWeapon)
	
	SelectEnemy(Enemy);

	//CHECK_IF_GO_TO_PREV_STATE(!Enemy.Enemy)
	
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

	CHECK_FOR_DEATH

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

#ifdef TEST_ACTIONS
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

void CAI_Soldier::TestMicroActionA()
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

void CAI_Soldier::TestMicroActionD()
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

void CAI_Soldier::TestMicroActionQ()
{
	WRITE_TO_LOG("left torso + head turn");
	
	SUB_ANGLE(r_spine_target.yaw,PI_DIV_4);
	SUB_ANGLE(r_target.yaw,PI_DIV_2 - PI_DIV_4);
	ADJUST_BONE_ANGLES;
		
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Soldier::TestMicroActionE()
{
	WRITE_TO_LOG("right torso + head turn");
	
	ADD_ANGLE(r_spine_target.yaw,PI_DIV_4);
	ADD_ANGLE(r_target.yaw,PI_DIV_2 - PI_DIV_4);
	ADJUST_BONE_ANGLES;
	
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Soldier::TestMicroActionZ()
{
	WRITE_TO_LOG("left head turn");
	
	SUB_ANGLE(r_target.yaw,PI_DIV_2);
	ADJUST_BONE_ANGLES;
	
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Soldier::TestMicroActionC()
{
	WRITE_TO_LOG("right head turn");
	
	ADD_ANGLE(r_target.yaw,PI_DIV_2);
	ADJUST_BONE_ANGLES;
	
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Soldier::TestMicroActionW()
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

void CAI_Soldier::TestMicroActionS()
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

void CAI_Soldier::TestMicroActionX()
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

void CAI_Soldier::TestMicroActionR()
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

void CAI_Soldier::TestMicroActionF()
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

void CAI_Soldier::TestMicroActionV()
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

void CAI_Soldier::TestMicroActionT()
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

void CAI_Soldier::TestMicroActionG()
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

void CAI_Soldier::TestMicroActionB()
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

void CAI_Soldier::TestMicroActionY()
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

void CAI_Soldier::TestMicroActionH()
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

void CAI_Soldier::TestMicroActionN()
{
	WRITE_TO_LOG("jump");
	
	eCurrentState = aiSoldierJumping;
	
	return;
}

void CAI_Soldier::TestMicroActionU()
{
	WRITE_TO_LOG("lie down");
	
	vfSetMovementType(BODY_STATE_LIE,0);
	eCurrentState = aiSoldierLyingDown;
	
	return;
}

void CAI_Soldier::TestMicroActionJ()
{
	WRITE_TO_LOG("lie down");
	
	vfSetMovementType(BODY_STATE_LIE,0);
	eCurrentState = aiSoldierLyingDown;
	
	return;
}

void CAI_Soldier::TestMicroActionM()
{
	WRITE_TO_LOG("lie down");
	
	vfSetMovementType(BODY_STATE_LIE,0);
	eCurrentState = aiSoldierLyingDown;
	
	return;
}

void CAI_Soldier::TestMacroActions()
{
	bStopThinking = true;
	
	if (Level().iGetKeyState(DIK_LSHIFT) || Level().iGetKeyState(DIK_RSHIFT)) {
		TEST_MACRO_ACTION(A);
		TEST_MACRO_ACTION(D);
		TEST_MACRO_ACTION(Q);
		TEST_MACRO_ACTION(E);
		TEST_MACRO_ACTION(Z);
		TEST_MACRO_ACTION(C);
		TEST_MACRO_ACTION(W);
		TEST_MACRO_ACTION(S);
		TEST_MACRO_ACTION(X);
		TEST_MACRO_ACTION(R);
		TEST_MACRO_ACTION(F);
		TEST_MACRO_ACTION(V);
		TEST_MACRO_ACTION(T);
		TEST_MACRO_ACTION(G);
		TEST_MACRO_ACTION(B);
		TEST_MACRO_ACTION(Y);
		TEST_MACRO_ACTION(H);
		TEST_MACRO_ACTION(N);
		TEST_MACRO_ACTION(U);
		TEST_MACRO_ACTION(J);
		TEST_MACRO_ACTION(M);
	}
}

void CAI_Soldier::TestMacroActionA()
{
	WRITE_TO_LOG("look and turn");
	
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Soldier::TestMacroActionD()
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

void CAI_Soldier::TestMacroActionQ()
{
	WRITE_TO_LOG("left torso + head turn");
	
	SUB_ANGLE(r_spine_target.yaw,PI_DIV_4);
	SUB_ANGLE(r_target.yaw,PI_DIV_2 - PI_DIV_4);
	ADJUST_BONE_ANGLES;
		
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Soldier::TestMacroActionE()
{
	WRITE_TO_LOG("right torso + head turn");
	
	ADD_ANGLE(r_spine_target.yaw,PI_DIV_4);
	ADD_ANGLE(r_target.yaw,PI_DIV_2 - PI_DIV_4);
	ADJUST_BONE_ANGLES;
	
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Soldier::TestMacroActionZ()
{
	WRITE_TO_LOG("left head turn");
	
	SUB_ANGLE(r_target.yaw,PI_DIV_2);
	ADJUST_BONE_ANGLES;
	
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Soldier::TestMacroActionC()
{
	WRITE_TO_LOG("right head turn");
	
	ADD_ANGLE(r_target.yaw,PI_DIV_2);
	ADJUST_BONE_ANGLES;
	
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Soldier::TestMacroActionW()
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

void CAI_Soldier::TestMacroActionS()
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

void CAI_Soldier::TestMacroActionX()
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

void CAI_Soldier::TestMacroActionR()
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

void CAI_Soldier::TestMacroActionF()
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

void CAI_Soldier::TestMacroActionV()
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

void CAI_Soldier::TestMacroActionT()
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

void CAI_Soldier::TestMacroActionG()
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

void CAI_Soldier::TestMacroActionB()
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

void CAI_Soldier::TestMacroActionY()
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

void CAI_Soldier::TestMacroActionH()
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

void CAI_Soldier::TestMacroActionN()
{
	WRITE_TO_LOG("jump");
	
	eCurrentState = aiSoldierJumping;
	
	return;
}

void CAI_Soldier::TestMacroActionU()
{
	WRITE_TO_LOG("lie down");
	
	vfSetMovementType(BODY_STATE_LIE,0);
	eCurrentState = aiSoldierLyingDown;
	
	return;
}

void CAI_Soldier::TestMacroActionJ()
{
	WRITE_TO_LOG("lie down");
	
	vfSetMovementType(BODY_STATE_LIE,0);
	eCurrentState = aiSoldierLyingDown;
	
	return;
}

void CAI_Soldier::TestMacroActionM()
{
	WRITE_TO_LOG("lie down");
	
	vfSetMovementType(BODY_STATE_LIE,0);
	eCurrentState = aiSoldierLyingDown;
	
	return;
}
#endif

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
			#ifdef TEST_ACTIONS
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
				case aiSoldierTestMacroActions : {
					TestMacroActions();
					break;
				}
				CASE_MACRO_ACTION(A);
				CASE_MACRO_ACTION(D);
				CASE_MACRO_ACTION(Q);
				CASE_MACRO_ACTION(E);
				CASE_MACRO_ACTION(Z);
				CASE_MACRO_ACTION(C);
				CASE_MACRO_ACTION(W);
				CASE_MACRO_ACTION(S);
				CASE_MACRO_ACTION(X);
				CASE_MACRO_ACTION(R);
				CASE_MACRO_ACTION(F);
				CASE_MACRO_ACTION(V);
				CASE_MACRO_ACTION(T);
				CASE_MACRO_ACTION(G);
				CASE_MACRO_ACTION(B);
				CASE_MACRO_ACTION(Y);
				CASE_MACRO_ACTION(H);
				CASE_MACRO_ACTION(N);
				CASE_MACRO_ACTION(U);
				CASE_MACRO_ACTION(J);
				CASE_MACRO_ACTION(M);
			#endif
		}
		m_bStateChanged = m_ePreviousState != eCurrentState;
	}
	while (!bStopThinking);
}
