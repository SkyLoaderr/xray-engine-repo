////////////////////////////////////////////////////////////////////////////
//	Module 		: level_graph_debug.cpp
//	Created 	: 02.10.2001
//  Modified 	: 11.11.2003
//	Author		: Oles Shihkovtsov, Dmitriy Iassenev
//	Description : Level graph debug functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef DEBUG
#ifndef AI_COMPILER

#include "level_graph.h"
#include "../customhud.h"
#include "level.h"
#include "ai_space.h"
#include "hudmanager.h"
#include "game_graph.h"
#include "game_sv_single.h"
#include "ai_alife.h"
#include "custommonster.h"
#include "ai/stalker/ai_stalker.h"

#define NORMALIZE_VECTOR(t) t.x /= 10.f, t.x += tCameraPosition.x, t.y /= 10.f, t.y += 20.f, t.z /= 10.f, t.z += tCameraPosition.z;
#define DRAW_GRAPH_POINT(t0,c0,c1,c2) {\
	Fvector t1 = (t0);\
	t1.y += .6f;\
	NORMALIZE_VECTOR(t1);\
	RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(c0,c1,c2));\
}

void CLevelGraph::render()
{
	if (!psHUD_Flags.test(HUD_DRAW))
		return;
#ifdef DEBUG
	draw_travel_line();
#endif
	if (psAI_Flags.test(aiBrain)) {
		if (ai().get_level_graph()) {
			if (!Level().CurrentEntity())
				return;

			Fvector tCameraPosition = Level().CurrentEntity()->Position();
			CGameFont* F		= HUD().pFontDI;
			for (int i=0; i<(int)ai().game_graph().header().vertex_count(); ++i) {
				Fvector t1 = ai().game_graph().vertex(i)->game_point();
				t1.y += .6f;
				NORMALIZE_VECTOR(t1);
				RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(0,0,255));
				CGameGraph::const_iterator	I, E;
				ai().game_graph().begin		(i,I,E);
				for ( ; I != E; ++I) {
					Fvector t2 = ai().game_graph().vertex((*I).vertex_id())->game_point();
					t2.y += .6f;
					NORMALIZE_VECTOR(t2);
					RCache.dbg_DrawLINE(Fidentity,t1,t2,D3DCOLOR_XRGB(0,255,0));
				}
				Fvector         T;
				Fvector4        S;
				T.set			(t1);
				//T.y+= 1.5f;
				T.y+= 1.5f/10.f;
				Device.mFullTransform.transform (S,T);
				F->SetSize	(0.05f/_sqrt(_abs(S.w)));
				F->SetColor(0xffffffff);
				F->Out(S.x,-S.y,"%d",i);
			}
//			if ((m_tpAStar) && (m_tpAStar->m_tpaNodes.size())) {
//				Fvector t1 = m_tpaGraph[m_tpAStar->m_tpaNodes[0]].tGlobalPoint;
//				t1.y += .6f;
//				NORMALIZE_VECTOR(t1);
//				RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(0,0,255));
//				for (int i=1; i<(int)m_tpAStar->m_tpaNodes.size(); ++i) {
//					Fvector t2 = m_tpaGraph[m_tpAStar->m_tpaNodes[i]].tGlobalPoint;
//					t2.y += .6f;
//					NORMALIZE_VECTOR(t2);
//					RCache.dbg_DrawAABB(t2,.05f,.05f,.05f,D3DCOLOR_XRGB(0,0,255));
//					RCache.dbg_DrawLINE(Fidentity,t1,t2,D3DCOLOR_XRGB(255,0,0));
//					t1 = t2;
//				}
//				//			i=1;
//				//			for (; m_tpAStar->m_tpIndexes[m_tpAStar->m_tpHeap[i].iIndex].dwTime == m_tpAStar->m_dwAStarStaticCounter; ++i) {
//				//				Fvector t2 = m_tpaGraph[m_tpAStar->m_tpHeap[i].iIndex].tGlobalPoint;
//				//				t2.y += .6f;
//				//				NORMALIZE_VECTOR(t2);
//				//				RCache.dbg_DrawAABB(t2,.05f,.05f,.05f,D3DCOLOR_XRGB(255,0,0));
//				//			}
//			}
			if (Level().game.type == GAME_SINGLE) {
				game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
				if ((tpGame) && (tpGame->m_tpALife)) {
					{
						_LEVEL_ID	J = ai().game_graph().vertex(tpGame->m_tpALife->m_tpActor->m_tGraphID)->level_id();
						for (int i=0, n=(int)ai().game_graph().header().vertex_count(); i<n; ++i) {
							if (ai().game_graph().vertex(i)->level_id() != J)
								continue;
							Fvector t1 = ai().game_graph().vertex(i)->level_point(), t2 = ai().game_graph().vertex(i)->game_point();
							t1.y += .6f;
							t2.y += .6f;
							NORMALIZE_VECTOR(t2);
							RCache.dbg_DrawAABB(t1,.5f,.5f,.5f,D3DCOLOR_XRGB(255,255,255));
							//RCache.dbg_DrawLINE(Fidentity,t1,t2,D3DCOLOR_XRGB(255,255,255));
							Fvector         T;
							Fvector4        S;
							T.set			(t1);
							//T.y+= 1.5f;
							T.y+= 1.5f;
							Device.mFullTransform.transform (S,T);
							F->SetSize	(0.1f/_sqrt(_abs(S.w)));
							F->SetColor(0xffffffff);
							F->Out(S.x,-S.y,"%d",i);
						}
					}

					D_OBJECT_PAIR_IT	I = tpGame->m_tpALife->m_tObjectRegistry.begin();
					D_OBJECT_PAIR_IT	E = tpGame->m_tpALife->m_tObjectRegistry.end();
					for ( ; I != E; ++I) {
						{
							CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract *>((*I).second);
							if (tpALifeMonsterAbstract && tpALifeMonsterAbstract->m_bDirectControl && !tpALifeMonsterAbstract->m_bOnline) {
								CSE_ALifeHumanAbstract *tpALifeHuman = dynamic_cast<CSE_ALifeHumanAbstract *>(tpALifeMonsterAbstract);
								if (tpALifeHuman && tpALifeHuman->m_tpPath.size()) {
									Fvector t1 = ai().game_graph().vertex(tpALifeHuman->m_tpPath[0])->game_point();
									t1.y += .6f;
									NORMALIZE_VECTOR(t1);
									RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(0,0,255));
									for (int i=1; i<(int)tpALifeHuman->m_tpPath.size(); ++i) {
										Fvector t2 = ai().game_graph().vertex(tpALifeHuman->m_tpPath[i])->game_point();
										t2.y += .6f;
										NORMALIZE_VECTOR(t2);
										RCache.dbg_DrawAABB(t2,.05f,.05f,.05f,D3DCOLOR_XRGB(0,0,255));
										RCache.dbg_DrawLINE(Fidentity,t1,t2,D3DCOLOR_XRGB(0,0,255));
										t1 = t2;
									}
								}
								if (tpALifeMonsterAbstract->m_fDistanceToPoint > EPS_L) {
									Fvector t1 = ai().game_graph().vertex(tpALifeMonsterAbstract->m_tGraphID)->game_point();
									Fvector t2 = ai().game_graph().vertex(tpALifeMonsterAbstract->m_tNextGraphID)->game_point();
									t2.sub(t1);
									t2.mul(tpALifeMonsterAbstract->m_fDistanceFromPoint/tpALifeMonsterAbstract->m_fDistanceToPoint);
									t1.add(t2);
									t1.y += .6f;
									NORMALIZE_VECTOR(t1);
									RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(255,0,0));
								}
								else {
									Fvector t1 = ai().game_graph().vertex((*I).second->m_tGraphID)->game_point();
									t1.y += .6f;
									NORMALIZE_VECTOR(t1);
									RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(255,0,0));
								}
							}
							else {
								CSE_ALifeInventoryItem *l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>((*I).second);
								if (l_tpALifeInventoryItem && !l_tpALifeInventoryItem->bfAttached()) {
									Fvector t1 = ai().game_graph().vertex((*I).second->m_tGraphID)->game_point();
									t1.y += .6f;
									NORMALIZE_VECTOR(t1);
									RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(255,255,0));
								}
								else {
									CSE_ALifeCreatureActor *tpALifeCreatureActor = dynamic_cast<CSE_ALifeCreatureActor*>((*I).second);
									if (tpALifeCreatureActor) {
										Fvector t1 = ai().game_graph().vertex((*I).second->m_tGraphID)->game_point();
										t1.y += .6f;
										NORMALIZE_VECTOR(t1);
										RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(255,255,255));
									}
									else {
										CSE_ALifeTrader *tpALifeTrader = dynamic_cast<CSE_ALifeTrader*>((*I).second);
										if (tpALifeTrader) {
											Fvector t1 = ai().game_graph().vertex((*I).second->m_tGraphID)->game_point();
											t1.y += .6f;
											NORMALIZE_VECTOR(t1);
											RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(0,0,0));
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

#ifdef DEBUG
	if (psAI_Flags.test(aiMotion)) {
		xr_vector<CObject*>::iterator	I = Level().Objects.objects.begin();
		xr_vector<CObject*>::iterator	E = Level().Objects.objects.end();
		for ( ; I != E; ++I) {
			CCustomMonster *tpCustomMonster = dynamic_cast<CCustomMonster*>(*I);
			if (tpCustomMonster) {
				tpCustomMonster->OnRender();
				if (!tpCustomMonster->CDetailPathManager::path().empty()) {
					Fvector temp = tpCustomMonster->CDetailPathManager::path()[tpCustomMonster->CDetailPathManager::path().size() - 1].m_position;
					RCache.dbg_DrawAABB(temp,1.f,1.f,1.f,D3DCOLOR_XRGB(0,0,255));
				}
			}
		}
	}
#endif

	if (!bDebug)	return;

	//	if (0==sh_debug)				return;
	if (!psAI_Flags.test(aiDebug))	return;

	CGameObject*	O	= dynamic_cast<CGameObject*> (Level().CurrentEntity());
	Fvector	POSITION	= O->Position();
	POSITION.y += 0.5f;

	// display
	Fvector P			= POSITION;

	CPosition			Local;
	vertex_position		(Local,P);

	u32 ID				= O->level_vertex_id();

	CGameFont* F		= HUD().pFontDI;
	F->SetSize			(.02f);
	F->Out				(0.f,0.5f,"%f,%f,%f",VPUSH(P));
//	float				x,z;
//	unpack_xz			(Local,x,z);
//	F->Out				(0.f,0.55f,"%3d,%4d,%3d -> %d",	iFloor(x),iFloor(Local.y()),iFloor(z),u32(ID));

	svector<u32,128>	linked;
	{
		const_iterator	i,e;
		begin			(ID,i,e);
		for(; i != e; ++i)
			linked.push_back(value(ID,i));
	}

	// render
	float	sc		= header().cell_size()/16;
	float	st		= 0.98f*header().cell_size()/2;
	float	tt		= 0.01f;

	Fvector	DUP;	DUP.set(0,1,0);

	RCache.set_Shader(sh_debug);
	F->SetColor		(color_rgba(255,255,255,255));

	for (u32 Nid=0; Nid<header().vertex_count(); ++Nid)
	{
		CLevelGraph::CVertex&	N	= *vertex(Nid);
		Fvector			PC;
		PC				= vertex_position(N);

		if (Device.vCameraPosition.distance_to(PC)>30) continue;

		float			sr	= header().cell_size();
		if (::Render->ViewBase.testSphere_dirty(PC,sr)) {
			u32	LL		= iFloor(float(N.light())/15.f*255.f);
			u32	CC		= D3DCOLOR_XRGB(0,0,255);
			u32	CT		= D3DCOLOR_XRGB(LL,LL,LL);
			u32	CH		= D3DCOLOR_XRGB(0,128,0);

			BOOL	bHL		= FALSE;
			if (Nid==u32(ID))	{ bHL = TRUE; CT = D3DCOLOR_XRGB(0,255,0); }
			else {
				for (u32 t=0; t<linked.size(); ++t) {
					if (linked[t]==Nid) { bHL = TRUE; CT = CH; break; }
				}
			}

			// unpack plane
			Fplane PL; Fvector vNorm;
			pvDecompress(vNorm,N.plane());
			PL.build	(PC,vNorm);

			// create vertices
			Fvector		v,v1,v2,v3,v4;
			v.set(PC.x-st,PC.y,PC.z-st);	PL.intersectRayPoint(v,DUP,v1);	v1.mad(v1,PL.n,tt);	// minX,minZ
			v.set(PC.x+st,PC.y,PC.z-st);	PL.intersectRayPoint(v,DUP,v2);	v2.mad(v2,PL.n,tt);	// maxX,minZ
			v.set(PC.x+st,PC.y,PC.z+st);	PL.intersectRayPoint(v,DUP,v3);	v3.mad(v3,PL.n,tt);	// maxX,maxZ
			v.set(PC.x-st,PC.y,PC.z+st);	PL.intersectRayPoint(v,DUP,v4);	v4.mad(v4,PL.n,tt);	// minX,maxZ

			// render quad
			RCache.dbg_DrawTRI	(Fidentity,v3,v2,v1,CT);
			RCache.dbg_DrawTRI	(Fidentity,v1,v4,v3,CT);

			// render center
			RCache.dbg_DrawAABB	(PC,sc,sc,sc,CC);

			// render id
			if (bHL) {
				Fvector		T;
				Fvector4	S;
				T.set		(PC); T.y+=0.3f;
				Device.mFullTransform.transform	(S,T);
				F->SetSize	(0.05f/_sqrt(_abs(S.w)));
				F->SetColor	(0xffffffff);
				F->Out		(S.x,-S.y,"~%d",Nid);
			}
		}
	}

#ifdef DEBUG
	xr_vector<CObject*>::iterator	I = Level().Objects.objects.begin();
	xr_vector<CObject*>::iterator	E = Level().Objects.objects.end();
	for ( ; I != E; ++I) {
		CCustomMonster *tpCustomMonster = dynamic_cast<CCustomMonster*>(*I);
		if (tpCustomMonster) {
			tpCustomMonster->OnRender();
			if (!tpCustomMonster->CDetailPathManager::path().empty()) {
				Fvector temp = tpCustomMonster->CDetailPathManager::path()[tpCustomMonster->CDetailPathManager::path().size() - 1].m_position;
				RCache.dbg_DrawAABB(temp,1.f,1.f,1.f,D3DCOLOR_XRGB(0,0,255));
			}
		}
	}
#endif // DEBUG

}

void CLevelGraph::draw_oriented_bounding_box(Fmatrix &T,	Fvector &half_dim, u32 C,	u32 C1) const
{
	RCache.dbg_DrawOBB(T,half_dim,C1);

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
	for (int i=0; i<8; ++i) {
		Fmatrix						V = Fidentity;
		mL2W_Transform.transform_tiny(V.c,aabb[i]);
		V.i.x						= .05f;
		V.j.y						= .05f;
		V.k.z						= .05f;
		RCache.dbg_DrawEllipse		(V,C);
	}
}

void CLevelGraph::draw_travel_line() const
{
	draw_dynamic_obstacles				();
	if (!m_tpTravelLine.empty()) {
		Fvector							P = m_tpTravelLine[0];
		P.y								+= .1f;
		RCache.dbg_DrawAABB				(P,.1f,.1f,.1f,D3DCOLOR_XRGB(0,0,255));
	}
	for (u32 I=1; I<m_tpTravelLine.size(); ++I) {
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

void CLevelGraph::compute_travel_line(xr_vector<u32> &/**vertex_path/**/, u32 /**vertex_id/**/, u32 /**finish_vertex_id/**/) const
{
//	Fvector						*tpDestinationPosition = &m_tFinishPoint;
//
//	if (tpDestinationPosition && inside(vertex(dwFinishNodeID),*tpDestinationPosition))
//		tpDestinationPosition->y = level_graph().vertex_plane_y(*vertex(dwFinishNodeID),tpDestinationPosition->x,tpDestinationPosition->z);
//
//	m_tpaPoints.clear			();
//	m_tpaDeviations.clear		();
//	m_tpaTravelPath.clear		();
//	m_tpaPointNodes.clear		();
//
//	u32							N = (int)m_level_path.size();
//	if (!N) {
//		Msg						("! vertex list is empty!");
//		m_level_path.clear		();
//		m_tpTravelLine.clear	();
//		return;
//	}
//
//	Fvector						tStartPosition = m_start_point;
//	u32							dwCurNode = dwStartNodeID;
//	m_tpaPoints.push_back		(tStartPosition);
//	m_tpaPointNodes.push_back	(dwCurNode);
//
//	for (u32 i=1; i<=N; ++i)
//		if (i<N) {
//			if (!ai().level_graph().check_vertex_in_direction(dwCurNode,tStartPosition,m_level_path[i])) {
//				if (dwCurNode != m_level_path[i - 1])
//					m_tpaPoints.push_back(tStartPosition = level_graph().vertex_position(dwCurNode = m_level_path[--i]));
//				else
//					m_tpaPoints.push_back(tStartPosition = level_graph().vertex_position(dwCurNode = m_level_path[i]));
//				m_tpaPointNodes.push_back(dwCurNode);
//			}
//		}
//		else
//			if (tpDestinationPosition)
//				if (level_graph().check_position_in_direction(dwCurNode,tStartPosition,*tpDestinationPosition) == u32(-1)) {
//					if (dwCurNode != dwFinishNodeID)
//						m_tpaPointNodes.push_back(dwFinishNodeID);
//					m_tpaPoints.push_back(*tpDestinationPosition);
//				}
//				else {
//					dwCurNode = dwFinishNodeID;
//					if (inside(vertex(dwCurNode),*tpDestinationPosition)) {
//						tpDestinationPosition->y = level_graph().vertex_plane_y(*vertex(dwFinishNodeID),tpDestinationPosition->x,tpDestinationPosition->z);
//						m_tpaPointNodes.push_back(dwFinishNodeID);
//						m_tpaPoints.push_back(*tpDestinationPosition);
//					}
//				}
//
//				m_tpaDeviations.resize	(N = (int)m_tpaPoints.size());
//
//				m_tpTravelLine.clear();
//
//				m_level_path.clear		();
//
//				Fvector			T;
//				T.set			(0,0,0);
//				for (i=1; i<N; ++i) {
//					m_tpaLine.clear();
//					m_tpaLine.push_back(m_tpaPoints[i-1]);
//					m_tpaLine.push_back(m_tpaPoints[i]);
//					bfCreateStraightPTN_Path(m_tpaPointNodes[i-1],m_tpaPoints[i-1],m_tpaPoints[i],m_tpaTravelPath,m_tpaNodes, i == 1);
//					u32 n = (u32)m_tpaTravelPath.size();
//					for (u32 j= 0; j<n; ++j) {
//						T = m_tpaTravelPath[j];
//						m_tpTravelLine.push_back(T);
//						m_level_path.push_back(m_tpaNodes[j]);
//					}
//				}
//				if (N > 1) {
//					m_level_path[m_level_path.size() - 1] = dwFinishNodeID;
//					if (tpDestinationPosition && m_tpTravelLine.size() && m_tpTravelLine[m_tpTravelLine.size() - 1].distance_to(*tpDestinationPosition) > EPS_L) {
//						if (inside(vertex(dwFinishNodeID),*tpDestinationPosition) && valid_vertex_id(check_position_in_direction(dwFinishNodeID,T,*tpDestinationPosition))) {
//							T = *tpDestinationPosition;
//							m_tpTravelLine.push_back(T);
//						}
//					}
//				}
//				else
//					if (tpDestinationPosition && m_tpTravelLine.size() && m_tpTravelLine[m_tpTravelLine.size() - 1].distance_to(*tpDestinationPosition) > EPS_L)
//						if (inside(vertex(dwFinishNodeID),*tpDestinationPosition) && valid_vertex_id(check_position_in_direction(dwFinishNodeID,T,*tpDestinationPosition)))
//							m_tpTravelLine.push_back(*tpDestinationPosition);
}

#include "graph_engine.h"

void CLevelGraph::draw_dynamic_obstacles() const
{
//	Level().CurrentEntity()->setEnabled	(false);
//	Level().ObjectSpace.GetNearest		(m_start_point,100.f); 
//	xr_vector<CObject*>					&tpNearestList = Level().ObjectSpace.q_nearest; 
//	Level().CurrentEntity()->setEnabled	(true);
//
//	{
//		xr_vector<CObject*>::iterator		I = tpNearestList.begin();
//		xr_vector<CObject*>::iterator		E = tpNearestList.end();
//		for ( ; I != E; ++I) {
//			Fvector							c, d, C2;
//			(*I)->Visual()->vis.box.get_CD	(c,d);
//			Fmatrix							M = (*I)->XFORM();
//			M.transform_tiny				(C2,c);
//			M.c								= C2;
//			draw_oriented_bounding_box		(M,d,color_rgba(0,0,255,255),color_rgba(0,255,0,255));
//		}
//	}
}

struct SPathPoint {
	Fvector			position;
	Fvector			direction;
	float			linear_velocity;
	float			angular_velocity; 
	u32				vertex_id;
};

struct SCirclePoint {
	Fvector			center;
	float			radius;
	Fvector			point;
	float			angle;
};

struct STrajectoryPoint :
	public SPathPoint,
	public SCirclePoint
{
};

IC	void adjust_point(
	const Fvector		&source, 
	float				yaw, 
	float				magnitude, 
	Fvector				&dest
)
{
	dest.setHP			(yaw,0);
	dest.mul			(magnitude);
	dest.add			(source);
}

IC	void assign_angle(
	float				&angle, 
	const float			start_yaw, 
	const float			dest_yaw, 
	const bool			positive,
	const bool			start = true
)
{
	if (positive)
		if (dest_yaw >= start_yaw)
			angle		= dest_yaw - start_yaw;
		else
			angle		= PI_MUL_2 - start_yaw + dest_yaw;
	else
		if (dest_yaw <= start_yaw)
			angle		= dest_yaw - start_yaw;
		else
			angle		= dest_yaw - start_yaw - PI_MUL_2;

	if (!start)
		if (angle < 0.f)
			angle = -angle - PI_MUL_2;
		else
			angle = PI_MUL_2 - angle;
}

IC	void compute_circles(
	STrajectoryPoint	&point, 
	SCirclePoint		*circles
)
{
	VERIFY				(!fis_zero(point.angular_velocity));
	point.radius		= point.linear_velocity/point.angular_velocity;
	circles[0].radius	= circles[1].radius = point.radius;
	float				yaw, pitch;
	VERIFY				(!fis_zero(point.direction.square_magnitude()));
	point.direction.getHP(yaw,pitch);
	adjust_point		(point.position,yaw + PI_DIV_2,point.radius,circles[0].center);
	adjust_point		(point.position,yaw - PI_DIV_2,point.radius,circles[1].center);
}

IC	float cross_product_2D_y(
	const Fvector &point1,
	const Fvector &point2
)
{
	return				(point1.z*point2.x - point1.x*point2.z);
}

IC	bool compute_tangent(
	const STrajectoryPoint	&start, 
	const SCirclePoint		&start_circle, 
	const STrajectoryPoint	&dest, 
	const SCirclePoint		&dest_circle, 
	SCirclePoint			*tangents
)
{
#pragma todo("Dima to Dima : If this will be a slow down, optimize it by using just a few square roots instead of arctangents and arccosinuses")
	float				start_cp, dest_cp, distance, alpha1, alpha2, start_yaw, dest_yaw, pitch, yaw1, yaw2;
	Fvector				direction, temp;
	
	// computing 2D cross product for start point
	direction.sub		(start.position,start_circle.center);
	if (fis_zero(direction.square_magnitude()))
		direction		= start.direction;
	direction.getHP		(start_yaw,pitch);
	start_yaw			= angle_normalize(start_yaw);
	start_cp			= cross_product_2D_y(start.direction,direction);
	
	// computing 2D cross product for dest point
	direction.sub		(dest.position,dest_circle.center);
	if (fis_zero(direction.square_magnitude()))
		direction		= dest.direction;
	direction.getHP		(dest_yaw,pitch);
	dest_yaw			= angle_normalize(dest_yaw);
	dest_cp				= cross_product_2D_y(dest.direction,direction);

	// direction from the first circle to the second one
	direction.sub		(dest_circle.center,start_circle.center);
	direction.getHP		(yaw1,pitch);
	yaw1 = yaw2			= angle_normalize(yaw1);

	if (start_cp*dest_cp >= 0.f) {
		// so, our tangents are outside
		if (start_circle.center.similar(dest_circle.center)) {
			if  (fsimilar(start_circle.radius,dest_circle.radius)) {
				// so, our circles are equal
				tangents[0]			= tangents[1] = start_circle;
				if (start_cp > 0.f) {
					adjust_point	(start_circle.center,yaw1 + PI_DIV_2,	start_circle.radius,tangents[0].point);
					assign_angle	(tangents[0].angle,start_yaw,angle_normalize(yaw1 + PI_DIV_2),true);
				}
				else {
					adjust_point	(start_circle.center,yaw1 - PI_DIV_2,	start_circle.radius,tangents[0].point);
					assign_angle	(tangents[0].angle,start_yaw,angle_normalize(yaw1 - PI_DIV_2),false);
				}

				tangents[1].point	= tangents[0].point;
				tangents[1].angle	= 0.f;
				return				(true);
			}
			else
				return				(false);
		}
		else {
			// distance between circle centers
			distance		= start_circle.center.distance_to_xz(dest_circle.center);
			// radius difference
			float			r_diff = start_circle.radius - dest_circle.radius;
			// angle between external tangents and circle centers segment
			alpha1 = alpha2	= angle_normalize(acosf(r_diff/distance));
		}
	}
	else {
		distance		= start_circle.center.distance_to_xz(dest_circle.center);
		// so, our tangents are inside (crossing)
		if (start_circle.radius + dest_circle.radius >= distance)
			return		(false);
	
		// angle between internal tangents and circle centers segment
		alpha1			= angle_normalize(acosf((start_circle.radius + dest_circle.radius)/distance));
		alpha2			= alpha1;;
		yaw2			= angle_normalize(yaw1 + PI);
	}

	tangents[0]			= start_circle;
	tangents[1]			= dest_circle;
	start_yaw			= angle_normalize(start_yaw);
	dest_yaw			= angle_normalize(dest_yaw);

	// compute external tangent points
	adjust_point		(start_circle.center,yaw1 + alpha1,	start_circle.radius,tangents[0].point);
	adjust_point		(dest_circle.center,yaw2  + alpha2,	dest_circle.radius, tangents[1].point);

	direction.sub		(tangents[1].point,tangents[0].point);
	temp.sub			(tangents[0].point,start_circle.center);
	float				tangent_cp = cross_product_2D_y(direction,temp);
	if (start_cp*tangent_cp >= 0) {
		assign_angle	(tangents[0].angle,start_yaw,angle_normalize(yaw1 + alpha1),start_cp >= 0);
		assign_angle	(tangents[1].angle,dest_yaw, angle_normalize(yaw2 + alpha2),dest_cp  >= 0,false);
		return			(true);
	}

	// compute external tangent points
	adjust_point		(start_circle.center,yaw1 - alpha1,	start_circle.radius,tangents[0].point);
	adjust_point		(dest_circle.center,yaw2  - alpha2,	dest_circle.radius, tangents[1].point);
	assign_angle		(tangents[0].angle,start_yaw,angle_normalize(yaw1 - alpha1),start_cp >= 0);
	assign_angle		(tangents[1].angle,dest_yaw, angle_normalize(yaw2 - alpha2),dest_cp  >= 0,false);

	return				(true);
}

IC	bool choose_tangent(
	STrajectoryPoint	&start, 
	STrajectoryPoint	&dest, 
	const SCirclePoint	tangents[4][2], 
	const u32			tangent_count
)
{
	float				min_distance = flt_max;
	u32					choosen = u32(-1);
	for (u32 i=0; i<tangent_count; ++i) {
		float			trajectory_length = 
			_abs(tangents[i][0].angle)*tangents[i][0].radius + 
			_abs(tangents[i][1].angle)*tangents[i][1].radius + 
			tangents[i][0].point.distance_to_xz(tangents[i][1].point);
		if (trajectory_length < min_distance) {
			min_distance = trajectory_length;
			choosen		= i;
		}
	}
	if (choosen > tangent_count)
		return			(false);

	SCirclePoint		&_start = start;
	SCirclePoint		&_dest  = dest;
	_start				= tangents[choosen][0];
	_dest				= tangents[choosen][1];

	return				(true);
}

IC	bool build_circle_trajectory(
	const STrajectoryPoint	&position, 
	xr_vector<Fvector>		&path,
	bool					start_point
)
{
	Fvector				direction;
	if (start_point)
		direction.sub	(position.position,position.center);
	else
		direction.sub	(position.point,position.center);

	float				yaw,pitch;
	direction.getHP		(yaw,pitch);
	yaw					= angle_normalize(yaw);
	u32					m = iFloor(_abs(position.angle)/position.angular_velocity*10.f +.5f);
	for (u32 i=start_point ? 0 : 1, n=fis_zero(position.angular_velocity) ? 1 : m; i<=n; ++i) {
		path.push_back	(Fvector());
		adjust_point	(position.center,yaw + float(i)*position.angle/float(n),position.radius,path.back());
	}
	return				(true);
}

IC	bool build_line_trajectory(
	const STrajectoryPoint	&start, 
	const STrajectoryPoint	&dest, 
	xr_vector<Fvector>		&path
)
{
	xr_vector<u32>		node_path;
	if (ai().level_graph().inside(start.vertex_id,dest.point)) {
		path.push_back	(dest.point);
		return			(true);
	}
	u32					vertex_id = ai().level_graph().check_position_in_direction(start.vertex_id,start.position,start.point);
	if (!ai().level_graph().valid_vertex_id(vertex_id))
		return			(false);
	return				(ai().level_graph().create_straight_PTN_path(vertex_id,start.point,dest.point,path,node_path,false,false));
}

IC	bool build_trajectory(
	const STrajectoryPoint	&start, 
	const STrajectoryPoint	&dest, 
	xr_vector<Fvector>		&path
)
{
	if (!build_circle_trajectory(start,path,true)) {
		Msg				("FALSE : Circle 0");
		return			(false);
	}
	if (!build_line_trajectory(start,dest,path)) {
		Msg				("FALSE : Line");
		return			(false);
	}
	if (!build_circle_trajectory(dest,path,false)) {
		Msg				("FALSE : Circle 1");
		return			(false);
	}
	Msg					("TRUE");
	return				(true);
}

IC	bool compute_trajectory(
	STrajectoryPoint &start, 
	STrajectoryPoint &dest, 
	xr_vector<Fvector> &path
)
{
	SCirclePoint	start_circles[2], dest_circles[2];
	compute_circles	(start,start_circles);
	compute_circles	(dest,dest_circles);
	
	u32				tangent_count = 0;
	SCirclePoint	tangent_points[4][2];
	for (u32 i=0; i<2; ++i)
		for (u32 j=0; j<2; ++j)
			if (compute_tangent(
					start,
					start_circles[i],
					dest,
					dest_circles[j],
					tangent_points[tangent_count]
				)
			)
			++tangent_count;

	if (!choose_tangent(start,dest,tangent_points,tangent_count))
		return		(false);

	return			(build_trajectory(start,dest,path));
}

void CLevelGraph::compute_path()
{
//	STrajectoryPoint		start, dest;
//
//	CObject					*obj = Level().Objects.FindObjectByName("m_stalker_e0000");
//	CAI_Stalker				*stalker = dynamic_cast<CAI_Stalker*>(obj);
//	obj						= Level().CurrentEntity();
//	CActor					*actor = dynamic_cast<CActor*>(obj);
//	
//	start.angular_velocity	= PI_DIV_2;
//	start.linear_velocity	= 2.15f;
//	start.position			= stalker->Position();
//	start.direction.setHP	(-stalker->m_body.current.yaw,0);
//	start.vertex_id			= stalker->level_vertex_id();
//	
//	dest.angular_velocity	= PI_DIV_2;
//	dest.linear_velocity	= 2.15f;
//	dest.position			= actor->Position();
//	dest.direction.setHP	(actor->r_model_yaw,0);
//	dest.vertex_id			= actor->level_vertex_id();
	
//	start.angular_velocity	= 1.f;
//	start.linear_velocity	= 2.f;
//	start.position			= Fvector().set(-50,0,-40);
//	start.direction.set		(0,0,1);
//	start.vertex_id			= vertex(start.position);
//	
//	dest.angular_velocity	= 1.f;
//	dest.linear_velocity	= 1.f;
//	dest.position			= Fvector().set(-50,0,-40);
//	dest.direction.set		(0,0,-1);
//	dest.vertex_id			= vertex(dest.position);

//	m_tpTravelLine.clear	();
//	compute_trajectory		(start,dest,m_tpTravelLine);
	return;
	//	u32						l_dwStartNodeID		= vertex(m_start_point);
	//	VERIFY					(inside(vertex(l_dwStartNodeID),m_start_point));
	//	u32						l_dwFinishNodeID	= vertex(m_finish_point);
	//	VERIFY					(inside(vertex(l_dwFinishNodeID),m_finish_point));
	//	xr_vector<u32>			l_tpNodePath;
	//	ai().graph_engine().search(ai().level_graph(),l_dwStartNodeID,l_dwFinishNodeID,&l_tpNodePath,CGraphEngine::CObstacleParams());
	//	compute_travel_line		(l_tpNodePath,l_dwStartNodeID,l_dwFinishNodeID);
}

#endif // DEBUG
#endif