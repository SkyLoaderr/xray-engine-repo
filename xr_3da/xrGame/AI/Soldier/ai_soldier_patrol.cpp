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

bool bfInsideNode(CAI_Space &AI, Fvector tPosition, DWORD dwNode)
{
	Fvector p0,p1;
	NodeCompressed *tpNode = AI.Node(dwNode);
	AI.UnpackPosition(p0,tpNode->p0);
	AI.UnpackPosition(p1,tpNode->p1);

	return((tPosition.x >= p0.x) && (tPosition.z >= p0.z) && (tPosition.x <= p1.x) && (tPosition.z <= p1.z));
}

void CAI_Soldier::vfCreateRealisticPath(vector<Fvector> &tpaPoints, vector<DWORD> &dwaNodes, vector<CTravelNode> &tpaPath, float fRoundedDistance, float fSegmentSize)
{
	CTravelNode tTravelNode;
	Fvector tPrevPoint, tStartPoint, tFinishPoint, tCircleCentre, tCurrentPosition, tFinalPosition;
	NodeCompressed *tpNode;
	NodeLink *taLinks;
	PContour tCurContour, tNextContour;
	PSegment tSegment;
	int i, iCurrentPatrolPoint, iCount, iNodeIndex;
	DWORD dwCurNode, dwStartNode, dwFinishNode;
	float fDistance, fRadius;
	bool bRounding = false, bFirstTime = false, bStopWorking = false;
	CAI_Space &AI = Level().AI;

	tpaPath.clear();
	do {
		iCurrentPatrolPoint=0;
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

		dwCurNode = dwStartNode;
		tPrevPoint = tStartPoint;
		do {
			if (!bRounding) {
				UnpackContour(tCurContour,dwCurNode);
				tpNode = AI.Node(dwCurNode);
				taLinks = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
				iCount = tpNode->link_count;
				for ( i=0; i < iCount; i++) {
					iNodeIndex = AI.UnpackLink(taLinks[i]);
					UnpackContour(tNextContour,iNodeIndex);
					IntersectContours(tSegment,tCurContour,tNextContour);
					if ((lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tSegment.v1.x,tSegment.v1.z,tSegment.v2.x,tSegment.v2.z,&tTravelNode.P.x,&tTravelNode.P.z)) &&	(fabsf(COMPUTE_DISTANCE_2D(tStartPoint,tTravelNode.P) + COMPUTE_DISTANCE_2D(tFinishPoint,tTravelNode.P) - fDistance) < EPS_L) && (fabsf(tPrevPoint.x + tPrevPoint.z - tTravelNode.P.x- tTravelNode.P.z) > EPS_L)) {
						if (bRounding) {
							if (COMPUTE_DISTANCE_2D(tFinishPoint,tTravelNode.P) > fRoundedDistance)
								bRounding = false;
						}
						else
							if (COMPUTE_DISTANCE_2D(tFinishPoint,tTravelNode.P) <= fRoundedDistance) {
								bRounding = true;
								bFirstTime = true;
							}

						if (!bRounding) {
							tTravelNode.P.y = ffGetY(*(tpNode),tTravelNode.P.x,tTravelNode.P.z);
							if ((fabsf(tTravelNode.P.y - tPrevPoint.y) > EPS_L) || (dwCurNode == dwStartNode))
								tpaPath.push_back(tTravelNode);
							else
								tpaPath[tpaPath.size() - 1].P = tTravelNode.P;
							tPrevPoint = tTravelNode.P;
							dwCurNode = iNodeIndex;
							break;
						}
						else
							break;
					}
				}
				R_ASSERT(i<iCount);
			}
			if (bRounding) {
				if (bFirstTime) {
					bFirstTime = false;
					tCurrentPosition.sub(tTravelNode.P,tPrevPoint);
					tCurrentPosition.normalize();
					tCurrentPosition.mul(COMPUTE_DISTANCE_2D(tFinishPoint,tPrevPoint) - fRoundedDistance);
					tCurrentPosition.add(tPrevPoint);
					tCurrentPosition.y = ffGetY(*(tpNode),tCurrentPosition.x,tCurrentPosition.z);
					tTravelNode.P = tCurrentPosition;
					tpaPath.push_back(tTravelNode);
					tPrevPoint = tCurrentPosition;
					vfComputeCircle(tCurrentPosition,tFinishPoint,tpaPoints[iCurrentPatrolPoint < tpaPoints.size() - 1 ? iCurrentPatrolPoint + 1 : 0],fRadius,tCircleCentre,tFinalPosition);
					dwFinishNode = DWORD(-1);
				}
				
				do {
					tCurrentPosition = tPrevPoint;
					tCurrentPosition.sub(tCircleCentre);
					tCurrentPosition.normalize();
					float fAlpha0 = acosf((fRadius*fRadius + fRadius*fRadius - fSegmentSize*fSegmentSize)/(2*fRadius*fRadius));
					float fAlpha = asinf(tCurrentPosition.x);
					_sincos(fAlpha + fAlpha0,tCurrentPosition.x,tCurrentPosition.z);
					if (COMPUTE_DISTANCE_2D(tFinalPosition,tPrevPoint) > COMPUTE_DISTANCE_2D(tCurrentPosition,tFinalPosition)) {
						float fTemp = fAlpha - fAlpha0;
						_sincos(fTemp,tCurrentPosition.x,tCurrentPosition.z);
					}
					
					// checking for stop to round
					Fvector t1,t2;
					t1.sub(tPrevPoint,tFinalPosition);
					t2.sub(tPrevPoint,tCurrentPosition);
					t1.normalize();
					t2.normalize();
					fAlpha = acosf(t1.dotproduct(t2));
					if (fAlpha < PI/2) {
						
						iCurrentPatrolPoint	= iCurrentPatrolPoint < tpaPoints.size() - 1 ? iCurrentPatrolPoint + 1 : 0;
						
						if (iCurrentPatrolPoint = 1) {
							bStopWorking = true;
							break;
						}
						
						bRounding = false;

						tStartPoint = tCurrentPosition;
						tFinishPoint = tpaPoints[iCurrentPatrolPoint < tpaPoints.size() - 1 ? iCurrentPatrolPoint + 1 : 0];
						dwStartNode = dwCurNode;
						dwFinishNode = dwaNodes[iCurrentPatrolPoint < dwaNodes.size() - 1 ? iCurrentPatrolPoint + 1 : 0];
						
						tStartPoint.y = ffGetY(*(AI.Node(dwStartNode)),tStartPoint.x,tStartPoint.z);
						tFinishPoint.y = ffGetY(*(AI.Node(dwFinishNode)),tFinishPoint.x,tFinishPoint.z);

						fDistance = COMPUTE_DISTANCE_2D(tStartPoint,tFinishPoint);
						break;
					}
					
					if (bfInsideNode(AI,tCurrentPosition,dwCurNode)) {
						tCurrentPosition.y = ffGetY(*(tpNode),tCurrentPosition.x,tCurrentPosition.z);
						tTravelNode.P = tCurrentPosition;
						tpaPath.push_back(tTravelNode);
						tPrevPoint = tCurrentPosition;
					}
					else { 
						tStartPoint = tPrevPoint;
						tFinishPoint = tCurrentPosition;
						break;
					}
				}
				while (true);

				if (bRounding) {
					tpNode = AI.Node(dwCurNode);
					taLinks = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
					iCount = tpNode->link_count;
					for ( i=0; i < iCount; i++) {
						iNodeIndex = AI.UnpackLink(taLinks[i]);
						if (bfInsideNode(AI,tCurrentPosition,iNodeIndex)) {
							tpNode = AI.Node(iNodeIndex);
							tTravelNode.P.y = ffGetY(*(tpNode),tTravelNode.P.x,tTravelNode.P.z);
							tpaPath.push_back(tTravelNode);
							tPrevPoint = tTravelNode.P;
							dwCurNode = iNodeIndex;
							break;
						}
					}
					R_ASSERT(i<iCount);
				}
			}
		}
		while (dwCurNode != dwFinishNode);

		if (fabsf(tPrevPoint.x + tPrevPoint.z - tFinishPoint.x- tFinishPoint.z) > EPS_L) {
			tTravelNode.P = tFinishPoint;
			tpaPath.push_back(tTravelNode);
		}
	}
	while (!bStopWorking);
}

void CAI_Soldier::vfComputeCircle(Fvector tPosition, Fvector tPoint0, Fvector tPoint1, float &fRadius, Fvector &tCircleCentre, Fvector &tFinalPosition)
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
	
	tFinalPosition = tP1;
	tFinalPosition.add(tPoint0);
	
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
			
			AI_Path.TravelPath.resize(tpaPatrolPath.size());

			for (int i=0; i<tpaPatrolPath.size(); i++)
				AI_Path.TravelPath[i] = tpaPatrolPath[i];

			AI_Path.TravelStart = 0;
		}

	vfSetFire(false,Group);

	vfSetMovementType(false,m_fMinSpeed);
	
	bStopThinking = true;
}