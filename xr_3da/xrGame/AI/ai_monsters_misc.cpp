////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_monsters_misc.cpp
//	Created 	: 23.07.2002
//  Modified 	: 23.07.2002
//	Author		: Dmitriy Iassenev
//	Description : Miscellanious routines for monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_monsters_misc.h"
#include "..\\xr_weapon_list.h"
#include "..\\WayPointDef.h"
#include "..\\CustomMonster.h"
#include "..\\Actor.h"

extern void	UnpackContour(PContour& C, DWORD ID);
extern void	IntersectContours(PSegment& Dest, PContour& C1, PContour& C2);

#define COMPUTE_DISTANCE_2D(t,p) (sqrtf(_sqr((t).x - (p).x) + _sqr((t).z - (p).z)))

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

/**/
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

void vfCreateFastRealisticPath(vector<Fvector> &tpaPoints, DWORD dwStartNode, vector<Fvector> &tpaDeviations, vector<CTravelNode> &tpaPath, vector<DWORD> &dwaNodes, bool bLooped, bool bUseDeviations, float fRoundedDistanceMin, float fRoundedDistanceMax, float fRadiusMin, float fRadiusMax, float fSuitableAngle, float fSegmentSizeMin, float fSegmentSizeMax)
{
	CTravelNode tTravelNode;
	Fvector tPrevPrevPoint,tTempPoint, tPrevPoint, tStartPoint, tFinishPoint, tCurrentPosition, tCircleCentre, tFinalPosition, t1, t2;
	NodeCompressed *tpNode;
	NodeLink *taLinks;
	PContour tCurContour, tNextContour;
	PSegment tSegment;
	DWORD dwCurNode, dwPrevNode, dwPrevPrevNode;
	int i, j, iCurrentPatrolPoint, iCount, iNodeIndex, iSavedIndex = -1, iStartI;
	float fSuitAngleCosinus = cosf(fSuitableAngle), fHalfSubNodeSize = (Level().AI.GetHeader().size)*.5f, fSegmentSize, fDistance, fRadius, fAlpha0, fAlpha, fTemp, fRoundedDistance = ::Random.randF(fRoundedDistanceMin,fRoundedDistanceMax), fPreviousRoundedDistance = fRoundedDistance;
	bool bStop = false, bOk = false;
	CAI_Space &AI = Level().AI;

	// init deviation points
	tpaDeviations[0].set(0,0,0);
	for ( i=1; i<tpaDeviations.size(); i++) {
		fRadius = ::Random.randF(fRadiusMin,fRadiusMax);
		fAlpha = ::Random.randF(0.f,PI_MUL_2);
		_sincos(fAlpha,fAlpha0,fTemp);
		if (bUseDeviations)
			tpaDeviations[i].set(fTemp*fRadius,0,fAlpha0*fRadius);
		else
			tpaDeviations[i].set(0,0,0);
		tTempPoint.add(tpaPoints[i],tpaDeviations[i]);
	}
	
	if (!bLooped)
		tpaDeviations[tpaDeviations.size() - 1].set(0,0,0);
		
	// init start data
	tpaPath.clear();
	dwaNodes.clear();
	iCurrentPatrolPoint = 1;
	tStartPoint.add(tpaPoints[0],tpaDeviations[0]);
	tFinishPoint.add(tpaPoints[iCurrentPatrolPoint],tpaDeviations[iCurrentPatrolPoint]);
	
	dwPrevNode = DWORD(-1);
	dwaNodes.push_back(dwStartNode);
	dwCurNode = dwStartNode;
	tTempPoint = tTravelNode.P = tPrevPoint = tStartPoint;
	
	fDistance = COMPUTE_DISTANCE_2D(tStartPoint,tFinishPoint);

	while (!bStop) {
		
		do {
			// if distance to corner is small enough - round the corner
			if (tpaPath.size() > 768) {
				//VERIFY(false);
				tpaPath.clear();
				return;
			}
			if (((COMPUTE_DISTANCE_2D(tPrevPoint,tFinishPoint) - fRoundedDistance < EPS_L))) {
				if ((!bLooped) && (iCurrentPatrolPoint == tpaPoints.size() - 1)) {
					bStop = true;
					break;
				}
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
				fSegmentSize = 2*fRadius*fRadius*(1 - fSuitAngleCosinus);
				if ((fRadius < 50.f) && (fSuitableAngle < fAlpha0) && (fSegmentSize < tPrevPoint.distance_to(tpaPoints[iCurrentPatrolPoint])) && (fSegmentSize < .5f*tpaPoints[iCurrentPatrolPoint < tpaPoints.size() - 1 ? iCurrentPatrolPoint + 1 : 0].distance_to(tpaPoints[iCurrentPatrolPoint]))) {
					fAlpha0 = fSuitableAngle;
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
							/**
							tTravelNode.P = tFinalPosition;
							if ((!tpaPath.size()) || COMPUTE_DISTANCE_2D(tCurrentPosition,tpaPath[tpaPath.size() - 1].P) > fSegmentSizeMin)
								tpaPath.push_back(tTravelNode);
							tPrevPoint = tTravelNode.P;
							/**/
							break;
						}
						else {
							tTravelNode.P = tCurrentPosition;
							if ((!tpaPath.size()) || COMPUTE_DISTANCE_2D(tCurrentPosition,tpaPath[tpaPath.size() - 1].P) > fSegmentSizeMin)
								tpaPath.push_back(tTravelNode);
							tPrevPoint = tCurrentPosition;
						}
					}
					while (true);
				}
				/**
				else {
					tTravelNode.P = tFinalPosition;
					tpaPath.push_back(tTravelNode);
					tPrevPoint = tTravelNode.P;
				}
				/**/
					
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
							dwaNodes.push_back(iNodeIndex);
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
								dwaNodes.push_back(iNodeIndex);
								tPrevPoint = tFinishPoint;
								j++;
								break;
							}
						}
						//VERIFY(i<iCount);
						if (i >= iCount) {
							DWORD dwBestNode;
							float fBestCost;
							NodePosition tNodePosition;
							AI.PackPosition(tNodePosition,tpaPath[j].P);
							AI.q_Range_Bit_X(dwCurNode,tPrevPoint,4*fHalfSubNodeSize,&tNodePosition,dwBestNode,fBestCost);
							if (bfInsideNode(AI,AI.Node(dwBestNode),tpaPath[j].P,fHalfSubNodeSize)) {
								dwCurNode = dwBestNode;
								tpaPath[j].P.y = ffGetY(*(AI.Node(dwBestNode)),tpaPath[j].P.x,tpaPath[j].P.z);
								dwaNodes.push_back(dwBestNode);
								tPrevPoint = tFinishPoint;
								j++;
								break;
							}
							tpaPath.clear();
							return;
						}
					}
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

				iSavedIndex = -1;
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
								(COMPUTE_DISTANCE_2D(tFinishPoint,tTravelNode.P) < COMPUTE_DISTANCE_2D(tFinishPoint,tTempPoint) + EPS) &&
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
						tPrevPoint = tTravelNode.P = tTempPoint;
						if (COMPUTE_DISTANCE_2D(tFinishPoint,tTempPoint) < EPS_L)
							tFinishPoint = tTempPoint;
						dwPrevNode = dwCurNode;
						dwCurNode = iSavedIndex;
						dwaNodes.push_back(iSavedIndex);
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
							//VERIFY(false);
							tpaPath.clear();
							return;
						}
				}
				/**/

				tStartPoint.add(tpaPoints[iCurrentPatrolPoint],tpaDeviations[iCurrentPatrolPoint]);
				iCurrentPatrolPoint = iCurrentPatrolPoint < tpaPoints.size() - 1 ? iCurrentPatrolPoint + 1 : 0;
				tFinishPoint.add(tpaPoints[iCurrentPatrolPoint],tpaDeviations[iCurrentPatrolPoint]);
				fDistance = COMPUTE_DISTANCE_2D(tStartPoint,tFinishPoint);
				tTempPoint = tTravelNode.P = tPrevPoint = tStartPoint;

				bStop = iCurrentPatrolPoint == 1;

				fPreviousRoundedDistance = fRoundedDistance;
				if (bLooped)
					fRoundedDistance = ::Random.randF(fRoundedDistanceMin,fRoundedDistanceMax);
				else
					if (iCurrentPatrolPoint != tpaPoints.size() - 1)
						fRoundedDistance = ::Random.randF(fRoundedDistanceMin,fRoundedDistanceMax);
					else
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
							(COMPUTE_DISTANCE_2D(tFinishPoint,tTravelNode.P) < COMPUTE_DISTANCE_2D(tFinishPoint,tTempPoint) + EPS) &&
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
					if ((COMPUTE_DISTANCE_2D(tPrevPoint,tStartPoint) >= fPreviousRoundedDistance) || 
						((!bLooped) && (iCurrentPatrolPoint == 1))) {
						if ((!tpaPath.size()) || COMPUTE_DISTANCE_2D(tTravelNode.P,tpaPath[tpaPath.size() - 1].P) > fSegmentSizeMin)
							/**/
							if (fabsf(tTravelNode.P.y - tPrevPoint.y) > 1.f/256.f)
								tpaPath.push_back(tTravelNode);
							else
							/**/
								tpaPath[tpaPath.size() - 1].P = tTravelNode.P;
						else
							tpaPath.push_back(tTravelNode);
					}
					tPrevPoint = tTravelNode.P;
					dwPrevNode = dwCurNode;
					dwCurNode = iSavedIndex;
					dwaNodes.push_back(iSavedIndex);
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
							if (dwPrevNode == iNodeIndex)
								continue;
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
									if (COMPUTE_DISTANCE_2D(tFinishPoint,tTravelNode.P) < COMPUTE_DISTANCE_2D(tFinishPoint,tTempPoint) + EPS) {
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
							if ((!tpaPath.size()) || COMPUTE_DISTANCE_2D(tTravelNode.P,tpaPath[tpaPath.size() - 1].P) > fSegmentSizeMin)
								if (fabsf(tTravelNode.P.y - tPrevPoint.y) > 1.f/256.f)
									tpaPath.push_back(tTravelNode);
								else
									tpaPath[tpaPath.size() - 1].P = tTravelNode.P;
							//if ((COMPUTE_DISTANCE_2D(tPrevPoint,tStartPoint) >= fPreviousRoundedDistance) || ((!bLooped) && (iCurrentPatrolPoint == 1)))
							//	tpaPath.push_back(tTravelNode);
							tPrevPoint = tTravelNode.P;
							dwPrevNode = dwCurNode;
							dwCurNode = iSavedIndex;
							dwaNodes.push_back(iSavedIndex);
						}
						else {
							//VERIFY(false);
							tpaPath.clear();
							return;
						}
					}
			}
		}
		while (true);
	}
	// close the path for looped ones
	if (bLooped)
		tpaPath.push_back(tpaPath[0]);

	/**/
	// check path for y-values - this is because of the bug I didn't fix
	for ( i=0, j=0; i<tpaPath.size(); i++) {
		int k=j;
		for ( ; (j < dwaNodes.size()) && (!bfInsideNode(AI,AI.Node(dwaNodes[j]),tpaPath[i].P,fHalfSubNodeSize)); j++) ;
		if (j >= dwaNodes.size()) {
			j=k;
			tpaPath.erase(tpaPath.begin() + i);
			i--;
			continue;
		}
		//R_ASSERT(j < dwaNodes.size());
		tpaPath[i].P.y = ffGetY(*(AI.Node(dwaNodes[j])),tpaPath[i].P.x,tpaPath[i].P.z);
	}
	
	/**
	// check path for incorrect points - this is because of the bug I didn't fix
	for ( i=1; i<tpaPath.size() - 1; i++) {
		Fvector tPrevious = tpaPath[i-1].P;
		Fvector tCurrent = tpaPath[i].P;
		Fvector tNext = tpaPath[i+1].P;
		Fvector tTemp1, tTemp2;
		tTemp1.sub(tCurrent,tPrevious);
		tTemp2.sub(tNext,tCurrent);
		tTemp1.normalize_safe();
		tTemp1.y = tTemp2.y = 0;
		tTemp2.normalize_safe();
		float fAlpha = tTemp1.dotproduct(tTemp2);
		clamp(fAlpha, -.99999f, .99999f);
		if (acosf(fAlpha) > PI_DIV_2) {
			tpaPath.erase(tpaPath.begin() + i);
			i--;
		}
	}
	/**/
	for ( i=2; i<tpaPath.size(); i++) {
		if (COMPUTE_DISTANCE_2D(tpaPath[i].P,tpaPath[i-2].P) <= fHalfSubNodeSize + COMPUTE_DISTANCE_2D(tpaPath[i-1].P,tpaPath[i-2].P)) {
			Fvector tPrevious = tpaPath[i-2].P;
			Fvector tCurrent = tpaPath[i-1].P;
			Fvector tNext = tpaPath[i].P;
			Fvector tTemp1, tTemp2;
			tTemp1.sub(tCurrent,tPrevious);
			tTemp2.sub(tNext,tCurrent);
			tTemp1.normalize_safe();
			tTemp1.y = tTemp2.y = 0;
			tTemp2.normalize_safe();
			float fAlpha = tTemp1.dotproduct(tTemp2);
			clamp(fAlpha, -.99999f, +.99999f);
			if ((acosf(fAlpha) > PI_DIV_8*.375f) && (acosf(fAlpha) < 2*PI_DIV_8*.375f))
				continue;
			else {
				tpaPath.erase(tpaPath.begin() + i);
				i--;
			}
		}
	}
	/**/

	//if (bLooped)
	//	tpaPath.push_back(tpaPath[0]);
}

void vfCreatePointSequence(CLevel::SPatrolPath &tpPatrolPath,vector<Fvector> &tpaPoints, bool &bLooped)
{
	tpaPoints.clear();
	int iStartPoint = -1, iCurPoint = -1;
	for (int i=0; i<tpPatrolPath.tpaWayPoints.size(); i++)
		if (tpPatrolPath.tpaWayPoints[i].dwFlags & START_WAYPOINT) {
			tpaPoints.push_back(tpPatrolPath.tpaWayPoints[i].tWayPoint);
			iStartPoint = iCurPoint = i;
			break;
		}
	
	bool bStop = false;
	do {
		for ( i=0; i<tpPatrolPath.tpaWayLinks.size(); i++)
			if (tpPatrolPath.tpaWayLinks[i].wFrom == iCurPoint) {
				i++;
				for (int j=0; i<tpPatrolPath.tpaWayLinks.size(); i++, j++)
					if (tpPatrolPath.tpaWayLinks[i].wFrom != iCurPoint)
						break;
				
				i = i - j - 1 + dwfRandom(j + 1);
				
				if (tpPatrolPath.tpaWayPoints[iCurPoint = tpPatrolPath.tpaWayLinks[i].wTo].dwFlags & FINISH_WAYPOINT) {
					bStop = true;
					if (iStartPoint != tpPatrolPath.tpaWayLinks[i].wTo) {
						tpaPoints.push_back(tpPatrolPath.tpaWayPoints[tpPatrolPath.tpaWayLinks[i].wTo].tWayPoint);
						bLooped = false;
					}
					else
						bLooped = true;
				}
				else
					tpaPoints.push_back(tpPatrolPath.tpaWayPoints[tpPatrolPath.tpaWayLinks[i].wTo].tWayPoint);

				break;
			}
	}
	while(!bStop);
}

float ffCalcSquare(float fAngle, float fAngleOfView, float _b0, float _b1, float _b2, float _b3)
{
	fAngle = fAngle >= PI ? 2*PI - fAngle : fAngle;
	float fSquare;
	if ((fAngle >= -PI/2 + fAngleOfView/2) && (fAngle < fAngleOfView/2))
		if (fAngle < PI/2 - fAngleOfView/2) {
			float b0 = _b0, b1 = _b1, b2 = _b2, b3 = _b3, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), fKsi = fAngleOfView - fAngle;
			fSquare = (CUBE(PI)*(_sqr(a1) + _sqr(a2))/24.f + _sqr(PI)*(a1*b1 + a2*b2)/4.f + PI*(_sqr(b1) + _sqr(b2))/2.f - (CUBE(fAngle)*_sqr(a1)/3.f + _sqr(fAngle)*a1*b1 + fAngle*_sqr(b1) + CUBE(fKsi)*_sqr(a2)/3.f + _sqr(fKsi)*a2*b2 + fKsi*_sqr(b2)))/2.f;
		}
		else {
			float b0 = _b1, b1 = _b2, b2 = _b3, b3 = _b0, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), a3 = (b1 - b3)/(PI/2.f), fKsi = PI- fAngleOfView - fAngle;
			fSquare = (CUBE(PI)*(_sqr(a3) + _sqr(a2))/24.f + _sqr(PI)*(a3*b3 + a2*b2)/4.f + PI*(_sqr(b3) + _sqr(b2))/2.f - (CUBE(fAngle)*_sqr(a1)/3.f + _sqr(fAngle)*a1*b1 + fAngle*_sqr(b1) + CUBE(fKsi)*_sqr(a3)/3.f + _sqr(fKsi)*a3*b3 + fKsi*_sqr(b3)))/2.f;
		}
		else
			if ((fAngle - PI/2 >= -PI/2 + fAngleOfView/2) && (fAngle - PI/2 < fAngleOfView/2))
				if (fAngle - PI/2 < PI/2 - fAngleOfView/2) {
					float b0 = _b1, b1 = _b2, b2 = _b3, b3 = _b0, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), fKsi = fAngleOfView - fAngle;;
					fSquare = (CUBE(PI)*(_sqr(a1) + _sqr(a2))/24.f + _sqr(PI)*(a1*b1 + a2*b2)/4.f + PI*(_sqr(b1) + _sqr(b2))/2.f - (CUBE(fAngle)*_sqr(a1)/3.f + _sqr(fAngle)*a1*b1 + fAngle*_sqr(b1) + CUBE(fKsi)*_sqr(a2)/3.f + _sqr(fKsi)*a2*b2 + fKsi*_sqr(b2)))/2.f;
				}
				else {
					float b0 = _b2, b1 = _b3, b2 = _b0, b3 = _b1, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), a3 = (b1 - b3)/(PI/2.f), fKsi = PI- fAngleOfView - fAngle;
					fSquare = (CUBE(PI)*(_sqr(a3) + _sqr(a2))/24.f + _sqr(PI)*(a3*b3 + a2*b2)/4.f + PI*(_sqr(b3) + _sqr(b2))/2.f - (CUBE(fAngle)*_sqr(a1)/3.f + _sqr(fAngle)*a1*b1 + fAngle*_sqr(b1) + CUBE(fKsi)*_sqr(a3)/3.f + _sqr(fKsi)*a3*b3 + fKsi*_sqr(b3)))/2.f;
				}
				else
					if ((fAngle - PI >= -PI/2 + fAngleOfView/2) && (fAngle - PI < fAngleOfView/2))
						if (fAngle - PI < PI/2 - fAngleOfView/2) {
							float b0 = _b2, b1 = _b3, b2 = _b0, b3 = _b1, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), fKsi = fAngleOfView - fAngle;;
							fSquare = (CUBE(PI)*(_sqr(a1) + _sqr(a2))/24.f + _sqr(PI)*(a1*b1 + a2*b2)/4.f + PI*(_sqr(b1) + _sqr(b2))/2.f - (CUBE(fAngle)*_sqr(a1)/3.f + _sqr(fAngle)*a1*b1 + fAngle*_sqr(b1) + CUBE(fKsi)*_sqr(a2)/3.f + _sqr(fKsi)*a2*b2 + fKsi*_sqr(b2)))/2.f;
						}
						else {
							float b0 = _b3, b1 = _b0, b2 = _b1, b3 = _b2, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), a3 = (b1 - b3)/(PI/2.f), fKsi = PI- fAngleOfView - fAngle;
							fSquare = (CUBE(PI)*(_sqr(a3) + _sqr(a2))/24.f + _sqr(PI)*(a3*b3 + a2*b2)/4.f + PI*(_sqr(b3) + _sqr(b2))/2.f - (CUBE(fAngle)*_sqr(a1)/3.f + _sqr(fAngle)*a1*b1 + fAngle*_sqr(b1) + CUBE(fKsi)*_sqr(a3)/3.f + _sqr(fKsi)*a3*b3 + fKsi*_sqr(b3)))/2.f;
						}
						else
							if (fAngle - 3*PI/2 < PI/2 - fAngleOfView/2) {
								float b0 = _b3, b1 = _b0, b2 = _b1, b3 = _b2, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), fKsi = fAngleOfView - fAngle;;
								fSquare = (CUBE(PI)*(_sqr(a1) + _sqr(a2))/24.f + _sqr(PI)*(a1*b1 + a2*b2)/4.f + PI*(_sqr(b1) + _sqr(b2))/2.f - (CUBE(fAngle)*_sqr(a1)/3.f + _sqr(fAngle)*a1*b1 + fAngle*_sqr(b1) + CUBE(fKsi)*_sqr(a2)/3.f + _sqr(fKsi)*a2*b2 + fKsi*_sqr(b2)))/2.f;
							}
							else {
								float b0 = _b0, b1 = _b1, b2 = _b2, b3 = _b3, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), a3 = (b1 - b3)/(PI/2.f), fKsi = PI- fAngleOfView - fAngle;
								fSquare = (CUBE(PI)*(_sqr(a3) + _sqr(a2))/24.f + _sqr(PI)*(a3*b3 + a2*b2)/4.f + PI*(_sqr(b3) + _sqr(b2))/2.f - (CUBE(fAngle)*_sqr(a1)/3.f + _sqr(fAngle)*a1*b1 + fAngle*_sqr(b1) + CUBE(fKsi)*_sqr(a3)/3.f + _sqr(fKsi)*a3*b3 + fKsi*_sqr(b3)))/2.f;
							}
	return(fSquare);
}

SRotation tfGetOrientation(CEntity *tpEntity)
{
	CCustomMonster *tpCustomMonster = dynamic_cast<CCustomMonster *>(tpEntity);
	if (tpCustomMonster)
		return(tpCustomMonster->r_torso_current);
	else {
		SRotation tTemp;
		CActor *tpActor = dynamic_cast<CActor *>(tpEntity);
		if (tpActor)
			return(tpActor->Orientation());
		else
			return(tTemp);
	}
}

