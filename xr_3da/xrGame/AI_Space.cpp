// AI_Space.cpp: implementation of the CAI_Space class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AI_Space.h"
#include "gameobject.h"
#include "hudmanager.h"
#include "ai_console.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAI_Space::CAI_Space	()
{
	m_nodes_ptr					= NULL;
	vfs							= NULL;
	m_tpGraphVFS				= NULL;
	sh_debug					= 0;

	m_tpHeap					= 0;
	m_tpIndexes					= 0;
	m_dwAStarStaticCounter		= 0;
	m_tpaNodes.					clear();
	Device.seqDevCreate.Add		(this);
	Device.seqDevDestroy.Add	(this);
	OnDeviceCreate				();
}

CAI_Space::~CAI_Space	()
{
	// for a* search
	_FREE(m_tpHeap);
	_FREE(m_tpIndexes);
	//

	Device.seqDevCreate.Remove	(this);
	Device.seqDevDestroy.Remove	(this);
	Device.Shader.Delete		(sh_debug);
	OnDeviceDestroy				();

	_FREE	(m_nodes_ptr);
	_DELETE	(vfs);
	_DELETE	(m_tpGraphVFS);
}

void CAI_Space::OnDeviceCreate()
{
	REQ_CREATE					();
	sh_debug					= Device.Shader.Create	("debug\\ai_nodes","$null");
}
void CAI_Space::OnDeviceDestroy()
{
	REQ_DESTROY	();
	Device.Shader.Delete		(sh_debug);
}

void CAI_Space::Load(LPCSTR name)
{
	FILE_NAME	fName;
	strconcat	(fName,name,"level.ai");
	if (!Engine.FS.Exist(fName))	return;

	vfs			= Engine.FS.Open	(fName);

	// m_header & data
	vfs->Read	(&m_header,sizeof(m_header));
	R_ASSERT	(m_header.version == XRAI_CURRENT_VERSION);
	m_nodes		= (BYTE*) vfs->Pointer();

	// dispatch table
	m_nodes_ptr	= (NodeCompressed**)xr_malloc(m_header.count*sizeof(void*));
	for (u32 I=0; I<m_header.count; I++)
	{
		m_nodes_ptr[I]	= (NodeCompressed*)vfs->Pointer();

		NodeCompressed	C;
		vfs->Read		(&C,sizeof(C));

		u32			L = C.links;
		vfs->Advance	(L*sizeof(NodeLink));
	}

	// special query tables
	q_mark.assign			(m_header.count,0);
	q_mark_bit.assign		(m_header.count,false);
	q_mark_bit_x.assign		(m_header.count,false);

	// for graph
	strconcat				(fName,name,"level.graph.10");
	if (Engine.FS.Exist(fName)) {
		m_tpGraphVFS			= Engine.FS.Open	(fName);
		m_tpGraphVFS->Read		(&m_tGraphHeader,sizeof(AI::SGraphHeader));
		R_ASSERT				(m_tGraphHeader.dwVersion == XRAI_CURRENT_VERSION);
		m_tpaGraph				= (AI::SGraphVertex*)m_tpGraphVFS->Pointer();
	}

	// for graph
	strconcat				(fName,name,"level.graph.12");
	if (Engine.FS.Exist(fName)) {
		m_tpGraphVFS1			= Engine.FS.Open	(fName);
		m_tpGraphVFS1->Read		(&m_tGraphHeader1,sizeof(AI::SGraphHeader));
		R_ASSERT				(m_tGraphHeader1.dwVersion == XRAI_CURRENT_VERSION);
		m_tpaGraph1				= (AI::SGraphVertex*)m_tpGraphVFS1->Pointer();
	}

	for (int i=0; i<(int)m_tGraphHeader.dwVertexCount; i++) {
		if (m_tpaGraph[i].tPoint.distance_to(m_tpaGraph1[i].tPoint) > EPS_L)
			Msg("%d : [%7.2f,%7.2f,%7.2f] -> [%7.2f,%7.2f,%7.2f]",i,m_tpaGraph[i].tPoint.x,m_tpaGraph[i].tPoint.y,m_tpaGraph[i].tPoint.z,m_tpaGraph1[i].tPoint.x,m_tpaGraph1[i].tPoint.y,m_tpaGraph1[i].tPoint.z);
		if (m_tpaGraph[i].dwNeighbourCount != m_tpaGraph1[i].dwNeighbourCount)
			Msg("%d : %d -> %d ",i,m_tpaGraph[i].dwNeighbourCount,m_tpaGraph1[i].dwNeighbourCount);
		AI::SGraphEdge *P1 = ((AI::SGraphEdge *)((BYTE *)m_tpaGraph + m_tpaGraph[i].dwEdgeOffset));
		AI::SGraphEdge *P2 = ((AI::SGraphEdge *)((BYTE *)m_tpaGraph1 + m_tpaGraph1[i].dwEdgeOffset));
		for (int j=0; j<min(m_tpaGraph[i].dwNeighbourCount,m_tpaGraph1[i].dwNeighbourCount); j++) {
			if (P1[j].dwVertexNumber != P2[j].dwVertexNumber)
				Msg("%d %d : %d -> %d ",i,j,P1[j].dwVertexNumber,P2[j].dwVertexNumber);
			if (P1[j].fPathDistance != P2[j].fPathDistance)
				Msg("%d %d : %7.2f -> %7.2f ",i,j,P1[j].fPathDistance,P2[j].fPathDistance);
		}
		if (m_tpaGraph[i].dwNeighbourCount > m_tpaGraph1[i].dwNeighbourCount)
			for (int j=min(m_tpaGraph[i].dwNeighbourCount,m_tpaGraph1[i].dwNeighbourCount); j<m_tpaGraph[i].dwNeighbourCount; j++)
				Msg("%d 000 : %d %7.2f",i,j,P1[j].dwVertexNumber,P1[j].fPathDistance);
		else
			if (m_tpaGraph[i].dwNeighbourCount < m_tpaGraph1[i].dwNeighbourCount)
				for (int j=min(m_tpaGraph1[i].dwNeighbourCount,m_tpaGraph1[i].dwNeighbourCount); j<m_tpaGraph1[i].dwNeighbourCount; j++)
					Msg("%d 001 : %d %7.2f",i,j,P2[j].dwVertexNumber,P2[j].fPathDistance);
	}


	for (int i=0; i<(int)m_tGraphHeader.dwVertexCount; i++) {
		AI::SGraphEdge *P1 = ((AI::SGraphEdge *)((BYTE *)m_tpaGraph + m_tpaGraph[i].dwEdgeOffset));
		for (int j=0; j<m_tpaGraph[i].dwNeighbourCount; j++)
			Msg("%3d[%7.2f,%7.2f,%7.2f] %3d[%3d][%7.2f,%7.2f,%7.2f] : %5.2f",i,m_tpaGraph[i].tPoint.x,m_tpaGraph[i].tPoint.y,m_tpaGraph[i].tPoint.z,j,P1[j].dwVertexNumber,m_tpaGraph[P1[j].dwVertexNumber].tPoint.x,m_tpaGraph[P1[j].dwVertexNumber].tPoint.y,m_tpaGraph[P1[j].dwVertexNumber].tPoint.z,P1[j].fPathDistance);
	}
	// for a* search
	m_fSize2				= _sqr(m_header.size)/4;
	m_fYSize2				= _sqr((float)(m_header.size_y/32767.0))/4;
	u32 S1					= (MAX_NODES + 1)*sizeof(SNode);
	m_tpHeap				= (SNode *)xr_malloc(S1);
	ZeroMemory				(m_tpHeap,S1);
	u32 S2					= max(vfs ? m_header.count : 0,m_tpGraphVFS ? m_tGraphHeader.dwVertexCount + 1 : 0)*sizeof(SIndexNode);
	m_tpIndexes				= (SIndexNode *)xr_malloc(S2);
	ZeroMemory				(m_tpIndexes,S2);
	Msg						("* AI path-finding structures: %d K",(S1 + S2)/(1024));
	
//	SetPriorityClass	(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
//	SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
//	Sleep				(1);
//	u64 t1x = CPU::GetCycleCount();
//	for (int i=0; i<(int)m_tGraphHeader.dwVertexCount; i++)
//		for (int j=0; j<(int)m_tGraphHeader.dwVertexCount; j++)
//			if (j != i)
//				ffFindMinimalPath(i,j,m_tpaNodes);
//	u64 t2x = CPU::GetCycleCount();
//	SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
//	SetPriorityClass	(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
//	t2x -= t1x;
//	Msg("A star time %11I64u",t2x);
}

void CAI_Space::Render()
{
	if (!bDebug)	return;

	if (m_tpaGraph)
	{
		CGameFont* F		= ((CHUDManager*)Level().HUD())->pFontDI;
		for (int i=0; i<(int)m_tGraphHeader.dwVertexCount; i++) {
			Fvector t1 = m_tpaGraph[i].tPoint;
			t1.y += .6f;
			Device.Primitive.dbg_DrawAABB(t1,.5f,.5f,.5f,D3DCOLOR_XRGB(0,0,255));
			for (int j=0; j<(int)m_tpaGraph[i].dwNeighbourCount; j++) {
				Fvector t2 = m_tpaGraph[((AI::SGraphEdge *)((BYTE *)m_tpaGraph + m_tpaGraph[i].dwEdgeOffset) + j)->dwVertexNumber].tPoint;
				t2.y += .6f;
				Device.Primitive.dbg_DrawLINE(Fidentity,t1,t2,D3DCOLOR_XRGB(0,255,0));
			}
			Fvector         T;
			Fvector4        S;
			T.set			(t1);
			T.y+= 1.5f;
			Device.mFullTransform.transform (S,T);
			F->SetSize	(0.05f/sqrtf(_abs(S.w)));
			F->SetColor(0xffffffff);
			F->Out(S.x,-S.y,"%d",i);
		}
		if (m_tpaNodes.size()) {
			Fvector t1 = m_tpaGraph[m_tpaNodes[0]].tPoint;
			t1.y += .6f;
			Device.Primitive.dbg_DrawAABB(t1,.5f,.5f,.5f,D3DCOLOR_XRGB(0,0,255));
			for (int i=1; i<(int)m_tpaNodes.size(); i++) {
				Fvector t2 = m_tpaGraph[m_tpaNodes[i]].tPoint;
				t2.y += .6f;
				Device.Primitive.dbg_DrawAABB(t2,.5f,.5f,.5f,D3DCOLOR_XRGB(0,0,255));
				Device.Primitive.dbg_DrawLINE(Fidentity,t1,t2,D3DCOLOR_XRGB(255,0,0));
				t1 = t2;
			}
		}
	}

	if (0==vfs)						return;
	if (0==sh_debug)				return;
	if (0==(psAI_Flags&aiDebug))	return;
	
	CGameObject*	O	= dynamic_cast<CGameObject*> (Level().CurrentEntity());
	Fvector	POSITION	= O->Position();
	POSITION.y += 0.5f;

	// display
	Fvector P			= POSITION;

	NodePosition		Local;
	PackPosition		(Local,P);

	u32 ID				= O->AI_NodeID;

	CGameFont* F		= ((CHUDManager*)Level().HUD())->pFontDI;
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

	Device.Shader.set_Shader(sh_debug);
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
			Device.Primitive.dbg_DrawTRI	(Fidentity,v3,v2,v1,CT);
			Device.Primitive.dbg_DrawTRI	(Fidentity,v1,v4,v3,CT);

			// render center
			Device.Primitive.dbg_DrawAABB	(PC,sc,sc,sc,CC);

			// render id
			if (bHL) {
				Fvector		T;
				Fvector4	S;
				T.set		(PC); T.y+=0.3f;
				Device.mFullTransform.transform	(S,T);
				F->SetSize	(0.05f/sqrtf(_abs(S.w)));
				F->SetColor(0xffffffff);
				F->Out		(S.x,-S.y,"~%d",Nid);
			}
		}
	}
}

int	CAI_Space::q_LoadSearch(const Fvector& pos)
{
	if (0==vfs)	return	0;
	Log			("***AAA***");
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
			Level().AI.UnpackPosition(P0,N.p0);
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
	return selected;
}