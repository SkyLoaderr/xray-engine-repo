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
	q_mark.assign		(m_header.count,0);
	q_mark_bit.assign	(m_header.count,false);
	q_mark_bit_x.assign	(m_header.count,false);

	// for a* search
	u32 S1					= (MAX_NODES + 1)*sizeof(SNode);
	m_tpHeap				= (SNode *)xr_malloc(S1);
	ZeroMemory				(m_tpHeap,S1);
	u32 S2					= (m_header.count)*sizeof(SIndexNode);
	m_tpIndexes				= (SIndexNode *)xr_malloc(S2);
	ZeroMemory				(m_tpIndexes,S2);
	Msg("* AI path-finding structures: %d K",(S1 + S2)/(1024));
	
	// for graph
	strconcat	(fName,name,"level.graph");
	if (!Engine.FS.Exist(fName))
		return;
	m_tpGraphVFS = Engine.FS.Open	(fName);
	m_tpGraphVFS->Read(&m_tGraphHeader,sizeof(SGraphHeader));
	R_ASSERT(m_tGraphHeader.dwVersion == XRAI_CURRENT_VERSION);
	m_tpaGraph = (SGraphVertex*)m_tpGraphVFS->Pointer();
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
				Fvector t2 = m_tpaGraph[((SGraphEdge *)((BYTE *)m_tpaGraph + m_tpaGraph[i].dwEdgeOffset) + j)->dwVertexNumber].tPoint;
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

//#define MAP_AVAILABLE_CELL		'.'
//#define MAP_UNAVAILABLE_CELL	'x'
//#define ABC_SIZE				62
//
//void CAI_Space::vfCreate2DMap(char *caFile0, char *caFile1, char *caFile2)
//{
//	// finding _min and max values
//	s16 sMinX = 30000, sMaxX = -30000, sMinZ = 30000, sMaxZ = -30000;
//	for (int i=1; i<(int)m_header.count; i++) {
//		NodeCompressed *tpNode = m_nodes_ptr[i];
//		if (tpNode->p0.x < sMinX)
//			sMinX = tpNode->p0.x;
//		if (tpNode->p0.x > sMaxX)
//			sMaxX = tpNode->p0.x;
//		if (tpNode->p0.z < sMinZ)
//			sMinZ = tpNode->p0.z;
//		if (tpNode->p0.z > sMaxZ)
//			sMaxZ = tpNode->p0.z;
//		if (tpNode->p1.x < sMinX)
//			sMinX = tpNode->p1.x;
//		if (tpNode->p1.x > sMaxX)
//			sMaxX = tpNode->p1.x;
//		if (tpNode->p1.z < sMinZ)
//			sMinZ = tpNode->p1.z;
//		if (tpNode->p1.z > sMaxZ)
//			sMaxZ = tpNode->p1.z;
//	}
//	
//	// allocating memory
//	int M = sMaxX - sMinX + 1;
//	int N = sMaxZ - sMinZ + 1;
//	unsigned char **tppMap;
//	tppMap = (unsigned char **)xr_malloc(sizeof(char *)*N);
//	memset(tppMap,0,sizeof(unsigned char *)*N);
//	for (int i=0; i<N; i++) {
//		tppMap[i] = (unsigned char *)xr_malloc(sizeof(unsigned char)*M);
//		memset(tppMap[i],MAP_UNAVAILABLE_CELL,sizeof(unsigned char)*M);
//	}
//
//	// generating mini-nodes
//	for (int i=1; i<(int)m_header.count; i++) {
//		NodeCompressed *tpNode = m_nodes_ptr[i];
//		Fvector tVector;
//		UnpackPosition(tVector,tpNode->p0);
//		if ((tVector.y < 1.5f) || (_min(tpNode->cover[0],_min(tpNode->cover[1],_min(tpNode->cover[2],tpNode->cover[3]))) > 200))
//			for (s16 j = tpNode->p0.z - sMinZ; j<tpNode->p1.z - sMinZ + 1; j++)
//				memset(tppMap[j] + sMaxX - tpNode->p1.x, MAP_AVAILABLE_CELL, tpNode->p1.x - tpNode->p0.x + 1);
//	}
//	
//	// saving mini-nodes
//	FILE *fOutput = fopen(caFile0,"wb");
//	for (int i=0; i<N; i++) {
//		fwrite(tppMap[i],sizeof(unsigned char),M,fOutput);
//		fprintf(fOutput,"\n");
//	}
//	fclose(fOutput);
//	
//	// initializing node alphabet
//	char caABC[ABC_SIZE] = {'0','1','2','3','4','5','6','7','8','9',
//					'a','b','c','d','e','f','g','h','i','j',
//					'k','l','m','n','o','p','q','r','s','t',
//					'u','v','w','x','y','z','A','B','C','D',
//					'E','F','G','H','I','J','K','L','M','N',
//					'O','P','Q','R','S','T','U','V','W','X',
//					'Y','Z'};
//	
//	// converting mini-nodes
//	for (int i=0; i<N; i++)
//		for (int j=0; j<M; j++)
//			if (tppMap[i][j] == MAP_AVAILABLE_CELL)
//				tppMap[i][j] = 0;
//			else
//				tppMap[i][j] = 255;
//	
//	// building izo-mini-nodes
//	for (unsigned char ucStart = 0; ucStart < ABC_SIZE; ucStart++) {
//		bool bOk = true;
//		unsigned char ucValue = ucStart ? ucStart : (unsigned char)255;
//		for (int i=0; i<N; i++)
//			for (int j=0; j<M; j++)
//				if (tppMap[i][j] == 0) {
//					if ((i > 0) && (tppMap[i - 1][j] == ucValue)) {
//						tppMap[i][j] = ucStart + 1;
//						bOk = false;
//						continue;
//					}
//					
//					if ((i < N) && (tppMap[i + 1][j] == ucValue)) {
//						tppMap[i][j] = ucStart + 1;
//						bOk = false;
//						continue;
//					}
//					
//					if ((j > 0) && (tppMap[i][j - 1] == ucValue)) {
//						tppMap[i][j] = ucStart + 1;
//						bOk = false;
//						continue;
//					}
//					
//					if ((j < M) && (tppMap[i][j + 1] == ucValue)) {
//						tppMap[i][j] = ucStart + 1;
//						bOk = false;
//						continue;
//					}
//				}
//		if (bOk)
//			break;
//	}
//	
//	// converting izo-mini-nodes
//	for (int i=0; i<N; i++)
//		for (int j=0; j<M; j++)
//			if (tppMap[i][j] < ABC_SIZE)
//				tppMap[i][j] = caABC[tppMap[i][j]];
//			else
//				if (tppMap[i][j] == 255)
//					tppMap[i][j] = '.';
//				else
//					tppMap[i][j] = '0';
//
//	// saving izo-mini-nodes
//	fOutput = fopen(caFile1,"wb");
//	for (int i=0; i<N; i++) {
//		fwrite(tppMap[i],sizeof(unsigned char),M,fOutput);
//		fprintf(fOutput,"\n");
//	}
//	fclose(fOutput);
//	
//	// freeing resources
//	for (int i=0; i<N; i++)
//		_FREE(tppMap[i]);
//	_FREE(tppMap);
//}
