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

#define NEXT_POINT(m_iCurrentPoint) (m_iCurrentPoint) == tpaPatrolPoints.size() - 1 ? 0 : (m_iCurrentPoint) + 1
#define PREV_POINT(m_iCurrentPoint) (m_iCurrentPoint) == 0 ? tpaPatrolPoints.size() - 1 : (m_iCurrentPoint) - 1

void CAI_Soldier::vfComputeCircle(DWORD dwNode0, DWORD dwNode1, float &fRadius, Fvector &tPosition)
{
	Fvector tTemp0, tTemp1, tPoint0, tPoint1, tPoint2, tPoint3;
	
	// initializing current patrol point
	Level().AI.UnpackPosition(tTemp0,Level().AI.Node(dwNode0)->p0);
	Level().AI.UnpackPosition(tTemp1,Level().AI.Node(dwNode0)->p1);
	tPoint0.average(tTemp0,tTemp1);

	// initializing the next patrol point
	Level().AI.UnpackPosition(tTemp0,Level().AI.Node(dwNode1)->p0);
	Level().AI.UnpackPosition(tTemp1,Level().AI.Node(dwNode1)->p1);
	tPoint1.average(tTemp0,tTemp1);

	// making that vector the same length
	tTemp0 = vPosition;
	tTemp0.sub(tPoint0);

	tPoint1.sub(tPoint0);
	tPoint1.normalize();
	tPoint1.mul(tTemp0.magnitude());
	tPoint1.add(tPoint0);

	tPoint2 = tTemp0;
	tPoint3 = tPoint1;
	
	tPoint2.normalize();
	tPoint3.normalize();

	float fAngle = acosf(tPoint3.dotproduct(tPoint2))*0.5f, fSinus, fCosinus, fRx;
	_sincos(fAngle,fSinus,fCosinus);
	
	fRadius = tTemp0.magnitude()*fSinus/fCosinus;
	fRx = fRadius*(1 - fSinus)/fSinus;
	
	/**
	Fvector2 tVector2D,tVector2D_0;
	tVector2D.set(-tTemp0.x,-tTemp0.z);
	tVector2D.mul(1/sqrt(SQR(tVector2D.x) + SQR(tVector2D.y)));
	tVector2D_0 = tVector2D;
	tVector2D_0.mul(-1);
	
	tVector2D.rot90();
	tVector2D.mul(fRadius);
	tVector2D.x += vPosition.x;
	tVector2D.y += vPosition.z;
	
	tVector2D_0.rot90();
	tVector2D_0.mul(fRadius);
	tVector2D_0.x += vPosition.x;
	tVector2D_0.y += vPosition.z;
	/**

	tTemp1 = tPoint1;
	tTemp1.sub(tPoint0);

	tPosition.add(tTemp0,tTemp1);
	tPosition.normalize();
	tPosition.mul(fRx + fRadius);
	tPosition.add(tPoint0);

	tTemp0.sub(vPosition,tPosition);
	tTemp1.sub(tPoint1,tPosition);

	bool bDummy = tTemp0.magnitude() == tTemp1.magnitude();
	/**/

	Fvector2 tVector2D,tVector2D_0;
	tVector2D.set(tTemp0.x,tTemp0.z);
	tVector2D.mul(fRadius/tTemp0.magnitude());
	tVector2D.rot90();
	tVector2D.x += vPosition.x;
	tVector2D.y += vPosition.z;
	
	tVector2D_0.set(-tTemp0.x,-tTemp0.z);
	tVector2D_0.mul(fRadius/tTemp0.magnitude());
	tVector2D_0.rot90();
	tVector2D_0.x += vPosition.x;
	tVector2D_0.y += vPosition.z;

	tVector2D.set(tTemp0.x,tTemp0.z);
	tVector2D.mul(fRadius/tTemp0.magnitude());
	tVector2D.rot90();
	tVector2D.rot90();
	tVector2D.x += vPosition.x;
	tVector2D.y += vPosition.z;
	
	tVector2D_0.set(-tTemp0.x,-tTemp0.z);
	tVector2D_0.mul(fRadius/tTemp0.magnitude());
	tVector2D_0.rot90();
	tVector2D_0.rot90();
	tVector2D_0.x += vPosition.x;
	tVector2D_0.y += vPosition.z;

	float f1 = (SQR(tVector2D.x - vPosition.x) + SQR(tVector2D.y - vPosition.z) + SQR(tVector2D.x - tPoint1.x) + SQR(tVector2D.y - tPoint1.z));
	float f2 = (SQR(tVector2D_0.x - vPosition.x) + SQR(tVector2D_0.y - vPosition.z) + SQR(tVector2D_0.x - tPoint1.x) + SQR(tVector2D_0.y - tPoint1.z));
	/**/
	if (f1 < f2)
		tPosition.set(tVector2D.x,vPosition.y,tVector2D.y);
	else
		tPosition.set(tVector2D_0.x,vPosition.y,tVector2D_0.y);
	/**/
	

	//fRadius += 10;
}

float CAI_Soldier::bfCheckForChange(DWORD dwNode0, DWORD dwNode1, float fSwitchAngle, float fSwitchDistance)
{
	Fvector tTemp0, tTemp1, tPoint0, tPoint1;
	
	Level().AI.UnpackPosition(tTemp0,Level().AI.Node(dwNode0)->p0);
	Level().AI.UnpackPosition(tTemp1,Level().AI.Node(dwNode0)->p1);
	tPoint0.average(tTemp0,tTemp1);
	tPoint0.sub(vPosition);
	
	Level().AI.UnpackPosition(tTemp0,Level().AI.Node(dwNode1)->p0);
	Level().AI.UnpackPosition(tTemp1,Level().AI.Node(dwNode1)->p1);
	tPoint1.average(tTemp0,tTemp1);
	tPoint1.sub(vPosition);

	//if (fabsf(tPoint0.magnitude() - tPoint1.magnitude()) > 5.f)
	if (tPoint1.magnitude() > 8.f)
		return(false);

	vfNormalizeSafe(tPoint0);
	vfNormalizeSafe(tPoint1);

	float fAngle = acosf(tPoint1.dotproduct(tPoint0));

	return(fAngle > fSwitchAngle);
}

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
	
	/**
	if (AI_Path.bNeedRebuild) {
		Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path,*(SelectorPatrol.m_tpEnemyNode),::Random.randF(1.5,2.5),0,0,100,5);
		if (AI_Path.Nodes.size() > 1)
			AI_Path.BuildTravelLine(Position());
		else {
			AI_Path.TravelPath.clear();
			AI_Path.bNeedRebuild = FALSE;
		}
	}
	else {
		if (bfCheckForChange(tpaPatrolPoints[PREV_POINT(m_iCurrentPoint)],tpaPatrolPoints[m_iCurrentPoint],2*PI/3.f,0.5f)) {
			int iSavePoint = m_iCurrentPoint;
			m_iCurrentPoint = NEXT_POINT(m_iCurrentPoint);
			
			Fvector tTemp0, tTemp1;
			Level().AI.UnpackPosition(tTemp0,Level().AI.Node(tpaPatrolPoints[m_iCurrentPoint])->p0);
			Level().AI.UnpackPosition(tTemp1,Level().AI.Node(tpaPatrolPoints[m_iCurrentPoint])->p1);
			tTemp1.average(tTemp0);
			tTemp1.sub(vPosition);
			
			if (fabsf(r_torso_current.yaw - r_torso_target.yaw) < TORSO_ANGLE_DELTA) {
				AI_Path.bNeedRebuild = TRUE;
				AI_Path.DestNode = tpaPatrolPoints[m_iCurrentPoint];
				SelectorPatrol.m_tpEnemyNode = Level().AI.Node(tpaPatrolPoints[PREV_POINT(m_iCurrentPoint)]);
			}
			else {
				m_iCurrentPoint = iSavePoint;
				//r_target.yaw = r_torso_current.yaw - PI/6.f;
			}
		}
		else {
			if (AI_Path.DestNode != tpaPatrolPoints[m_iCurrentPoint])
				AI_Path.bNeedRebuild = TRUE;

			AI_Path.DestNode = tpaPatrolPoints[m_iCurrentPoint];
			SelectorPatrol.m_tpEnemyNode = Level().AI.Node(tpaPatrolPoints[PREV_POINT(m_iCurrentPoint)]);

			//if (!bfCheckPath(AI_Path))
			//	AI_Path.bNeedRebuild = TRUE;
		}
	}

	/**/
	vfInitSelector(SelectorPatrol,Squad,Leader);

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else {
		SelectorPatrol.m_tEnemyPosition = Leader->Position();
		SelectorPatrol.m_tCurrentPosition = Position();
		m_dwLastRangeSearch = 0;
		vfSearchForBetterPositionWTime(SelectorPatrol,Squad,Leader);
	}
	/**/
	
	//SetLessCoverLook(AI_Node);

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

	SetLessCoverLook(AI_Node);

	if (AI_Path.bNeedRebuild) {
		Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path,SelectorPatrol.m_tEnemyPosition,SelectorPatrol.m_fRadius,0,0,100,4);
		//Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path,*(SelectorPatrol.m_tpEnemyNode),2,0,0,100,33);
		//Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path);
		if (AI_Path.Nodes.size() > 1)
			AI_Path.BuildTravelLine(Position());
		else {
			AI_Path.TravelPath.clear();
			AI_Path.bNeedRebuild = FALSE;
		}
	}
	else {
		if (bfCheckForChange(tpaPatrolPoints[PREV_POINT(m_iCurrentPoint)],tpaPatrolPoints[m_iCurrentPoint],2*PI/3.f,0.5f)) {
			/**/
			int iSavePoint = m_iCurrentPoint;
			m_iCurrentPoint = NEXT_POINT(m_iCurrentPoint);
			
			Fvector tTemp0, tTemp1;
			Level().AI.UnpackPosition(tTemp0,Level().AI.Node(tpaPatrolPoints[m_iCurrentPoint])->p0);
			Level().AI.UnpackPosition(tTemp1,Level().AI.Node(tpaPatrolPoints[m_iCurrentPoint])->p1);
			tTemp1.average(tTemp0);
			tTemp1.sub(vPosition);
			
			//mk_rotation(tTemp1,r_torso_target);

			if (fabsf(r_torso_current.yaw - r_torso_target.yaw) < TORSO_ANGLE_DELTA) {
				AI_Path.bNeedRebuild = TRUE;
				AI_Path.DestNode = tpaPatrolPoints[m_iCurrentPoint];
				//SelectorPatrol.m_tpEnemyNode = Level().AI.Node(tpaPatrolPoints[PREV_POINT(m_iCurrentPoint)]);
				vfComputeCircle(tpaPatrolPoints[PREV_POINT(m_iCurrentPoint)],tpaPatrolPoints[m_iCurrentPoint],SelectorPatrol.m_fRadius,SelectorPatrol.m_tEnemyPosition);
			}
			else {
				m_iCurrentPoint = iSavePoint;
				//r_target.yaw = r_torso_current.yaw - PI/6.f;
			}
			/**
			m_iCurrentPoint = m_iCurrentPoint == tpaPatrolPoints.size() - 1 ? 0 : m_iCurrentPoint + 1;
			AI_Path.bNeedRebuild = TRUE;
			AI_Path.DestNode = tpaPatrolPoints[m_iCurrentPoint];
			/**/
		}
		/**/
		else {
			if (AI_Path.DestNode != tpaPatrolPoints[m_iCurrentPoint])
				AI_Path.bNeedRebuild = TRUE;

			AI_Path.DestNode = tpaPatrolPoints[m_iCurrentPoint];
			//SelectorPatrol.m_tpEnemyNode = Level().AI.Node(tpaPatrolPoints[PREV_POINT(m_iCurrentPoint)]);
			vfComputeCircle(tpaPatrolPoints[PREV_POINT(m_iCurrentPoint)],tpaPatrolPoints[m_iCurrentPoint],SelectorPatrol.m_fRadius,SelectorPatrol.m_tEnemyPosition);

			//if (!bfCheckPath(AI_Path))
			//	AI_Path.bNeedRebuild = TRUE;
		}
		/**/
	}

	vfSetFire(false,Group);

	vfSetMovementType(false,m_fMinSpeed);
	
	bStopThinking = true;
}