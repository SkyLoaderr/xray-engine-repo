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

extern void	UnpackContour(PContour& C, DWORD ID);
extern void	IntersectContours(PSegment& Dest, PContour& C1, PContour& C2);

//******* 2D segments intersection
#define	LI_NONE				0
#define LI_COLLINEAR        0
#define	LI_INTERSECT		1
#define LI_EQUAL	        2

IC int lines_intersect( 
			float x1, float y1,		/* First line segment */
		    float x2, float y2,

		    float x3, float y3,		/* Second line segment */
		    float x4, float y4,

		    float *x, float *y      /* Output value: point of intersection */
               )
{
    float a1, a2, b1, b2, c1, c2;	/* Coefficients of line eqns. */
    float r1, r2, r3, r4;			/* 'Sign' values */
    float denom, num;				/* Intermediate values */
	
									/* Compute a1, b1, c1, where line joining points 1 and 2
									* is "a1 x  +  b1 y  +  c1  =  0".
	*/
	
    a1 = y2 - y1;
    b1 = x1 - x2;
    c1 = x2 * y1 - x1 * y2;
	
    /* Compute r3 and r4.
	*/
	
    r3 = a1 * x3 + b1 * y3 + c1;
    r4 = a1 * x4 + b1 * y4 + c1;
	
    /* Check signs of r3 and r4.  If both point 3 and point 4 lie on
	* same side of line 1, the line segments do not intersect.
	*/
	
    if ( r3*r4 > 0 )      return ( LI_NONE );
	
    /* Compute a2, b2, c2 */
	
    a2 = y4 - y3;
    b2 = x3 - x4;
    c2 = x4 * y3 - x3 * y4;
	
    /* Compute r1 and r2 */
	
    r1 = a2 * x1 + b2 * y1 + c2;
    r2 = a2 * x2 + b2 * y2 + c2;
	
    /* Check signs of r1 and r2.  If both point 1 and point 2 lie
	* on same side of second line segment, the line segments do
	* not intersect.
	*/
	
    if ( r1*r2 > 0 )		return ( LI_NONE );
	
	// Check for equality
	if ( fabsf(r1*r2)<EPS_S && fabsf(r3*r4)<EPS_S ) return LI_NONE;

    /* Line segments intersect: compute intersection point. 
	*/
	
    denom = a1 * b2 - a2 * b1;
    if ( fabsf(denom) < EPS ) return ( LI_COLLINEAR );
	
    num = b1 * c2 - b2 * c1;
    *x = num / denom;
	
    num = a2 * c1 - a1 * c2;
    *y = num / denom;
	
    return ( LI_INTERSECT );
} /* lines_intersect */

IC float ffGetY(NodeCompressed &tNode, float X, float Z)
{
	Fvector	DUP, vNorm, v, v1, P0;
	DUP.set(0,1,0);
	pvDecompress(vNorm,tNode.plane);
	Fplane PL; 
	Level().AI.UnpackPosition(P0,tNode.p0);
	PL.build(P0,vNorm);
	v.set(X,P0.y,Z);	
	PL.intersectRayPoint(v,DUP,v1);	
	return(v1.y);
}

#define COMPUTE_DISTANCE_2D(t,p) (sqrtf(SQR((t).x - (p).x) + SQR((t).z - (p).z)))

void CAI_Soldier::vfCreateStraightForwardPath(Fvector &tStartPoint, Fvector tFinishPoint, DWORD dwStartNode, DWORD dwFinishNode, vector<CTravelNode> &tpaPath)
{
	CTravelNode tTravelNode;
	Fvector tPrevPoint;
	NodeCompressed *tpNode;
	NodeLink *taLinks;
	PContour tCurContour, tNextContour;
	PSegment tSegment;
	int i, iCount, iNodeIndex;
	DWORD dwCurNode;
	float fDistance;
	CAI_Space &AI = Level().AI;

	tStartPoint.y = ffGetY(*(AI.Node(dwStartNode)),tStartPoint.x,tStartPoint.z);
	tFinishPoint.y = ffGetY(*(AI.Node(dwFinishNode)),tFinishPoint.x,tFinishPoint.z);

	fDistance = COMPUTE_DISTANCE_2D(tStartPoint,tFinishPoint);

	tpaPath.clear();
	tTravelNode.P = tStartPoint;
	tTravelNode.floating = FALSE;
	tpaPath.push_back(tTravelNode);

	dwCurNode = dwStartNode;
	tPrevPoint = tStartPoint;
	do {
		UnpackContour(tCurContour,dwCurNode);
		tpNode = AI.Node(dwCurNode);
		taLinks = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
		iCount = tpNode->link_count;
		for ( i=0; i < iCount; i++) {
			iNodeIndex = AI.UnpackLink(taLinks[i]);
			UnpackContour(tNextContour,iNodeIndex);
			IntersectContours(tSegment,tCurContour,tNextContour);
			if ((lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tSegment.v1.x,tSegment.v1.z,tSegment.v2.x,tSegment.v2.z,&tTravelNode.P.x,&tTravelNode.P.z)) &&	(fabsf(COMPUTE_DISTANCE_2D(tStartPoint,tTravelNode.P) + COMPUTE_DISTANCE_2D(tFinishPoint,tTravelNode.P) - fDistance) < .1f) && (fabsf(tPrevPoint.x + tPrevPoint.z - tTravelNode.P.x- tTravelNode.P.z) > 0.1)) {
				tTravelNode.P.y = ffGetY(*(tpNode),tTravelNode.P.x,tTravelNode.P.z);
				tpaPath.push_back(tTravelNode);
				tPrevPoint = tTravelNode.P;
				dwCurNode = iNodeIndex;
				break;
			}
		}
		VERIFY(i<iCount);
	}
	while (dwCurNode != dwFinishNode);
	if (fabsf(tPrevPoint.x + tPrevPoint.z - tFinishPoint.x- tFinishPoint.z) > 0.1) {
		tTravelNode.P = tFinishPoint;
		tpaPath.push_back(tTravelNode);
	}
}

void CAI_Soldier::vfCreateRealisticPath(vector<Fvector> &tpaPoints, vector<DWORD> &dwaNodes, vector<CTravelNode> &tpaPath)
{
	CTravelNode tTravelNode;
	Fvector tPrevPoint, tStartPoint, tFinishPoint;
	NodeCompressed *tpNode;
	NodeLink *taLinks;
	PContour tCurContour, tNextContour;
	PSegment tSegment;
	int i, iCurrentPatrolPoint, iCount, iNodeIndex;
	DWORD dwCurNode, dwStartNode, dwFinishNode;
	float fDistance;
	CAI_Space &AI = Level().AI;

	tpaPath.clear();
	for ( iCurrentPatrolPoint=0; iCurrentPatrolPoint < tpaPoints.size(); iCurrentPatrolPoint++) {
		// init points
		tStartPoint = tpaPoints[iCurrentPatrolPoint];
		tFinishPoint = tpaPoints[iCurrentPatrolPoint < tpaPoints.size() - 1 ? iCurrentPatrolPoint + 1 : 0];
		dwStartNode = dwaNodes[iCurrentPatrolPoint];
		dwFinishNode = dwaNodes[iCurrentPatrolPoint < dwaNodes.size() - 1 ? iCurrentPatrolPoint + 1 : 0];
		
		// correct y coordinates
		tStartPoint.y = ffGetY(*(AI.Node(dwStartNode)),tStartPoint.x,tStartPoint.z);
		tFinishPoint.y = ffGetY(*(AI.Node(dwFinishNode)),tFinishPoint.x,tFinishPoint.z);

		// compute distance
		fDistance = COMPUTE_DISTANCE_2D(tStartPoint,tFinishPoint);

		if (!iCurrentPatrolPoint) {
			tTravelNode.P = tStartPoint;
			tTravelNode.floating = FALSE;
			tpaPath.push_back(tTravelNode);
		}

		dwCurNode = dwaNodes[iCurrentPatrolPoint];
		tPrevPoint = tStartPoint;
		do {
			UnpackContour(tCurContour,dwCurNode);
			tpNode = AI.Node(dwCurNode);
			taLinks = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
			iCount = tpNode->link_count;
			for ( i=0; i < iCount; i++) {
				iNodeIndex = AI.UnpackLink(taLinks[i]);
				UnpackContour(tNextContour,iNodeIndex);
				IntersectContours(tSegment,tCurContour,tNextContour);
				if ((lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tSegment.v1.x,tSegment.v1.z,tSegment.v2.x,tSegment.v2.z,&tTravelNode.P.x,&tTravelNode.P.z)) &&	(fabsf(COMPUTE_DISTANCE_2D(tStartPoint,tTravelNode.P) + COMPUTE_DISTANCE_2D(tFinishPoint,tTravelNode.P) - fDistance) < .1f) && (fabsf(tPrevPoint.x + tPrevPoint.z - tTravelNode.P.x- tTravelNode.P.z) > 0.1)) {
					tTravelNode.P.y = ffGetY(*(tpNode),tTravelNode.P.x,tTravelNode.P.z);
					tpaPath.push_back(tTravelNode);
					tPrevPoint = tTravelNode.P;
					dwCurNode = iNodeIndex;
					break;
				}
			}
			VERIFY(i<iCount);
		}
		while (dwCurNode != dwFinishNode);

		if (fabsf(tPrevPoint.x + tPrevPoint.z - tFinishPoint.x- tFinishPoint.z) > 0.1) {
			tTravelNode.P = tFinishPoint;
			tpaPath.push_back(tTravelNode);
		}
	}
}

void CAI_Soldier::vfComputeCircle(Fvector tPosition, Fvector tPoint0, Fvector tPoint1, float &fRadius, Fvector &tCircleCentre)
{
	Fvector tP0, tP1;
	float fAlpha, fSinus, fCosinus, fRx;

	tP0.sub(tPosition,tPoint0);
	tP1.sub(tPoint1,tPoint0);
	fRx = tP0.magnitude();
	tP0.normalize();
	tP1.normalize();

	fAlpha = .5f*acosf(tP0.dotproduct(tP1));

	tP0.mul(fRx);
	tP1.mul(fRx);
	tCircleCentre.add(tP0,tP1);
	_sincos(fAlpha,fSinus,fCosinus);
	fRadius = fRx*fSinus/fCosinus;
	fRx = fRadius*(1 - fSinus)/fSinus;
	tCircleCentre.mul((fRadius + fRx)/tCircleCentre.magnitude());
	tP0.sub(tCircleCentre);
	tP1.sub(tCircleCentre);

	float fTemp0 = tP0.magnitude(), fTemp1 = tP1.magnitude();

	tCircleCentre.add(tPoint0);
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
	if (tPoint1.magnitude() > 15.f)
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
		AI_Path.DestNode = dwaPatrolNodes[m_iCurrentPoint];
		Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path,0,0);
		if (AI_Path.Nodes.size() > 1) {
			AI_Path.BuildTravelLine(Position());
			CTravelNode tTemp;
			tTemp.floating = FALSE;
			tTemp.P = AI_Path.TravelPath[AI_Path.TravelPath.size() - 1].P;
			//if (fabsf(tTemp.P.x + tTemp.P.z - tpaPatrolPoints[m_iCurrentPoint].x - tpaPatrolPoints[m_iCurrentPoint].z) > 0.1)
			//	AI_Path.TravelPath.push_back(tTemp);
		}
		else {
			AI_Path.TravelPath.clear();
			AI_Path.bNeedRebuild = FALSE;
		}
	}
	else
		if (!(AI_Path.fSpeed)) {
			
			m_iCurrentPoint = m_iCurrentPoint == dwaPatrolNodes.size() - 1 ? 0 : m_iCurrentPoint + 1;
			
			/**
			for (int i=0, iCount = 0; i<tpaPatrolPathes.size(); i++)
				iCount += tpaPatrolPathes[i].size();
			
			AI_Path.TravelPath.resize(iCount);

			for ( i=0, iCount = 0; i<tpaPatrolPathes.size(); i++) {
				for (int j=0; j<tpaPatrolPathes[i].size(); j++)
					AI_Path.TravelPath[iCount + j] = tpaPatrolPathes[i][j];
				iCount += tpaPatrolPathes[i].size();
			}

			AI_Path.TravelStart = 0;
			/**/
			AI_Path.TravelPath.resize(tpaPatrolPath.size());

			for (int i=0; i<tpaPatrolPath.size(); i++)
				AI_Path.TravelPath[i] = tpaPatrolPath[i];

			AI_Path.TravelStart = 0;
		}

	vfSetFire(false,Group);

	vfSetMovementType(false,m_fMinSpeed);
	
	bStopThinking = true;
}