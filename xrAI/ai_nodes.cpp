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

#define EPS_H				0.5f

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
	
	if ((r3*r4 > EPS) && (fabsf(r3) > EPS_H) && (fabsf(r4) > EPS_H))
		return(LI_NONE);
	
	a2 = y4 - y3;
	b2 = x3 - x4;
	c2 = x4 * y3 - x3 * y4;
	
	r1 = a2 * x1 + b2 * y1 + c2;
	r2 = a2 * x2 + b2 * y2 + c2;
	
	if ((r1*r2 > EPS) && (fabsf(r1) > EPS_H) && (fabsf(r2) > EPS_H))
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

IC float ffGetY(NodeCompressed &tNode, float X, float Z)
{
	Fvector	DUP, vNorm, v, v1, P0;
	DUP.set(0,1,0);
	pvDecompress(vNorm,tNode.plane);
	Fplane PL; 
	UnpackPosition(P0,tNode.p0);
	PL.build(P0,vNorm);
	v.set(X,P0.y,Z);	
	PL.intersectRayPoint(v,DUP,v1);	
	return(v1.y);
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
		(tCurrentPosition.z <= tP1.z + fHalfSubNodeSize + EPS) &&
		(fabsf(tCurrentPosition.y - ffGetY(*tpNode,tCurrentPosition.x,tCurrentPosition.z)) < 1.f)
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

void vfGetIntersectionPoints(NodeCompressed &Node, SContour tCurContour, Fvector tStartPoint, Fvector tFinishPoint, Fvector &tPoint)
{
	u32 dwIntersect, dwCount = 0;
	Fvector tTravelNode, tPoints[4];
	dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tCurContour.v1.x,tCurContour.v1.z,tCurContour.v2.x,tCurContour.v2.z,&tTravelNode.x,&tTravelNode.z);
	if (dwIntersect == LI_INTERSECT)
		tPoints[dwCount++] = tTravelNode;
	else
		if (dwIntersect == LI_EQUAL) {
			tPoints[dwCount++] = tCurContour.v1;
			tPoints[dwCount++] = tCurContour.v2;
		}

	dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tCurContour.v3.x,tCurContour.v3.z,tCurContour.v2.x,tCurContour.v2.z,&tTravelNode.x,&tTravelNode.z);
	if (dwIntersect == LI_INTERSECT)
		tPoints[dwCount++] = tTravelNode;
	else
		if (dwIntersect == LI_EQUAL) {
			tPoints[dwCount++] = tCurContour.v3;
			tPoints[dwCount++] = tCurContour.v2;
		}
	
	if (dwCount < 4) {
		dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tCurContour.v3.x,tCurContour.v3.z,tCurContour.v4.x,tCurContour.v4.z,&tTravelNode.x,&tTravelNode.z);
		if (dwIntersect == LI_INTERSECT)
			tPoints[dwCount++] = tTravelNode;
		else
			if (dwIntersect == LI_EQUAL) {
				tPoints[dwCount++] = tCurContour.v3;
				tPoints[dwCount++] = tCurContour.v4;
			}
	}
	
	if (dwCount < 4) {
		dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tCurContour.v1.x,tCurContour.v1.z,tCurContour.v4.x,tCurContour.v4.z,&tTravelNode.x,&tTravelNode.z);
		if (dwIntersect == LI_INTERSECT)
			tPoints[dwCount++] = tTravelNode;
		else
			if (dwIntersect == LI_EQUAL) {
				tPoints[dwCount++] = tCurContour.v1;
				tPoints[dwCount++] = tCurContour.v4;
			}
	}
	for (int i=0; i<(int)dwCount; i++)
		tPoints[i].y = ffGetY(Node,tPoints[i].x,tPoints[i].z);

	switch (dwCount) {
		case 0 : {
			//R_ASSERT(false);
			throw("");
			break;
		}
		case 1 : {
			tPoint = tPoints[0];
			break;
		}
		case 2 : {
			if (tFinishPoint.distance_to(tPoints[0]) < tFinishPoint.distance_to(tPoints[1]))
				tPoint = tPoints[0];
			else
				tPoint = tPoints[1];
			break;
		}
		case 3 : {
			if (tFinishPoint.distance_to(tPoints[0]) < tFinishPoint.distance_to(tPoints[1]))
				if (tFinishPoint.distance_to(tPoints[0]) < tFinishPoint.distance_to(tPoints[2]))
					tPoint = tPoints[0];
				else
					tPoint = tPoints[2];
			else
				if (tFinishPoint.distance_to(tPoints[1]) < tFinishPoint.distance_to(tPoints[2]))
					tPoint = tPoints[1];
				else
					tPoint = tPoints[2];
			break;
		}
		case 4 : {
			if (tFinishPoint.distance_to(tPoints[0]) < tFinishPoint.distance_to(tPoints[1]))
				if (tFinishPoint.distance_to(tPoints[0]) < tFinishPoint.distance_to(tPoints[2]))
					if (tFinishPoint.distance_to(tPoints[0]) < tFinishPoint.distance_to(tPoints[3]))
						tPoint = tPoints[0];
					else
						tPoint = tPoints[3];
				else
					if (tFinishPoint.distance_to(tPoints[2]) < tFinishPoint.distance_to(tPoints[3]))
						tPoint = tPoints[2];
					else
						tPoint = tPoints[3];
			else
				if (tFinishPoint.distance_to(tPoints[1]) < tFinishPoint.distance_to(tPoints[2]))
					if (tFinishPoint.distance_to(tPoints[1]) < tFinishPoint.distance_to(tPoints[3]))
						tPoint = tPoints[1];
					else
						tPoint = tPoints[3];
				else
					if (tFinishPoint.distance_to(tPoints[2]) < tFinishPoint.distance_to(tPoints[3]))
						tPoint = tPoints[2];
					else
						tPoint = tPoints[3];
			break;
		}
		default : NODEFAULT;
	}
}

IC bool bfBetweenPoints(SSegment tSegment, Fvector tPoint)
{
	return(((tSegment.v1.x - EPS_L <= tPoint.x) && (tSegment.v1.z - EPS_L <= tPoint.z) && (tSegment.v2.x + EPS_L >= tPoint.x) && (tSegment.v2.z + EPS_L >= tPoint.z)) || ((tSegment.v2.x - EPS_L <= tPoint.x) && (tSegment.v2.z - EPS_L <= tPoint.z) && (tSegment.v1.x + EPS_L >= tPoint.x) && (tSegment.v1.z + EPS_L >= tPoint.z)));
}

float ffCheckPositionInDirection(u32 dwStartNode, Fvector tStartPoint, Fvector tFinishPoint, float fMaxDistance)
{
	NodeCompressed *tpNode;
	NodeLink *taLinks;
	SContour tCurContour, tNextContour;
	SSegment tSegment;
	int i, iNodeIndex, iCount, iSavedIndex;
	Fvector tPrevPoint, tPoint;
	float fCurDistance = 0.f, fCumulativeDistance = 0.f, fPrevDistance = m_header.size;
	u32 dwCurNode, dwPrevNode = u32(-1);

	dwCurNode = dwStartNode;
	tPrevPoint = tStartPoint;

	while (!bfInsideNode(m_nodes_ptr[dwCurNode],tFinishPoint)) {
		if (fCumulativeDistance > fMaxDistance)
			return(fCumulativeDistance);
		UnpackContour(tCurContour,dwCurNode);
		tpNode = Node(dwCurNode);
//		if (dwCurNode == 60199) {
//			dwCurNode = dwCurNode;
//		}
		vfGetIntersectionPoints(*tpNode,tCurContour,tStartPoint,tFinishPoint,tPoint);
		taLinks = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
		iCount = tpNode->links;
		iSavedIndex = -1;
		for ( i=0; i < iCount; i++) {
			iNodeIndex = UnpackLink(taLinks[i]);
			UnpackContour(tNextContour,iNodeIndex);
			vfIntersectContours(tSegment,tCurContour,tNextContour);
			if (bfBetweenPoints(tSegment,tPoint) && (iNodeIndex != (int)dwPrevNode)) {
				iSavedIndex = iNodeIndex;
				break;
			}
		}

		if (iSavedIndex > -1) {
			fCumulativeDistance += tPrevPoint.distance_to(tPoint);
			fCurDistance = tStartPoint.distance_to_xz(tPoint);
			tPrevPoint = tPoint;
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
	if (bfInsideNode(m_nodes_ptr[dwCurNode],tFinishPoint)) {
		fCumulativeDistance += tPrevPoint.distance_to(tFinishPoint);
		return(fCumulativeDistance);
	}
	else
		return(MAX_VALUE);
}
