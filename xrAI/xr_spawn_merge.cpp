////////////////////////////////////////////////////////////////////////////
//	Module 		: xr_spawn_merge.cpp
//	Created 	: 29.01.2003
//  Modified 	: 29.01.2003
//	Author		: Dmitriy Iassenev
//	Description : Merging level spawns for off-line AI NPC computations
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xr_ini.h"

#include "xrAI.h"
#include "xrLevel.h"
#include "xrGraph.h"
#include "ai_nodes.h"

#include "xrServer_Entities.h"
#include "xr_spawn_merge.h"

DEFINE_VECTOR(u32,				DWORD_VECTOR,			DWORD_IT);
DEFINE_VECTOR(xrServerEntity *, SERVER_ENTITY_P_VECTOR, SERVER_ENTITY_P_IT);

CVirtualFileStream				*tpGraphVFS = 0;
SCompressedGraphVertex			*tpaGameGraph;

class CSpawnComparePredicate {
private:
	u32							m_dwStartNode;
public:
	CSpawnComparePredicate(u32 dwStartNode)
	{
		m_dwStartNode = dwStartNode;
	}

	IC bool operator()(u32 dwNode1, u32 dwNode2) const 
	{
		return(ffGetDistanceBetweenNodeCenters(m_dwStartNode,dwNode1) < ffGetDistanceBetweenNodeCenters(m_dwStartNode,dwNode2));
	};
};

class CSpawn : public CThread {
public:
	SLevel						m_tLevel;
	CStream*					vfs;			// virtual file
	hdrNODES					m_header;		// m_header
	BYTE*						m_nodes;		// virtual nodes DATA array
	NodeCompressed**			m_nodes_ptr;	// pointers to node's data
	SERVER_ENTITY_P_VECTOR		m_tpSpawnPoints;
	u32							m_dwLevelID;
	u32							m_dwStart;
	u32							m_dwEnd;
	u32							m_dwAStarStaticCounter;
	SNode						*m_tpHeap;
	SNode						**m_tppHeap;
	SIndexNode					*m_tpIndexes;
	vector<u32>					m_tpaNodes;
	CAStarSearch<CAIMapShortestPathNode,SAIMapData> m_tpMapPath;
	SAIMapData					m_tData;
	u32							m_dwMaxNodeCount;
	DWORD_VECTOR				m_tpSpawnNodes;
	DWORD_VECTOR				m_tpGraphNodes;
	DWORD_VECTOR				m_tpResultNodes;

								CSpawn(SLevel &tLevel, u32 dwLevelID) : CThread(dwLevelID)
	{
		thDestroyOnComplete		= FALSE;
		// loading AI map
		m_tLevel				= tLevel;
		m_dwLevelID				= dwLevelID;
		string256				fName;
		strconcat				(fName,"gamedata\\levels\\",m_tLevel.caLevelName);
		strconcat				(fName,fName,"\\level.ai");
		vfs						= xr_new<CVirtualFileStream>(fName);
		
		// m_header & data
		vfs->Read				(&m_header,sizeof(m_header));
		R_ASSERT				(m_header.version == XRAI_CURRENT_VERSION);
		m_nodes					= (BYTE*) vfs->Pointer();

		m_fSize2				= _sqr(m_header.size)/4;
		m_fYSize2				= _sqr((float)(m_header.size_y/32767.0))/4;
		// dispatch table
		m_nodes_ptr				= (NodeCompressed**)xr_malloc(m_header.count*sizeof(void*));
		Progress				(0.0f);
		for (u32 I=0; I<m_header.count; Progress(float(++I)/m_header.count)) {
			m_nodes_ptr[I]		= (NodeCompressed*)vfs->Pointer();

			NodeCompressed		C;
			vfs->Read			(&C,sizeof(C));

			u32					L = C.links;
			vfs->Advance		(L*sizeof(NodeLink));
		}
		m_dwMaxNodeCount		= m_header.count + 2;

		// loading spawn points
		fName[0]				= 0;
		strconcat				(fName,"gamedata\\levels\\",m_tLevel.caLevelName);
		strconcat				(fName,fName,"\\level.spawn");
		CVirtualFileStream		*SP = xr_new<CVirtualFileStream>(fName);
		CStream					*S = 0;
		NET_Packet				P;
		int						S_id	= 0;
		while (0!=(S = SP->OpenChunk(S_id)))
		{
			P.B.count			= S->Length();
			S->Read				(P.B.data,P.B.count);
			S->Close			();
			u16					ID;
			P.r_begin			(ID);
			R_ASSERT			(M_SPAWN==ID);
			//Process_spawn	(P,0);
			string64			s_name;
			P.r_string			(s_name);
			// create server entity
			xrServerEntity*	E	= F_entity_Create	(s_name);
			R_ASSERT2(E,"Can't create entity.");
			E->Spawn_Read		(P);
			//
			m_tpSpawnPoints.push_back(E);
			S_id++;
		}
	};
	virtual 					~CSpawn()
	{
		for (int i=0; i<(int)m_tpSpawnPoints.size(); i++)
			xr_delete(m_tpSpawnPoints[i]);
		xr_free(m_tppHeap);
		xr_free(m_tpHeap);
		xr_free(m_tpIndexes);
		xr_delete(vfs);
	};

	u32							dwfFindCorrespondingNode(Fvector &tLocalPoint)
	{
		NodePosition	P;
		PackPosition	(P,tLocalPoint);
		short min_dist	= 32767;
		int selected	= -1;
		for (int i=0; i<(int)m_header.count; i++) {
			NodeCompressed& N = *m_nodes_ptr[i];
			if (u_InsideNode(N,P)) {
				Fvector	DUP, vNorm, v, v1, P0;
				DUP.set(0,1,0);
				pvDecompress(vNorm,N.plane);
				Fplane PL; 
				UnpackPosition(P0,N.p0);
				PL.build(P0,vNorm);
				v.set(tLocalPoint.x,P0.y,tLocalPoint.z);	
				PL.intersectRayPoint(v,DUP,v1);
				int dist = iFloor((v1.y - tLocalPoint.y)*(v1.y - tLocalPoint.y));
				if (dist < min_dist) {
					min_dist = (short)dist;
					selected = i;
				}
			}
		}
		return(selected);
	}

	void						Execute()
	{
		m_tpGraphNodes.resize	(tGraphHeader.dwVertexCount);
		m_tpSpawnNodes.resize	(m_tpSpawnPoints.size());
		m_tpResultNodes.resize	(m_tpSpawnPoints.size());
		u32						dwStart = tGraphHeader.dwVertexCount, dwFinish = tGraphHeader.dwVertexCount;
		for (int i=0; i<(int)tGraphHeader.dwVertexCount; i++)
			if (tpaGameGraph[i].tLevelID == m_dwLevelID) {
				m_tpGraphNodes[i] = dwfFindCorrespondingNode(tpaGameGraph[i].tLocalPoint);
				if (dwStart > (u32)i)
					dwStart = (u32)i;
			}
			else {
				m_tpGraphNodes[i] = -1;
				if ((dwStart <= (u32)i) && (dwFinish > (u32)i))
					dwFinish = i;
			}
		DWORD_IT				BB = m_tpGraphNodes.begin();
		DWORD_IT				B = BB + dwStart;
		DWORD_IT				E = m_tpGraphNodes.begin() + dwFinish;
		for (int i=0; i<(int)m_tpSpawnPoints.size(); i++) {
			R_ASSERT((m_tpSpawnNodes[i] = dwfFindCorrespondingNode(m_tpSpawnPoints[i]->o_Position)) != -1);
			sort(B,E,CSpawnComparePredicate(m_tpSpawnNodes[i]));
			DWORD_IT			I = B;
			float				fCurrentBestDistance = MAX_DISTANCE_TO_CONNECT;
			u32					dwBest = u32(-1);
			for ( ; I != E; I++) {
				if (ffGetDistanceBetweenNodeCenters(m_tpSpawnNodes[i],*I) >= fCurrentBestDistance)
					break;
				m_tData.dwFinishNode	= *I;
				float			fDistance;
				m_tpMapPath.vfFindOptimalPath(
					m_tppHeap,
					m_tpHeap,
					m_tpIndexes,
					m_dwAStarStaticCounter,
					m_tData,
					m_tpSpawnNodes[i],
					*I,
					fDistance,
					fCurrentBestDistance,
					m_tpSpawnPoints[i]->o_Position,
					tpaGameGraph[I - BB].tLocalPoint,
					m_tpaNodes,
					m_dwMaxNodeCount);
			}
			m_tpResultNodes[i] = dwBest;
		}
	};
	void						Save(CFS_Memory &FS, u32 &dwOffset)
	{
		NET_Packet		P;
		u32				position;
		for (u32 i=0; i<m_tpSpawnPoints.size(); i++) {
			xrServerEntity*	E	= m_tpSpawnPoints[i];

			FS.open_chunk		(i);

			// Spawn
			E->Spawn_Write		(P,TRUE);
			FS.Wword			(u16(P.B.count));
			FS.write			(P.B.data,P.B.count);

			// Update
			P.w_begin			(M_UPDATE);
			P.w_u16				(E->ID);
			P.w_chunk_open8		(position);
			E->UPDATE_Write		(P);
			P.w_chunk_close8	(position);

			FS.Wword			(u16(P.B.count));
			FS.write			(P.B.data,P.B.count);

			FS.close_chunk		();
		}
	};
};

void vfLoadGameGraph()
{
	string256						fName = "gamedata\\game.graph";
	tpGraphVFS						= xr_new<CVirtualFileStream>(fName);
	tGraphHeader.dwVersion			= tpGraphVFS->Rdword();
	tGraphHeader.dwVertexCount		= tpGraphVFS->Rdword();
	tGraphHeader.dwLevelCount		= tpGraphVFS->Rdword();
	tGraphHeader.tpLevels.resize	(tGraphHeader.dwLevelCount);
	R_ASSERT						(tGraphHeader.dwVersion == XRAI_CURRENT_VERSION);
	{
		vector<SLevel>::iterator	I = tGraphHeader.tpLevels.begin();
		vector<SLevel>::iterator	E = tGraphHeader.tpLevels.end();
		for ( ; I != E; I++) {
			tpGraphVFS->RstringZ	((*I).caLevelName);
			tpGraphVFS->Rvector		((*I).tOffset);
		}
	}
	tpaGameGraph					= (SCompressedGraphVertex *)tpGraphVFS->Pointer();
}

void xrMergeSpawns()
{
	// load all the graphs
	Phase						("Reading level graphs");
	CInifile ENGINE_API *pIni	= xr_new<CInifile>(INI_FILE);
	vfLoadGameGraph				();
	SSpawnHeader				tSpawnHeader;
	tSpawnHeader.dwVersion		= XRAI_CURRENT_VERSION;
	tSpawnHeader.dwLevelCount	= 0;
	tSpawnHeader.dwSpawnCount	= 0;
	vector<CSpawn *>			tpLevels;
	string256					S1;
	SLevel						tLevel;
	float						fDummy;
	u32							dwLevelID;
	for (dwLevelID = 0; pIni->LineExists("game_levels",itoa(dwLevelID,S1,10)); dwLevelID++) {
		sscanf(pIni->ReadSTRING("game_levels",itoa(dwLevelID,S1,10)),"%f,%f,%f,%s",&fDummy,&fDummy,&fDummy,S1);
		Memory.mem_copy(tLevel.caLevelName,S1,strlen(S1) + 1);
		tpLevels.push_back(xr_new<CSpawn>(tLevel,dwLevelID));
	}
	R_ASSERT(tpLevels.size());
	
	CThreadManager				tThreadManager;		// multithreading

	Phase						("Searching for corresponding graph vertices");
	for (u32 thID=0, N = tpLevels.size(); thID<N; thID++)
		tThreadManager.start(tpLevels[thID]);
	tThreadManager.wait();
	
	Phase						("Merging spawn files");
	for (u32 i=0, N = tpLevels.size(); i<N; i++)
		tSpawnHeader.dwSpawnCount += tpLevels[i]->m_tpSpawnPoints.size();
	
	CFS_Memory					tMemoryStream;
	tMemoryStream.write			(&tSpawnHeader,sizeof(tSpawnHeader));
	for (u32 i=0, dwOffset = 0, N = tpLevels.size(); i<N; i++)
		tpLevels[thID]->Save(tMemoryStream,dwOffset);
	tMemoryStream.SaveTo("game.spawn",0);

	Phase						("Freeing resources being allocated");
	for (u32 i=0, N = tpLevels.size(); i<N; i++)
		xr_delete(tpLevels[thID]);
}