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

/**/
IC bool bfInsideContour(Fvector &tPoint, PContour &tContour)
{
	return((tContour.v1.x - EPS_L <= tPoint.x) && (tContour.v1.z - EPS_L <= tPoint.z) && (tContour.v3.x + EPS_L >= tPoint.x) && (tContour.v3.z + EPS_L >= tPoint.z));
}

IC bool bfSimilar(Fvector &tPoint0, Fvector &tPoint1)
{
	return((fabsf(tPoint0.x - tPoint1.x) < EPS_L) && (fabsf(tPoint0.z - tPoint1.z) < EPS_L));
}

IC void vfIntersectContours(PSegment &tSegment, PContour &tContour0, PContour &tContour1)
{
	bool bFound = false;
	
	if (bfInsideContour(tContour0.v1,tContour1)) {
		tSegment.v1 = tContour0.v1;
		bFound = true;
	}

	if (bfInsideContour(tContour0.v2,tContour1)) {
		if (!bFound) {
			tSegment.v1 = tContour0.v2;
			bFound = true;
		}
		else {
			tSegment.v2 = tContour0.v2;
			return;
		}
	}
	if (bfInsideContour(tContour0.v3,tContour1)) {
		if (!bFound) {
			tSegment.v1 = tContour0.v3;
			bFound = true;
		}
		else {
			tSegment.v2 = tContour0.v3;
			return;
		}
	}
	if (bfInsideContour(tContour0.v4,tContour1)) {
		if (!bFound) {
			tSegment.v1 = tContour0.v4;
			bFound = true;
		}
		else {
			tSegment.v2 = tContour0.v4;
			return;
		}
	}
	/**
	if (bfInsideContour(tContour1.v1,tContour0)) {
		if (!bFound) {
			tSegment.v1 = tContour1.v1;
			bFound = true;
		}
		else 
			if (!bfSimilar(tSegment.v1,tContour1.v1)){
				tSegment.v2 = tContour1.v1;
				return;
			}
	}
	if (bfInsideContour(tContour1.v2,tContour0)) {
		if (!bFound) {
			tSegment.v1 = tContour1.v2;
			bFound = true;
		}
		else 
			if (!bfSimilar(tSegment.v1,tContour1.v2)){
				tSegment.v2 = tContour1.v2;
				return;
			}
	}
	if (bfInsideContour(tContour1.v3,tContour0)) {
		if (!bFound) {
			tSegment.v1 = tContour1.v3;
			bFound = true;
		}
		else 
			if (!bfSimilar(tSegment.v1,tContour1.v3)){
				tSegment.v2 = tContour1.v3;
				return;
			}
	}
	if (bfInsideContour(tContour1.v4,tContour0)) {
		if (!bFound) {
			tSegment.v1 = tContour1.v4;
			bFound = true;
		}
		else 
			if (!bfSimilar(tSegment.v1,tContour1.v4)){
				tSegment.v2 = tContour1.v4;
				return;
			}
	}
	/**/
	if (bFound) {
		if (bfInsideContour(tContour1.v1,tContour0) && (!(bfSimilar(tSegment.v1,tContour1.v1)))) {
			tSegment.v2 = tContour1.v1;
			return;
		}
		if (bfInsideContour(tContour1.v2,tContour0) && (!(bfSimilar(tSegment.v1,tContour1.v2)))) {
			tSegment.v2 = tContour1.v2;
			return;
		}
		if (bfInsideContour(tContour1.v3,tContour0) && (!(bfSimilar(tSegment.v1,tContour1.v3)))) {
			tSegment.v2 = tContour1.v3;
			return;
		}
		if (bfInsideContour(tContour1.v4,tContour0) && (!(bfSimilar(tSegment.v1,tContour1.v4)))) {
			tSegment.v2 = tContour1.v4;
			return;
		}
	}
	else {
		if (bfInsideContour(tContour1.v1,tContour0)) {
			tSegment.v1 = tContour1.v1;
			bFound = true;
		}
		if (bfInsideContour(tContour1.v2,tContour0)) {
			if (!bFound) {
				tSegment.v1 = tContour1.v2;
				bFound = true;
			}
			else {
				tSegment.v2 = tContour1.v2;
				return;
			}
		}
		if (bfInsideContour(tContour1.v3,tContour0)) {
			if (!bFound) {
				tSegment.v1 = tContour1.v3;
				bFound = true;
			}
			else {
				tSegment.v2 = tContour1.v3;
				return;
			}
		}
		if (bfInsideContour(tContour1.v4,tContour0)) {
			if (!bFound) {
				tSegment.v1 = tContour1.v4;
				bFound = true;
			}
			else {
				tSegment.v2 = tContour1.v4;
				return;
			}
		}
	}

	if (bFound) {
		tSegment.v2 = tSegment.v1;
		Log("! AI_PathNodes: segment has null length");
	}
	else
		Log("! AI_PathNodes: Can't find intersection segment");
}
/**
IC bool bfInsideNode(float fX, float fZ, NoeCompressed &tNode)
{
	return((tNode.p0.x <= fX) && (tNode.p0.z <= fZ) && (tNode.p1.x >= fX) && (tNode.p1.z >= fZ));
}

IC void vfIntersectContours(PSegment &tSegment, NodeCompressed &tNode0, NodeCompressed &tNode1)
{
	bool bFound = false;
	if (bfInsideNode(tNode0.p0.x,tNode0.p0.z,tNode1)) {
		AI.UnpackPosition(tSegment.v1,tNode0.p0);
		bFound = true;
	}
	if (bfInsideNode(tNode0.p0.x,tNode0.p1.z,tNode1)) {
		if (!bFound) {
			tSegment.v1 = tContour0.v2;
			bFound = true;
		}
		else {
			tSegment.v2 = tContour0.v2;
			return;
		}
	}
	if (bfInsideContour(tContour0.v3,tContour1)) {
		if (!bFound) {
			tSegment.v1 = tContour0.v3;
			bFound = true;
		}
		else {
			tSegment.v2 = tContour0.v3;
			return;
		}
	}
	if (bfInsideContour(tContour0.v4,tContour1)) {
		if (!bFound) {
			tSegment.v1 = tContour0.v4;
			bFound = true;
		}
		else {
			tSegment.v2 = tContour0.v4;
			return;
		}
	}
	if (bfInsideContour(tContour1.v1,tContour0)) {
		if (!bFound) {
			tSegment.v1 = tContour1.v1;
			bFound = true;
		}
		else {
			tSegment.v2 = tContour1.v1;
			return;
		}
	}
	if (bfInsideContour(tContour1.v2,tContour0)) {
		if (!bFound) {
			tSegment.v1 = tContour1.v2;
			bFound = true;
		}
		else {
			tSegment.v2 = tContour1.v2;
			return;
		}
	}
	if (bfInsideContour(tContour1.v3,tContour0)) {
		if (!bFound) {
			tSegment.v1 = tContour1.v3;
			bFound = true;
		}
		else {
			tSegment.v2 = tContour1.v3;
			return;
		}
	}
	if (bfInsideContour(tContour1.v4,tContour0)) {
		if (!bFound) {
			tSegment.v1 = tContour1.v4;
			bFound = true;
		}
		else {
			tSegment.v2 = tContour1.v4;
			return;
		}
	}
	if (bFound) {
		tSegment.v2 = tSegment.v1;
		Log("! AI_PathNodes: segment has null length");
	}
	else
		Log("! AI_PathNodes: Can't find intersection segment");
}
/**/

#define COMPUTE_DISTANCE_2D(t,p) (sqrtf(SQR((t).x - (p).x) + SQR((t).z - (p).z)))

void CAI_Soldier::vfCreateRealisticPath(vector<Fvector> &tpaPoints, vector<DWORD> &dwaNodes, vector<CTravelNode> &tpaPath, float fRoundedDistance, float fSegmentSize)
{
	CTravelNode tTravelNode;
	Fvector tPrevPoint, tStartPoint, tFinishPoint, tCurrentPosition, tCircleCentre, tFinalPosition, t1, t2;
	NodeCompressed *tpNode;
	NodeLink *taLinks;
	PContour tCurContour, tNextContour;
	PSegment tSegment;
	int i, j, iCurrentPatrolPoint, iCount, iNodeIndex;
	DWORD dwCurNode, dwStartNode, dwFinishNode;
	float fDistance, fRadius, fAlpha0, fAlpha, fTemp;
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
		else
			if (iCurrentPatrolPoint == 2) {
				iCurrentPatrolPoint = iCurrentPatrolPoint;
			}

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
				vfIntersectContours(tSegment,tCurContour,tNextContour);
				DWORD dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tSegment.v1.x,tSegment.v1.z,tSegment.v2.x,tSegment.v2.z,&tTravelNode.P.x,&tTravelNode.P.z);
				if ((dwIntersect) && (fabsf(COMPUTE_DISTANCE_2D(tStartPoint,tTravelNode.P) + COMPUTE_DISTANCE_2D(tFinishPoint,tTravelNode.P) - fDistance) < EPS_L) && (fabsf(tPrevPoint.x + tPrevPoint.z - tTravelNode.P.x- tTravelNode.P.z) > EPS_L)) {
					tTravelNode.P.y = ffGetY(*(tpNode),tTravelNode.P.x,tTravelNode.P.z);
					if ((fabsf(tTravelNode.P.y - tPrevPoint.y) > 1.f/256.f) || (dwCurNode == dwStartNode))
						tpaPath.push_back(tTravelNode);
					else
						tpaPath[tpaPath.size() - 1].P = tTravelNode.P;
					tPrevPoint = tTravelNode.P;
					dwCurNode = iNodeIndex;
					break;
				}
			}
/*
			if (i>=iCount) {
				i=i;
			}
*/
			R_ASSERT(i<iCount);
		}
		while (dwCurNode != dwFinishNode);

		if (fabsf(tPrevPoint.x + tPrevPoint.z - tFinishPoint.x- tFinishPoint.z) > 0.1) {
			tTravelNode.P = tFinishPoint;
			tpaPath.push_back(tTravelNode);
		}
	}
	// make path being built a bit more realistic
	iCurrentPatrolPoint = 1;
	tFinishPoint = tpaPoints[iCurrentPatrolPoint];
	for (i=0; i<tpaPath.size(); i++)
		if (COMPUTE_DISTANCE_2D(tpaPath[i].P,tFinishPoint) - fRoundedDistance < EPS_L) {
			tCurrentPosition = tpaPath[i].P;
			i--;
			tCurrentPosition.sub(tpaPath[i].P);
			tCurrentPosition.normalize();
			tCurrentPosition.mul(COMPUTE_DISTANCE_2D(tpaPath[i].P,tFinishPoint) - fRoundedDistance);
			tCurrentPosition.add(tpaPath[i].P);
			//tCurrentPosition.y = ffGetY(*(tpNode),tCurrentPosition.x,tCurrentPosition.z);
			tTravelNode.P = tCurrentPosition;
			tpaPath.insert(tpaPath.begin() + i++,tTravelNode);
			tPrevPoint = tCurrentPosition;
			vfComputeCircle(tCurrentPosition,tFinishPoint,tpaPoints[iCurrentPatrolPoint < tpaPoints.size() - 1 ? iCurrentPatrolPoint + 1 : 0],fRadius,tCircleCentre,tFinalPosition);
			// build circle points
			fAlpha0 = acosf((fRadius*fRadius + fRadius*fRadius - fSegmentSize*fSegmentSize)/(2*fRadius*fRadius));
			do {
				//tCurrentPosition = tPrevPoint;
				tCurrentPosition.sub(tCircleCentre);
				tCurrentPosition.normalize();
				if (tCurrentPosition.z > 0)
					fAlpha = acosf(tCurrentPosition.x);
				else
					fAlpha = -acosf(tCurrentPosition.x);
				fTemp = fAlpha - fAlpha0;
				_sincos(fTemp,tCurrentPosition.z,tCurrentPosition.x);
				tCurrentPosition.mul(fRadius);
				tCurrentPosition.add(tCircleCentre);
				if (COMPUTE_DISTANCE_2D(tPrevPoint,tFinalPosition) < COMPUTE_DISTANCE_2D(tCurrentPosition,tFinalPosition)) {
					tCurrentPosition = tPrevPoint;
					tCurrentPosition.sub(tCircleCentre);
					tCurrentPosition.normalize();
					fTemp = fAlpha + fAlpha0;
					_sincos(fTemp,tCurrentPosition.z,tCurrentPosition.x);
					tCurrentPosition.mul(fRadius);
					tCurrentPosition.add(tCircleCentre);
				}
				// checking for stop to round
				t1.sub(tCurrentPosition,tFinalPosition);
				t2.sub(tCurrentPosition,tPrevPoint);
				t1.normalize();
				t2.normalize();
				fAlpha = acosf(t1.dotproduct(t2));
				if (fAlpha < PI/2) {
					tFinalPosition.y = tPrevPoint.y;
					tTravelNode.P = tFinalPosition;
					tpaPath.insert(tpaPath.begin() + i++,tTravelNode);
					break;
				}
				else {
					//tCurrentPosition.y = ffGetY(*(tpNode),tCurrentPosition.x,tCurrentPosition.z);
					tTravelNode.P = tCurrentPosition;
					tpaPath.insert(tpaPath.begin() + i++,tTravelNode);
					tPrevPoint = tCurrentPosition;
				}
			}
			while (true);
			// delete dummy points
			j = iCurrentPatrolPoint < tpaPoints.size() - 1 ? iCurrentPatrolPoint + 1 : 0;
			if (iCurrentPatrolPoint) {
				fDistance = COMPUTE_DISTANCE_2D(tpaPoints[iCurrentPatrolPoint],tpaPoints[j]) - fRoundedDistance;
				for ( ; ; )
					if ((COMPUTE_DISTANCE_2D(tpaPath[i].P,tpaPoints[iCurrentPatrolPoint]) <= fRoundedDistance) || (fDistance < COMPUTE_DISTANCE_2D(tpaPath[i].P,tpaPoints[j])))
						tpaPath.erase(tpaPath.begin() + i);
					else 
						break;
			}
			else {
				fDistance = COMPUTE_DISTANCE_2D(tpaPoints[iCurrentPatrolPoint],tpaPoints[j]) - fRoundedDistance;
				for ( ; ; ) {
					i = i == tpaPath.size() ? 0 : i;
					if ((COMPUTE_DISTANCE_2D(tpaPath[i].P,tpaPoints[iCurrentPatrolPoint]) <= fRoundedDistance) || (fDistance < COMPUTE_DISTANCE_2D(tpaPath[i].P,tpaPoints[j])))
						tpaPath.erase(tpaPath.begin() + i);
					else 
						break;
				}
				tTravelNode.P = tFinalPosition;
				tpaPath.insert(tpaPath.begin(),tTravelNode);
				break;	
			}
			// init new patrol point
			iCurrentPatrolPoint = j;
			tFinishPoint = tpaPoints[iCurrentPatrolPoint];
		}
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