// AI_Space.cpp: implementation of the CAI_Space class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AI_Space.h"
#include "gameobject.h"

// for a* search
#include "ai_a_star.h"
#include "ai_console.h"
//
#include "ai_pathnodes.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAI_Space::CAI_Space	()
{
	m_nodes_ptr	= NULL;
	vfs			= NULL;
	sh_debug	= 0;

	Device.seqDevCreate.Add		(this);
	Device.seqDevDestroy.Add	(this);
	OnDeviceCreate				();
}

CAI_Space::~CAI_Space	()
{
	// for a* search
	vfUnloadSearch();
	//

	Device.seqDevCreate.Remove	(this);
	Device.seqDevDestroy.Remove	(this);
	Device.Shader.Delete		(sh_debug);
	OnDeviceDestroy				();

	_FREE	(m_nodes_ptr);
	_DELETE	(vfs);
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

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//extern void	UnpackContour(PContour& C, u32 ID);
//extern void	IntersectContours(PSegment& Dest, PContour& C1, PContour& C2);
//
//IC bool bfInsideContour(Fvector &tPoint, PContour &tContour)
//{
//	return((tContour.v1.x - EPS_L <= tPoint.x) && (tContour.v1.z - EPS_L <= tPoint.z) && (tContour.v3.x + EPS_L >= tPoint.x) && (tContour.v3.z + EPS_L >= tPoint.z));
//}
//
//IC bool bfSimilar(Fvector &tPoint0, Fvector &tPoint1)
//{
//	return((_abs(tPoint0.x - tPoint1.x) < EPS_L) && (_abs(tPoint0.z - tPoint1.z) < EPS_L));
//}
//
//bool bfIntersectContours(PSegment &tSegment, PContour &tContour0, PContour &tContour1, bool bLog = true)
//{
//	bool bFound = false;
//	
//	if (bfInsideContour(tContour0.v1,tContour1)) {
//		tSegment.v1 = tContour0.v1;
//		bFound = true;
//	}
//
//	if (bfInsideContour(tContour0.v2,tContour1)) {
//		if (!bFound) {
//			tSegment.v1 = tContour0.v2;
//			bFound = true;
//		}
//		else {
//			tSegment.v2 = tContour0.v2;
//			return(true);
//		}
//	}
//	if (bfInsideContour(tContour0.v3,tContour1)) {
//		if (!bFound) {
//			tSegment.v1 = tContour0.v3;
//			bFound = true;
//		}
//		else {
//			tSegment.v2 = tContour0.v3;
//			return(true);
//		}
//	}
//	if (bfInsideContour(tContour0.v4,tContour1)) {
//		if (!bFound) {
//			tSegment.v1 = tContour0.v4;
//			bFound = true;
//		}
//		else {
//			tSegment.v2 = tContour0.v4;
//			return(true);
//		}
//	}
//	if (bFound) {
//		if (bfInsideContour(tContour1.v1,tContour0) && (!(bfSimilar(tSegment.v1,tContour1.v1)))) {
//			tSegment.v2 = tContour1.v1;
//			return(true);
//		}
//		if (bfInsideContour(tContour1.v2,tContour0) && (!(bfSimilar(tSegment.v1,tContour1.v2)))) {
//			tSegment.v2 = tContour1.v2;
//			return(true);
//		}
//		if (bfInsideContour(tContour1.v3,tContour0) && (!(bfSimilar(tSegment.v1,tContour1.v3)))) {
//			tSegment.v2 = tContour1.v3;
//			return(true);
//		}
//		if (bfInsideContour(tContour1.v4,tContour0) && (!(bfSimilar(tSegment.v1,tContour1.v4)))) {
//			tSegment.v2 = tContour1.v4;
//			return(true);
//		}
//	}
//	else {
//		if (bfInsideContour(tContour1.v1,tContour0)) {
//			tSegment.v1 = tContour1.v1;
//			bFound = true;
//		}
//		if (bfInsideContour(tContour1.v2,tContour0)) {
//			if (!bFound) {
//				tSegment.v1 = tContour1.v2;
//				bFound = true;
//			}
//			else {
//				tSegment.v2 = tContour1.v2;
//				return(true);
//			}
//		}
//		if (bfInsideContour(tContour1.v3,tContour0)) {
//			if (!bFound) {
//				tSegment.v1 = tContour1.v3;
//				bFound = true;
//			}
//			else {
//				tSegment.v2 = tContour1.v3;
//				return(true);
//			}
//		}
//		if (bfInsideContour(tContour1.v4,tContour0)) {
//			if (!bFound) {
//				tSegment.v1 = tContour1.v4;
//				bFound = true;
//			}
//			else {
//				tSegment.v2 = tContour1.v4;
//				return(true);
//			}
//		}
//	}
//
//	if (bLog)
//		if (bFound) {
//			tSegment.v2 = tSegment.v1;
//			Msg("! AI_PathNodes: segment has null length ([%6.4f,%6.4f],[%6.4f,%6.4f] -> [%6.4f,%6.4f],[%6.4f,%6.4f])",tContour0.v1.x,tContour0.v1.z,tContour0.v3.x,tContour0.v3.z,tContour1.v1.x,tContour1.v1.z,tContour1.v3.x,tContour1.v3.z);
//		}
//		else
//			Msg("! AI_PathNodes: Can't find intersection segment ([%6.4f,%6.4f],[%6.4f,%6.4f] -> [%6.4f,%6.4f],[%6.4f,%6.4f])",tContour0.v1.x,tContour0.v1.z,tContour0.v3.x,tContour0.v3.z,tContour1.v1.x,tContour1.v1.z,tContour1.v3.x,tContour1.v3.z);
//
//	return(false);
//}
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

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

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//	PContour tCurContour, tNextContour, tContour0, tContour1;
//	PSegment tSegment;
//	NodeCompressed *tpNode;
//	NodeLink *taLinks,*taLinks1;
//	u32		dwErrorsCount = 0;
//	
//	for ( I=0; I<m_header.count; I++)
//	{
//		tpNode = Node(I);
//		UnpackContour(tCurContour,I);
//		taLinks = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
//		int iCount = tpNode->link_count;
//		int iNodeIndex;
//		bool bOk = true;
//		for (int i=0; i < iCount; i++) {
//			iNodeIndex = UnpackLink(taLinks[i]);
//			q_mark[iNodeIndex] = true;
//			UnpackContour(tNextContour,iNodeIndex);
//			bool bTemp = bfIntersectContours(tSegment,tCurContour,tNextContour);
//			if (!bTemp) {
//				Msg("First : node %d (%d), neighbour %d (%d)",I,iCount,iNodeIndex,i);
//				bOk = false;
//			}
//		}
//		if (!bOk) {
//			dwErrorsCount++;
//			for (int j=0; j<m_header.count; j++) {
//				if (q_mark[j] || (I == j))
//					continue;
//				q_mark[j] = true;
//				UnpackContour(tNextContour,j);
//				if (bfIntersectContours(tSegment,tCurContour,tNextContour,false) && (_abs(tCurContour.v1.y - tNextContour.v1.y) < 1.5f)) {
//					NodeCompressed *tpDummy0 = Node(j);
//					NodeCompressed *tpDummy1 = Node(I);
//					UnpackContour(tContour0,j);
//					UnpackContour(tContour1,I);
//					taLinks1 = (NodeLink *)((BYTE *)tpDummy0 + sizeof(NodeCompressed));
//					for (int k=0; k<tpDummy0->link_count; k++)
//						if (UnpackLink(taLinks1[k]) == I) {
//							Msg("Third : node %d (%d) neighbour %d (%d)",j,tpDummy0->link_count,I,k);
//							Msg("[%6.4f,%6.4f],[%6.4f,%6.4f] -> [%6.4f,%6.4f],[%6.4f,%6.4f])",tContour0.v1.x,tContour0.v1.z,tContour0.v3.x,tContour0.v3.z,tContour1.v1.x,tContour1.v1.z,tContour1.v3.x,tContour1.v3.z);
//							k = -1;
//							break;
//						}
//					if (k != -1) {
//						Msg("Second : node %d (%d)",j,tpDummy0->link_count);
//						Msg("[%6.4f,%6.4f],[%6.4f,%6.4f] -> [%6.4f,%6.4f],[%6.4f,%6.4f])",tContour0.v1.x,tContour0.v1.z,tContour0.v3.x,tContour0.v3.z,tContour1.v1.x,tContour1.v1.z,tContour1.v3.x,tContour1.v3.z);
//					}
//				}
//			}
//			for (int j=0; j<m_header.count; j++)
//				q_mark[j] = false;
//		}
//		else
//			for (int i=0; i < iCount; i++) {
//				iNodeIndex = UnpackLink(taLinks[i]);
//				q_mark[iNodeIndex] = false;
//			}
//	}
//	Msg("Total errors : %d",dwErrorsCount);
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// for a* search
	vfLoadSearch();
	//
}

void CAI_Space::Render()
{
	// temporary
	for (int i=0; i<3; i++) {
		vector<Fvector> &tpaVector = Level().m_PatrolPaths["path02"].tpaVectors[i];
		for (int j=0; j<(int)tpaVector.size(); j++) {
			Fvector tP1 = tpaVector[j];
			tP1.y += .11f;
			Device.Primitive.dbg_DrawAABB(tP1,.1f,.1f,.1f,D3DCOLOR_XRGB(0,0,255));
			if (j) {
				Fvector tP2 = tpaVector[j - 1];
				tP2.y += .11f;
				Device.Primitive.dbg_DrawLINE(Fidentity,tP1,tP2,D3DCOLOR_XRGB(0,255,0));
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

	u32 ID			= O->AI_NodeID;

	pApp->pFont->Size	(.02f);
	pApp->pFont->Out	(0.f,0.5f,"%f,%f,%f",VPUSH(P));
	pApp->pFont->Out	(0.f,0.55f,"%3d,%4d,%3d -> %d",	iFloor(Local.x),iFloor(Local.y),iFloor(Local.z),u32(ID));

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
	pApp->pFont->Color		(D3DCOLOR_RGBA(255,255,255,255));

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
				pApp->pFont->Size	(0.05f/sqrtf(_abs(S.w)));
				pApp->pFont->Out	(S.x,-S.y,"~%d",Nid);
			}
		}
	}
}

int	CAI_Space::q_LoadSearch(const Fvector& pos)
{
	if (0==vfs)	return	0;

	//vfCreate2DMap("level2_dima.map0", "level2_dima.map1", "level2_dima.map2");
	Log			("***AAA***");

	NodePosition	P;
	PackPosition	(P,pos);
	short min_dist	= 32767;
	int selected	= -1;
	for (u32 I=0; I<m_header.count; I++)
	{
		NodeCompressed& N = *m_nodes_ptr[I];

		if (I == 1735) {
			I = I;
		}
		if (u_InsideNode(N,P)) 
		{
			/**
			int dist = _abs(iFloor(P.y)-iFloor((N.p0.y + N.p1.y)/2.f));
			//if (dist>=0) {
				if (dist<min_dist) {
					min_dist = dist;
					selected = I;
				}
			//}
			/**/
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
			/**/
		}
	}
	return selected;
}

#define MAP_AVAILABLE_CELL		'.'
#define MAP_UNAVAILABLE_CELL	'x'
#define ABC_SIZE				62

void CAI_Space::vfCreate2DMap(char *caFile0, char *caFile1, char *caFile2)
{
	// finding _min and max values
	s16 sMinX = 30000, sMaxX = -30000, sMinZ = 30000, sMaxZ = -30000;
	for (int i=1; i<(int)m_header.count; i++) {
		NodeCompressed *tpNode = m_nodes_ptr[i];
		if (tpNode->p0.x < sMinX)
			sMinX = tpNode->p0.x;
		if (tpNode->p0.x > sMaxX)
			sMaxX = tpNode->p0.x;
		if (tpNode->p0.z < sMinZ)
			sMinZ = tpNode->p0.z;
		if (tpNode->p0.z > sMaxZ)
			sMaxZ = tpNode->p0.z;
		if (tpNode->p1.x < sMinX)
			sMinX = tpNode->p1.x;
		if (tpNode->p1.x > sMaxX)
			sMaxX = tpNode->p1.x;
		if (tpNode->p1.z < sMinZ)
			sMinZ = tpNode->p1.z;
		if (tpNode->p1.z > sMaxZ)
			sMaxZ = tpNode->p1.z;
	}
	
	// allocating memory
	int M = sMaxX - sMinX + 1;
	int N = sMaxZ - sMinZ + 1;
	unsigned char **tppMap;
	tppMap = (unsigned char **)xr_malloc(sizeof(char *)*N);
	memset(tppMap,0,sizeof(unsigned char *)*N);
	for (int i=0; i<N; i++) {
		tppMap[i] = (unsigned char *)xr_malloc(sizeof(unsigned char)*M);
		memset(tppMap[i],MAP_UNAVAILABLE_CELL,sizeof(unsigned char)*M);
	}

	// generating mini-nodes
	for (int i=1; i<(int)m_header.count; i++) {
		NodeCompressed *tpNode = m_nodes_ptr[i];
		Fvector tVector;
		UnpackPosition(tVector,tpNode->p0);
		if ((tVector.y < 1.5f) || (_min(tpNode->cover[0],_min(tpNode->cover[1],_min(tpNode->cover[2],tpNode->cover[3]))) > 200))
			for (s16 j = tpNode->p0.z - sMinZ; j<tpNode->p1.z - sMinZ + 1; j++)
				memset(tppMap[j] + sMaxX - tpNode->p1.x, MAP_AVAILABLE_CELL, tpNode->p1.x - tpNode->p0.x + 1);
	}
	
	// saving mini-nodes
	FILE *fOutput = fopen(caFile0,"wb");
	for (int i=0; i<N; i++) {
		fwrite(tppMap[i],sizeof(unsigned char),M,fOutput);
		fprintf(fOutput,"\n");
	}
	fclose(fOutput);
	
	// initializing node alphabet
	char caABC[ABC_SIZE] = {'0','1','2','3','4','5','6','7','8','9',
					'a','b','c','d','e','f','g','h','i','j',
					'k','l','m','n','o','p','q','r','s','t',
					'u','v','w','x','y','z','A','B','C','D',
					'E','F','G','H','I','J','K','L','M','N',
					'O','P','Q','R','S','T','U','V','W','X',
					'Y','Z'};
	
	// converting mini-nodes
	for (int i=0; i<N; i++)
		for (int j=0; j<M; j++)
			if (tppMap[i][j] == MAP_AVAILABLE_CELL)
				tppMap[i][j] = 0;
			else
				tppMap[i][j] = 255;
	
	// building izo-mini-nodes
	for (unsigned char ucStart = 0; ucStart < ABC_SIZE; ucStart++) {
		bool bOk = true;
		unsigned char ucValue = ucStart ? ucStart : (unsigned char)255;
		for (int i=0; i<N; i++)
			for (int j=0; j<M; j++)
				if (tppMap[i][j] == 0) {
					if ((i > 0) && (tppMap[i - 1][j] == ucValue)) {
						tppMap[i][j] = ucStart + 1;
						bOk = false;
						continue;
					}
					
					if ((i < N) && (tppMap[i + 1][j] == ucValue)) {
						tppMap[i][j] = ucStart + 1;
						bOk = false;
						continue;
					}
					
					if ((j > 0) && (tppMap[i][j - 1] == ucValue)) {
						tppMap[i][j] = ucStart + 1;
						bOk = false;
						continue;
					}
					
					if ((j < M) && (tppMap[i][j + 1] == ucValue)) {
						tppMap[i][j] = ucStart + 1;
						bOk = false;
						continue;
					}
				}
		if (bOk)
			break;
	}
	
	// converting izo-mini-nodes
	for (int i=0; i<N; i++)
		for (int j=0; j<M; j++)
			if (tppMap[i][j] < ABC_SIZE)
				tppMap[i][j] = caABC[tppMap[i][j]];
			else
				if (tppMap[i][j] == 255)
					tppMap[i][j] = '.';
				else
					tppMap[i][j] = '0';

	// saving izo-mini-nodes
	fOutput = fopen(caFile1,"wb");
	for (int i=0; i<N; i++) {
		fwrite(tppMap[i],sizeof(unsigned char),M,fOutput);
		fprintf(fOutput,"\n");
	}
	fclose(fOutput);
	
	// freeing resources
	for (int i=0; i<N; i++)
		_FREE(tppMap[i]);
	_FREE(tppMap);
}
