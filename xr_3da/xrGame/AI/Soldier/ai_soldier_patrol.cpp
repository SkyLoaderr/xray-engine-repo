////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier_patrol.cpp
//	Created 	: 04.06.2002
//  Modified 	: 04.06.2002
//	Author		: Dmitriy Iassenev
//	Description : AI patrol behaviour for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_soldier.h"
#include "ai_soldier_selectors.h"

#define TORSO_ANGLE_DELTA				(PI/30.f)

void CAI_Soldier::FollowLeaderPatrol()
{
	// if no more health then soldier is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Following leader patrol");
#endif
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
		tHitDir = Group.m_tLastHitDirection;
		dwHitTime = Group.m_dwLastHitTime;
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierUnderFire;
		m_dwLastRangeSearch = 0;
		return;
	}

	if (Leader == this) {
		eCurrentState = aiSoldierPatrolDetour;
		return;
	}

	SetLessCoverLook(AI_Node);
	
	if (AI_Path.bNeedRebuild) {
		Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path);
		if (AI_Path.Nodes.size() > 1)
			AI_Path.BuildTravelLine(Position());
		else {
			AI_Path.TravelPath.clear();
			AI_Path.bNeedRebuild = FALSE;
		}
	}
	else {
		if (AI_NodeID == tpaPatrolPoints[m_iCurrentPoint]) {
			int iSavePoint = m_iCurrentPoint;
			m_iCurrentPoint = m_iCurrentPoint == tpaPatrolPoints.size() - 1 ? 0 : m_iCurrentPoint + 1;
			
			Fvector tTemp0, tTemp1;
			Level().AI.UnpackPosition(tTemp0,Level().AI.Node(tpaPatrolPoints[m_iCurrentPoint])->p0);
			Level().AI.UnpackPosition(tTemp1,Level().AI.Node(tpaPatrolPoints[m_iCurrentPoint])->p1);
			tTemp1.average(tTemp0);
			tTemp1.sub(vPosition);
			
			mk_rotation(tTemp1,r_torso_target);

			if (fabsf(r_torso_current.yaw - r_torso_target.yaw) < TORSO_ANGLE_DELTA) {
				AI_Path.bNeedRebuild = TRUE;
				AI_Path.DestNode = tpaPatrolPoints[m_iCurrentPoint];
			}
			else {
				m_iCurrentPoint = iSavePoint;
				//r_torso_speed = PI/10;
			}
		}
		else {
			if (AI_Path.DestNode != tpaPatrolPoints[m_iCurrentPoint])
				AI_Path.bNeedRebuild = TRUE;

			AI_Path.DestNode = tpaPatrolPoints[m_iCurrentPoint];

			if (!bfCheckPath(AI_Path))
				AI_Path.bNeedRebuild = TRUE;

		}
	}

	vfSetFire(false,Group);

	vfSetMovementType(false,m_fMinSpeed);
	// stop processing more rules
	bStopThinking = true;
}

void CAI_Soldier::Patrol()
{
	// if no more health then soldier is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Patrol detour");
#endif
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
		tHitDir = Group.m_tLastHitDirection;
		dwHitTime = Group.m_dwLastHitTime;
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierUnderFire;
		m_dwLastRangeSearch = 0;
		return;
	}

	if (AI_Path.bNeedRebuild) {
		Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path);
		if (AI_Path.Nodes.size() > 1)
			AI_Path.BuildTravelLine(Position());
		else {
			AI_Path.TravelPath.clear();
			AI_Path.bNeedRebuild = FALSE;
		}
		SetLessCoverLook(AI_Node);
	}
	else {
		if (AI_NodeID == tpaPatrolPoints[m_iCurrentPoint]) {
			int iSavePoint = m_iCurrentPoint;
			m_iCurrentPoint = m_iCurrentPoint == tpaPatrolPoints.size() - 1 ? 0 : m_iCurrentPoint + 1;
			
			Fvector tTemp0, tTemp1;
			Level().AI.UnpackPosition(tTemp0,Level().AI.Node(tpaPatrolPoints[m_iCurrentPoint])->p0);
			Level().AI.UnpackPosition(tTemp1,Level().AI.Node(tpaPatrolPoints[m_iCurrentPoint])->p1);
			tTemp1.average(tTemp0);
			tTemp1.sub(vPosition);
			
			mk_rotation(tTemp1,r_torso_target);

			if (fabsf(r_torso_current.yaw - r_torso_target.yaw) < TORSO_ANGLE_DELTA) {
				AI_Path.bNeedRebuild = TRUE;
				AI_Path.DestNode = tpaPatrolPoints[m_iCurrentPoint];
				SetLessCoverLook(AI_Node);
			}
			else {
				m_iCurrentPoint = iSavePoint;
				//r_torso_speed = PI/10;
			}
		}
		else {
			if (AI_Path.DestNode != tpaPatrolPoints[m_iCurrentPoint])
				AI_Path.bNeedRebuild = TRUE;

			AI_Path.DestNode = tpaPatrolPoints[m_iCurrentPoint];

			if (!bfCheckPath(AI_Path))
				AI_Path.bNeedRebuild = TRUE;
			SetLessCoverLook(AI_Node);
		}
	}

	vfSetFire(false,Group);

	vfSetMovementType(false,m_fMinSpeed);
	
	bStopThinking = true;
}