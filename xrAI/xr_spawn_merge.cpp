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
#include "game_base.h"
#include "xr_spawn_merge.h"

typedef struct tagSAlife {
	u16			tGraphID;
	float		fDistance;
} SALife;

DEFINE_VECTOR(u32,				DWORD_VECTOR,			DWORD_IT);
DEFINE_VECTOR(SALife,			ALIFE_VECTOR,			ALIFE_IT);
DEFINE_VECTOR(xrServerEntity *, SERVER_ENTITY_P_VECTOR, SERVER_ENTITY_P_IT);

CVirtualFileStream				*tpGraphVFS = 0;
SCompressedGraphVertex			*tpaGameGraph;

class CSpawnComparePredicate {
private:
	u32							m_dwStartNode;
	CAI_Map						*m_tpAI_Map;
public:
	CSpawnComparePredicate(u32 dwStartNode, CAI_Map &tAI_Map)
	{
		m_dwStartNode	= dwStartNode;
		m_tpAI_Map		= &tAI_Map;
	}

	IC bool operator()(u32 dwNode1, u32 dwNode2) const 
	{
		return(m_tpAI_Map->ffGetDistanceBetweenNodeCenters(m_dwStartNode,dwNode1) < m_tpAI_Map->ffGetDistanceBetweenNodeCenters(m_dwStartNode,dwNode2));
	};
};

class CSpawn : public CThread {
public:
	SLevel						m_tLevel;
	SERVER_ENTITY_P_VECTOR		m_tpSpawnPoints;
	u32							m_dwLevelID;
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
	ALIFE_VECTOR				m_tpResultNodes;
	CAI_Map						*m_tpAI_Map;

								CSpawn(SLevel &tLevel, u32 dwLevelID) : CThread(dwLevelID)
	{
		thDestroyOnComplete		= FALSE;
		// loading AI map
		m_tLevel				= tLevel;
		m_dwLevelID				= dwLevelID;
		string256				fName;
		strconcat				(fName,"gamedata\\levels\\",m_tLevel.caLevelName);
		strconcat				(fName,fName,"\\");
		m_tpAI_Map				= xr_new<CAI_Map>(fName);
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
			if (E->s_gameid == GAME_SINGLE)
				m_tpSpawnPoints.push_back(E);
			else
				xr_delete(E);
			S_id++;
		}
		R_ASSERT(m_tpSpawnPoints.size());
		m_dwAStarStaticCounter	= 0;
		u32 S1					= (m_tpAI_Map->m_header.count + 2)*sizeof(SNode);
		m_tpHeap				= (SNode *)xr_malloc(S1);
		ZeroMemory				(m_tpHeap,S1);

		m_tpHeap				= xr_alloc<SNode> (m_tpAI_Map->m_header.count + 2);

		u32 S2					= (m_tpAI_Map->m_header.count)*sizeof(SIndexNode);
		m_tpIndexes				= (SIndexNode *)xr_malloc(S2);
		ZeroMemory				(m_tpIndexes,S2);
		u32 S3					= (m_tpAI_Map->m_header.count)*sizeof(SNode *);
		m_tppHeap				= (SNode **)xr_malloc(S1);
		ZeroMemory				(m_tpHeap,S1);
	};
	virtual 					~CSpawn()
	{
		for (int i=0; i<(int)m_tpSpawnPoints.size(); i++)
			xr_delete(m_tpSpawnPoints[i]);
		xr_free(m_tppHeap);
		xr_free(m_tpHeap);
		xr_free(m_tpIndexes);
		xr_delete(m_tpAI_Map);
	};

	void						Execute()
	{
		thProgress				= 0.0f;
		m_tpGraphNodes.resize	(tGraphHeader.dwVertexCount);
		m_tpSpawnNodes.resize	(m_tpSpawnPoints.size());
		m_tpResultNodes.resize	(m_tpSpawnPoints.size());
		u32						dwStart = tGraphHeader.dwVertexCount, dwFinish = tGraphHeader.dwVertexCount;
		for (int i=0; i<(int)tGraphHeader.dwVertexCount; i++)
			if (tpaGameGraph[i].tLevelID == m_dwLevelID) {
				R_ASSERT((m_tpGraphNodes[i] = m_tpAI_Map->dwfFindCorrespondingNode(tpaGameGraph[i].tLocalPoint))!=-1);
				if (dwStart > (u32)i)
					dwStart = (u32)i;
			}
			else {
				m_tpGraphNodes[i] = -1;
				if ((dwStart <= (u32)i) && (dwFinish > (u32)i)) {
					dwFinish = i;
					break;
				}
			}
		DWORD_IT				BB = m_tpGraphNodes.begin();
		DWORD_IT				B = BB + dwStart;
		DWORD_IT				E = m_tpGraphNodes.begin() + dwFinish;
		for (int i=0; i<(int)m_tpSpawnPoints.size(); i++) {
			R_ASSERT((m_tpSpawnNodes[i] = m_tpAI_Map->dwfFindCorrespondingNode(m_tpSpawnPoints[i]->o_Position)) != -1);
			sort(B,E,CSpawnComparePredicate(m_tpSpawnNodes[i],*m_tpAI_Map));
			DWORD_IT			I = B;
			float				fCurrentBestDistance = MAX_DISTANCE_TO_CONNECT;
			u32					dwBest = u32(-1);
			for ( ; I != E; I++) {
				if (m_tpAI_Map->ffGetDistanceBetweenNodeCenters(m_tpSpawnNodes[i],*I) >= fCurrentBestDistance)
					break;
				m_tData.dwFinishNode	= *I;
				m_tData.m_tpAI_Map		= m_tpAI_Map;
				float					fDistance;
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
				if (fDistance < fCurrentBestDistance) {
					fCurrentBestDistance = fDistance;
					dwBest = I - B;
				}
			}
			m_tpResultNodes[i].tGraphID		= dwBest;
			m_tpResultNodes[i].fDistance	= fCurrentBestDistance;
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
			FS.open_chunk		(++i);
			FS.write			(m_tpResultNodes.begin() + i,sizeof(SALife));
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
		Msg("Reading level %d...",dwLevelID);
		sscanf(pIni->ReadSTRING("game_levels",itoa(dwLevelID,S1,10)),"%f,%f,%f,%s",&fDummy,&fDummy,&fDummy,S1);
		Memory.mem_copy(tLevel.caLevelName,S1,strlen(S1) + 1);
		tpLevels.push_back(xr_new<CSpawn>(tLevel,dwLevelID));
	}
	R_ASSERT(tpLevels.size());
	
	CThreadManager				tThreadManager;		// multithreading

	Phase						("Searching for corresponding graph vertices");
	for (u32 i=0, N = tpLevels.size(); i<N; i++)
		tThreadManager.start(tpLevels[i]);
	tThreadManager.wait();
	
	Phase						("Merging spawn files");
	for (u32 i=0, N = tpLevels.size(); i<N; i++)
		tSpawnHeader.dwSpawnCount += tpLevels[i]->m_tpSpawnPoints.size();
	
	CFS_Memory					tMemoryStream;
	tMemoryStream.write			(&tSpawnHeader,sizeof(tSpawnHeader));
	for (u32 i=0, dwOffset = 0, N = tpLevels.size(); i<N; i++)
		tpLevels[i]->Save(tMemoryStream,dwOffset);
	tMemoryStream.SaveTo("game.spawn",0);

	Phase						("Freeing resources being allocated");
	for (u32 i=0, N = tpLevels.size(); i<N; i++)
		xr_delete(tpLevels[i]);
}