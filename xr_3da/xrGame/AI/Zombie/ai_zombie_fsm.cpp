////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie.cpp
//	Created 	: 25.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_zombie.h"
#include "ai_zombie_selectors.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\hudmanager.h"
#include "..\\..\\..\\3dsound.h"
#include "..\\..\\..\\xr_trims.h"

void CAI_Zombie::AttackFire()
{
	WRITE_TO_LOG("Attacking enemy...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)
	
	SelectEnemy(Enemy);
	
	DWORD dwCurTime = Level().timeServer();
	
	//CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime) && (m_cBodyState != BODY_STATE_LIE),aiZombieLyingDown)
	
	if (!(Enemy.Enemy)) {
		CHECK_IF_GO_TO_PREV_STATE(((tSavedEnemy) && (tSavedEnemy->g_Health() <= 0)) || (!tSavedEnemy))
		dwLostEnemyTime = Level().timeServer();
		GO_TO_NEW_STATE(aiZombiePursuit);
	}
	
	CHECK_IF_SWITCH_TO_NEW_STATE(!(Enemy.bVisible),aiZombieFindEnemy)
		
	CHECK_IF_GO_TO_NEW_STATE((Enemy.Enemy->Position().distance_to(vPosition) > 2.f),aiZombieAttackRun)

	Fvector tTemp;
	tTemp.sub(Enemy.Enemy->Position(),vPosition);
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);
	
	CHECK_IF_GO_TO_NEW_STATE(fabsf(r_torso_current.yaw - sTemp.yaw) > 2*PI_DIV_6,aiZombieAttackRun)
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),Enemy.Enemy->Position());

	AI_Path.TravelPath.clear();
	
	vfSaveEnemy();

	if (!m_bFiring)
		vfAimAtEnemy();

	vfSetFire(true,Group);

	vfSetMovementType(m_cBodyState,0);
}

void CAI_Zombie::AttackRun()
{
	WRITE_TO_LOG("Attack enemy");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)
	
	SelectEnemy(Enemy);
	
	DWORD dwCurTime = Level().timeServer();
	
	//CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiZombieLyingDown)
	
	if (!(Enemy.Enemy)) {
		CHECK_IF_GO_TO_PREV_STATE(((tSavedEnemy) && (tSavedEnemy->g_Health() <= 0)) || (!tSavedEnemy))
		dwLostEnemyTime = Level().timeServer();
		GO_TO_NEW_STATE(aiZombiePursuit);
	}
	
	CHECK_IF_SWITCH_TO_NEW_STATE(!(Enemy.bVisible),aiZombieFindEnemy)
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),Enemy.Enemy->Position());
	
	CHECK_IF_GO_TO_NEW_STATE((tDistance.square_magnitude() <= 2.f),aiZombieAttackFire);

	INIT_SQUAD_AND_LEADER;
	
	vfInitSelector(SelectorAttack,Squad,Leader);
	
	/**
	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(&SelectorAttack);
	else
		vfSearchForBetterPosition(SelectorAttack,Squad,Leader);
	/**/
	GoToPointViaSubnodes(Enemy.Enemy->Position());
	
	vfAimAtEnemy();
	
	vfSetMovementType(m_cBodyState,m_fMaxSpeed);
}

void CAI_Zombie::Defend()
{
	WRITE_TO_LOG("Defend from enemy");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)
}

void CAI_Zombie::Die()
{
	WRITE_TO_LOG("Dying...");

	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	AI_Path.TravelPath.clear();
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

	//bActive = false;
	bEnabled = false;
}

void CAI_Zombie::FindEnemy()
{
	WRITE_TO_LOG("Looking for enemy");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)
	
	SelectEnemy(Enemy);
	
	if (!(Enemy.Enemy)) {
		CHECK_IF_GO_TO_PREV_STATE(((tSavedEnemy) && (tSavedEnemy->g_Health() <= 0)) || (!tSavedEnemy))
		dwLostEnemyTime = Level().timeServer();
		GO_TO_NEW_STATE(aiZombiePursuit);
	}
	
	CHECK_IF_GO_TO_PREV_STATE(Enemy.bVisible)
		
	INIT_SQUAD_AND_LEADER;
	
	vfInitSelector(SelectorFindEnemy,Squad,Leader);
	
	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		vfSearchForBetterPosition(SelectorFindEnemy,Squad,Leader);
	
	vfAimAtEnemy();
	
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	vfSetMovementType(BODY_STATE_STAND,m_fMaxSpeed);
}

void CAI_Zombie::FollowLeader()
{
	WRITE_TO_LOG("Following leader");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)
		
	SelectEnemy(Enemy);
	
	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiZombieAttackFire)
	
	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiZombieUnderFire)
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwSenseTime < SENSE_JUMP_TIME) && (dwSenseTime),aiZombieSenseSomething)
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiZombieUnderFire)

	if (Leader == this) {
		CHECK_IF_SWITCH_TO_NEW_STATE(m_tpaPatrolPoints.size(),aiZombiePatrolRoute)
		GO_TO_NEW_STATE(aiZombieFreeHunting)
	}
	else
		CHECK_IF_SWITCH_TO_NEW_STATE(m_tpaPatrolPoints.size(),aiZombieFollowLeaderPatrol)

	vfInitSelector(SelectorFollowLeader,Squad,Leader);

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		vfSearchForBetterPosition(SelectorFollowLeader,Squad,Leader);
		
	SetLessCoverLook(AI_Node);

	vfSetMovementType(BODY_STATE_STAND,m_fMinSpeed);
}

void CAI_Zombie::FreeHunting()
{
	WRITE_TO_LOG("Free hunting");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)
		
	SelectEnemy(Enemy);
	
	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiZombieAttackFire)
	
	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiZombieUnderFire)
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwSenseTime < SENSE_JUMP_TIME) && (dwSenseTime),aiZombieSenseSomething)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiZombieUnderFire)
	
	CHECK_IF_SWITCH_TO_NEW_STATE(m_tpaPatrolPoints.size(),aiZombiePatrolRoute)

	vfInitSelector(SelectorFreeHunting,Squad,Leader);

	if (AI_Path.bNeedRebuild) {
		vfBuildPathToDestinationPoint(0);
		Fvector tTemp1 = vPosition;
		tTemp1.sub(AI_Path.TravelPath[AI_Path.TravelStart ? AI_Path.TravelStart - 1 : 0].P);
		vfNormalizeSafe(tTemp1);
		SRotation sRot;
		mk_rotation(tTemp1,sRot);
		if (!((fabsf(r_torso_target.yaw - sRot.yaw) < PI_DIV_6) || ((fabsf(fabsf(r_torso_target.yaw - sRot.yaw) - PI_MUL_2) < PI_DIV_6)))) {
			r_torso_target.yaw = sRot.yaw > PI ? sRot.yaw - 2*PI : sRot.yaw;
			r_spine_target.yaw = r_torso_target.yaw; 
			r_target.yaw = 0;//r_torso_target.yaw; 
			SWITCH_TO_NEW_STATE(aiZombieTurnOver);
		}
	}
	else
		if ((AI_Path.TravelPath.empty()) || (!AI_Path.fSpeed)) {
			vfSearchForBetterPositionWTime(SelectorFreeHunting,Squad,Leader);
		}

	SetDirectionLook();

	vfSetMovementType(BODY_STATE_STAND,m_fMinSpeed);
}

void CAI_Zombie::Injuring()
{
	WRITE_TO_LOG("Feeling pain...");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)

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

void CAI_Zombie::StandingUp()
{
	WRITE_TO_LOG("Standing up...");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)

	//if (m_cBodyState == BODY_STATE_LIE)
	//	m_tpCurrentGlobalAnimation = m_tpCurrentTorsoAnimation = m_tpCurrentHandsAnimation = m_tpCurrentLegsAnimation = 0;

	vfSetMovementType(BODY_STATE_STAND,0);
	
	AI_Path.TravelPath.clear();
	
	CHECK_IF_GO_TO_PREV_STATE(true)
	//if ((m_tpCurrentGlobalAnimation == tZombieAnimations.tLie.tGlobal.tpLieDown) && (Level().timeServer() - dwHitTime > 500)) {
	//}
}

void CAI_Zombie::Sitting()
{
	WRITE_TO_LOG("Sitting...");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)

	vfSetMovementType(BODY_STATE_CROUCH,0);
	
	AI_Path.TravelPath.clear();
	
	CHECK_IF_GO_TO_PREV_STATE(true)
	//if ((m_tpCurrentGlobalAnimation == tZombieAnimations.tLie.tGlobal.tpLieDown) && (Level().timeServer() - dwHitTime > 500)) {
	//}
}

void CAI_Zombie::LyingDown()
{
	WRITE_TO_LOG("Lying down...");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)

	vfSetMovementType(BODY_STATE_LIE,0);
	AI_Path.TravelPath.clear();
	
	//CHECK_IF_GO_TO_PREV_STATE((m_tpCurrentGlobalAnimation == tZombieAnimations.tLie.tGlobal.tpLieDown) && (Level().timeServer() - dwHitTime > 500))
}

void CAI_Zombie::MoreDeadThanAlive()
{
	WRITE_TO_LOG("More dead than alive");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)

}

void CAI_Zombie::PatrolUnderFire()
{
	WRITE_TO_LOG("Patrol under fire");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)

	SelectEnemy(Enemy);

	CHECK_IF_GO_TO_NEW_STATE(Enemy.Enemy,aiZombieAttackFire)
	
	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_GO_TO_PREV_STATE((dwCurTime - dwHitTime > HIT_REACTION_TIME) && (dwHitTime))
	
	CHECK_IF_SWITCH_TO_NEW_STATE(dwCurTime - dwHitTime < m_dwPatrolShock,aiZombiePatrolHurt)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
	CHECK_IF_GO_TO_PREV_STATE((dwCurTime - Group.m_dwLastHitTime > HIT_REACTION_TIME) && (Group.m_dwLastHitTime))
	
	//CHECK_IF_SWITCH_TO_NEW_STATE(m_cBodyState != BODY_STATE_LIE,aiZombieLyingDown)
	
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
}

void CAI_Zombie::PatrolHurtAggressiveUnderFire()
{
	// if no more health then zombie is dead
	WRITE_TO_LOG("Patrol hurt aggressive under fire");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)

	SelectEnemy(Enemy);

	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_GO_TO_NEW_STATE((Enemy.Enemy) && (dwCurTime - dwHitTime >= m_dwPatrolShock + m_dwUnderFireShock),aiZombieAttackFire)
	
	CHECK_IF_GO_TO_NEW_STATE(dwCurTime - dwHitTime >= m_dwPatrolShock + m_dwUnderFireShock,aiZombiePatrolUnderFire)
	
	CHECK_IF_GO_TO_NEW_STATE(dwCurTime - dwHitTime < m_dwPatrolShock,aiZombiePatrolHurt)

	//CHECK_IF_SWITCH_TO_NEW_STATE(m_cBodyState != BODY_STATE_LIE,aiZombieLyingDown)

	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
	tWatchDirection.sub(Group.m_tHitPosition,vPosition);
	LOOK_AT_DIRECTION(tWatchDirection)
	SUB_ANGLE(r_torso_target.yaw,EYE_WEAPON_DELTA)
	//r_torso_target.pitch = r_target.pitch = 0;

	//vfSetMovementType(BODY_STATE_LIE,0);
}

void CAI_Zombie::PatrolHurtNonAggressiveUnderFire()
{
	WRITE_TO_LOG("Patrol hurt non-aggressive under fire");
	
	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)

	SelectEnemy(Enemy);

	CHECK_IF_GO_TO_NEW_STATE(Enemy.Enemy,aiZombieAttackFire)

	DWORD dwCurTime = Level().timeServer();

	CHECK_IF_GO_TO_PREV_STATE(dwCurTime - dwHitTime > HIT_REACTION_TIME)
	
	//CHECK_IF_SWITCH_TO_NEW_STATE(m_cBodyState != BODY_STATE_LIE,aiZombieLyingDown)
	
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
}

void CAI_Zombie::PatrolHurt()
{
	WRITE_TO_LOG("Patrol hurt");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)

	//CHECK_IF_SWITCH_TO_NEW_STATE(m_cBodyState != BODY_STATE_LIE,aiZombieLyingDown)
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	DWORD dwCurTime = Level().timeServer();

	if (dwCurTime - dwHitTime >= m_dwPatrolShock) {
		CHECK_IF_GO_TO_NEW_STATE(iHealth < 0,aiZombiePatrolHurtNonAggressiveUnderFire)
		GO_TO_NEW_STATE(aiZombiePatrolHurtAggressiveUnderFire);
	}

	AI_Path.TravelPath.clear();

	//vfSetMovementType(BODY_STATE_LIE,0);
}

void CAI_Zombie::FollowLeaderPatrol()
{
	WRITE_TO_LOG("Following leader patrol");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)
	
	SelectEnemy(Enemy);
	
	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiZombieAttackFire)
	
	DWORD dwCurTime = Level().timeServer();

	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiZombiePatrolHurt)
	
	CHECK_IF_SWITCH_TO_NEW_STATE(dwCurTime - dwSenseTime < SENSE_JUMP_TIME,aiZombieSenseSomething)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiZombiePatrolUnderFire)
	
	CHECK_IF_GO_TO_NEW_STATE(Leader == this,aiZombiePatrolRoute)

	CHECK_IF_SWITCH_TO_NEW_STATE(m_bStateChanged,aiZombiePatrolReturnToRoute)
	
	if ((!(AI_Path.fSpeed)) || (AI_Path.TravelPath.empty()) || (AI_Path.TravelPath[AI_Path.TravelStart].P.distance_to(AI_Path.TravelPath[AI_Path.TravelPath.size() - 1].P) <= .5f)) {
		CAI_Zombie *ZombieLeader = dynamic_cast<CAI_Zombie *>(Leader);
		if (!m_bLooped) {
			Fvector tTemp1 = m_tpaPatrolPoints[m_tpaPatrolPoints.size() - 2];
			tTemp1.sub(m_tpaPatrolPoints[m_tpaPatrolPoints.size() - 1]);
			vfNormalizeSafe(tTemp1);
			SRotation sRot;
			mk_rotation(tTemp1,sRot);
			if (!((fabsf(r_torso_target.yaw - sRot.yaw) < PI_DIV_6) || ((fabsf(fabsf(r_torso_target.yaw - sRot.yaw) - PI_MUL_2) < PI_DIV_6)))) {
				AI_Path.TravelStart = AI_Path.TravelPath.size() - 1;
				r_torso_target.yaw = sRot.yaw > PI ? sRot.yaw - 2*PI : sRot.yaw;
				r_spine_target.yaw = r_torso_target.yaw; 
				r_target.yaw = 0;//r_torso_target.yaw; 
				SWITCH_TO_NEW_STATE(aiZombieTurnOver);
			}
		}
		
		if ((dwCurTime - ZombieLeader->m_dwLastRangeSearch < 3000) && (ZombieLeader->AI_Path.fSpeed > EPS_L)) {
			m_dwLoopCount = ZombieLeader->m_dwLoopCount;
			
			AI_Path.TravelPath.clear();
			AI_Path.TravelPath.resize(ZombieLeader->AI_Path.TravelPath.size());
			
			for (int i=0, j=0; i<ZombieLeader->AI_Path.TravelPath.size(); i++, j++) {
				AI_Path.TravelPath[j].floating = FALSE;
				AI_Path.TravelPath[j].P = ZombieLeader->AI_Path.TravelPath[i].P;
				Fvector tTemp;
				
				if (m_bLooped)
					tTemp.sub(ZombieLeader->AI_Path.TravelPath[i < ZombieLeader->AI_Path.TravelPath.size() - 1 ? i + 1 : 0].P, ZombieLeader->AI_Path.TravelPath[i].P);
				else
					if (i < ZombieLeader->AI_Path.TravelPath.size() - 1)
						tTemp.sub(ZombieLeader->AI_Path.TravelPath[i < ZombieLeader->AI_Path.TravelPath.size() - 1 ? i + 1 : 0].P, ZombieLeader->AI_Path.TravelPath[i].P);
					else
						tTemp.sub(ZombieLeader->AI_Path.TravelPath[i].P, ZombieLeader->AI_Path.TravelPath[i - 1].P);

				if ((tTemp.magnitude() < .1f) || ((j > 0) && (COMPUTE_DISTANCE_2D(ZombieLeader->AI_Path.TravelPath[i - 1].P,ZombieLeader->AI_Path.TravelPath[i].P) >= COMPUTE_DISTANCE_2D(ZombieLeader->AI_Path.TravelPath[i - 1].P,ZombieLeader->AI_Path.TravelPath[i < ZombieLeader->AI_Path.TravelPath.size() - 1 ? i + 1 : 0].P)))) {
					j--;
					continue;
				}
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
			SET_LOOK_FIRE_MOVEMENT(false,BODY_STATE_STAND,1.1f*m_fMinSpeed);
		}
		else
			if (fDistance <= m_fMinPatrolDistance) {
				SET_LOOK_FIRE_MOVEMENT(false,BODY_STATE_STAND,.9f*m_fMinSpeed);
			}
			else {
				SET_LOOK_FIRE_MOVEMENT(false,BODY_STATE_STAND,((fDistance - (m_fMaxPatrolDistance + m_fMinPatrolDistance)*.5f)/((m_fMaxPatrolDistance - m_fMinPatrolDistance)*.5f)*.1f + m_fMinPatrolDistance)*m_fMinSpeed);
			}
	}
	else {
		SET_LOOK_FIRE_MOVEMENT(false,BODY_STATE_STAND,.9f*m_fMinSpeed);
	}
}

void CAI_Zombie::Patrol()
{
	WRITE_TO_LOG("Patrol detour...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)
	
	SelectEnemy(Enemy);
	
	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiZombieAttackFire)
	
	DWORD dwCurTime = Level().timeServer();

	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiZombiePatrolHurt)
	
	CHECK_IF_SWITCH_TO_NEW_STATE(dwCurTime - dwSenseTime < SENSE_JUMP_TIME,aiZombieSenseSomething)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiZombiePatrolUnderFire)
	
	CHECK_IF_SWITCH_TO_NEW_STATE(m_bStateChanged,aiZombiePatrolReturnToRoute)
	
	if ((!(AI_Path.fSpeed)) || (AI_Path.TravelPath.empty()) || (AI_Path.TravelPath[AI_Path.TravelStart].P.distance_to(AI_Path.TravelPath[AI_Path.TravelPath.size() - 1].P) <= .5f)) {
		
		AI_Path.TravelPath.clear();
		AI_Path.TravelStart = 0;

		m_dwLastRangeSearch = 0;
		
		if (m_bStateChanged)
			m_dwLoopCount = 0;
		
		for (int i=0; i<Group.Members.size(); i++)
			if (((CCustomMonster*)(Group.Members[i]))->AI_Path.fSpeed > EPS_L)
				return;

		if (!m_bLooped) {
			Fvector tTemp1 = m_tpaPatrolPoints[m_tpaPatrolPoints.size() - 2];
			tTemp1.sub(m_tpaPatrolPoints[m_tpaPatrolPoints.size() - 1]);
			vfNormalizeSafe(tTemp1);
			SRotation sRot;
			mk_rotation(tTemp1,sRot);
			if (!((fabsf(r_torso_target.yaw - sRot.yaw) < PI_DIV_6) || ((fabsf(fabsf(r_torso_target.yaw - sRot.yaw) - PI_MUL_2) < PI_DIV_6)))) {
				AI_Path.TravelStart = AI_Path.TravelPath.size() - 1;
				r_torso_target.yaw = sRot.yaw > PI ? sRot.yaw - PI_MUL_2 : sRot.yaw;
				r_spine_target.yaw = r_torso_target.yaw; 
				r_target.yaw = 0;//r_torso_target.yaw; 
				SWITCH_TO_NEW_STATE(aiZombieTurnOver);
			}
		}

		m_dwLastRangeSearch = dwCurTime;
		
		if ((m_bLooped) || (m_dwLoopCount % 2 == 0)) {
			vector<CLevel::SPatrolPath> &tpaPatrolPaths = Level().tpaPatrolPaths;
			m_dwStartPatrolNode = tpaPatrolPaths[m_dwPatrolPathIndex].dwStartNode;
			vfCreatePointSequence(tpaPatrolPaths[m_dwPatrolPathIndex],m_tpaPatrolPoints,m_bLooped);
			m_tpaPointDeviations.resize(m_tpaPatrolPoints.size());
		}
		
		vfCreateFastRealisticPath(m_tpaPatrolPoints, m_dwStartPatrolNode, m_tpaPointDeviations, AI_Path.TravelPath, m_bLooped);
		
		if (AI_Path.TravelPath.size()) {
			if (!m_bLooped) {
				m_dwLoopCount++;
				if (m_dwLoopCount % 2 == 0) {
					DWORD dwCount = AI_Path.TravelPath.size();
					for (int i=0; i<dwCount / 2; i++) {
						Fvector tTemp = AI_Path.TravelPath[i].P;
						AI_Path.TravelPath[i].P = AI_Path.TravelPath[dwCount - i - 1].P;
						AI_Path.TravelPath[dwCount - i - 1].P = tTemp;
					}
				}
			}
			m_dwLastRangeSearch = Level().timeServer();
		}
	}
	
	SET_LOOK_FIRE_MOVEMENT(false,BODY_STATE_STAND,m_fMinSpeed)
}

void CAI_Zombie::PatrolReturn()
{
	WRITE_TO_LOG("Patrol return to route...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)
	
	SelectEnemy(Enemy);
	
	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiZombieAttackFire)
	
	DWORD dwCurTime = Level().timeServer();

	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiZombiePatrolHurt)
	
	CHECK_IF_SWITCH_TO_NEW_STATE(dwCurTime - dwSenseTime < SENSE_JUMP_TIME,aiZombieSenseSomething)
	
	INIT_SQUAD_AND_LEADER;
	
	CGroup &Group = Squad.Groups[g_Group()];
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiZombiePatrolUnderFire)
	
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

	SET_LOOK_FIRE_MOVEMENT(false,BODY_STATE_STAND,Leader != this ? m_fMaxSpeed : m_fMinSpeed)
}

void CAI_Zombie::Pursuit()
{
	WRITE_TO_LOG("Pursuiting");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)

	SelectEnemy(Enemy);
	
	CHECK_IF_GO_TO_NEW_STATE(Enemy.Enemy,aiZombieAttackFire)

	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_GO_TO_PREV_STATE(dwCurTime - dwLostEnemyTime > LOST_ENEMY_REACTION_TIME)
				
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiZombiePatrolHurt)
	
	CHECK_IF_SWITCH_TO_NEW_STATE(dwCurTime - dwSenseTime < SENSE_JUMP_TIME,aiZombieSenseSomething)
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiZombieUnderFire)

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

	if (_min(AI_Node->cover[0],_min(AI_Node->cover[1],_min(AI_Node->cover[2],AI_Node->cover[3]))) > MIN_COVER_MOVE)
		vfSetMovementType(BODY_STATE_STAND,m_fMaxSpeed);
	else
		vfSetMovementType(BODY_STATE_CROUCH,m_fMinSpeed);
}

void CAI_Zombie::Retreat()
{
	WRITE_TO_LOG("Retreating...");
}

void CAI_Zombie::SenseSomething()
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

void CAI_Zombie::UnderFire()
{
	WRITE_TO_LOG("Under fire...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)

	SelectEnemy(Enemy);

	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiZombieAttackFire)

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
	
	vfSetMovementType(BODY_STATE_STAND,m_fMaxSpeed);
}

#ifdef TEST_ACTIONS
void CAI_Zombie::TestMicroActions()
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

void CAI_Zombie::TestMicroActionA()
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

void CAI_Zombie::TestMicroActionD()
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

void CAI_Zombie::TestMicroActionQ()
{
	WRITE_TO_LOG("left torso + head turn");
	
	SUB_ANGLE(r_spine_target.yaw,PI_DIV_4);
	SUB_ANGLE(r_target.yaw,PI_DIV_2 - PI_DIV_4);
	ADJUST_BONE_ANGLES;
		
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Zombie::TestMicroActionE()
{
	WRITE_TO_LOG("right torso + head turn");
	
	ADD_ANGLE(r_spine_target.yaw,PI_DIV_4);
	ADD_ANGLE(r_target.yaw,PI_DIV_2 - PI_DIV_4);
	ADJUST_BONE_ANGLES;
	
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Zombie::TestMicroActionZ()
{
	WRITE_TO_LOG("left head turn");
	
	SUB_ANGLE(r_target.yaw,PI_DIV_2);
	ADJUST_BONE_ANGLES;
	
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Zombie::TestMicroActionC()
{
	WRITE_TO_LOG("right head turn");
	
	ADD_ANGLE(r_target.yaw,PI_DIV_2);
	ADJUST_BONE_ANGLES;
	
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Zombie::TestMicroActionW()
{
	WRITE_TO_LOG("stand up");
	
	if (m_cBodyState != BODY_STATE_STAND)
		eCurrentState = aiZombieStandingUp;
	else {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
	}

	return;
}

void CAI_Zombie::TestMicroActionS()
{
	WRITE_TO_LOG("sit down");
	
	if (m_cBodyState != BODY_STATE_CROUCH)
		eCurrentState = aiZombieSitting;
	else {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
	}

	return;
}

void CAI_Zombie::TestMicroActionX()
{
	WRITE_TO_LOG("lie down");
	
	if (m_cBodyState != BODY_STATE_LIE)
		eCurrentState = aiZombieLyingDown;
	else {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
	}

	return;
}

void CAI_Zombie::TestMicroActionR()
{
	WRITE_TO_LOG("recharge");
	
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	
	return;
}

void CAI_Zombie::TestMicroActionF()
{
	WRITE_TO_LOG("fire one shot");
	
	eCurrentState = tStateStack.top();
	tStateStack.pop();
				
	return;
}

void CAI_Zombie::TestMicroActionV()
{
	WRITE_TO_LOG("aim 0");
	
	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
			//m_tpCurrentTorsoAnimation = tZombieAnimations.tNormal.tTorso.tpaAim[0];
			break;
		}
	}
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	
	return;
}

void CAI_Zombie::TestMicroActionT()
{
	WRITE_TO_LOG("aim 1");
	
	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
			//m_tpCurrentTorsoAnimation = tZombieAnimations.tNormal.tTorso.tpaAim[1];
			break;
		}
	}
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	
	return;
}

void CAI_Zombie::TestMicroActionG()
{
	WRITE_TO_LOG("attack 0");
	
	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
			//m_tpCurrentTorsoAnimation = tZombieAnimations.tNormal.tTorso.tpaAttack[0];
			break;
		}
	}
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	
	return;
}

void CAI_Zombie::TestMicroActionB()
{
	WRITE_TO_LOG("attack 1");
	
	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
			//m_tpCurrentTorsoAnimation = tZombieAnimations.tNormal.tTorso.tpaAttack[1];
			break;
		}
	}
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	
	return;
}

void CAI_Zombie::TestMicroActionY()
{
	WRITE_TO_LOG("idle 0");
	
	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
			//m_tpCurrentTorsoAnimation = tZombieAnimations.tNormal.tTorso.tpaIdle[0];
			break;
		}
	}
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	
	return;
}

void CAI_Zombie::TestMicroActionH()
{
	WRITE_TO_LOG("idle 1");
	
	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
			//m_tpCurrentTorsoAnimation = tZombieAnimations.tNormal.tTorso.tpaIdle[1];
			break;
		}
	}
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	
	return;
}

void CAI_Zombie::TestMicroActionN()
{
	WRITE_TO_LOG("jump");
	
	eCurrentState = aiZombieJumping;
	
	return;
}

void CAI_Zombie::TestMicroActionU()
{
	WRITE_TO_LOG("lie down");
	
	vfSetMovementType(BODY_STATE_LIE,0);
	eCurrentState = aiZombieLyingDown;
	
	return;
}

void CAI_Zombie::TestMicroActionJ()
{
	WRITE_TO_LOG("lie down");
	
	vfSetMovementType(BODY_STATE_LIE,0);
	eCurrentState = aiZombieLyingDown;
	
	return;
}

void CAI_Zombie::TestMicroActionM()
{
	WRITE_TO_LOG("lie down");
	
	vfSetMovementType(BODY_STATE_LIE,0);
	eCurrentState = aiZombieLyingDown;
	
	return;
}

void CAI_Zombie::TestMacroActions()
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

void CAI_Zombie::TestMacroActionA()
{
	WRITE_TO_LOG("look and turn");
	
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Zombie::TestMacroActionD()
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

void CAI_Zombie::TestMacroActionQ()
{
	WRITE_TO_LOG("left torso + head turn");
	
	SUB_ANGLE(r_spine_target.yaw,PI_DIV_4);
	SUB_ANGLE(r_target.yaw,PI_DIV_2 - PI_DIV_4);
	ADJUST_BONE_ANGLES;
		
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Zombie::TestMacroActionE()
{
	WRITE_TO_LOG("right torso + head turn");
	
	ADD_ANGLE(r_spine_target.yaw,PI_DIV_4);
	ADD_ANGLE(r_target.yaw,PI_DIV_2 - PI_DIV_4);
	ADJUST_BONE_ANGLES;
	
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Zombie::TestMacroActionZ()
{
	WRITE_TO_LOG("left head turn");
	
	SUB_ANGLE(r_target.yaw,PI_DIV_2);
	ADJUST_BONE_ANGLES;
	
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Zombie::TestMacroActionC()
{
	WRITE_TO_LOG("right head turn");
	
	ADD_ANGLE(r_target.yaw,PI_DIV_2);
	ADJUST_BONE_ANGLES;
	
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	return;
}

void CAI_Zombie::TestMacroActionW()
{
	WRITE_TO_LOG("stand up");
	
	if (m_cBodyState != BODY_STATE_STAND)
		eCurrentState = aiZombieStandingUp;
	else {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
	}

	return;
}

void CAI_Zombie::TestMacroActionS()
{
	WRITE_TO_LOG("sit down");
	
	if (m_cBodyState != BODY_STATE_CROUCH)
		eCurrentState = aiZombieSitting;
	else {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
	}

	return;
}

void CAI_Zombie::TestMacroActionX()
{
	WRITE_TO_LOG("lie down");
	
	if (m_cBodyState != BODY_STATE_LIE)
		eCurrentState = aiZombieLyingDown;
	else {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
	}

	return;
}

void CAI_Zombie::TestMacroActionR()
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

void CAI_Zombie::TestMacroActionF()
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

void CAI_Zombie::TestMacroActionV()
{
	WRITE_TO_LOG("aim 0");
	
	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
			//m_tpCurrentTorsoAnimation = tZombieAnimations.tNormal.tTorso.tpaAim[0];
			break;
		}
	}
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	
	return;
}

void CAI_Zombie::TestMacroActionT()
{
	WRITE_TO_LOG("aim 1");
	
	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
			//m_tpCurrentTorsoAnimation = tZombieAnimations.tNormal.tTorso.tpaAim[1];
			break;
		}
	}
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	
	return;
}

void CAI_Zombie::TestMacroActionG()
{
	WRITE_TO_LOG("attack 0");
	
	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
			//m_tpCurrentTorsoAnimation = tZombieAnimations.tNormal.tTorso.tpaAttack[0];
			break;
		}
	}
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	
	return;
}

void CAI_Zombie::TestMacroActionB()
{
	WRITE_TO_LOG("attack 1");
	
	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
			//m_tpCurrentTorsoAnimation = tZombieAnimations.tNormal.tTorso.tpaAttack[1];
			break;
		}
	}
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	
	return;
}

void CAI_Zombie::TestMacroActionY()
{
	WRITE_TO_LOG("idle 0");
	
	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
//			m_tpCurrentTorsoAnimation = tZombieAnimations.tNormal.tTorso.tpaIdle[0];
			break;
		}
	}
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	
	return;
}

void CAI_Zombie::TestMacroActionH()
{
	WRITE_TO_LOG("idle 1");
	
	switch (m_cBodyState) {
		case BODY_STATE_STAND : {
//			m_tpCurrentTorsoAnimation = tZombieAnimations.tNormal.tTorso.tpaIdle[1];
			break;
		}
	}
	eCurrentState = tStateStack.top();
	tStateStack.pop();
	
	return;
}

void CAI_Zombie::TestMacroActionN()
{
	WRITE_TO_LOG("jump");
	
	eCurrentState = aiZombieJumping;
	
	return;
}

void CAI_Zombie::TestMacroActionU()
{
	WRITE_TO_LOG("lie down");
	
	vfSetMovementType(BODY_STATE_LIE,0);
	eCurrentState = aiZombieLyingDown;
	
	return;
}

void CAI_Zombie::TestMacroActionJ()
{
	WRITE_TO_LOG("lie down");
	
	vfSetMovementType(BODY_STATE_LIE,0);
	eCurrentState = aiZombieLyingDown;
	
	return;
}

void CAI_Zombie::TestMacroActionM()
{
	WRITE_TO_LOG("lie down");
	
	vfSetMovementType(BODY_STATE_LIE,0);
	eCurrentState = aiZombieLyingDown;
	
	return;
}
#endif

void CAI_Zombie::TurnOver()
{
	WRITE_TO_LOG("Turning over...");

	CHECK_IF_SWITCH_TO_NEW_STATE(g_Health() <= 0,aiZombieDie)
		
	SelectEnemy(Enemy);
	
	CHECK_IF_SWITCH_TO_NEW_STATE(Enemy.Enemy,aiZombieAttackFire)
	
	DWORD dwCurTime = Level().timeServer();
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime),aiZombieUnderFire)
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - dwSenseTime < SENSE_JUMP_TIME) && (dwSenseTime),aiZombieSenseSomething)
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	CHECK_IF_SWITCH_TO_NEW_STATE((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime),aiZombieUnderFire)

	if ((fabsf(r_torso_target.yaw - r_torso_current.yaw) < PI_DIV_6) || ((fabsf(fabsf(r_torso_target.yaw - r_torso_current.yaw) - PI_MUL_2) < PI_DIV_6))) {
		m_ePreviousState = tStateStack.top();
		GO_TO_PREV_STATE
	}

	if (!AI_Path.TravelPath.empty())
		AI_Path.TravelStart = AI_Path.TravelPath.size() - 1;
	else
		AI_Path.TravelStart = 0;
	
	vfSetMovementType(m_cBodyState,0);

	r_torso_speed = PI_DIV_4/1;
	r_spine_speed = PI_DIV_4/1;
	q_look.o_look_speed = PI_DIV_4/1;
}

void CAI_Zombie::Think()
{
	bStopThinking = false;
	do {
		m_ePreviousState = eCurrentState;
		switch(eCurrentState) {
			case aiZombieAttackRun : {
				AttackRun();
				break;
			}
			case aiZombieAttackFire : {
				AttackFire();
				break;
			}
			case aiZombieDefend : {
				Defend();
				break;
			}
			case aiZombieDie : {
				Die();
				break;
			}
			case aiZombieFindEnemy : {
				FindEnemy();
				break;
			}
			case aiZombieFollowLeader : {
				FollowLeader();
				break;
			}
			case aiZombieFreeHunting : {
				FreeHunting();
				break;
			}
			case aiZombieInjuring : {
				Injuring();
				break;
			}
			case aiZombieJumping : {
				Jumping();
				break;
			}
			case aiZombieStandingUp : {
				StandingUp();
				break;
			}
			case aiZombieSitting : {
				Sitting();
				break;
			}
			case aiZombieLyingDown : {
				LyingDown();
				break;
			}
			case aiZombieMoreDeadThanAlive : {
				MoreDeadThanAlive();
				break;
			}
			case aiZombiePatrolRoute : {
				Patrol();
				break;
			}
			case aiZombiePatrolReturnToRoute : {
				PatrolReturn();
				break;
			}
			case aiZombieFollowLeaderPatrol : {
				FollowLeaderPatrol();
				break;
			}
			case aiZombiePatrolUnderFire : {
				PatrolUnderFire();
				break;
			}
			case aiZombiePatrolHurt : {
				PatrolHurt();
				break;
			}
			case aiZombiePatrolHurtAggressiveUnderFire : {
				PatrolHurtAggressiveUnderFire();
				break;
			}
			case aiZombiePatrolHurtNonAggressiveUnderFire : {
				PatrolHurtAggressiveUnderFire();
				break;
			}
			case aiZombiePursuit : {
				Pursuit();
				break;
			}
			case aiZombieRetreat : {
				Retreat();
				break;
			}
			case aiZombieSenseSomething : {
				SenseSomething();
				break;
			}
			case aiZombieUnderFire : {
				UnderFire();
				break;
			}
			case aiZombieTurnOver : {
				TurnOver();
				break;
			}
			#ifdef TEST_ACTIONS
				case aiMonsterTestMicroActions : {
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
				case aiMonsterTestMacroActions : {
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
