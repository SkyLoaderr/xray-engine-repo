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

#define MAX_PATROL_DISTANCE		6.f
#define MIN_PATROL_DISTANCE		1.f

#ifdef WRITE_LOG
	#define WRITE_TO_LOG(S) Msg("creature : %s, mode : %s",cName(),S);
#else
	#define WRITE_TO_LOG(S)
#endif

#define CHECK_FOR_STATE_TRANSITIONS(S) \
	WRITE_TO_LOG(S);\
	\
	if (g_Health() <= 0) {\
		eCurrentState = aiSoldierDie;\
		return;\
	}\
	\
	SelectEnemy(Enemy);\
	\
	if (Enemy.Enemy) {\
		tStateStack.push(eCurrentState);\
		eCurrentState = aiSoldierAttackFire;\
		m_dwLastRangeSearch = 0;\
		return;\
	}\
	\
	DWORD dwCurTime = Level().timeServer();\
	\
	if ((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime)) {\
		tStateStack.push(eCurrentState);\
		eCurrentState = aiSoldierUnderFire;\
		m_dwLastRangeSearch = 0;\
		return;\
	}\
	\
	if (dwCurTime - dwSenseTime < SENSE_JUMP_TIME) {\
		tStateStack.push(eCurrentState);\
		eCurrentState = aiSoldierSenseSomething;\
		m_dwLastRangeSearch = 0;\
		return;\
	}\
	\
	INIT_SQUAD_AND_LEADER;\
	\
	CGroup &Group = Squad.Groups[g_Group()];\
	\
	if ((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime)) {\
		tHitDir = Group.m_tLastHitDirection;\
		dwHitTime = Group.m_dwLastHitTime;\
		tStateStack.push(eCurrentState);\
		eCurrentState = aiSoldierUnderFire;\
		m_dwLastRangeSearch = 0;\
		return;\
	}

#define SET_LOOK_FIRE_MOVEMENT(a,b,c)\
	SetLessCoverLook(AI_Node);\
	\
	vfSetFire(a,Group);\
	\
	vfSetMovementType(b,c);\
	\
	bStopThinking = true;


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
	if ( fabsf(r1*r2)<EPS_S && fabsf(r3*r4)<EPS_S ) return LI_EQUAL;

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

void vfComputeCircle(Fvector tPosition, Fvector tPoint0, Fvector tPoint1, float &fRadius, Fvector &tCircleCentre, Fvector &tFinalPosition, float &fBeta)
{
	Fvector tP0, tP1;
	float fAlpha, fSinus, fCosinus, fRx;

	tP0.sub(tPosition,tPoint0);
	tP1.sub(tPoint1,tPoint0);
	fRx = tP0.magnitude();
	tP0.normalize();
	tP1.normalize();

	clamp(fAlpha = tP0.dotproduct(tP1),-0.9999999f,0.9999999f);
	fAlpha = .5f*(fBeta = acosf(fAlpha));
	fBeta = PI - fBeta;

	tP0.mul(fRx);
	tP1.mul(fRx);
	
	tFinalPosition = tP1;
	tFinalPosition.add(tPoint0);
	
	tCircleCentre.add(tP0,tP1);
	_sincos(fAlpha,fSinus,fCosinus);
	fRadius = fRx*fSinus/fCosinus;
	fRx = fRadius*(1.f/fSinus - 1.f);
	tCircleCentre.mul((fRadius + fRx)/tCircleCentre.magnitude());
	tCircleCentre.add(tPoint0);
}

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

#define COMPUTE_DISTANCE_2D(t,p) (sqrtf(SQR((t).x - (p).x) + SQR((t).z - (p).z)))

IC bool bfInsideNode(CAI_Space &AI, NodeCompressed *tpNode, Fvector &tCurrentPosition, float fHalfSubNodeSize)
{
	Fvector tP0, tP1;
	AI.UnpackPosition(tP0,tpNode->p0);
	AI.UnpackPosition(tP1,tpNode->p1);
	return(
		(tCurrentPosition.x >= tP0.x - fHalfSubNodeSize) &&
		(tCurrentPosition.z >= tP0.z - fHalfSubNodeSize) &&
		(tCurrentPosition.x <= tP1.x + fHalfSubNodeSize) &&
		(tCurrentPosition.z <= tP1.z + fHalfSubNodeSize)
	);
}

void vfCreateFastRealisticPath(vector<Fvector> &tpaPoints, DWORD dwStartNode, vector<Fvector> &tpaDeviations, vector<CTravelNode> &tpaPath, bool bUseDeviations = true, float fRoundedDistanceMin = 1.5f, float fRoundedDistanceMax = 3.0f, float fRadiusMin = 0.5f, float fRadiusMax = 1.5f, float fSuitableAngle = PI_DIV_8*.25f, float fSegmentSizeMin = Level().AI.GetHeader().size*.5f, float fSegmentSizeMax = Level().AI.GetHeader().size*2.f)
{
	CTravelNode tTravelNode;
	Fvector tPrevPrevPoint,tTempPoint, tPrevPoint, tStartPoint, tFinishPoint, tCurrentPosition, tCircleCentre, tFinalPosition, t1, t2;
	NodeCompressed *tpNode;
	NodeLink *taLinks;
	PContour tCurContour, tNextContour;
	PSegment tSegment;
	DWORD dwCurNode, dwPrevNode, dwPrevPrevNode;
	int i, j, iCurrentPatrolPoint, iCount, iNodeIndex, iSavedIndex, iStartI;
	float fSuitAngleCosinus = cosf(fSuitableAngle), fHalfSubNodeSize = (Level().AI.GetHeader().size)*.5f, fSegmentSize, fDistance, fRadius, fAlpha0, fAlpha, fTemp, fRoundedDistance = ::Random.randF(fRoundedDistanceMin,fRoundedDistanceMax), fPreviousRoundedDistance = fRoundedDistance;
	bool bStop = false, bOk = false;
	CAI_Space &AI = Level().AI;

	// init deviation points
	tpaDeviations[0].set(0,0,0);
	for ( i=1; i<tpaDeviations.size(); i++) {
		fRadius = ::Random.randF(fRadiusMin,fRadiusMax), fAlpha = ::Random.randF(0.f,PI_MUL_2);
		_sincos(fAlpha,fAlpha0,fTemp);
		if (bUseDeviations)
			tpaDeviations[i].set(fTemp*fRadius,0,fAlpha0*fRadius);
		else
			tpaDeviations[i].set(0,0,0);
		tTempPoint.add(tpaPoints[i],tpaDeviations[i]);
	}
		
	// init start data
	tpaPath.clear();
	iCurrentPatrolPoint = 1;
	tStartPoint.add(tpaPoints[0],tpaDeviations[0]);
	tFinishPoint.add(tpaPoints[iCurrentPatrolPoint],tpaDeviations[iCurrentPatrolPoint]);
	
	dwPrevNode = DWORD(-1);
	dwCurNode = dwStartNode;
	tTempPoint = tTravelNode.P = tPrevPoint = tStartPoint;
	
	fDistance = COMPUTE_DISTANCE_2D(tStartPoint,tFinishPoint);

	while (!bStop) {
		do {
			// if distance is small enough - round the corner
			if (COMPUTE_DISTANCE_2D(tPrevPoint,tFinishPoint) - fRoundedDistance < EPS_L) {
				iStartI = tpaPath.size() - 1;
				dwPrevPrevNode = dwPrevNode;
				dwPrevNode = dwCurNode;
				tPrevPrevPoint = tpaPath[tpaPath.size() - 1].P;
				tCurrentPosition = tFinishPoint;
				tCurrentPosition.sub(tStartPoint);
				tCurrentPosition.normalize();
				tCurrentPosition.mul(fDistance - fRoundedDistance);
				tCurrentPosition.add(tStartPoint);
				tTravelNode.P = tCurrentPosition;
				tpaPath[iStartI].P = tTravelNode.P;
				tPrevPoint = tCurrentPosition;
				vfComputeCircle(tCurrentPosition,tFinishPoint,tpaPoints[iCurrentPatrolPoint < tpaPoints.size() - 1 ? iCurrentPatrolPoint + 1 : 0],fRadius,tCircleCentre,tFinalPosition,fAlpha0);
				// build circle points
				if (fSuitableAngle < fAlpha0) {
					fSegmentSize = 2*fRadius*fRadius*(1 - fSuitAngleCosinus);
					fAlpha0 = fSuitableAngle;
					//clamp(fAlpha0 = (fRadius*fRadius + fRadius*fRadius - fSegmentSize*fSegmentSize)/(2*fRadius*fRadius),-0.9999999f,0.9999999f);
					//fAlpha0 = acosf(fAlpha0);
					do {
						tCurrentPosition.sub(tCircleCentre);
						tCurrentPosition.normalize();
						clamp(tCurrentPosition.x,-0.9999999f,0.9999999f);
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
						clamp(fAlpha = t1.dotproduct(t2),-0.9999999f,0.9999999f);
						fAlpha = acosf(fAlpha);
						if (fAlpha < PI/2) {
							tTravelNode.P = tFinalPosition;
							tpaPath.push_back(tTravelNode);
							tPrevPoint = tTravelNode.P;
							break;
						}
						else {
							tTravelNode.P = tCurrentPosition;
							tpaPath.push_back(tTravelNode);
							tPrevPoint = tCurrentPosition;
						}
					}
					while (true);
					
					// assign y-values to the circle points being built
					j = iStartI;
					dwCurNode = iNodeIndex = dwPrevNode;
					if (bfInsideNode(AI,tpNode = AI.Node(iNodeIndex),tpaPath[j].P, fHalfSubNodeSize)) {
						do	{
							tpaPath[j].P.y = ffGetY(*(AI.Node(iNodeIndex)),tpaPath[j].P.x,tpaPath[j].P.z);
							j++;
						}
						while ((j<tpaPath.size()) && (bfInsideNode(AI,tpNode,tpaPath[j].P, fHalfSubNodeSize)));
						dwCurNode = iNodeIndex;
					}
					while (j < tpaPath.size()) {
						tpNode = AI.Node(dwCurNode);
						taLinks = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
						iCount = tpNode->link_count;
						for ( i=0; i < iCount; i++) {
							iNodeIndex = AI.UnpackLink(taLinks[i]);
							if (bfInsideNode(AI,tpNode = AI.Node(iNodeIndex),tpaPath[j].P, fHalfSubNodeSize)) {
								do	{
									tpaPath[j].P.y = ffGetY(*(AI.Node(iNodeIndex)),tpaPath[j].P.x,tpaPath[j].P.z);
									tPrevPoint = tpaPath[j - 1].P;
									j++;
								}
								while ((j<tpaPath.size()) && (bfInsideNode(AI,tpNode,tpaPath[j].P, fHalfSubNodeSize)));
								dwCurNode = iNodeIndex;
								break;
							}
						}
						if (i >= iCount) {
							tStartPoint = tpaPath[j-1].P;
							tFinishPoint = tpaPath[j].P;
							UnpackContour(tCurContour,dwCurNode);
							tpNode = AI.Node(dwCurNode);
							taLinks = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
							iCount = tpNode->link_count;
							for ( i=0; i < iCount; i++) {
								iNodeIndex = AI.UnpackLink(taLinks[i]);
								UnpackContour(tNextContour,iNodeIndex);
								vfIntersectContours(tSegment,tCurContour,tNextContour);
								DWORD dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tSegment.v1.x,tSegment.v1.z,tSegment.v2.x,tSegment.v2.z,&tTravelNode.P.x,&tTravelNode.P.z);
								if ((dwIntersect) && (COMPUTE_DISTANCE_2D(tPrevPoint,tTravelNode.P) >= 2*EPS_L)) {
									dwCurNode = iNodeIndex;
									tPrevPoint = tFinishPoint;
									break;
								}
							}
							VERIFY(i<iCount);
							tpaPath.clear();
							return;
						}
					}
				}
				else {
					tTravelNode.P = tFinalPosition;
					tpaPath.push_back(tTravelNode);
					tPrevPoint = tTravelNode.P;
				}
				
				// finding the node
				iCurrentPatrolPoint = iCurrentPatrolPoint > 0 ? iCurrentPatrolPoint - 1 : tpaPoints.size() - 1;
				tStartPoint.add(tpaPoints[iCurrentPatrolPoint],tpaDeviations[iCurrentPatrolPoint]);
				iCurrentPatrolPoint = iCurrentPatrolPoint < tpaPoints.size() - 1 ? iCurrentPatrolPoint + 1 : 0;
				tFinishPoint.add(tpaPoints[iCurrentPatrolPoint],tpaDeviations[iCurrentPatrolPoint]);
				fDistance = COMPUTE_DISTANCE_2D(tStartPoint,tFinishPoint);
				dwCurNode = dwPrevNode;
				dwPrevNode = dwPrevPrevNode;
				tTravelNode.P = tPrevPoint = tPrevPrevPoint;

				while (!bfInsideNode(AI,AI.Node(dwCurNode),tFinishPoint, fHalfSubNodeSize)) {
					UnpackContour(tCurContour,dwCurNode);
					tpNode = AI.Node(dwCurNode);
					taLinks = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
					iCount = tpNode->link_count;
					iSavedIndex = -1;
					tTempPoint = tStartPoint;
					for ( i=0; i < iCount; i++) {
						iNodeIndex = AI.UnpackLink(taLinks[i]);
						UnpackContour(tNextContour,iNodeIndex);
						vfIntersectContours(tSegment,tCurContour,tNextContour);
						DWORD dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tSegment.v1.x,tSegment.v1.z,tSegment.v2.x,tSegment.v2.z,&tTravelNode.P.x,&tTravelNode.P.z);
						if (dwIntersect == LI_INTERSECT) {
							if (
								(COMPUTE_DISTANCE_2D(tFinishPoint,tTravelNode.P) < COMPUTE_DISTANCE_2D(tFinishPoint,tTempPoint) + EPS_L) &&
								(iNodeIndex != dwPrevNode)
								) {
								tTravelNode.P.y = ffGetY(*(tpNode),tTravelNode.P.x,tTravelNode.P.z);
								tTempPoint = tTravelNode.P;
								iSavedIndex = iNodeIndex;
							}
						}
						else
							if (dwIntersect == LI_EQUAL) {
								if (COMPUTE_DISTANCE_2D(tStartPoint,tSegment.v1) > COMPUTE_DISTANCE_2D(tStartPoint,tTempPoint))
									if (COMPUTE_DISTANCE_2D(tStartPoint,tSegment.v1) > COMPUTE_DISTANCE_2D(tStartPoint,tSegment.v2)) {
										tTempPoint = tSegment.v1;
										iSavedIndex = iNodeIndex;
									}
									else {
										tTempPoint = tSegment.v2;
										iSavedIndex = iNodeIndex;
									}
								else
									if (COMPUTE_DISTANCE_2D(tStartPoint,tSegment.v2) > COMPUTE_DISTANCE_2D(tStartPoint,tTempPoint)) {
										tTempPoint = tSegment.v2;
										iSavedIndex = iNodeIndex;
									}

							}
					}

					if (iSavedIndex > -1) {
						tTravelNode.P = tTempPoint;
						tPrevPoint = tTravelNode.P;
						dwPrevNode = dwCurNode;
						dwCurNode = iSavedIndex;
					}
					else
						if (bfInsideNode(AI,tpNode,tFinishPoint, fHalfSubNodeSize)) {
							tTravelNode.P = tFinishPoint;
							tTravelNode.P.y = ffGetY(*(tpNode),tTravelNode.P.x,tTravelNode.P.z);
							tPrevPoint = tTravelNode.P;
							dwPrevNode = dwCurNode;
							break;
						}
						else {
							VERIFY(false);
							tpaPath.clear();
							return;
						}
				}

				tStartPoint.add(tpaPoints[iCurrentPatrolPoint],tpaDeviations[iCurrentPatrolPoint]);
				iCurrentPatrolPoint = iCurrentPatrolPoint < tpaPoints.size() - 1 ? iCurrentPatrolPoint + 1 : 0;
				tFinishPoint.add(tpaPoints[iCurrentPatrolPoint],tpaDeviations[iCurrentPatrolPoint]);
				fDistance = COMPUTE_DISTANCE_2D(tStartPoint,tFinishPoint);
				tTempPoint = tTravelNode.P = tPrevPoint = tStartPoint;

				bStop = iCurrentPatrolPoint == 1;

				fPreviousRoundedDistance = fRoundedDistance;
				fRoundedDistance = ::Random.randF(fRoundedDistanceMin,fRoundedDistanceMax);

				dwPrevNode = DWORD(-1);
				
				break;
			}
			else {
				UnpackContour(tCurContour,dwCurNode);
				tpNode = AI.Node(dwCurNode);
				taLinks = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
				iCount = tpNode->link_count;
				iSavedIndex = -1;
				tTempPoint = tStartPoint;
				for ( i=0; i < iCount; i++) {
					iNodeIndex = AI.UnpackLink(taLinks[i]);
					UnpackContour(tNextContour,iNodeIndex);
					vfIntersectContours(tSegment,tCurContour,tNextContour);
					DWORD dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tSegment.v1.x,tSegment.v1.z,tSegment.v2.x,tSegment.v2.z,&tTravelNode.P.x,&tTravelNode.P.z);
					if (dwIntersect == LI_INTERSECT) {
						if (
							(COMPUTE_DISTANCE_2D(tFinishPoint,tTravelNode.P) < COMPUTE_DISTANCE_2D(tFinishPoint,tTempPoint) + EPS_L) &&
							(iNodeIndex != dwPrevNode)
							) {
							tTravelNode.P.y = ffGetY(*(tpNode),tTravelNode.P.x,tTravelNode.P.z);
							tTempPoint = tTravelNode.P;
							iSavedIndex = iNodeIndex;
						}
					}
					else
						if (dwIntersect == LI_EQUAL) {
							if (COMPUTE_DISTANCE_2D(tStartPoint,tSegment.v1) > COMPUTE_DISTANCE_2D(tStartPoint,tTempPoint))
								if (COMPUTE_DISTANCE_2D(tStartPoint,tSegment.v1) > COMPUTE_DISTANCE_2D(tStartPoint,tSegment.v2)) {
									tTempPoint = tSegment.v1;
									iSavedIndex = iNodeIndex;
								}
								else {
									tTempPoint = tSegment.v2;
									iSavedIndex = iNodeIndex;
								}
							else
								if (COMPUTE_DISTANCE_2D(tStartPoint,tSegment.v2) > COMPUTE_DISTANCE_2D(tStartPoint,tTempPoint)) {
									tTempPoint = tSegment.v2;
									iSavedIndex = iNodeIndex;
								}

						}
				}

				if (iSavedIndex > -1) {
					tTravelNode.P = tTempPoint;
					if (COMPUTE_DISTANCE_2D(tPrevPoint,tStartPoint) >= fPreviousRoundedDistance) {
						if (fabsf(tTravelNode.P.y - tPrevPoint.y) > 1.f/256.f)
							tpaPath.push_back(tTravelNode);
						else
							tpaPath[tpaPath.size() - 1].P = tTravelNode.P;
					}
					tPrevPoint = tTravelNode.P;
					dwPrevNode = dwCurNode;
					dwCurNode = iSavedIndex;
				}
				else
					if (bfInsideNode(AI,tpNode,tFinishPoint, fHalfSubNodeSize)) {
						tTravelNode.P = tFinishPoint;
						tTravelNode.P.y = ffGetY(*(tpNode),tTravelNode.P.x,tTravelNode.P.z);
						tPrevPoint = tTravelNode.P;
						dwPrevNode = dwCurNode;
						break;
					}
					else {
						// Ooops! there is no proper neighbour node, try to search the nearest
						// nodes for the closest to the current node intersection with the 
						// line Start -> Finish
						tTempPoint = tPrevPoint;
						bOk = false;
						for ( i=0; i < iCount; i++) {
							iNodeIndex = AI.UnpackLink(taLinks[i]);
							UnpackContour(tNextContour,iNodeIndex);
							for ( j=0; j<4; j++) {
								switch(j) {
									case 0: {
										tSegment.v1 = tNextContour.v1;
										tSegment.v2 = tNextContour.v2;
										break;
									}
									case 1: {
										tSegment.v1 = tNextContour.v2;
										tSegment.v2 = tNextContour.v3;
										break;
									}
									case 2: {
										tSegment.v1 = tNextContour.v3;
										tSegment.v2 = tNextContour.v4;
										break;
									}
									case 3: {
										tSegment.v1 = tNextContour.v4;
										tSegment.v2 = tNextContour.v1;
										break;
									}
								}
								DWORD dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tSegment.v1.x,tSegment.v1.z,tSegment.v2.x,tSegment.v2.z,&tTravelNode.P.x,&tTravelNode.P.z);
								if (dwIntersect == LI_INTERSECT) {
									if (COMPUTE_DISTANCE_2D(tFinishPoint,tTravelNode.P) < COMPUTE_DISTANCE_2D(tFinishPoint,tTempPoint) + EPS_L) {
										if (bOk)
											break;
										tTravelNode.P.y = ffGetY(*(tpNode),tTravelNode.P.x,tTravelNode.P.z);
										tTempPoint = tTravelNode.P;
										iSavedIndex = iNodeIndex;
										bOk = true;
									}
									else 
										if (bOk) {
											tTravelNode.P.y = ffGetY(*(tpNode),tTravelNode.P.x,tTravelNode.P.z);
											tTempPoint = tTravelNode.P;
											iSavedIndex = iNodeIndex;
											break;
										}
									
								}
								else
									if (dwIntersect == LI_EQUAL) {
										if (COMPUTE_DISTANCE_2D(tStartPoint,tSegment.v1) > COMPUTE_DISTANCE_2D(tStartPoint,tTempPoint))
											if (COMPUTE_DISTANCE_2D(tStartPoint,tSegment.v1) > COMPUTE_DISTANCE_2D(tStartPoint,tSegment.v2)) {
												tTempPoint = tSegment.v1;
												iSavedIndex = iNodeIndex;
											}
											else {
												tTempPoint = tSegment.v2;
												iSavedIndex = iNodeIndex;
											}
										else
											if (COMPUTE_DISTANCE_2D(tStartPoint,tSegment.v2) > COMPUTE_DISTANCE_2D(tStartPoint,tTempPoint)) {
												tTempPoint = tSegment.v2;
												iSavedIndex = iNodeIndex;
											}

									}
							}
							if (bOk)
								break;
						}
						if (iSavedIndex > -1) {
							tTravelNode.P = tTempPoint;
							if (COMPUTE_DISTANCE_2D(tPrevPoint,tStartPoint) >= fPreviousRoundedDistance)
								tpaPath.push_back(tTravelNode);
							tPrevPoint = tTravelNode.P;
							dwPrevNode = dwCurNode;
							dwCurNode = iSavedIndex;
						}
						else {
							VERIFY(false);
							tpaPath.clear();
							return;
						}
					}
			}
		}
		while (true);
	}
	// close the path
	tpaPath.push_back(tpaPath[0]);
}

void CAI_Soldier::PatrolUnderFire()
{
	// if no more health then soldier is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Patrol under fire");
#endif
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

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		if (m_dwLastSuccessfullSearch <= Group.m_dwLastHitTime)
			vfSearchForBetterPosition(SelectorUnderFire,Squad,Leader);

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
	if (dwCurTime - dwHitTime >= 0*1000) {
//		mk_rotation(Group.m_tLastHitDirection,r_torso_target);
//		r_target.yaw = r_torso_target.yaw;
//		r_torso_target.yaw = r_torso_target.yaw - EYE_WEAPON_DELTA;
		tWatchDirection.sub(Group.m_tHitPosition,eye_matrix.c);
		mk_rotation(tWatchDirection,r_torso_target);
		r_target.yaw = r_torso_target.yaw;
		r_torso_target.yaw = r_torso_target.yaw - EYE_WEAPON_DELTA;
	}
	/**/
	
	vfSetFire(dwCurTime - dwHitTime < 1000,Group);

	vfSetMovementType(false,m_fMaxSpeed);
	// stop processing more rules
	bStopThinking = true;
}

void CAI_Soldier::PatrolHurt()
{
	// if no more health then soldier is dead
	WRITE_TO_LOG("Patrol hurt");

	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		return;
	}

	INIT_SQUAD_AND_LEADER;
	CGroup &Group = Squad.Groups[g_Group()];
	
	//if (dwCurTime - dwHitTime >= 2000) {
		Group.m_dwLastHitTime = dwHitTime;
		Group.m_tLastHitDirection = tHitDir;
		
		Group.m_tHitPosition = tHitPosition;
	//}

	DWORD dwCurTime = Level().timeServer();

	if (dwCurTime - dwHitTime >= 1*1000) {
		eCurrentState = aiSoldierPatrolUnderFire;
		m_dwLastRangeSearch = 0;
		return;
	}

	AI_Path.TravelPath.clear();

	tWatchDirection.sub(tHitPosition,eye_matrix.c);
	mk_rotation(tWatchDirection,r_torso_target);
	r_target.yaw = r_torso_target.yaw;
	r_torso_target.yaw = r_torso_target.yaw - EYE_WEAPON_DELTA;
	/**
	mk_rotation(tHitDir,r_torso_target);
	r_target.yaw = r_torso_target.yaw;
	r_torso_target.yaw = r_torso_target.yaw - EYE_WEAPON_DELTA;
	/**/

	vfSetFire(false,Group);

	vfSetMovementType(true,m_fMinSpeed);
	// stop processing more rules
	bStopThinking = true;
}

void CAI_Soldier::FollowLeaderPatrol()
{
	CHECK_FOR_STATE_TRANSITIONS("Following leader patrol");
	
	if (Leader == this) {
		eCurrentState = aiSoldierPatrolRoute;
		return;
	}

	SetLessCoverLook(AI_Node);
	
	vfInitSelector(SelectorPatrol,Squad,Leader);

	if (AI_Path.bNeedRebuild) {
		AI_Path.DestNode = m_dwStartPatrolNode;
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
		if ((!(AI_Path.fSpeed)) || (AI_Path.TravelStart >= AI_Path.TravelPath.size() - 4)) {
			CAI_Soldier *SoldierLeader = dynamic_cast<CAI_Soldier *>(Leader);
			if ((Level().timeServer() - SoldierLeader->m_dwLastRangeSearch < 3000) && (SoldierLeader->m_dwLastRangeSearch)) {
				AI_Path.TravelPath.resize(SoldierLeader->AI_Path.TravelPath.size());
				for (int i=0, j=0; i<SoldierLeader->AI_Path.TravelPath.size(); i++, j++) {
					AI_Path.TravelPath[j] = SoldierLeader->AI_Path.TravelPath[i];
					AI_Path.TravelPath[j].floating = FALSE;
					Fvector tTemp;
					tTemp.sub(SoldierLeader->AI_Path.TravelPath[i < SoldierLeader->AI_Path.TravelPath.size() - 1 ? i + 1 : 0].P, SoldierLeader->AI_Path.TravelPath[i].P);
					if (tTemp.magnitude() < .1f) {
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
			else {
				if (Leader->Position().distance_to(m_tpaPatrolPoints[0]) < Position().distance_to(m_tpaPatrolPoints[0])) {
					vfInitSelector(SelectorPatrol,Squad,Leader);
					SelectorPatrol.m_tEnemyPosition = m_tpaPatrolPoints[0];
					/**
					float fTemp, fDistance = m_tpaPatrolPoints[0].distance_to(Position());
					for (int i=1; i<m_tpaPatrolPoints.size(); i++)
						if ((fTemp = m_tpaPatrolPoints[i].distance_to(Position())) < fDistance) {
							fDistance = fTemp;
							SelectorPatrol.m_tEnemyPosition = m_tpaPatrolPoints[i];
						}
					/**/
					vfSearchForBetterPosition(SelectorPatrol,Squad,Leader);
					AI_Path.bNeedRebuild = TRUE;
				}
			}
		}

	if ((!m_dwLastRangeSearch) || (Level().timeServer() - m_dwLastRangeSearch >= 5000)) {
		m_dwLastRangeSearch = Level().timeServer();
		m_fMinPatrolDistance = MIN_PATROL_DISTANCE;
		m_fMaxPatrolDistance = MAX_PATROL_DISTANCE - ::Random.randF(0,4);
	}

	vfSetFire(false,Group);

	Fvector tTemp0;
	tTemp0.sub(Leader->Position(),Position());
	tTemp0.normalize();
	tWatchDirection.normalize();
	if (acosf(tWatchDirection.dotproduct(tTemp0)) < PI_DIV_2) {
		float fDistance = Leader->Position().distance_to(Position());
		if (fDistance >= m_fMaxPatrolDistance)
			vfSetMovementType(false,1.1f*m_fMinSpeed);
		else
			if (fDistance <= m_fMinPatrolDistance)
				vfSetMovementType(false,.9f*m_fMinSpeed);
			else 
				vfSetMovementType(false,((fDistance - (m_fMaxPatrolDistance + m_fMinPatrolDistance)*.5f)/((m_fMaxPatrolDistance - m_fMinPatrolDistance)*.5f)*.1f + m_fMinPatrolDistance)*m_fMinSpeed);
	}
	else
		vfSetMovementType(false,.9f*m_fMinSpeed);
	// stop processing more rules
	bStopThinking = true;
}

void CAI_Soldier::Patrol()
{
	CHECK_FOR_STATE_TRANSITIONS("Patrol detour...");
	
	if (AI_Path.bNeedRebuild) {
		AI_Path.DestNode = m_dwStartPatrolNode;
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
		if ((!(AI_Path.fSpeed)) || (AI_Path.TravelStart >= AI_Path.TravelPath.size() - 4)) {
			vfCreateFastRealisticPath(m_tpaPatrolPoints, m_dwStartPatrolNode, m_tpaPointDeviations, AI_Path.TravelPath);
			AI_Path.TravelStart = 0;
			m_dwLastRangeSearch = Level().timeServer();
		}

	SET_LOOK_FIRE_MOVEMENT(false,false,m_fMinSpeed)
}
