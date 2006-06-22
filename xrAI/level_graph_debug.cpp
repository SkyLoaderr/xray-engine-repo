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
#include "level.h"
#include "game_base_space.h"
#include "hudmanager.h"
#include "xrserver_objects_alife_monsters.h"
#include "alife_simulator.h"
#include "alife_graph_registry.h"
#include "alife_object_registry.h"
#include "alife_human_brain.h"
#include "alife_monster_movement_manager.h"
#include "alife_monster_detail_path_manager.h"

u32	vertex_in_direction(u32 level_vertex_id, Fvector direction, float max_distance);

#define NORMALIZE_VECTOR(t) t.x /= 10.f, t.x += tCameraPosition.x, t.y /= 10.f, t.y += 20.f, t.z /= 10.f, t.z += tCameraPosition.z;
#define DRAW_GRAPH_POINT(t0,c0,c1,c2) {\
	Fvector t1 = (t0);\
	t1.y += .6f;\
	NORMALIZE_VECTOR(t1);\
	RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(c0,c1,c2));\
}

void CLevelGraph::render()
{
	if (!psAI_Flags.test(aiDebug))
		return;

	if (bDebug)
		on_render1			();

	on_render2				();

	draw_travel_line		();

	if (!psHUD_Flags.test(HUD_DRAW))
		return;

	if (psAI_Flags.test(aiCover)) {
		on_render3			();
	}

	if (psAI_Flags.test(aiBrain)) {
		if (ai().get_level_graph()) {
			on_render4		();
		}
	}

	if (psAI_Flags.test(aiMotion)) {
		on_render5			();
	}

	on_render6				();

#ifdef DEBUG
#ifndef AI_COMPILER
	on_render7	();
#endif
#endif

}
extern xr_vector<u32> map_point_path;

void CLevelGraph::on_render4	()
{
	if (!Level().CurrentEntity())
		return;

	Fvector tCameraPosition = Level().CurrentEntity()->Position();
	CGameFont* F		= HUD().Font().pFontDI;
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
		//out of screen
		if (S.z < 0 || S.w < 0)												continue;
		if (S.x < -1.f || S.x > 1.f || S.y<-1.f || S.x>1.f)					continue;
		F->SetSizeI	(0.05f/_sqrt(_abs(S.w)));
		F->SetColor(0xffffffff);
		F->OutI(S.x,-S.y,"%d",i);
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
	{
		const xr_vector<u32>	&path = map_point_path;
		if( path.size() ){
			Fvector t1 = ai().game_graph().vertex(path.back())->game_point();
			t1.y += .6f;
			NORMALIZE_VECTOR(t1);
			RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(0,0,255));
			for (int i=(int)path.size() - 2; i>=0;--i) {
				Fvector t2 = ai().game_graph().vertex(path[i])->game_point();
				t2.y += .6f;
				NORMALIZE_VECTOR(t2);
				RCache.dbg_DrawAABB(t2,.05f,.05f,.05f,D3DCOLOR_XRGB(0,0,255));
				RCache.dbg_DrawLINE(Fidentity,t1,t2,D3DCOLOR_XRGB(0,0,255));
				t1 = t2;
			}
		}
	}

	if (GameID() == GAME_SINGLE && ai().get_alife()) {
		{
			GameGraph::_LEVEL_ID	J = ai().game_graph().vertex(ai().alife().graph().actor()->m_tGraphID)->level_id();
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
				//out of screen
				if (S.z < 0 || S.w < 0)												continue;
				if (S.x < -1.f || S.x > 1.f || S.y<-1.f || S.x>1.f)					continue;
				F->SetSizeI	(0.1f/_sqrt(_abs(S.w)));
				F->SetColor(0xffffffff);
				F->OutI(S.x,-S.y,"%d",i);
			}
		}

		ALife::D_OBJECT_P_MAP::const_iterator	I = ai().alife().objects().objects().begin();
		ALife::D_OBJECT_P_MAP::const_iterator	E = ai().alife().objects().objects().end();
		for ( ; I != E; ++I) {
			{
				CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract = smart_cast<CSE_ALifeMonsterAbstract *>((*I).second);
				if (tpALifeMonsterAbstract && tpALifeMonsterAbstract->m_bDirectControl && !tpALifeMonsterAbstract->m_bOnline) {
					/**/
					CSE_ALifeHumanAbstract *tpALifeHuman = smart_cast<CSE_ALifeHumanAbstract *>(tpALifeMonsterAbstract);
					if (tpALifeHuman && tpALifeHuman->brain().movement().detail().path().size()) {
						Fvector t1 = ai().game_graph().vertex(tpALifeHuman->brain().movement().detail().path().back())->game_point();
						t1.y += .6f;
						NORMALIZE_VECTOR(t1);
						RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(0,0,255));
						for (int i=(int)tpALifeHuman->brain().movement().detail().path().size() - 2; i>=0;--i) {
							Fvector t2 = ai().game_graph().vertex(tpALifeHuman->brain().movement().detail().path()[i])->game_point();
							t2.y += .6f;
							NORMALIZE_VECTOR(t2);
							RCache.dbg_DrawAABB(t2,.05f,.05f,.05f,D3DCOLOR_XRGB(0,0,255));
							RCache.dbg_DrawLINE(Fidentity,t1,t2,D3DCOLOR_XRGB(0,0,255));
							t1 = t2;
						}
					}
					/**/
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
					CSE_ALifeInventoryItem *l_tpALifeInventoryItem = smart_cast<CSE_ALifeInventoryItem*>((*I).second);
					if (l_tpALifeInventoryItem && !l_tpALifeInventoryItem->attached()) {
						Fvector t1 = ai().game_graph().vertex((*I).second->m_tGraphID)->game_point();
						t1.y += .6f;
						NORMALIZE_VECTOR(t1);
						RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(255,255,0));
					}
					else {
						CSE_ALifeCreatureActor *tpALifeCreatureActor = smart_cast<CSE_ALifeCreatureActor*>((*I).second);
						if (tpALifeCreatureActor) {
							Fvector t1 = ai().game_graph().vertex((*I).second->m_tGraphID)->game_point();
							t1.y += .6f;
							NORMALIZE_VECTOR(t1);
							RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(255,255,255));
						}
						else {
							CSE_ALifeTrader *tpALifeTrader = smart_cast<CSE_ALifeTrader*>((*I).second);
							if (tpALifeTrader) {
								Fvector t1 = ai().game_graph().vertex((*I).second->m_tGraphID)->game_point();
								t1.y += .6f;
								NORMALIZE_VECTOR(t1);
								RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(0,0,0));
							}
							else {
								CSE_ALifeSmartZone *smart_zone = smart_cast<CSE_ALifeSmartZone*>((*I).second);
								if (smart_zone) {
									Fvector t1 = ai().game_graph().vertex((*I).second->m_tGraphID)->game_point();
									t1.y += .6f;
									NORMALIZE_VECTOR(t1);
									RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(255,0,0));
								}
							}
						}
					}
				}
			}
		}
	}
}

#endif // AI_COMPILER
#endif // DEBUG