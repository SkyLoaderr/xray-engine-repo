// AI_Space.cpp: implementation of the CAI_Space class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AI_Space.h"
#include "gameobject.h"
#include "hudmanager.h"

// for a* search
//#include "ai_a_star.h"
#include "ai_console.h"
//
#include "ai_pathnodes.h"
#define MAX_NODES				65535

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
	strconcat				(fName,name,"level.graph");
	if (Engine.FS.Exist(fName)) {
		m_tpGraphVFS			= Engine.FS.Open	(fName);
		m_tpGraphVFS->Read		(&m_tGraphHeader,sizeof(AI::SGraphHeader));
		R_ASSERT				(m_tGraphHeader.dwVersion == XRAI_CURRENT_VERSION);
		m_tpaGraph				= (AI::SGraphVertex*)m_tpGraphVFS->Pointer();
	}

	// for a* search
	m_fSize2				= _sqr(m_fSize = m_header.size)/4;
	m_fYSize2				= _sqr(m_fYSize = (float)(m_header.size_y/32767.0))/4;
	u32 S1					= (MAX_NODES + 1)*sizeof(SNode);
	m_tpHeap				= (SNode *)xr_malloc(S1);
	ZeroMemory				(m_tpHeap,S1);
	u32 S2					= max(vfs ? m_header.count : 0,m_tpGraphVFS ? m_tGraphHeader.dwVertexCount + 1 : 0)*sizeof(SIndexNode);
	m_tpIndexes				= (SIndexNode *)xr_malloc(S2);
	ZeroMemory				(m_tpIndexes,S2);
	Msg						("* AI path-finding structures: %d K",(S1 + S2)/(1024));
}

void CAI_Space::Render()
{
	if (!bDebug)	return;

	if (m_tpaGraph)
	{
		for (int i=0; i<(int)m_tGraphHeader.dwVertexCount; i++) {
			Fvector t1 = m_tpaGraph[i].tPoint;
			t1.y += .6f;
			Device.Primitive.dbg_DrawAABB(t1,.5f,.5f,.5f,D3DCOLOR_XRGB(0,0,255));
			for (int j=0; j<(int)m_tpaGraph[i].dwNeighbourCount; j++) {
				Fvector t2 = m_tpaGraph[((AI::SGraphEdge *)((BYTE *)m_tpaGraph + m_tpaGraph[i].dwEdgeOffset) + j)->dwVertexNumber].tPoint;
				t2.y += .6f;
				Device.Primitive.dbg_DrawLINE(Fidentity,t1,t2,D3DCOLOR_XRGB(0,255,0));
			}
			CGameFont* F		= ((CHUDManager*)Level().HUD())->pFontDI;
			Fvector         T;
			Fvector4        S;
			T.set			(t1);
			T.y+= 1.5f;
			Device.mFullTransform.transform (S,T);
			F->SetSize	(0.05f/sqrtf(_abs(S.w)));
			F->SetColor(0xffffffff);
			F->Out(S.x,-S.y,"%d",i);
		}
	}

//	// temporary
//	for (int i=0; i<3; i++) {
//		vector<Fvector> &tpaVector = Level().m_PatrolPaths["path02"].tpaVectors[i];
//		for (int j=0; j<(int)tpaVector.size(); j++) {
//			Fvector tP1 = tpaVector[j];
//			tP1.y += .11f;
//			Device.Primitive.dbg_DrawAABB(tP1,.1f,.1f,.1f,D3DCOLOR_XRGB(0,0,255));
//			if (j) {
//				Fvector tP2 = tpaVector[j - 1];
//				tP2.y += .11f;
//				Device.Primitive.dbg_DrawLINE(Fidentity,tP1,tP2,D3DCOLOR_XRGB(0,255,0));
//			}
//		}
//
//	}

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

//////////////////////////////////////////////////////////////////////////
// CAIMapShortestPathNode
//////////////////////////////////////////////////////////////////////////

CAIMapShortestPathNode::CAIMapShortestPathNode(SAIMapData &tAIMapData)
{
	tData					= tAIMapData;
	m_dwLastBestNode		= u32(-1);
	NodeCompressed &tNode1	= *tData.tpAI_Space->Node(tData.dwFinishNode);
	x3						= (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
	y3						= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
	z3						= (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
}

IC void CAIMapShortestPathNode::begin(u32 dwNode, CAIMapTemplateNode::iterator &tIterator, CAIMapTemplateNode::iterator &tEnd)
{
	tEnd = (tIterator = (NodeLink *)((BYTE *)tData.tpAI_Space->Node(dwNode) + sizeof(NodeCompressed))) + tData.tpAI_Space->Node(dwNode)->links;
}

IC u32 CAIMapShortestPathNode::get_value(CAIMapTemplateNode::iterator &tIterator)
{
	return(tData.tpAI_Space->UnpackLink(*tIterator));
}

IC bool CAIMapShortestPathNode::bfCheckIfAccessible(u32 dwNode)
{
	return(tData.tpAI_Space->q_mark_bit[dwNode]);
}

IC float CAIMapShortestPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode)
{
	if (m_dwLastBestNode != dwStartNode) {
		NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode), &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);
		
		x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
		y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
		z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
		
		x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

		return(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
	}
	else {
		NodeCompressed &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);

		x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

		return(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
	}
}

IC float CAIMapShortestPathNode::ffAnticipate(u32 dwStartNode)
{
	NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode);
	
	x2 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	y2 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	z2 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	return(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x3 - x2) + _sqr(z3 - z2)) + tData.tpAI_Space->m_fYSize2*_sqr(y3 - y2))));
}

IC float CAIMapShortestPathNode::ffAnticipate()
{
	return(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x3 - x2) + _sqr(z3 - z2)) + tData.tpAI_Space->m_fYSize2*_sqr(y3 - y2))));
}

//////////////////////////////////////////////////////////////////////////
// CAIMapLCDPathNode
//////////////////////////////////////////////////////////////////////////

CAIMapLCDPathNode::CAIMapLCDPathNode(SAIMapDataL &tAIMapData)
{
	tData					= tAIMapData;
	m_dwLastBestNode		= u32(-1);
	NodeCompressed &tNode1	= *tData.tpAI_Space->Node(tData.dwFinishNode);
	x3						= (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
	y3						= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
	z3						= (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
	float fCover			= 1/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);
	float fLight			= (float)(tNode1.light)/255.f;
	m_fSum					= fCover + fLight;
}

IC void CAIMapLCDPathNode::begin(u32 dwNode, CAIMapTemplateNode::iterator &tIterator, CAIMapTemplateNode::iterator &tEnd)
{
	tEnd = (tIterator = (NodeLink *)((BYTE *)tData.tpAI_Space->Node(dwNode) + sizeof(NodeCompressed))) + tData.tpAI_Space->Node(dwNode)->links;
}

IC u32 CAIMapLCDPathNode::get_value(CAIMapTemplateNode::iterator &tIterator)
{
	return(tData.tpAI_Space->UnpackLink(*tIterator));
}

IC bool CAIMapLCDPathNode::bfCheckIfAccessible(u32 dwNode)
{
	return(tData.tpAI_Space->q_mark_bit[dwNode]);
}

IC float CAIMapLCDPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode)
{
	if (m_dwLastBestNode != dwStartNode) {
		NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode), &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);

		x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
		y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
		z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
		
		x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

		float fCover = 1/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

		float fLight = (float)(tNode1.light)/255.f;
		
		return(fLight*tData.fLight + fCover*tData.fCover + tData.fDistance*_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
	}
	else {
		NodeCompressed &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);

		x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

		float fCover = 1/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

		float fLight = (float)(tNode1.light)/255.f;
		
		return(fLight*tData.fLight + fCover*tData.fCover + tData.fDistance*_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
	}
}

IC float CAIMapLCDPathNode::ffAnticipate(u32 dwStartNode)
{
	NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode);

	x2 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	y2 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	z2 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	return(m_fSum + tData.fDistance*_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
}

IC float CAIMapLCDPathNode::ffAnticipate()
{
	return(m_fSum + tData.fDistance*_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
}

//////////////////////////////////////////////////////////////////////////
// CAIMapEnemyPathNode
//////////////////////////////////////////////////////////////////////////

CAIMapEnemyPathNode::CAIMapEnemyPathNode(SAIMapDataE &tAIMapData)
{
	tData					= tAIMapData;
	m_dwLastBestNode		= u32(-1);
	{
		NodeCompressed &tNode1	= *tData.tpAI_Space->Node(tData.dwEnemyNode);
		x4						= (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y4						= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z4						= (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
	}
	NodeCompressed &tNode1	= *tData.tpAI_Space->Node(tData.dwFinishNode);
	x3						= (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
	y3						= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
	z3						= (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
	float fCover			= 1/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);
	float fLight			= (float)(tNode1.light)/255.f;
	m_fSum					= fCover + fLight + tData.fEnemyView*(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x4 - x1) + _sqr(z4 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y4 - y1))) - tData.fEnemyDistance);
}

IC void CAIMapEnemyPathNode::begin(u32 dwNode, CAIMapTemplateNode::iterator &tIterator, CAIMapTemplateNode::iterator &tEnd)
{
	tEnd = (tIterator = (NodeLink *)((BYTE *)tData.tpAI_Space->Node(dwNode) + sizeof(NodeCompressed))) + tData.tpAI_Space->Node(dwNode)->links;
}

IC u32 CAIMapEnemyPathNode::get_value(CAIMapTemplateNode::iterator &tIterator)
{
	return(tData.tpAI_Space->UnpackLink(*tIterator));
}

IC bool CAIMapEnemyPathNode::bfCheckIfAccessible(u32 dwNode)
{
	return(tData.tpAI_Space->q_mark_bit[dwNode]);
}

IC float CAIMapEnemyPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode)
{
	if (m_dwLastBestNode != dwStartNode) {
		NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode), &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);
		
		x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
		y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
		z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
		
		x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

		float fCover = 1.f/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

		float fLight = (float)(tNode1.light)/255.f;
		
		return(tData.fEnemyView*(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x4 - x1) + _sqr(z4 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y4 - y1))) - tData.fEnemyDistance) + fLight*tData.fLight + fCover*tData.fCover + tData.fDistance*(float)sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
	}
	else {
		NodeCompressed &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);
		
		x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

		float fCover = 1.f/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

		float fLight = (float)(tNode1.light)/255.f;
		
		return(tData.fEnemyView*(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x4 - x1) + _sqr(z4 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y4 - y1))) - tData.fEnemyDistance) + fLight*tData.fLight + fCover*tData.fCover + tData.fDistance*(float)sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
	}
}

IC float CAIMapEnemyPathNode::ffAnticipate(u32 dwStartNode)
{
	NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode);
	
	x2 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	y2 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	z2 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	return(m_fSum + tData.fDistance*(float)sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
}		   
		   
IC float CAIMapEnemyPathNode::ffAnticipate()
{		   
	return(m_fSum + tData.fDistance*(float)sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
}

//////////////////////////////////////////////////////////////////////////
// CAIMapEnemyPositionPathNode
//////////////////////////////////////////////////////////////////////////

CAIMapEnemyPositionPathNode::CAIMapEnemyPositionPathNode(SAIMapDataF &tAIMapData)
{
	tData					= tAIMapData;
	m_dwLastBestNode		= u32(-1);
	{
		x4						= tData.tEnemyPosition.x;
		y4						= tData.tEnemyPosition.y;
		z4						= tData.tEnemyPosition.z;
	}
	NodeCompressed &tNode1	= *tData.tpAI_Space->Node(tData.dwFinishNode);
	x3						= (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
	y3						= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
	z3						= (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
	float fCover			= 1/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);
	float fLight			= (float)(tNode1.light)/255.f;
	m_fSum					= fCover + fLight + tData.fEnemyView*(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x4 - x1) + _sqr(z4 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y4 - y1))) - tData.fEnemyDistance);
}

IC void CAIMapEnemyPositionPathNode::begin(u32 dwNode, CAIMapTemplateNode::iterator &tIterator, CAIMapTemplateNode::iterator &tEnd)
{
	tEnd = (tIterator = (NodeLink *)((BYTE *)tData.tpAI_Space->Node(dwNode) + sizeof(NodeCompressed))) + tData.tpAI_Space->Node(dwNode)->links;
}

IC u32 CAIMapEnemyPositionPathNode::get_value(CAIMapTemplateNode::iterator &tIterator)
{
	return(tData.tpAI_Space->UnpackLink(*tIterator));
}

IC bool CAIMapEnemyPositionPathNode::bfCheckIfAccessible(u32 dwNode)
{
	return(tData.tpAI_Space->q_mark_bit[dwNode]);
}

IC float CAIMapEnemyPositionPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode)
{
	if (m_dwLastBestNode != dwStartNode) {
		NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode), &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);
		
		x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
		y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
		z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
		
		x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

		float fCover = 1.f/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

		float fLight = (float)(tNode1.light)/255.f;
		
		return(tData.fEnemyView*(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x4 - x1) + _sqr(z4 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y4 - y1))) - tData.fEnemyDistance) + fLight*tData.fLight + fCover*tData.fCover + tData.fDistance*(float)sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
	}
	else {
		NodeCompressed &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);
		
		x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

		float fCover = 1.f/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

		float fLight = (float)(tNode1.light)/255.f;
		
		return(tData.fEnemyView*(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x4 - x1) + _sqr(z4 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y4 - y1))) - tData.fEnemyDistance) + fLight*tData.fLight + fCover*tData.fCover + tData.fDistance*(float)sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
	}
}

IC float CAIMapEnemyPositionPathNode::ffAnticipate(u32 dwStartNode)
{
	NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode);
	
	x2 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	y2 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	z2 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	return(m_fSum + tData.fDistance*(float)sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
}		   
		   
IC float CAIMapEnemyPositionPathNode::ffAnticipate()
{		   
	return(m_fSum + tData.fDistance*(float)sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
}

//////////////////////////////////////////////////////////////////////////
// CAIGraphShortestPathNode
//////////////////////////////////////////////////////////////////////////

CAIGraphShortestPathNode::CAIGraphShortestPathNode(SAIMapData &tAIMapData)
{
	tData = tAIMapData;
	m_dwLastBestNode = u32(-1);
}

IC void CAIGraphShortestPathNode::begin(u32 dwNode, CAIGraphTemplateNode::iterator &tIterator, CAIGraphTemplateNode::iterator &tEnd)
{
	tIterator = (AI::SGraphEdge *)((BYTE *)tData.tpAI_Space->m_tpaGraph + tData.tpAI_Space->m_tpaGraph[dwNode].dwEdgeOffset);
	tEnd = tIterator + tData.tpAI_Space->m_tpaGraph[dwNode].dwNeighbourCount;
}

IC bool CAIGraphShortestPathNode::bfCheckIfAccessible(u32 dwNode)
{
	return(true);
}

IC float CAIGraphShortestPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode)
{
	return(((AI::SGraphEdge *)((BYTE *)tData.tpAI_Space->m_tpaGraph + tData.tpAI_Space->m_tpaGraph[dwStartNode].dwEdgeOffset) + (m_dwLastBestNode = dwFinishNode))->fPathDistance);
}

IC float CAIGraphShortestPathNode::ffAnticipate(u32 dwStartNode)
{
	return(tData.tpAI_Space->m_tpaGraph[dwStartNode].tPoint.distance_to(tData.tpAI_Space->m_tpaGraph[tData.dwFinishNode].tPoint));
}

IC float CAIGraphShortestPathNode::ffAnticipate()
{
	return(tData.tpAI_Space->m_tpaGraph[m_dwLastBestNode].tPoint.distance_to(tData.tpAI_Space->m_tpaGraph[tData.dwFinishNode].tPoint));
}

//////////////////////////////////////////////////////////////////////////
// Optimal paths
//////////////////////////////////////////////////////////////////////////


float CAI_Space::vfFindMinimalPath(u32 dwStartNode, u32 dwGoalNode, AI::Path& Result, bool bUseMarks)
{
	SAIMapData			tData;
	float				fDistance;
	tData.dwFinishNode	= dwGoalNode;
	tData.tpAI_Space	= this;
	m_tpMapPath.vfFindOptimalPath(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,tData,dwStartNode,dwGoalNode,1000.f,fDistance,Result.Nodes,bUseMarks);
	return(fDistance);
}

float CAI_Space::vfFindOptimalPath(u32 dwStartNode, u32 dwGoalNode, AI::Path& Result, float fLightWeight, float fCoverWeight, float fDistanceWeight, bool bUseMarks)
{
	SAIMapDataL			tData;
	float				fDistance;
	tData.dwFinishNode	= dwGoalNode;
	tData.tpAI_Space	= this;
	tData.fLight		= fLightWeight;
	tData.fCover		= fCoverWeight;
	tData.fDistance		= fDistanceWeight;
	m_tpLCDPath.vfFindOptimalPath(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,tData,dwStartNode,dwGoalNode,1000.f,fDistance,Result.Nodes,bUseMarks);
	return(fDistance);
}

float CAI_Space::vfFindOptimalPath(u32 dwStartNode, u32 dwGoalNode, AI::Path& Result, u32 dwEnemyNode, float fOptimalEnemyDistance, float fLightWeight, float fCoverWeight, float fDistanceWeight, float fEnemyViewWeight, bool bUseMarks)
{
	SAIMapDataE			tData;
	float				fDistance;
	tData.dwFinishNode	= dwGoalNode;
	tData.tpAI_Space	= this;
	tData.fLight		= fLightWeight;
	tData.fCover		= fCoverWeight;
	tData.fDistance		= fDistanceWeight;
	tData.dwEnemyNode	= dwEnemyNode;
	tData.fEnemyDistance = fOptimalEnemyDistance;
	tData.fEnemyView	= fEnemyViewWeight;
	m_tpEnemyPath.vfFindOptimalPath(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,tData,dwStartNode,dwGoalNode,1000.f,fDistance,Result.Nodes,bUseMarks);
	return(fDistance);
}

float CAI_Space::vfFindOptimalPath(u32 dwStartNode, u32 dwGoalNode, AI::Path& Result, Fvector tEnemyPosition, float fOptimalEnemyDistance, float fLightWeight, float fCoverWeight, float fDistanceWeight, float fEnemyViewWeight, bool bUseMarks)
{
	SAIMapDataF			tData;
	float				fDistance;
	tData.fLight		= fLightWeight;
	tData.fCover		= fCoverWeight;
	tData.fDistance		= fDistanceWeight;
	tData.tEnemyPosition = tEnemyPosition;
	tData.fEnemyDistance = fOptimalEnemyDistance;
	tData.fEnemyView	= fEnemyViewWeight;
	m_tpEnemyPositionPath.vfFindOptimalPath(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,tData,dwStartNode,dwGoalNode,1000.f,fDistance,Result.Nodes,bUseMarks);
	return(fDistance);
}