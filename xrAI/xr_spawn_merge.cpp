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

DEFINE_VECTOR(CALifeObject *,	ALIFE_OBJECT_P_VECTOR,	ALIFE_OBJECT_P_IT);

CVirtualFileStream				*tpGraphVFS = 0;
SCompressedGraphVertex			*tpaGameGraph;

class CSpawnComparePredicate {
private:
	u32							m_dwStartNode;
	const CAI_Map				*m_tpAI_Map;
public:
	CSpawnComparePredicate(u32 dwStartNode, const CAI_Map &tAI_Map)
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
	ALIFE_OBJECT_P_VECTOR		m_tpSpawnPoints;
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
	CAI_Map						*m_tpAI_Map;

								CSpawn(const SLevel &tLevel, u32 dwLevelID, u32 *dwGroupOffset) : CThread(dwLevelID)
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
		map<u32,vector<CALifeObject*> >	tpSGMap;
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
			if ((E->s_gameid == GAME_SINGLE) || (E->s_gameid == GAME_ANY)) {
				CALifeObject	*tpALifeObject = dynamic_cast<CALifeObject*>(E);
				if (tpALifeObject) {
					m_tpSpawnPoints.push_back(tpALifeObject);
					map<u32,vector<CALifeObject*> >::iterator I = tpSGMap.find(tpALifeObject->m_dwSpawnGroup);
					if (I == tpSGMap.end()) {
						vector<CALifeObject*> tpTemp;
						tpTemp.clear();
						tpTemp.push_back(tpALifeObject);
						tpSGMap.insert(make_pair(tpALifeObject->m_dwSpawnGroup,tpTemp));
					}
					else
						(*I).second.push_back(tpALifeObject);
				}
				else
					xr_delete(E);
			}
			else
				xr_delete(E);
			S_id++;
		}
		R_ASSERT(m_tpSpawnPoints.size());
		ALIFE_OBJECT_P_IT		I = m_tpSpawnPoints.begin();
		ALIFE_OBJECT_P_IT		E = m_tpSpawnPoints.end();
		for ( ; I != E; I++) {
			map<u32,vector<CALifeObject*> >::iterator J = tpSGMap.find((*I)->m_dwSpawnGroup);
			if (J != tpSGMap.end()) {
				if ((*I)->m_dwSpawnGroup > 0) {
					for (u32 i=0; i<(*J).second.size(); i++)
						(*J).second[i]->m_dwSpawnGroup = *dwGroupOffset;
					++*dwGroupOffset;
				}
				else
					for (u32 i=0; i<(*J).second.size(); i++)
						(*J).second[i]->m_dwSpawnGroup = (*dwGroupOffset)++;
				(*J).second.clear();
			}
		}

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
		u32						dwStart = tGraphHeader.dwVertexCount, dwFinish = tGraphHeader.dwVertexCount, dwCount = 0;
		for (int i=0; i<(int)tGraphHeader.dwVertexCount; i++)
			if (tpaGameGraph[i].tLevelID == m_dwLevelID)
				dwCount++;
		float fRelation = float(dwCount)/(float(dwCount) + 2*m_tpSpawnPoints.size());
		for (int i=0; i<(int)tGraphHeader.dwVertexCount; i++)
			if (tpaGameGraph[i].tLevelID == m_dwLevelID) {
				R_ASSERT((m_tpGraphNodes[i] = m_tpAI_Map->dwfFindCorrespondingNode(tpaGameGraph[i].tLocalPoint))!=-1);
				if (dwStart > (u32)i)
					dwStart = (u32)i;
				thProgress = float(i - dwStart + 1)/float(dwCount)*float(fRelation);
			}
			else {
				m_tpGraphNodes[i] = -1;
				if ((dwStart <= (u32)i) && (dwFinish > (u32)i)) {
					dwFinish = i;
					break;
				}
			}
//		thProgress				= .75f;
		DWORD_IT				BB = m_tpGraphNodes.begin();
		DWORD_IT				B = BB + dwStart;
		DWORD_IT				E = m_tpGraphNodes.begin() + dwFinish;
		R_ASSERT				(B != E);
		for (int i=0; i<(int)m_tpSpawnPoints.size(); i++, thProgress = fRelation + float(i)/float(m_tpSpawnPoints.size())*(1.f - fRelation)) {
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
				if (m_tpSpawnNodes[i] != *I) {
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
				else
					fDistance = m_tpSpawnPoints[i]->o_Position.distance_to(tpaGameGraph[I - BB].tLocalPoint);
				if (fDistance < fCurrentBestDistance) {
					fCurrentBestDistance = fDistance;
					dwBest = I - BB;
				}
			}
			R_ASSERT(dwBest != -1);
			m_tpSpawnPoints[i]->m_tGraphID	= dwBest;
			m_tpSpawnPoints[i]->m_fDistance	= fCurrentBestDistance;
			thProgress						= 1.0f;
		}
	};

	void						Save(CFS_Memory &FS, u32 &dwID)
	{
		NET_Packet		P;
		for (u32 i=0 ; i<m_tpSpawnPoints.size(); i++, dwID++) {
			xrServerEntity*	E	= m_tpSpawnPoints[i];

			FS.open_chunk		(dwID);

			// Spawn
			E->Spawn_Write		(P,TRUE);
			FS.Wword			(u16(P.B.count));
			FS.write			(P.B.data,P.B.count);

			// Update
			P.w_begin			(M_UPDATE);
			E->UPDATE_Write		(P);
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
	Phase						("Loading game graph");
	vfLoadGameGraph				();
	
	Phase						("Reading level graphs");
	CInifile 					*Ini = xr_new<CInifile>(INI_FILE);
	SSpawnHeader				tSpawnHeader;
	tSpawnHeader.dwVersion		= XRAI_CURRENT_VERSION;
	tSpawnHeader.dwLevelCount	= 0;
	tSpawnHeader.dwSpawnCount	= 0;
	u32							dwGroupOffset = 0;
	vector<CSpawn *>			tpLevels;
	SLevel						tLevel;
    LPCSTR						N,V;
	R_ASSERT					(Ini->SectionExists("levels"));
    for (u32 k = 0; Ini->ReadLINE("levels",k,&N,&V); k++) {
		R_ASSERT				(Ini->SectionExists(N));
		V						= Ini->ReadSTRING(N,"name");
		Memory.mem_copy			(tLevel.caLevelName,V,strlen(V) + 1);
		Msg						("Reading level %s...",tLevel.caLevelName);
		u32						id = Ini->ReadINT(N,"id");
		tpLevels.push_back		(xr_new<CSpawn>(tLevel,id,&dwGroupOffset));
    }
	R_ASSERT					(tpLevels.size());
	
	CThreadManager				tThreadManager;		// multithreading

	Phase						("Searching for corresponding graph vertices");
	for (u32 i=0, N = tpLevels.size(); i<N; i++)
		tThreadManager.start	(tpLevels[i]);
	tThreadManager.wait();
	
	Phase						("Merging spawn files");
	for (u32 i=0, N = tpLevels.size(); i<N; i++)
		tSpawnHeader.dwSpawnCount += tpLevels[i]->m_tpSpawnPoints.size();
	
	CFS_Memory					tMemoryStream;
	tMemoryStream.open_chunk	(SPAWN_POINT_CHUNK_VERSION);
	tMemoryStream.write			(&tSpawnHeader,sizeof(tSpawnHeader));
	tMemoryStream.close_chunk	();
	//tMemoryStream.open_chunk	(SPAWN_POINT_CHUNK_DATA);
	for (u32 i=0, dwID = 0, N = tpLevels.size(); i<N; i++)
		tpLevels[i]->Save		(tMemoryStream,dwID);
	//tMemoryStream.close_chunk	();
	tMemoryStream.SaveTo		("game.spawn",0);

	Phase						("Freeing resources being allocated");
	for (u32 i=0, N = tpLevels.size(); i<N; i++)
		xr_delete				(tpLevels[i]);
}