////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_path.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Path finding, curve building, position prediction
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "..\\..\\a_star.h"
#include "..\\ai_monsters_misc.h"

using namespace AI_Biting;
using namespace AI;

#define MIN_RANGE_SEARCH_TIME_INTERVAL	 4000		// 2 сек
#define TIME_TO_SEARCH					60000
#define DODGE_AMPLITUDE					.5f
#define MAX_DODGE_DISTANCE				1.5f

/////////////////////////////////////////////////////////////////////////////////////
// ‘ункци€ InitSelector

void CAI_Biting::vfInitSelector(IBaseAI_NodeEvaluator &S, CSquad &Squad)
{
	S.m_dwCurTime		= m_dwCurrentUpdate;
	S.m_tMe				= this;
	S.m_tpMyNode		= AI_Node;
	S.m_tMyPosition		= Position();


	VisionElem ve;
	GetEnemy(ve);
	//R_ASSERT(ve.obj);
	if (!ve.obj) return;

	S.m_tEnemy			= ve.obj;
	S.m_tEnemyPosition	= ve.position;
	S.m_dwEnemyNode		= ve.node_id;
	S.m_tpEnemyNode		= ve.node;

	S.m_taMembers		= &(Squad.Groups[g_Group()].Members);
	S.m_dwStartNode		= AI_NodeID;		// текущий узел
	S.m_tStartPosition	= Position();
}

/////////////////////////////////////////////////////////////////////////////////////
// ‘ункци€ SearchForBetterPosition
void CAI_Biting::vfSearchForBetterPosition(IBaseAI_NodeEvaluator &tNodeEvaluator, CSquad &Squad, CEntity* &Leader)
{
	Device.Statistic.AI_Range.Begin();	// определение времени вып. функции

	if ((!m_dwLastRangeSearch) || (AI_Path.TravelPath.empty()) || (int(AI_Path.TravelStart) > int(AI_Path.TravelPath.size()) - 4) || (AI_Path.fSpeed < EPS_L) || ((tNodeEvaluator.m_dwCurTime - m_dwLastRangeSearch > MIN_RANGE_SEARCH_TIME_INTERVAL))) {
	//   если запускаетс€ в первый раз || нет пути             ||  

		m_dwLastRangeSearch = tNodeEvaluator.m_dwCurTime;

		float fOldCost = MAX_NODE_ESTIMATION_COST;

		if (AI_Path.DestNode != u32(-1)) {
			tNodeEvaluator.m_tpCurrentNode	= getAI().Node(AI_Path.DestNode);
			tNodeEvaluator.m_fDistance		= Position().distance_to(getAI().tfGetNodeCenter(AI_Path.DestNode));
			fOldCost						= tNodeEvaluator.ffEvaluateNode();
		}

		Squad.Groups[g_Group()].GetAliveMemberInfo(tNodeEvaluator.m_taMemberPositions, tNodeEvaluator.m_taMemberNodes, tNodeEvaluator.m_taDestMemberPositions, tNodeEvaluator.m_taDestMemberNodes, this);

		Device.Statistic.AI_Range.End();

		tNodeEvaluator.vfShallowGraphSearch(getAI().q_mark_bit);

		Device.Statistic.AI_Range.Begin();

		if ((AI_Path.DestNode != tNodeEvaluator.m_dwBestNode) && (tNodeEvaluator.m_fBestCost < fOldCost - 0.f)){
			AI_Path.DestNode		= tNodeEvaluator.m_dwBestNode;
			if (!AI_Path.DestNode) {
				Msg("! Invalid Node Evaluator node");
			}
			AI_Path.Nodes.clear		();
			m_tPathState			= ePathStateBuildNodePath;
			m_bIfSearchFailed		= false;
		} 
		else
			m_bIfSearchFailed		= true;
	}

	Device.Statistic.AI_Range.End();
}

/////////////////////////////////////////////////////////////////////////////////////
// ‘ункци€ BuildPathToDestinationPoint
void CAI_Biting::vfBuildPathToDestinationPoint(IBaseAI_NodeEvaluator *tpNodeEvaluator)
{
	Device.Statistic.AI_Path.Begin();

	if (AI_Path.DestNode == AI_NodeID) {
		AI_Path.Nodes.clear		();
		AI_Path.TravelPath.clear();
		AI_Path.TravelStart		= 0;
		m_fCurSpeed				= 0;
		m_tPathState			= ePathStateSearchNode;
		Device.Statistic.AI_Path.End();
		return;
	}

	if (tpNodeEvaluator)
		getAI().m_tpAStar->ffFindOptimalPath(AI_NodeID,AI_Path.DestNode,AI_Path,tpNodeEvaluator->m_dwEnemyNode,tpNodeEvaluator->m_fOptEnemyDistance);
	else
		getAI().m_tpAStar->ffFindMinimalPath(AI_NodeID,AI_Path.DestNode,AI_Path);

	if (AI_Path.Nodes.empty()) {
		Msg("! !!!! node_start %d, node_finish %d",AI_NodeID,AI_Path.DestNode);
		
		getAI().m_tpAStar->ffFindMinimalPath(AI_NodeID,AI_Path.DestNode,AI_Path);
		
		if (AI_Path.Nodes.empty())
			m_tPathState = ePathStateSearchNode;
		else
			m_tPathState = ePathStateBuildTravelLine;
	}
	else
		m_tPathState = ePathStateBuildTravelLine;

	Device.Statistic.AI_Path.End();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// ѕостроить TravelLine
void CAI_Biting::vfBuildTravelLine(Fvector *tpDestinationPosition)
{
	Device.Statistic.TEST1.Begin();

	m_tpaPoints.clear			();
	m_tpaTravelPath.clear		();
	m_tpaPointNodes.clear		();

	u32							N = (u32)AI_Path.Nodes.size();
	if (!N) {
		Msg("! Node list is empty!");
		AI_Path.Nodes.clear();
		AI_Path.TravelPath.clear();
		m_tPathState = ePathStateSearchNode;
		Device.Statistic.TEST1.End();
		return;
	}
	Fvector						tStartPosition = Position();

	u32							dwCurNode = AI_NodeID;
	m_tpaPoints.push_back		(Position());
	m_tpaPointNodes.push_back	(dwCurNode);

	for (u32 i=1; i<=N; i++)
		if (i<N) {
			if (!getAI().bfCheckNodeInDirection(dwCurNode,tStartPosition,AI_Path.Nodes[i])) {
				if (dwCurNode != AI_Path.Nodes[i - 1])
					m_tpaPoints.push_back(tStartPosition = getAI().tfGetNodeCenter(dwCurNode = AI_Path.Nodes[--i]));
				else
					m_tpaPoints.push_back(tStartPosition = getAI().tfGetNodeCenter(dwCurNode = AI_Path.Nodes[i]));
				m_tpaPointNodes.push_back(dwCurNode);
			}
		}
		else
			if (tpDestinationPosition)
				if (getAI().dwfCheckPositionInDirection(dwCurNode,tStartPosition,*tpDestinationPosition) == u32(-1)) {
					//VERIFY(false);
					if (dwCurNode != AI_Path.DestNode)
						m_tpaPointNodes.push_back(AI_Path.DestNode);
					tpDestinationPosition->y = getAI().ffGetY(*getAI().Node(dwCurNode),tpDestinationPosition->x,tpDestinationPosition->z);
					m_tpaPoints.push_back(*tpDestinationPosition);
				}
				else {
						tpDestinationPosition->y = getAI().ffGetY(*getAI().Node(AI_Path.DestNode),tpDestinationPosition->x,tpDestinationPosition->z);
						m_tpaPointNodes.push_back(AI_Path.DestNode);
						m_tpaPoints.push_back(*tpDestinationPosition);
				}

	if (!tpDestinationPosition && (tStartPosition.distance_to(getAI().tfGetNodeCenter(AI_Path.Nodes[N - 1])) > getAI().Header().size)) {
		m_tpaPoints.push_back(getAI().tfGetNodeCenter(AI_Path.Nodes[N - 1]));
		m_tpaPointNodes.push_back(AI_Path.Nodes[N - 1]);
	}

	N = (u32)m_tpaPoints.size();

	AI_Path.TravelPath.clear();
	AI_Path.Nodes.clear		();

	AI::CTravelNode	T;
	T.floating		= false;
	T.P.set			(0,0,0);
	for (i=1; i<N; i++) {
		m_tpaLine.clear();
		m_tpaLine.push_back(m_tpaPoints[i-1]);
		m_tpaLine.push_back(m_tpaPoints[i]);
		getAI().bfCreateStraightPTN_Path(m_tpaPointNodes[i-1],m_tpaPoints[i-1],m_tpaPoints[i],m_tpaTravelPath,m_tpaNodes, i == 1);
		u32 n = (u32)m_tpaTravelPath.size();
		for (u32 j= 0; j<n; j++) {
			T.P = m_tpaTravelPath[j];
			AI_Path.TravelPath.push_back(T);
			AI_Path.Nodes.push_back(m_tpaNodes[j]);
		}
	}
	
	if (AI_Path.Nodes.size())
		AI_Path.Nodes[AI_Path.Nodes.size() - 1] = AI_Path.DestNode;
	else
		AI_Path.Nodes.push_back(AI_Path.DestNode);
	
	if ((N > 1) && tpDestinationPosition && AI_Path.TravelPath.size() && AI_Path.TravelPath[AI_Path.TravelPath.size() - 1].P.distance_to(*tpDestinationPosition) > EPS_L) {
		if (getAI().bfInsideNode(getAI().Node(AI_Path.DestNode),*tpDestinationPosition) && getAI().dwfCheckPositionInDirection(AI_Path.DestNode,T.P,*tpDestinationPosition) != -1) {
			T.P = *tpDestinationPosition;
			AI_Path.TravelPath.push_back(T);
		}
	}

	AI_Path.TravelStart = 0;
	m_tPathState		= ePathStateSearchNode;

	Device.Statistic.TEST1.End();
}


//---------------------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////
//	Build Travel Line dups
/////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------------------



//******* Distance 2 line segment
IC float	ClosestPointOnSegment(Fvector& Dest, const Fvector& P, const Fvector& A, const Fvector& B)
{
	// Determine t (the length of the xr_vector from СaТ to СpТ)
	Fvector c; c.sub(P,A);
	Fvector V; V.sub(B,A); 
	
	float d = V.magnitude();
	
	V.div	(d); 
	float t = V.dotproduct(c);
	
	// Check to see if СtТ is beyond the extents of the line segment
	if (t <= 0.0f)	{ Dest.set(A); return P.distance_to_sqr(Dest); }
	if (t >= d)		{ Dest.set(B); return P.distance_to_sqr(Dest); }
	
	// Return the point between СaТ and СbТ
	// set length of V to t. V is normalized so this is easy
	Dest.mad(A,V,t);
	return P.distance_to_sqr(Dest);
}

//******* Vertical projection of point onto plane
IC void projectPoint(const Fplane& PL, Fvector& P) 
{	P.y -= PL.classify(P)/PL.n.y; }


//******* PContour unpacking
void	CAI_Biting::UnpackContour(PContour& C, u32 ID)
{
	CAI_Space&	AI			= getAI();
	NodeCompressed* Node	= AI.Node(ID);
	
	// decompress positions
	Fvector P0,P1;
	AI.UnpackPosition		(P0,Node->p0);
	AI.UnpackPosition		(P1,Node->p1);
	
	// decompress plane
	Fplane	PL;	
	pvDecompress			(PL.n,Node->plane);
	PL.d = - PL.n.dotproduct(P0);
	
	// create vertices
	float		st = AI.Header().size/2;
	
	/**/
	C.v1.set(P0.x-st,P0.y,P0.z-st);	projectPoint(PL,C.v1);	// minX,minZ
	C.v2.set(P1.x+st,P0.y,P0.z-st);	projectPoint(PL,C.v2);	// maxX,minZ
	C.v3.set(P1.x+st,P1.y,P1.z+st);	projectPoint(PL,C.v3);	// maxX,maxZ
	C.v4.set(P0.x-st,P1.y,P1.z+st);	projectPoint(PL,C.v4);	// minX,maxZ
}

/**/
IC bool bfInsideContour(Fvector &tPoint, PContour &tContour)
{
	return((tContour.v1.x - EPS_L <= tPoint.x) && (tContour.v1.z - EPS_L <= tPoint.z) && (tContour.v3.x + EPS_L >= tPoint.x) && (tContour.v3.z + EPS_L >= tPoint.z));
}

IC bool bfSimilar(Fvector &tPoint0, Fvector &tPoint1)
{
	return((_abs(tPoint0.x - tPoint1.x) < EPS_L) && (_abs(tPoint0.z - tPoint1.z) < EPS_L));
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
		Msg("! AI_PathNodes: segment has null length ([%6.4f,%6.4f],[%6.4f,%6.4f] -> [%6.4f,%6.4f],[%6.4f,%6.4f])",tContour0.v1.x,tContour0.v1.z,tContour0.v3.x,tContour0.v3.z,tContour1.v1.x,tContour1.v1.z,tContour1.v3.x,tContour1.v3.z);
	}
	else
		Msg("! AI_PathNodes: Can't find intersection segment ([%6.4f,%6.4f],[%6.4f,%6.4f] -> [%6.4f,%6.4f],[%6.4f,%6.4f])",tContour0.v1.x,tContour0.v1.z,tContour0.v3.x,tContour0.v3.z,tContour1.v1.x,tContour1.v1.z,tContour1.v3.x,tContour1.v3.z);
}
/**/

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
	if ( _abs(r1*r2)<EPS_S && _abs(r3*r4)<EPS_S ) return LI_NONE;

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

//******* pseudo 3D segments intersection
const float corner_r = 0.05f;
BOOL CAI_Biting::SegmentsIntersect(Fvector& dst, Fvector& v1, Fvector& v2, Fvector& v3, Fvector& v4)
{
	// corner check (v4 - end, v1-v2 - segm)
	if (v4.similar(v1,corner_r))	{ dst.set(v1); return TRUE; }
	if (v4.similar(v2,corner_r))	{ dst.set(v2); return TRUE; }
	if (v3.similar(v1,corner_r))	{ dst.set(v1); return TRUE; }
	if (v3.similar(v2,corner_r))	{ dst.set(v2); return TRUE; }

	// projected intersection
	Fvector T;
	if (LI_INTERSECT!=lines_intersect(v1.x,v1.z,v2.x,v2.z,v3.x,v3.z,v4.x,v4.z,&T.x,&T.z))	return FALSE;
	
	// unproject into 3D
	float lx	= (v2.x-v1.x);
	float lz	= (v2.z-v1.z);
	float bary1 = (T.x-v1.x)/lx;
	float bary2 = (T.z-v1.z)/lz;
	if (fis_zero(lx,EPS_L))	bary1=bary2;
	if (fis_zero(lz,EPS_L))	bary2=bary1;
	//VERIFY(fsimilar(bary1,bary2,EPS_L));

	float bary	= (bary1+bary2)/2;
	dst.x = T.x;
	dst.y = v1.y + bary*(v2.y-v1.y);
	dst.z = T.z;
	return TRUE;

}


//---------------------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////
//	Build Travel Line dups ENDDDDDDDDDDDDDDDDDDDD
/////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////////////////////////////
// ѕостроить TravelLine
void CAI_Biting::vfBuildTravelLine(const Fvector &tpDestinationPosition, const Fvector& current_pos)
{

	R_ASSERT						(!AI_Path.Nodes.empty());
	CTravelNode						current,next;
	AI_Path.bNeedRebuild			= FALSE;

	// start point
	AI_Path.TravelPath.clear		();
	current.P.set					(current_pos);
	current.floating				= FALSE;
	AI_Path.TravelPath.push_back	(current);

	// end point
	Fvector		Last		= tpDestinationPosition;

	// segmentation
	PContour				Ccur,Cnext;
	UnpackContour			(Ccur,AI_Path.Nodes[0]);
	AI_Path.Segments.clear			();
	for (u32 i=1; i<AI_Path.Nodes.size(); i++)
	{
		PSegment		S;
		UnpackContour		(Cnext,AI_Path.Nodes[i]);
		vfIntersectContours	(S,Ccur,Cnext);
		AI_Path.Segments.push_back	(S);
		Ccur				= Cnext;
	}

	// path building
	for (i=0; i<AI_Path.Segments.size(); i++)
	{
		// build probe point
		PSegment& S = AI_Path.Segments[i];
		ClosestPointOnSegment(next.P,current.P,S.v1,S.v2);

		// select far point
		Fvector fp	= Last;
		if ((i+1)<AI_Path.Segments.size())	{
			PSegment& F = AI_Path.Segments[i+1];
			ClosestPointOnSegment(fp,current.P,F.v1,F.v2);
		}

		// try to cast a line from 'current.P' to 'fp'
		Fvector	ip;
		if (SegmentsIntersect(ip,S.v1,S.v2,current.P,fp)) {
			next.P.set(ip);
		} else {
			// find nearest point to segment 'current.P' to 'fp'
			float d1 = S.v1.distance_to(fp)+S.v1.distance_to(current.P);
			float d2 = S.v2.distance_to(fp)+S.v2.distance_to(current.P);
			if (d1<d2)	next.P.set(S.v1);
			else		next.P.set(S.v2);
		}

		// record _new point
		if (!next.P.similar(AI_Path.TravelPath.back().P))	AI_Path.TravelPath.push_back(next);
		current				= next;
	}

	next.P.set			(Last);
	if (!next.P.similar(AI_Path.TravelPath.back().P))	AI_Path.TravelPath.push_back(next);

	AI_Path.TravelStart				= 0;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////
// ¬ыбор точки, построение пути, построение TravelLine
void CAI_Biting::vfChoosePointAndBuildPath(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDestinationPosition, bool bSearchForNode, bool bSelectorPath, u32 TimeToRebuild)
{
	if (m_tPathState == ePathStateBuilt) {
		if (m_dwPathBuiltLastTime + TimeToRebuild > m_dwCurrentUpdate) return;
		m_tPathState = ePathStateSearchNode;
	}


	INIT_SQUAD_AND_LEADER;

	if (tpNodeEvaluator)
		vfInitSelector			(*tpNodeEvaluator,Squad);

	Fvector tempV;

	switch (m_tPathState) {
		case ePathStateSearchNode : 
			if (tpNodeEvaluator && bSearchForNode)  // необходимо искать ноду?
				vfSearchForBetterPosition(*tpNodeEvaluator,Squad,Leader);
			else
				if (!bSearchForNode || !tpDestinationPosition || !AI_Path.TravelPath.size() || (AI_Path.TravelPath[AI_Path.TravelPath.size() - 1].P.distance_to(*tpDestinationPosition) > EPS_L))
					m_tPathState = ePathStateBuildNodePath;	// 
			break;
									
		case ePathStateBuildNodePath : 
			if ((AI_Path.DestNode != AI_NodeID) && (AI_Path.Nodes.empty() || (AI_Path.Nodes[AI_Path.Nodes.size() - 1] != AI_Path.DestNode) || AI_Path.TravelPath.empty() || ((AI_Path.TravelPath.size() - 1) <= AI_Path.TravelStart)))
				vfBuildPathToDestinationPoint(bSelectorPath ? tpNodeEvaluator : 0);
			else
				if ((AI_Path.DestNode == AI_NodeID) && tpDestinationPosition && (!AI_Path.TravelPath.size() || (tpDestinationPosition->distance_to_xz(AI_Path.TravelPath[AI_Path.TravelPath.size() - 1].P) > EPS_L))) {
					AI_Path.Nodes.clear();
					AI_Path.Nodes.push_back(AI_NodeID);
					m_tPathState = ePathStateBuildTravelLine;
				}
				else
					if (bSearchForNode && tpNodeEvaluator)
						m_tPathState = ePathStateSearchNode;
					else
						if (AI_Path.TravelPath.size() && tpDestinationPosition && (AI_Path.TravelPath[AI_Path.TravelPath.size() - 1].P.distance_to(*tpDestinationPosition) > EPS_L))
							m_tPathState = ePathStateBuildTravelLine;
			break;
									
		case ePathStateBuildTravelLine : 
			
			tempV = ((tpDestinationPosition) ?  *tpDestinationPosition : getAI().tfGetNodeCenter(AI_Path.Nodes.back()));
			//getAI().bfCreateStraightPTN_Path(AI_NodeID,Position(),tempV,m_tpaTravelPath,m_tpaNodes, true);					

			vfBuildTravelLine(tempV,Position());
				
			m_tPathState = ePathStateBuilt;
			m_dwPathBuiltLastTime = m_dwCurrentUpdate;
			break;
										
	}	
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// ¬ыбор следующей точки графа
void CAI_Biting::vfChooseNextGraphPoint()
{
	_GRAPH_ID						tGraphID		= m_tNextGP;
	u16								wNeighbourCount = (u16)getAI().m_tpaGraph[tGraphID].tNeighbourCount;
	CSE_ALifeGraph::SGraphEdge		*tpaEdges		= (CSE_ALifeGraph::SGraphEdge *)((BYTE *)getAI().m_tpaGraph + getAI().m_tpaGraph[tGraphID].dwEdgeOffset);

	int								iPointCount		= (int)m_tpaTerrain.size();
	int								iBranches		= 0;	// количество различных вариантов
	for (int i=0; i<wNeighbourCount; i++)
		for (int j=0; j<iPointCount; j++)
			if (getAI().bfCheckMask(m_tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes) && (tpaEdges[i].dwVertexNumber != m_tCurGP))
				iBranches++;
	if (!iBranches) {
		for (int i=0; i<wNeighbourCount; i++) {
			for (int j=0; j<iPointCount; j++)
				if (getAI().bfCheckMask(m_tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes)) {
					m_tCurGP		= m_tNextGP;
					m_tNextGP		= (_GRAPH_ID)tpaEdges[i].dwVertexNumber;
					m_dwTimeToChange= Level().timeServer() + ::Random.randI(m_tpaTerrain[j].dwMinTime,m_tpaTerrain[j].dwMaxTime);
					return;
				}
			R_ASSERT2(false,"There are no proper graph neighbours!");
		}
	}
	else {
		int							iChosenBranch = ::Random.randI(0,iBranches);
		iBranches					= 0;
		for (int i=0; i<wNeighbourCount; i++) {
			for (int j=0; j<iPointCount; j++)
				if (getAI().bfCheckMask(m_tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes) && (tpaEdges[i].dwVertexNumber != m_tCurGP)) {
					if (iBranches == iChosenBranch) {
						m_tCurGP	= m_tNextGP;
						m_tNextGP	= (_GRAPH_ID)tpaEdges[i].dwVertexNumber;
						m_dwTimeToChange	= Level().timeServer() + ::Random.randI(m_tpaTerrain[j].dwMinTime,m_tpaTerrain[j].dwMaxTime);
						return;
					}
					iBranches++;
				}
		}
	}
}

// ¬ыбор точки графа, в соответствии с выбором лидера
void CAI_Biting::vfUpdateDetourPoint()
{
	if (!g_Alive())
		return;

	INIT_SQUAD_AND_LEADER;

	if (this != Leader)	{
		CAI_Biting *tpLeader			= dynamic_cast<CAI_Biting*>(Leader);
		if (tpLeader) {
			m_tNextGraphPoint			= tpLeader->m_tNextGraphPoint;
			m_tNextGP					= tpLeader->m_tNextGP;
		}
	}
	else
		if ((Level().timeServer() >= m_dwTimeToChange) && (getAI().m_tpaCrossTable[AI_NodeID].tGraphIndex == m_tNextGP)) {
			m_tNextGP					= getAI().m_tpaCrossTable[AI_NodeID].tGraphIndex;
			vfChooseNextGraphPoint		();
			m_tNextGraphPoint.set		(getAI().m_tpaGraph[m_tNextGP].tLocalPoint);
			m_dwTimeToChange			= Level().timeServer() + 2000;
		}
		
}

