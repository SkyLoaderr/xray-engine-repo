////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_space_obstacles.cpp
//	Created 	: 02.11.2003
//  Modified 	: 02.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Space obstacles solver
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_space.h"

#ifdef DEBUG

#include "_fbox.h"

#pragma warning(disable:4995)
#pragma warning(disable:4267)

#include "ConvexHull3D.h"
//#include "Triangle3D.h"

#pragma warning(default:4267)
#pragma warning(default:4995)

void CAI_Space::dbg_DrawOBBVertices(Fmatrix &T, Fvector &half_dim, u32 C)
{
	Fmatrix mL2W_Transform,mScaleTransform;

	mScaleTransform.scale(half_dim);
	mL2W_Transform.mul_43(T,mScaleTransform);

	Fvector aabb[8];
	aabb[0].set( -1, -1, -1); // 0
	aabb[1].set( -1, +1, -1); // 1
	aabb[2].set( +1, +1, -1); // 2
	aabb[3].set( +1, -1, -1); // 3
	aabb[4].set( -1, -1, +1); // 4
	aabb[5].set( -1, +1, +1); // 5
	aabb[6].set( +1, +1, +1); // 6
	aabb[7].set( +1, -1, +1); // 7

	//	u16             aabb_id[12*2] = {
	//		0,1,  1,2,  2,3,  3,0,  4,5,  5,6,  6,7,  7,4,  1,5,  2,6,  3,7,  0,4
	//	};
	for (int i=0; i<8; i++) {
		Fmatrix						V = Fidentity;
		mL2W_Transform.transform_tiny(V.c,aabb[i]);
		V.i.x						= .05f;
		V.j.y						= .05f;
		V.k.z						= .05f;
		RCache.dbg_DrawEllipse		(V,C);
	}
}

void CAI_Space::DrawTravelLine()
{
	DrawDynamicObstacles				();
	if (!m_tpTravelLine.empty()) {
		Fvector							P = m_tpTravelLine[0];
		P.y								+= .1f;
		RCache.dbg_DrawAABB				(P,.1f,.1f,.1f,D3DCOLOR_XRGB(0,0,255));
	}
	for (u32 I=1; I<m_tpTravelLine.size(); I++) {
		Fvector	P1, P2; 
		P1.set							(m_tpTravelLine[I - 1]);
		P2.set							(m_tpTravelLine[I]);

		P1.y							+= 0.1f;
		P2.y							+= 0.1f;

		RCache.dbg_DrawLINE				(Fidentity,P1,P2,D3DCOLOR_XRGB(0,255,0));

		if (I == (m_tpTravelLine.size() - 1 ))
			RCache.dbg_DrawAABB			(P1,.1f,.1f,.1f,D3DCOLOR_XRGB(255,0,0));
		else 
			RCache.dbg_DrawAABB			(P1,.1f,.1f,.1f,D3DCOLOR_XRGB(0,0,255));

		Fvector         T;
		Fvector4        S;

		T.set							(m_tpTravelLine[I]);
		T.y								+= (1.5f);

		Device.mFullTransform.transform (S,T);
	}
}

void CAI_Space::ComputeTravelLine(AI::NodePath &AI_Path, u32 dwStartNodeID, u32 dwFinishNodeID)
{
	Fvector						*tpDestinationPosition = &m_tFinishPoint;

	if (tpDestinationPosition && bfInsideNode(Node(dwFinishNodeID),*tpDestinationPosition))
		tpDestinationPosition->y = ffGetY(*Node(dwFinishNodeID),tpDestinationPosition->x,tpDestinationPosition->z);

	m_tpaPoints.clear			();
	m_tpaDeviations.clear		();
	m_tpaTravelPath.clear		();
	m_tpaPointNodes.clear		();

	u32							N = (int)AI_Path.Nodes.size();
	if (!N) {
		Msg						("! Node list is empty!");
		AI_Path.Nodes.clear		();
		m_tpTravelLine.clear	();
		return;
	}

	ComputeDynamicObstacles		();

	Fvector						tStartPosition = m_tStartPoint;
	u32							dwCurNode = dwStartNodeID;
	m_tpaPoints.push_back		(tStartPosition);
	m_tpaPointNodes.push_back	(dwCurNode);

	for (u32 i=1; i<=N; i++)
		if (i<N) {
			if (!bfCheckNodeInDirection(dwCurNode,tStartPosition,AI_Path.Nodes[i])) {
				if (dwCurNode != AI_Path.Nodes[i - 1])
					m_tpaPoints.push_back(tStartPosition = tfGetNodeCenter(dwCurNode = AI_Path.Nodes[--i]));
				else
					m_tpaPoints.push_back(tStartPosition = tfGetNodeCenter(dwCurNode = AI_Path.Nodes[i]));
				m_tpaPointNodes.push_back(dwCurNode);
			}
		}
		else
			if (tpDestinationPosition)
				if (dwfCheckPositionInDirection(dwCurNode,tStartPosition,*tpDestinationPosition) == u32(-1)) {
					if (dwCurNode != dwFinishNodeID)
						m_tpaPointNodes.push_back(dwFinishNodeID);
					m_tpaPoints.push_back(*tpDestinationPosition);
				}
				else {
					dwCurNode = dwFinishNodeID;
					if (bfInsideNode(Node(dwCurNode),*tpDestinationPosition)) {
						tpDestinationPosition->y = ffGetY(*Node(dwFinishNodeID),tpDestinationPosition->x,tpDestinationPosition->z);
						m_tpaPointNodes.push_back(dwFinishNodeID);
						m_tpaPoints.push_back(*tpDestinationPosition);
					}
				}

				m_tpaDeviations.resize	(N = (int)m_tpaPoints.size());

				m_tpTravelLine.clear();

				AI_Path.Nodes.clear		();

				Fvector			T;
				T.set			(0,0,0);
				for (i=1; i<N; i++) {
					m_tpaLine.clear();
					m_tpaLine.push_back(m_tpaPoints[i-1]);
					m_tpaLine.push_back(m_tpaPoints[i]);
					bfCreateStraightPTN_Path(m_tpaPointNodes[i-1],m_tpaPoints[i-1],m_tpaPoints[i],m_tpaTravelPath,m_tpaNodes, i == 1);
					u32 n = (u32)m_tpaTravelPath.size();
					for (u32 j= 0; j<n; j++) {
						T = m_tpaTravelPath[j];
						m_tpTravelLine.push_back(T);
						AI_Path.Nodes.push_back(m_tpaNodes[j]);
					}
				}
				if (N > 1) {
					AI_Path.Nodes[AI_Path.Nodes.size() - 1] = dwFinishNodeID;
					if (tpDestinationPosition && m_tpTravelLine.size() && m_tpTravelLine[m_tpTravelLine.size() - 1].distance_to(*tpDestinationPosition) > EPS_L) {
						if (bfInsideNode(Node(dwFinishNodeID),*tpDestinationPosition) && dwfCheckPositionInDirection(dwFinishNodeID,T,*tpDestinationPosition) != -1) {
							T = *tpDestinationPosition;
							m_tpTravelLine.push_back(T);
						}
					}
				}
				else
					if (tpDestinationPosition && m_tpTravelLine.size() && m_tpTravelLine[m_tpTravelLine.size() - 1].distance_to(*tpDestinationPosition) > EPS_L)
						if (bfInsideNode(Node(dwFinishNodeID),*tpDestinationPosition) && dwfCheckPositionInDirection(dwFinishNodeID,T,*tpDestinationPosition) != -1)
							m_tpTravelLine.push_back(*tpDestinationPosition);
}

#include "a_star.h"
void CAI_Space::ComputePath()
{
	u32						l_dwStartNodeID		= q_LoadSearch(m_tStartPoint);
	VERIFY					(bfInsideNode(Node(l_dwStartNodeID),m_tStartPoint));
	u32						l_dwFinishNodeID	= q_LoadSearch(m_tFinishPoint);
	VERIFY					(bfInsideNode(Node(l_dwFinishNodeID),m_tFinishPoint));
	AI::NodePath			l_tpNodePath;
	m_tpAStar->ffFindMinimalPath(l_dwStartNodeID,l_dwFinishNodeID,l_tpNodePath,false);
	ComputeTravelLine		(l_tpNodePath,l_dwStartNodeID,l_dwFinishNodeID);
}

void CAI_Space::DrawDynamicObstacles()
{
	Level().CurrentEntity()->setEnabled	(false);
	Level().ObjectSpace.GetNearest		(m_tStartPoint,100.f); 
	xr_vector<CObject*>					&tpNearestList = Level().ObjectSpace.q_nearest; 
	Level().CurrentEntity()->setEnabled	(true);

	{
		xr_vector<CObject*>::iterator		I = tpNearestList.begin();
		xr_vector<CObject*>::iterator		E = tpNearestList.end();
		for ( ; I != E; ++I) {
			Fvector							c, d, C2;
			(*I)->Visual()->vis.box.get_CD	(c,d);
			Fmatrix							M = (*I)->XFORM();
			M.transform_tiny				(C2,c);
			M.c								= C2;
			RCache.dbg_DrawOBB				(M,d,color_rgba(0,255,0,255));
			dbg_DrawOBBVertices				(M,d,color_rgba(0,0,255,255));
		}
	}

	{
		xr_vector<CConvexHull*>::iterator	I = m_tpConvexGroups.begin();
		xr_vector<CConvexHull*>::iterator	E = m_tpConvexGroups.end();
		for ( ; I != E; ++I) {
			VERIFY							(!(*I)->m_bAlreadyMerged);
			for (int i=0, n=(*I)->m_tpConvexHull->GetTriangleQuantity(); i<n; i++) {
				CConvexHull3D::Triangle		a = (*I)->m_tpConvexHull->GetTriangle(i);
				RCache.dbg_DrawLINE			(Fidentity,(*I)->m_tpVertices[a.m_aiVertex[0]],(*I)->m_tpVertices[a.m_aiVertex[1]],color_rgba(255,0,0,255));
				RCache.dbg_DrawLINE			(Fidentity,(*I)->m_tpVertices[a.m_aiVertex[0]],(*I)->m_tpVertices[a.m_aiVertex[2]],color_rgba(255,0,0,255));
				RCache.dbg_DrawLINE			(Fidentity,(*I)->m_tpVertices[a.m_aiVertex[2]],(*I)->m_tpVertices[a.m_aiVertex[1]],color_rgba(255,0,0,255));
				RCache.dbg_DrawAABB			((*I)->m_tpVertices[a.m_aiVertex[0]],.1f,.1f,.1f,color_rgba(255,0,0,255));
				RCache.dbg_DrawAABB			((*I)->m_tpVertices[a.m_aiVertex[1]],.1f,.1f,.1f,color_rgba(255,0,0,255));
				RCache.dbg_DrawAABB			((*I)->m_tpVertices[a.m_aiVertex[2]],.1f,.1f,.1f,color_rgba(255,0,0,255));
			}
		}
	}
}

void CAI_Space::PushOBBVertices			(xr_vector<Fvector> &tpVertices, Fmatrix &T, Fvector &half_dim)
{
	Fmatrix								mL2W_Transform,mScaleTransform;
	Fvector								aabb[8], V;

	mScaleTransform.scale				(half_dim);
	mL2W_Transform.mul_43				(T,mScaleTransform);

	aabb[0].set							( -1, -1, -1); // 0
	aabb[1].set							( -1, +1, -1); // 1
	aabb[2].set							( +1, +1, -1); // 2
	aabb[3].set							( +1, -1, -1); // 3
	aabb[4].set							( -1, -1, +1); // 4
	aabb[5].set							( -1, +1, +1); // 5
	aabb[6].set							( +1, +1, +1); // 6
	aabb[7].set							( +1, -1, +1); // 7

	for (int i=0; i<8; i++) {
		mL2W_Transform.transform_tiny	(V,aabb[i]);
		tpVertices.push_back			(V);
	}
}

CAI_Space::CConvexHull::CConvexHull		()
{
	m_tpConvexHull					= 0;
	m_bAlreadyMerged				= false;
}

CAI_Space::CConvexHull::~CConvexHull	()
{
	xr_delete						(m_tpConvexHull);
}

bool CAI_Space::CConvexHull::bfIsCloseEnough(const CConvexHull &tConvexHull, const float fDistance) const
{
	xr_vector<Fvector>::const_iterator	I = m_tpVertices.begin();
	xr_vector<Fvector>::const_iterator	E = m_tpVertices.end();
	for ( ; I != E; ++I) {
		xr_vector<Fvector>::const_iterator	i = tConvexHull.m_tpVertices.begin();
		xr_vector<Fvector>::const_iterator	e = tConvexHull.m_tpVertices.end();
		for ( ; i != e; ++i)
			if ((*I).distance_to(*i) <= fDistance)
				return					(true);
	}
	return								(false);
//	float								fDistanceSqr = _sqr(fDistance);
//	for (int i=0, in=m_tpConvexHull->GetTriangleQuantity(); i<in; ++i) {
//		const CConvexHull3D::Triangle	&ii = m_tpConvexHull->GetTriangle(i);
//		for (int j=0, jn = tConvexHull.m_tpConvexHull->GetTriangleQuantity(); j<jn; ++j) {
//			const CConvexHull3D::Triangle	&jj = tConvexHull.m_tpConvexHull->GetTriangle(j);
//			if (ii.SqrDistance(jj) <= fDistanceSqr)
//				return					(true);
//		}
//	}
//	return								(false);
}

void CAI_Space::CConvexHull::vfMergeConvexHulls(CConvexHull &tConvexHull)
{
	tConvexHull.m_bAlreadyMerged		= true;
	m_tpVertices.insert					(m_tpVertices.end(),tConvexHull.m_tpVertices.begin(),tConvexHull.m_tpVertices.end());
	xr_delete							(m_tpConvexHull);
	xr_delete							(tConvexHull.m_tpConvexHull);
	tConvexHull.m_tpVertices.clear		();
	m_tpConvexHull						= xr_new<CConvexHull3D>(m_tpVertices.size(),&m_tpVertices.front());
}

class CRemoveConvexPredicate {
public:
	bool operator()(const CAI_Space::CConvexHull *tConvexHull) const
	{
		return							(tConvexHull->m_bAlreadyMerged);
	}
};

void CAI_Space::ComputeDynamicObstacles	()
{
	// getting nearest list, i.e. dynamic obstacles
	Level().CurrentEntity()->setEnabled	(false);
	Level().ObjectSpace.GetNearest		(m_tStartPoint,100.f); 
	xr_vector<CObject*>					&l_tpNearestList = Level().ObjectSpace.q_nearest; 
	Level().CurrentEntity()->setEnabled	(true);

	// reserving memory for the vertices
	m_tpConvexGroups.clear				();
	m_tpConvexGroups.resize				(l_tpNearestList.size());
	// iterating on objects to fill the vertex buffer
	{
		xr_vector<CObject*>::const_iterator	I = l_tpNearestList.begin(), B = I;
		xr_vector<CObject*>::const_iterator	E = l_tpNearestList.end();
		for ( ; I != E; ++I) {
			Fvector							c, d, C2;
			(*I)->Visual()->vis.box.get_CD	(c,d);
			Fmatrix							M = (*I)->XFORM();
			M.transform_tiny				(C2,c);
			M.c								= C2;
			m_tpConvexGroups[I - B]			= xr_new<CConvexHull>();
			PushOBBVertices					(m_tpConvexGroups[I - B]->m_tpVertices,M,d);
			m_tpConvexGroups[I - B]->m_tpConvexHull = xr_new<CConvexHull3D>(8,&m_tpConvexGroups[I - B]->m_tpVertices.front());
		}
	}
	{
		for (;;) {
			xr_vector<CConvexHull*>::iterator	I = m_tpConvexGroups.begin(), B = I;
			xr_vector<CConvexHull*>::iterator	E = m_tpConvexGroups.end();
			for ( ; I != E; ++I) {
				if ((*I)->m_bAlreadyMerged)
					continue;
				xr_vector<CConvexHull*>::iterator	i = I + 1;
				for ( ; i != E; ++i) {
					if ((*i)->m_bAlreadyMerged)
						continue;
					if ((*I)->bfIsCloseEnough	(**i,.7f)) {
						(*I)->vfMergeConvexHulls(**i);
						for (int ii=0; ii<(int)m_tpConvexGroups.size(); ii++)
							if (!m_tpConvexGroups[ii]->m_bAlreadyMerged && !m_tpConvexGroups[ii]->m_tpConvexHull) {
								ii = ii;
							}
					}
				}
			}

			B									= remove_if(m_tpConvexGroups.begin(),m_tpConvexGroups.end(),CRemoveConvexPredicate());
			if (B == E)
				break;
//			for ( I = B; I != E; ++I)
//				xr_delete						(*I);
			m_tpConvexGroups.erase				(B,m_tpConvexGroups.end());
		}
	}
}
#endif
