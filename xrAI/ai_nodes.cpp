////////////////////////////////////////////////////////////////////////////
//	Module 		: xr_a_star.cpp
//	Created 	: 21.03.2002
//  Modified 	: 05.04.2002
//	Author		: Dmitriy Iassenev
//	Description : A* algortihm for finding the x-est path between two nodes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ai_nodes.h"
#include "xrGraph.h"

typedef struct tagSContour {
	Fvector v1,v2,v3,v4;
} SContour;

typedef struct tagSSegment {
	Fvector v1,v2;
} SSegment;

#define	LI_NONE				0
#define LI_COLLINEAR        0
#define	LI_INTERSECT		1
#define LI_EQUAL	        2

IC int lines_intersect(	float x1, float y1,	float x2, float y2,	float x3, float y3, float x4, float y4,	float *x, float *y)
{
	float a1, a2, b1, b2, c1, c2;	/* Coefficients of line eqns. */
	float r1, r2, r3, r4;			/* 'Sign' values */
	float denom, num;				/* Intermediate values */
	
	a1 = y2 - y1;
	b1 = x1 - x2;
	c1 = x2 * y1 - x1 * y2;
	
	r3 = a1 * x3 + b1 * y3 + c1;
	r4 = a1 * x4 + b1 * y4 + c1;
	
	if (r3*r4 > EPS_L)
		return(LI_NONE);
	
	a2 = y4 - y3;
	b2 = x3 - x4;
	c2 = x4 * y3 - x3 * y4;
	
	r1 = a2 * x1 + b2 * y1 + c2;
	r2 = a2 * x2 + b2 * y2 + c2;
	
	if (r1*r2 > EPS_S)
		return(LI_NONE);
	
	if ( _abs(r1*r2)<EPS_S && _abs(r3*r4)<EPS_S ) 
		return (LI_EQUAL);

	denom = a1 * b2 - a2 * b1;
	if ( _abs(denom) < EPS ) return ( LI_COLLINEAR );
	
	num = b1 * c2 - b2 * c1;
	*x = num / denom;
	
	num = a2 * c1 - a1 * c2;
	*y = num / denom;
	
	return(LI_INTERSECT);
}

IC bool bfInsideNode(NodeCompressed *tpNode, Fvector &tCurrentPosition)
{
	Fvector tP0, tP1;
	float fHalfSubNodeSize = m_header.size*.5f;
	UnpackPosition(tP0,tpNode->p0);
	UnpackPosition(tP1,tpNode->p1);
	return(
		(tCurrentPosition.x >= tP0.x - fHalfSubNodeSize - EPS) &&
		(tCurrentPosition.z >= tP0.z - fHalfSubNodeSize - EPS) &&
		(tCurrentPosition.x <= tP1.x + fHalfSubNodeSize + EPS) &&
		(tCurrentPosition.z <= tP1.z + fHalfSubNodeSize + EPS)
	);
}

IC void projectPoint(const Fplane& PL, Fvector& P) 
{
	P.y -= PL.classify(P)/PL.n.y; 
}

void UnpackContour(SContour &C, u32 ID)
{
	NodeCompressed *tpNode = Node(ID);
	
	// decompress positions
	Fvector P0,P1;
	UnpackPosition(P0,tpNode->p0);
	UnpackPosition(P1,tpNode->p1);
	
	// decompress plane
	Fplane	PL;	
	pvDecompress(PL.n,tpNode->plane);
	PL.d = - PL.n.dotproduct(P0);
	
	// create vertices
	float st = m_header.size/2;
	
	C.v1.set(P0.x-st,P0.y,P0.z-st);	projectPoint(PL,C.v1);	// minX,minZ
	C.v2.set(P1.x+st,P0.y,P0.z-st);	projectPoint(PL,C.v2);	// maxX,minZ
	C.v3.set(P1.x+st,P1.y,P1.z+st);	projectPoint(PL,C.v3);	// maxX,maxZ
	C.v4.set(P0.x-st,P1.y,P1.z+st);	projectPoint(PL,C.v4);	// minX,maxZ
}

IC bool bfSimilar(Fvector &tPoint0, Fvector &tPoint1)
{
	return((_abs(tPoint0.x - tPoint1.x) < EPS_L) && (_abs(tPoint0.z - tPoint1.z) < EPS_L));
}

IC bool bfInsideContour(Fvector &tPoint, SContour &tContour)
{
	return((tContour.v1.x - EPS_L <= tPoint.x) && (tContour.v1.z - EPS_L <= tPoint.z) && (tContour.v3.x + EPS_L >= tPoint.x) && (tContour.v3.z + EPS_L >= tPoint.z));
}

IC void vfIntersectContours(SSegment &tSegment, SContour &tContour0, SContour &tContour1)
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

float ffCheckPositionInDirection(u32 dwStartNode, Fvector tStartPosition, Fvector tFinishPosition, float fMaxDistance)
{
	NodeCompressed *tpNode;
	NodeLink *taLinks;
	SContour tCurContour, tNextContour;
	SSegment tSegment;
	int i, iNodeIndex, iCount, iSavedIndex;
	Fvector tPrevPoint, tTempPoint, tStartPoint, tFinishPoint, tTravelNode;
	float fCurDistance = 0.f, fCumulativeDistance = 0.f;
	u32 dwCurNode, dwPrevNode = u32(-1);

	tStartPoint = tStartPosition;
	tFinishPoint = tFinishPosition;
	dwCurNode = dwStartNode;
	tTempPoint = tTravelNode = tPrevPoint = tStartPoint;

	while (!bfInsideNode(m_nodes_ptr[dwCurNode],tFinishPosition)) {
		if (fCumulativeDistance > fMaxDistance)
			return(fCumulativeDistance);
		UnpackContour(tCurContour,dwCurNode);
		tpNode = Node(dwCurNode);
		taLinks = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
		iCount = tpNode->links;
		iSavedIndex = -1;
		tTempPoint = tStartPoint;
		for ( i=0; i < iCount; i++) {
			iNodeIndex = UnpackLink(taLinks[i]);
			UnpackContour(tNextContour,iNodeIndex);
			vfIntersectContours(tSegment,tCurContour,tNextContour);
			u32 dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tSegment.v1.x,tSegment.v1.z,tSegment.v2.x,tSegment.v2.z,&tTravelNode.x,&tTravelNode.z);
			if (dwIntersect == LI_INTERSECT) {
				if (
					(tFinishPoint.distance_to_xz(tTravelNode) < tFinishPoint.distance_to_xz(tTempPoint) + EPS_S) &&
					(iNodeIndex != (int)dwPrevNode)
					) {
					tTempPoint = tTravelNode;
					iSavedIndex = iNodeIndex;
				}
			}
			else
				if (dwIntersect == LI_EQUAL) {
					if (tStartPoint.distance_to_xz(tSegment.v1) > tStartPoint.distance_to_xz(tTempPoint))
						if (tStartPoint.distance_to_xz(tSegment.v1) > tStartPoint.distance_to_xz(tSegment.v2)) {
							tTempPoint = tSegment.v1;
							iSavedIndex = iNodeIndex;
						}
						else {
							tTempPoint = tSegment.v2;
							iSavedIndex = iNodeIndex;
						}
					else
						if (tStartPoint.distance_to_xz(tSegment.v2) > tStartPoint.distance_to_xz(tTempPoint)) {
							tTempPoint = tSegment.v2;
							iSavedIndex = iNodeIndex;
						}

				}
		}

		if (iSavedIndex > -1) {
			fCumulativeDistance += tPrevPoint.distance_to(tTempPoint);
			fCurDistance = tStartPoint.distance_to_xz(tTempPoint);
			tPrevPoint = tTempPoint;
			dwPrevNode = dwCurNode;
			dwCurNode = iSavedIndex;
		}
		else
			if (bfInsideNode(tpNode,tFinishPoint)) {
				dwPrevNode = dwCurNode;
				break;
			}
			else
				return(MAX_VALUE);
	}
	if (bfInsideNode(m_nodes_ptr[dwCurNode],tFinishPosition)) {
		fCumulativeDistance += tPrevPoint.distance_to(tFinishPoint);
		return(fCumulativeDistance);
	}
	else
		return(MAX_VALUE);
}
