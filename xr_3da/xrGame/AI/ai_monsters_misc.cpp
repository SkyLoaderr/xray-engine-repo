////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_monsters_misc.cpp
//	Created 	: 23.07.2002
//  Modified 	: 23.07.2002
//	Author		: Dmitriy Iassenev
//	Description : Miscellanious routines for monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_monsters_misc.h"
#include "..\\custommonster.h"
#include "..\\actor.h"
#include "..\\ai_funcs.h"

using namespace AI;

extern void	UnpackContour		(CPathNodes::PContour& C, u32 ID);
extern void	IntersectContours	(CPathNodes::PSegment& Dest, CPathNodes::PContour& C1, CPathNodes::PContour& C2);

#define COMPUTE_DISTANCE_2D(t,p) (sqrtf(_sqr((t).x - (p).x) + _sqr((t).z - (p).z)))

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
	if ( _abs(r1*r2)<EPS_S && _abs(r3*r4)<EPS_S ) return LI_EQUAL;

	/* Line segments intersect: compute intersection point. 
	*/
	
	denom = a1 * b2 - a2 * b1;
	if ( _abs(denom) < EPS ) return ( LI_COLLINEAR );
	
	num = b1 * c2 - b2 * c1;
	*x = num / denom;
	
	num = a2 * c1 - a1 * c2;
	*y = num / denom;
	
	return ( LI_INTERSECT );
} /* lines_intersect */

IC bool bfSimilar(Fvector &tPoint0, Fvector &tPoint1)
{
	return((_abs(tPoint0.x - tPoint1.x) < EPS_L) && (_abs(tPoint0.z - tPoint1.z) < EPS_L));
}

IC bool bfInsideContour(Fvector &tPoint, CPathNodes::PContour &tContour)
{
	return((tContour.v1.x - EPS_L <= tPoint.x) && (tContour.v1.z - EPS_L <= tPoint.z) && (tContour.v3.x + EPS_L >= tPoint.x) && (tContour.v3.z + EPS_L >= tPoint.z));
}

IC void vfIntersectContours(CPathNodes::PSegment &tSegment, CPathNodes::PContour &tContour0, CPathNodes::PContour &tContour1)
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

void vfGoToPointViaNodes(vector<CPathNodes::CTravelNode> &tpaPath, u32 dwCurNode, Fvector tStartPoint, Fvector tFinishPoint)
{
	NodeCompressed *tpNode;
	CPathNodes::PContour tCurContour,tNextContour;
	NodeLink *taLinks;
	int iCount, iSavedIndex, iNodeIndex, i;
	Fvector tTempPoint;
	CPathNodes::CTravelNode tTravelNode;
	CAI_Space &AI = Level().AI;
	CPathNodes::PSegment tSegment;

	tpaPath.clear();
	UnpackContour(tCurContour,dwCurNode);
	tpNode = AI.Node(dwCurNode);
	taLinks = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
	iCount = tpNode->links;
	iSavedIndex = -1;
	tTempPoint = tStartPoint;
	for ( i=0; i < iCount; i++) {
		iNodeIndex = AI.UnpackLink(taLinks[i]);
		UnpackContour(tNextContour,iNodeIndex);
		vfIntersectContours(tSegment,tCurContour,tNextContour);
		u32 dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tSegment.v1.x,tSegment.v1.z,tSegment.v2.x,tSegment.v2.z,&tTravelNode.P.x,&tTravelNode.P.z);
		if (dwIntersect == LI_INTERSECT) {
			if (
				(COMPUTE_DISTANCE_2D(tFinishPoint,tTravelNode.P) < COMPUTE_DISTANCE_2D(tFinishPoint,tTempPoint) + EPS)
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
		tpaPath.push_back(tTravelNode);
		dwCurNode = iSavedIndex;
	}
	else
		if (AI.bfInsideNode(tpNode,tFinishPoint)) {
			tTravelNode.P = tFinishPoint;
			tTravelNode.P.y = ffGetY(*(tpNode),tTravelNode.P.x,tTravelNode.P.z);
		}
		else {
			tpaPath.clear();
			return;
		}
}

IC	float ffGetSquare(float a1, float b1, float fAlpha = PI_DIV_2)
{
	float a = 2*(b1 - a1)/PI, b = a1;	
	return(fAlpha*fAlpha*fAlpha*a*a/6 + fAlpha*fAlpha*a*b/2 + fAlpha*b*b/2);
}

float ffCalcSquare(float fAngle, float fAngleOfView, float b0, float b1, float b2, float b3)
{
	fAngle -= PI_DIV_2;

	while (fAngle >= PI_MUL_2)
		fAngle -= PI_MUL_2;
	
	while (fAngle < 0)
		fAngle += PI_MUL_2;
	
	float fSquare, bx, by;
	
	if (fAngle < PI_DIV_2) {
		if (fAngle + fAngleOfView >= PI_DIV_2) {
			if (fAngle - fAngleOfView < 0) {
				fSquare = ffGetSquare(b2,b1);
				fSquare += ffGetSquare(b1,b0,fAngle + fAngleOfView - PI_DIV_2);
				fSquare += ffGetSquare(b3,b2,-(fAngle - fAngleOfView));
			}
			else {
				fSquare = ffGetSquare(b1,b2,PI_DIV_2 - (fAngle - fAngleOfView));
				fSquare += ffGetSquare(b1,b0,fAngle + fAngleOfView - PI_DIV_2);
			}
		}
		else
			if (fAngle - fAngleOfView < 0) {
				fSquare = ffGetSquare(b2,b1,fAngle + fAngleOfView);
				fSquare += ffGetSquare(b3,b2,-(fAngle - fAngleOfView));
			}
			else {
				fSquare = ffGetSquare(b2,b1,fAngle + fAngleOfView);
				fSquare -= ffGetSquare(b1,b0,fAngle - fAngleOfView);
			}
	}
	else
		if (fAngle < PI) {
			fAngle -= PI_DIV_2;
			bx = b2;
			b2 = b3;
			b1 = bx;
			b0 = b1;
			b3 = b0;
			if (fAngle + fAngleOfView >= PI_DIV_2) {
				if (fAngle - fAngleOfView < 0) {
					fSquare = ffGetSquare(b2,b1);
					fSquare += ffGetSquare(b1,b0,fAngle + fAngleOfView - PI_DIV_2);
					fSquare += ffGetSquare(b3,b2,-(fAngle - fAngleOfView));
				}
				else {
					fSquare = ffGetSquare(b1,b2,PI_DIV_2 - (fAngle - fAngleOfView));
					fSquare += ffGetSquare(b1,b0,fAngle + fAngleOfView - PI_DIV_2);
				}
			}
			else
				if (fAngle - fAngleOfView < 0) {
					fSquare = ffGetSquare(b2,b1,fAngle + fAngleOfView);
					fSquare += ffGetSquare(b3,b2,-(fAngle - fAngleOfView));
				}
				else {
					fSquare = ffGetSquare(b2,b1,fAngle + fAngleOfView);
					fSquare -= ffGetSquare(b1,b0,fAngle - fAngleOfView);
				}
		}
		else
			if (fAngle < 3*PI_DIV_2) {
				fAngle -= PI;
				bx = b2;
				by = b1;
				b2 = b0;
				b1 = b3;
				b0 = bx;
				b3 = by;
				if (fAngle + fAngleOfView >= PI_DIV_2) {
					if (fAngle - fAngleOfView < 0) {
						fSquare = ffGetSquare(b2,b1);
						fSquare += ffGetSquare(b1,b0,fAngle + fAngleOfView - PI_DIV_2);
						fSquare += ffGetSquare(b3,b2,-(fAngle - fAngleOfView));
					}
					else {
						fSquare = ffGetSquare(b1,b2,PI_DIV_2 - (fAngle - fAngleOfView));
						fSquare += ffGetSquare(b1,b0,fAngle + fAngleOfView - PI_DIV_2);
					}
				}
				else
					if (fAngle - fAngleOfView < 0) {
						fSquare = ffGetSquare(b2,b1,fAngle + fAngleOfView);
						fSquare += ffGetSquare(b3,b2,-(fAngle - fAngleOfView));
					}
					else {
						fSquare = ffGetSquare(b2,b1,fAngle + fAngleOfView);
						fSquare -= ffGetSquare(b1,b0,fAngle - fAngleOfView);
					}
			}
			else {
				fAngle -= 3*PI_DIV_2;
				bx = b2;
				b2 = b1;
				b1 = b0;
				b0 = b3;
				b3 = bx;
				if (fAngle + fAngleOfView >= PI_DIV_2) {
					if (fAngle - fAngleOfView < 0) {
						fSquare = ffGetSquare(b2,b1);
						fSquare += ffGetSquare(b1,b0,fAngle + fAngleOfView - PI_DIV_2);
						fSquare += ffGetSquare(b3,b2,-(fAngle - fAngleOfView));
					}
					else {
						fSquare = ffGetSquare(b1,b2,PI_DIV_2 - (fAngle - fAngleOfView));
						fSquare += ffGetSquare(b1,b0,fAngle + fAngleOfView - PI_DIV_2);
					}
				}
				else
					if (fAngle - fAngleOfView < 0) {
						fSquare = ffGetSquare(b2,b1,fAngle + fAngleOfView);
						fSquare += ffGetSquare(b3,b2,-(fAngle - fAngleOfView));
					}
					else {
						fSquare = ffGetSquare(b2,b1,fAngle + fAngleOfView);
						fSquare -= ffGetSquare(b1,b0,fAngle - fAngleOfView);
					}
			}
	return(fSquare);
}

#define NORMALIZE_NODE_COVER(a,b) (float(a->cover[b])/255.f)

float ffCalcSquare(float fAngle, float fAngleOfView, NodeCompressed *tpNode)
{
	return(ffCalcSquare(fAngle, fAngleOfView,NORMALIZE_NODE_COVER(tpNode,0),NORMALIZE_NODE_COVER(tpNode,1),NORMALIZE_NODE_COVER(tpNode,2),NORMALIZE_NODE_COVER(tpNode,3)));
}

float ffCalcSquare(float fAngle, float fAngleOfView, u32 dwNodeID)
{
	return(ffCalcSquare(fAngle, fAngleOfView,Level().AI.Node(dwNodeID)));
}

float ffGetCoverInDirection(float fAngle, float b0, float b1, float b2, float b3)
{
	fAngle -= PI_DIV_2;

	while (fAngle >= PI_MUL_2)
		fAngle -= PI_MUL_2;
	
	while (fAngle < 0)
		fAngle += PI_MUL_2;
	
	float fResult, bx, by;
	
	if (fAngle < PI_DIV_2)
		;
	else
		if (fAngle < PI) {
			fAngle -= PI_DIV_2;
			bx = b2;
			b2 = b3;
			b1 = bx;
			b0 = b1;
			b3 = b0;
		}
		else
			if (fAngle < 3*PI_DIV_2) {
				fAngle -= PI;
				bx = b2;
				by = b1;
				b2 = b0;
				b1 = b3;
				b0 = bx;
				b3 = by;
			}
			else {
				fAngle -= 3*PI_DIV_2;
				bx = b2;
				b2 = b1;
				b1 = b0;
				b0 = b3;
				b3 = bx;
			}
	fResult = (b1 - b2)*fAngle/PI_DIV_2 + b2;
	return(fResult);
}

float ffGetCoverInDirection(float fAngle, NodeCompressed *tpNode)
{
	return(ffGetCoverInDirection(fAngle, NORMALIZE_NODE_COVER(tpNode,0),NORMALIZE_NODE_COVER(tpNode,1),NORMALIZE_NODE_COVER(tpNode,2),NORMALIZE_NODE_COVER(tpNode,3)));
}

float ffGetCoverInDirection(float fAngle, u32 dwNodeID)
{
	return(ffGetCoverInDirection(fAngle, Level().AI.Node(dwNodeID)));
}

int ifFindNearestPatrolPoint(vector<Fvector> &tpaVector, const Fvector &tPosition)
{
	float fMinDistance = 1000.f, fCurDistance;
	for (int i=0, iIndex = 0; i<(int)(tpaVector.size()); i++)
		if ((fCurDistance = tPosition.distance_to(tpaVector[i])) < fMinDistance) {
			fMinDistance = fCurDistance;
			iIndex = i;
		}
	return(iIndex);
}

bool bfGetActionSuccessProbability(EntityVec &Members, objVisible &VisibleEnemies, float fMinProbability, CBaseFunction &fSuccessProbabilityFunction)
{
	int i = 0, j = 0, I = (int)Members.size(), J = (int)VisibleEnemies.size();
	while ((i < I) && (j < J)) {
		Level().m_tpAI_DDD->m_tpCurrentMember = dynamic_cast<CEntityAlive *>(Members[i]);
		if (!(Level().m_tpAI_DDD->m_tpCurrentMember) || !(Level().m_tpAI_DDD->m_tpCurrentMember->g_Alive())) {
			i++;
			continue;
		}
		Level().m_tpAI_DDD->m_tpCurrentEnemy = dynamic_cast<CEntityAlive *>(VisibleEnemies[j].key);
		if (!(Level().m_tpAI_DDD->m_tpCurrentEnemy) || !(Level().m_tpAI_DDD->m_tpCurrentEnemy->g_Alive())) {
			j++;
			continue;
		}
		float fProbability = fSuccessProbabilityFunction.ffGetValue()/100.f, fCurrentProbability;
		if (fProbability > fMinProbability) {
			fCurrentProbability = fProbability;
			for (j++; (i < I) && (j < J); j++) {
				Level().m_tpAI_DDD->m_tpCurrentEnemy = dynamic_cast<CEntityAlive *>(VisibleEnemies[j].key);
				if (!(Level().m_tpAI_DDD->m_tpCurrentEnemy) || !(Level().m_tpAI_DDD->m_tpCurrentEnemy->g_Alive())) {
					j++;
					continue;
				}
				fProbability = fSuccessProbabilityFunction.ffGetValue()/100.f;
				if (fCurrentProbability*fProbability < fMinProbability) {
					i++;
					break;
				}
				else
					fCurrentProbability *= fProbability;
			}
		}
		else {
			fCurrentProbability = 1.0f - fProbability;
			for (i++; (i < I) && (j < J); i++) {
				Level().m_tpAI_DDD->m_tpCurrentMember = dynamic_cast<CEntityAlive *>(Members[i]);
				if (!(Level().m_tpAI_DDD->m_tpCurrentMember) || !(Level().m_tpAI_DDD->m_tpCurrentMember->g_Alive())) {
					i++;
					continue;
				}
				fProbability = 1.0f - fSuccessProbabilityFunction.ffGetValue()/100.f;
				if (fCurrentProbability*fProbability < fMinProbability) {
					j++;
					break;
				}
				else
					fCurrentProbability *= fProbability;
			}
		}
	}
	return(j >= J);
}

u32 dwfChooseAction(u32 dwActionRefreshRate, float fMinProbability, u32 dwTeam, u32 dwSquad, u32 dwGroup, u32 a1, u32 a2, u32 a3)
{
	//return(a1);
	CGroup &Group = Level().Teams[dwTeam].Squads[dwSquad].Groups[dwGroup];
	
	if (Level().timeServer() - Group.m_dwLastActionTime < dwActionRefreshRate) {
		switch (Group.m_dwLastAction) {
			case 0: return(a1);
			case 1: return(a2);
			default: return(a3);
		}
	}

	objVisible &VisibleEnemies = Level().Teams[dwTeam].Squads[dwSquad].KnownEnemys;
	
	if (!VisibleEnemies.size())
		switch (Group.m_dwLastAction) {
			case 0: return(a1);
			case 1: return(a2);
			default: return(a3);
		}

#pragma todo("Leadership changed")
	EntityVec	Members;
	Members.push_back(Level().Teams[dwTeam].Squads[dwSquad].Leader);
	for (int k=0; k<(int)Group.Members.size(); k++) {
		if (Group.Members[k]->g_Alive())
			Members.push_back(Group.Members[k]);
	}

	WRITE_QUERY_TO_LOG("\nNew query");
	if (bfGetActionSuccessProbability(Members,VisibleEnemies,fMinProbability,Level().m_tpAI_DDD->pfAttackSuccessProbability)) {
		Group.m_dwLastActionTime = Level().timeServer();
		Group.m_dwLastAction = 0;
		WRITE_QUERY_TO_LOG("Attack");
		return(a1);
	}
	else
		if (bfGetActionSuccessProbability(Members,VisibleEnemies,fMinProbability,Level().m_tpAI_DDD->pfDefendSuccessProbability)) {
			Group.m_dwLastActionTime = Level().timeServer();
			Group.m_dwLastAction = 1;
			WRITE_QUERY_TO_LOG("Defend");
			return(Group.m_dwLastAction = a2);
		}
		else {
			Group.m_dwLastActionTime = Level().timeServer();
			Group.m_dwLastAction = 2;
			WRITE_QUERY_TO_LOG("Retreat");
			return(Group.m_dwLastAction = a3);
		}
}

Fvector	tfComputeSpringPull(Fvector &tCurrentPosition, Fvector &tSpringPosition, float fInflexibilityCoefficient)
{
	Fvector tResult;
	tResult.sub(tCurrentPosition,tSpringPosition);
	tResult.y = 0.f;
	float m = tResult.magnitude();
	float F = 1/(fInflexibilityCoefficient*m*m/2);
	tResult.mul(fabsf(fInflexibilityCoefficient*m) > EPS_L ? F*m : 0.f);
	return(tResult);
}

Fvector tfGetNextCollisionPosition(CCustomMonster *tpCustomMonster, Fvector &tFuturePosition)
{
	Fvector	tForcePosition = tfComputeSpringPull(tpCustomMonster->Position(),tFuturePosition,-10.f*tpCustomMonster->m_fCurSpeed);
	
	for (int i=0; i<(int)tpCustomMonster->feel_touch.size(); i++)
		tForcePosition.add(tfComputeSpringPull(tpCustomMonster->Position(),tpCustomMonster->feel_touch[i]->Position(),100.f));

	Fvector tResult;
	tResult.add(tpCustomMonster->Position(),tForcePosition);
	return(tResult);
}