// AI_Space.cpp: implementation of the CAI_Space class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AI_Space.h"
#include "gameobject.h"
#include "hudmanager.h"
#include "ai_console.h"
#include "a_star.h"
#include "game_sv_single.h"
#include "CustomMonster.h"
#include "..\\net_client.h"
#include "ai_alife.h"
using namespace AI;

CAI_Space *	tpAI_Space = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAI_Space::CAI_Space	()
{
	sh_debug.create				("debug\\ai_nodes","$null");

	m_nodes_ptr					= NULL;
	vfs							= NULL;
	m_tpAStar					= 0;
	
	string256					caFileName;
	if (FS.exist(caFileName,"$game_data$",GRAPH_NAME))
		CSE_ALifeGraph::Load		(caFileName);
	
	m_dwCurrentLevelID			= u32(-1);
}

CAI_Space::~CAI_Space	()
{
	Unload();
}

void CAI_Space::Unload()
{
	m_dwCurrentLevelID			= u32(-1);
	xr_free						(m_nodes_ptr);
	xr_delete					(vfs);
	xr_delete					(m_tpAStar);
}

void CAI_Space::Load()
{
	CSE_ALifeCrossTable::Unload();
	Unload		();

	string256	fName;
	if (!FS.exist(fName,"$level$","level.ai"))
		return;

	vfs			= FS.r_open	(fName);

	// m_header & data
	vfs->r		(&m_header,sizeof(m_header));
	R_ASSERT	(m_header.version == XRAI_CURRENT_VERSION);
	m_nodes		= (BYTE*) vfs->pointer();

	// dispatch table
	m_nodes_ptr	= (NodeCompressed**)xr_malloc(m_header.count*sizeof(void*));
	{
		for (u32 I=0; I<m_header.count; I++) {
			m_nodes_ptr[I]	= (NodeCompressed*)vfs->pointer();

			NodeCompressed	C;
			vfs->r			(&C,sizeof(C));

			u32			L	= C.links;
			vfs->advance	(L*sizeof(NodeLink));
		}
	}

	
	// special query tables
	q_mark.assign			(m_header.count,0);
	q_mark_bit.assign		(m_header.count,false);
	q_mark_bit_x.assign		(m_header.count,false);
	m_baNodeMarks.assign	(m_header.count,false);
#pragma todo("Dima to Dima : This is a very big memory assumption, it must be reduced to a reasonable value")
	q_stack.reserve			(m_header.count);
	m_dwaNodeStackM.reserve	(m_header.count);
	m_dwaNodeStackS.reserve	(m_header.count);

	// a*
	m_fSize2				= _sqr(m_header.size)/4;
	m_fYSize2				= _sqr((float)(m_header.size_y/32767.0))/4;
	m_tpAStar				= xr_new<CAStar>(65535);

	if (!FS.exist(fName,"$level$",CROSS_TABLE_NAME))
		return;

	CSE_ALifeCrossTable::Load	(fName);

	bool	bOk = false;
	u32		N = m_tGraphHeader.dwLevelCount, I = (u32)(-1);
	for ( I=0; I<N; I++)
		if (!stricmp(m_tGraphHeader.tpLevels[I].caLevelName,Level().net_SessionName())) {
			I = m_tGraphHeader.tpLevels[I].dwLevelID;
			bOk = true;
			break;
		}

	m_dwCurrentLevelID = I;

	if (!bOk){
		Msg("! !!!!!!!!There is no graph for the level %s",Level().net_SessionName());
		return;
	}

	N = m_tGraphHeader.dwVertexCount;
	for (_GRAPH_ID i=0; i<N; i++)
		if ((m_tpaGraph[i].tLevelID == I) && (m_tpaCrossTable[m_tpaGraph[i].tNodeID].tGraphIndex != i)) {
			Msg("! Graph doesn't correspond to the cross table");
			R_ASSERT2(false,"Graph doesn't correspond to the cross table");
		}
	Msg("* Graph corresponds to the cross table");
}

#define NORMALIZE_VECTOR(t) t.x /= 10.f, t.x += tCameraPosition.x, t.y /= 10.f, t.y += 20.f, t.z /= 10.f, t.z += tCameraPosition.z;
#define DRAW_GRAPH_POINT(t0,c0,c1,c2) {\
	Fvector t1 = (t0);\
	t1.y += .6f;\
	NORMALIZE_VECTOR(t1);\
	RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(c0,c1,c2));\
}

void CAI_Space::Render()
{
	if (!psHUD_Flags.test(HUD_DRAW))
		return;
	if (psAI_Flags.test(aiBrain)) {
		if (bfCheckIfGraphLoaded()) {
			if (!Level().CurrentEntity())
				return;
			Fvector tCameraPosition = Level().CurrentEntity()->Position();
			CGameFont* F		= HUD().pFontDI;
			for (int i=0; i<(int)GraphHeader().dwVertexCount; i++) {
				Fvector t1 = m_tpaGraph[i].tGlobalPoint;
				t1.y += .6f;
				NORMALIZE_VECTOR(t1);
				RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(0,0,255));
				for (int j=0; j<(int)m_tpaGraph[i].tNeighbourCount; j++) {
					Fvector t2 = m_tpaGraph[((CSE_ALifeGraph::SGraphEdge *)((BYTE *)m_tpaGraph + m_tpaGraph[i].dwEdgeOffset) + j)->dwVertexNumber].tGlobalPoint;
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
			if ((m_tpAStar) && (m_tpAStar->m_tpaNodes.size())) {
				Fvector t1 = m_tpaGraph[m_tpAStar->m_tpaNodes[0]].tGlobalPoint;
				t1.y += .6f;
				NORMALIZE_VECTOR(t1);
				RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(0,0,255));
				for (int i=1; i<(int)m_tpAStar->m_tpaNodes.size(); i++) {
					Fvector t2 = m_tpaGraph[m_tpAStar->m_tpaNodes[i]].tGlobalPoint;
					t2.y += .6f;
					NORMALIZE_VECTOR(t2);
					RCache.dbg_DrawAABB(t2,.05f,.05f,.05f,D3DCOLOR_XRGB(0,0,255));
					RCache.dbg_DrawLINE(Fidentity,t1,t2,D3DCOLOR_XRGB(255,0,0));
					t1 = t2;
				}
				//			i=1;
				//			for (; m_tpAStar->m_tpIndexes[m_tpAStar->m_tpHeap[i].iIndex].dwTime == m_tpAStar->m_dwAStarStaticCounter; i++) {
				//				Fvector t2 = m_tpaGraph[m_tpAStar->m_tpHeap[i].iIndex].tGlobalPoint;
				//				t2.y += .6f;
				//				NORMALIZE_VECTOR(t2);
				//				RCache.dbg_DrawAABB(t2,.05f,.05f,.05f,D3DCOLOR_XRGB(255,0,0));
				//			}
			}
			if (Level().game.type == GAME_SINGLE) {
				game_sv_Single *tpGame = dynamic_cast<game_sv_Single *>(Level().Server->game);
				if ((tpGame) && (tpGame->m_tpALife)) {
					D_OBJECT_PAIR_IT	I = tpGame->m_tpALife->m_tObjectRegistry.begin();
					D_OBJECT_PAIR_IT	E = tpGame->m_tpALife->m_tObjectRegistry.end();
					for ( ; I != E; I++) {
						{
							CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract *>((*I).second);
							if (tpALifeMonsterAbstract && tpALifeMonsterAbstract->m_bDirectControl && !tpALifeMonsterAbstract->m_bOnline) {
								CSE_ALifeHumanAbstract *tpALifeHuman = dynamic_cast<CSE_ALifeHumanAbstract *>(tpALifeMonsterAbstract);
								if (tpALifeHuman && tpALifeHuman->m_tpPath.size()) {
									Fvector t1 = m_tpaGraph[tpALifeHuman->m_tpPath[0]].tGlobalPoint;
									t1.y += .6f;
									NORMALIZE_VECTOR(t1);
									RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(0,0,255));
									for (int i=1; i<(int)tpALifeHuman->m_tpPath.size(); i++) {
										Fvector t2 = m_tpaGraph[tpALifeHuman->m_tpPath[i]].tGlobalPoint;
										t2.y += .6f;
										NORMALIZE_VECTOR(t2);
										RCache.dbg_DrawAABB(t2,.05f,.05f,.05f,D3DCOLOR_XRGB(0,0,255));
										RCache.dbg_DrawLINE(Fidentity,t1,t2,D3DCOLOR_XRGB(0,0,255));
										t1 = t2;
									}
								}
								if (tpALifeMonsterAbstract->m_fDistanceToPoint > EPS_L) {
									Fvector t1 = m_tpaGraph[tpALifeMonsterAbstract->m_tGraphID].tGlobalPoint;
									Fvector t2 = m_tpaGraph[tpALifeMonsterAbstract->m_tNextGraphID].tGlobalPoint;
									t2.sub(t1);
									t2.mul(tpALifeMonsterAbstract->m_fDistanceFromPoint/tpALifeMonsterAbstract->m_fDistanceToPoint);
									t1.add(t2);
									t1.y += .6f;
									NORMALIZE_VECTOR(t1);
									RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(255,0,0));
								}
								else {
									Fvector t1 = m_tpaGraph[(*I).second->m_tGraphID].tGlobalPoint;
									t1.y += .6f;
									NORMALIZE_VECTOR(t1);
									RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(255,0,0));
								}
							}
							else {
								CSE_ALifeItem *tpALifeItem = dynamic_cast<CSE_ALifeItem *>((*I).second);
								if (tpALifeItem && !tpALifeItem->bfAttached()) {
									Fvector t1 = m_tpaGraph[(*I).second->m_tGraphID].tGlobalPoint;
									t1.y += .6f;
									NORMALIZE_VECTOR(t1);
									RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(255,255,0));
								}
								else {
									CSE_ALifeCreatureActor *tpALifeCreatureActor = dynamic_cast<CSE_ALifeCreatureActor*>((*I).second);
									if (tpALifeCreatureActor) {
										Fvector t1 = m_tpaGraph[(*I).second->m_tGraphID].tGlobalPoint;
										t1.y += .6f;
										NORMALIZE_VECTOR(t1);
										RCache.dbg_DrawAABB(t1,.05f,.05f,.05f,D3DCOLOR_XRGB(255,255,255));
									}
									else {
										CSE_ALifeTrader *tpALifeTrader = dynamic_cast<CSE_ALifeTrader*>((*I).second);
										if (tpALifeTrader) {
											Fvector t1 = m_tpaGraph[(*I).second->m_tGraphID].tGlobalPoint;
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

	{
		CLevel::SPathPairIt I = Level().m_PatrolPaths.begin();
		CLevel::SPathPairIt E = Level().m_PatrolPaths.end();
		for ( ; I != E; I++) {
			u32	N = (u32)(*I).second.tpaVectors[0].size();
			RCache.dbg_DrawAABB((*I).second.tpaVectors[0][0],.1f,.1f,.1f,D3DCOLOR_XRGB(0,0,255));
			for (u32 i=1; i<N; i++) {
				RCache.dbg_DrawLINE(Fidentity,(*I).second.tpaVectors[0][i-1],(*I).second.tpaVectors[0][i],D3DCOLOR_XRGB(0,255,0));
				RCache.dbg_DrawAABB((*I).second.tpaVectors[0][i],.1f,.1f,.1f,D3DCOLOR_XRGB(0,0,255));
			}
		}
	}

#ifdef DEBUG
	if (psAI_Flags.test(aiMotion)) {
		xr_vector<CObject*>::iterator	I = Level().Objects.objects.begin();
		xr_vector<CObject*>::iterator	E = Level().Objects.objects.end();
		for ( ; I != E; I++) {
			CCustomMonster *tpCustomMonster = dynamic_cast<CCustomMonster*>(*I);
			if (tpCustomMonster) {
				tpCustomMonster->OnRender();
				if (!tpCustomMonster->AI_Path.TravelPath.empty())
					RCache.dbg_DrawAABB(tpCustomMonster->AI_Path.TravelPath[tpCustomMonster->AI_Path.TravelPath.size() - 1].P,1.f,1.f,1.f,D3DCOLOR_XRGB(0,0,255));
			}
		}
	}
#endif
	
	if (!bDebug)	return;

	if (0==vfs)						return;
//	if (0==sh_debug)				return;
	if (!psAI_Flags.test(aiDebug))	return;

	CGameObject*	O	= dynamic_cast<CGameObject*> (Level().CurrentEntity());
	Fvector	POSITION	= O->Position();
	POSITION.y += 0.5f;

	// display
	Fvector P			= POSITION;

	NodePosition		Local;
	PackPosition		(Local,P);

	u32 ID				= O->AI_NodeID;

	CGameFont* F		= HUD().pFontDI;
	F->SetSize			(.02f);
	F->Out				(0.f,0.5f,"%f,%f,%f",VPUSH(P));
	F->Out				(0.f,0.55f,"%3d,%4d,%3d -> %d",	iFloor(Local.x),iFloor(Local.y),iFloor(Local.z),u32(ID));

	svector<u32,128>	linked;
	{
		NodeCompressed*	N	=	m_nodes_ptr[ID];
		u32	count		=	u32(N->links);
		BYTE* pData			=	(BYTE*)N;
		pData				+=	sizeof(NodeCompressed);
		NodeLink* it		=	(NodeLink*)pData;
		NodeLink* end		=	it+count;
		for(; it!=end; it++)
			linked.push_back(UnpackLink(*it));
	}

	// render
	float	sc		= m_header.size/16;
	float	st		= 0.98f*m_header.size/2;
	float	tt		= 0.01f;

	Fvector	DUP;	DUP.set(0,1,0);

	RCache.set_Shader(sh_debug);
	F->SetColor		(D3DCOLOR_RGBA(255,255,255,255));

	for (u32 Nid=0; Nid<m_header.count; Nid++)
	{
		NodeCompressed&	N	= *m_nodes_ptr[Nid];
		Fvector			P0,P1,PC;
		UnpackPosition	(P0,N.p0);	// p0
		UnpackPosition	(P1,N.p1);	// p1
		PC.add			(P0,P1);
		PC.mul			(0.5f);		// sphere center

		if (Device.vCameraPosition.distance_to(PC)>30) continue;

		float			sr	= P0.distance_to(P1)/2+m_header.size;
		if (::Render->ViewBase.testSphere_dirty(PC,sr)) {
			u32	LL		= u32(N.light);
			u32	CC		= D3DCOLOR_XRGB(0,0,255);
			u32	CT		= D3DCOLOR_XRGB(LL,LL,LL);
			u32	CH		= D3DCOLOR_XRGB(0,128,0);

			BOOL	bHL		= FALSE;
			if (Nid==u32(ID))	{ bHL = TRUE; CT = D3DCOLOR_XRGB(0,255,0); }
			else {
				for (u32 t=0; t<linked.size(); t++) {
					if (linked[t]==Nid) { bHL = TRUE; CT = CH; break; }
				}
			}

			// unpack plane
			Fplane PL; Fvector vNorm;
			pvDecompress(vNorm,N.plane);
			PL.build	(P0,vNorm);

			// create vertices
			Fvector		v,v1,v2,v3,v4;
			v.set(P0.x-st,P0.y,P0.z-st);	PL.intersectRayPoint(v,DUP,v1);	v1.mad(v1,PL.n,tt);	// minX,minZ
			v.set(P1.x+st,P0.y,P0.z-st);	PL.intersectRayPoint(v,DUP,v2);	v2.mad(v2,PL.n,tt);	// maxX,minZ
			v.set(P1.x+st,P1.y,P1.z+st);	PL.intersectRayPoint(v,DUP,v3);	v3.mad(v3,PL.n,tt);	// maxX,maxZ
			v.set(P0.x-st,P1.y,P1.z+st);	PL.intersectRayPoint(v,DUP,v4);	v4.mad(v4,PL.n,tt);	// minX,maxZ

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
	for ( ; I != E; I++) {
		CCustomMonster *tpCustomMonster = dynamic_cast<CCustomMonster*>(*I);
		if (tpCustomMonster) {
			tpCustomMonster->OnRender();
			if (!tpCustomMonster->AI_Path.TravelPath.empty())
				RCache.dbg_DrawAABB(tpCustomMonster->AI_Path.TravelPath[tpCustomMonster->AI_Path.TravelPath.size() - 1].P,1.f,1.f,1.f,D3DCOLOR_XRGB(0,0,255));
		}
	}
#endif
	
}

int	CAI_Space::q_LoadSearch(const Fvector& pos)
{
	if (0==vfs)	return	0;
	// Log		("***AAA***");
	NodePosition	P;
	PackPosition	(P,pos);
	short min_dist	= 32767;
	int selected	= -1;
	for (u32 I=0; I<m_header.count; I++) {
		NodeCompressed& N = *m_nodes_ptr[I];
		if (u_InsideNode(N,P)) {
			Fvector	DUP, vNorm, v, v1, P0;
			DUP.set(0,1,0);
			pvDecompress(vNorm,N.plane);
			Fplane PL; 
			UnpackPosition(P0,N.p0);
			PL.build(P0,vNorm);
			v.set(pos.x,P0.y,pos.z);	
			PL.intersectRayPoint(v,DUP,v1);
			int dist = iFloor((v1.y - pos.y)*(v1.y - pos.y));
			if (dist < min_dist) {
				min_dist = (short)dist;
				selected = I;
			}
		}
	}
//	if (selected == u32(-1)) {
//		float		min_dist = 32767.f;
//		selected	= -1;
//		for (u32 I=0; I<m_header.count; I++) {
//			NodeCompressed	*N = m_nodes_ptr[I];
//			float			f = u_SqrDistance2Node(pos,N);
//			if (f < min_dist) {
//				min_dist = f;
//				selected = I;
//			}
//		}
//	}
//	if (selected == u32(-1))
//		Msg			("Can't find an appropriate node for the specified position! (% : [%f][%f][%f])",VPUSH(pos));
//	R_ASSERT2		(selected != u32(-1),"Can't find an appropriate node for the specified position!");
	return selected;
}