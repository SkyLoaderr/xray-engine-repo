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
#include "xrServer_Objects_ALife_All.h"
#include "game_base.h"
#include "xr_spawn_merge.h"
#include "xrCrossTable.h"

DEFINE_VECTOR(CALifeObject *,	ALIFE_OBJECT_P_VECTOR,	ALIFE_OBJECT_P_IT);

CALifeGraph						*tpGraph = 0;

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
	CALifeGraph::SLevel			m_tLevel;
	ALIFE_OBJECT_P_VECTOR		m_tpSpawnPoints;
	u32							m_dwLevelID;
	CAI_Map						*m_tpAI_Map;
	CALifeCrossTable			*m_tpCrossTable;

								CSpawn(LPCSTR name, const CALifeGraph::SLevel &tLevel, u32 dwLevelID, u32 *dwGroupOffset) : CThread(dwLevelID)
	{
		thDestroyOnComplete		= FALSE;
		// loading AI map
		m_tLevel				= tLevel;
		m_dwLevelID				= dwLevelID;
		string256				fName;
		strconcat				(fName,name,m_tLevel.caLevelName);
		strconcat				(fName,fName,"\\");
		m_tpAI_Map				= xr_new<CAI_Map>(fName);
		strcat					(fName,CROSS_TABLE_NAME);
		m_tpCrossTable			= xr_new<CALifeCrossTable>(fName);
		// loading spawn points
		fName[0]				= 0;
		strconcat				(fName,name,m_tLevel.caLevelName);
		strconcat				(fName,fName,"\\level.spawn");
		IReader					*SP = FS.r_open(fName);
		IReader					*S = 0;
		NET_Packet				P;
		int						S_id	= 0;
		xr_map<u32,xr_vector<CALifeObject*> >	tpSGMap;
		while (0!=(S = SP->open_chunk(S_id)))
		{
			P.B.count			= S->length();
			S->r				(P.B.data,P.B.count);
			S->close			();
			u16					ID;
			P.r_begin			(ID);
			R_ASSERT2			(M_SPAWN==ID,"ID doesn't match to the spawn-point ID!");
			string64			s_name;
			P.r_string			(s_name);
			// create server entity
			CAbstractServerObject*	E	= F_entity_Create	(s_name);
			if (!E) {
				string4096		S;
				sprintf			(S,"Can't create entity '%s' !\n",E->s_name_replace);
				R_ASSERT2		(E,S);
			}
			E->Spawn_Read		(P);
			//
			if ((E->s_gameid == GAME_SINGLE) || (E->s_gameid == GAME_ANY)) {
				CALifeObject	*tpALifeObject = dynamic_cast<CALifeObject*>(E);
				if (tpALifeObject) {
					m_tpSpawnPoints.push_back(tpALifeObject);
					xr_map<u32,xr_vector<CALifeObject*> >::iterator I = tpSGMap.find(tpALifeObject->m_dwSpawnGroup);
					if (I == tpSGMap.end()) {
						xr_vector<CALifeObject*> tpTemp;
						tpTemp.clear();
						tpTemp.push_back(tpALifeObject);
						tpSGMap.insert(mk_pair(tpALifeObject->m_dwSpawnGroup,tpTemp));
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
		R_ASSERT2(m_tpSpawnPoints.size(),"There are no spawn-points!");
		ALIFE_OBJECT_P_IT		I = m_tpSpawnPoints.begin();
		ALIFE_OBJECT_P_IT		E = m_tpSpawnPoints.end();
		for ( ; I != E; I++) {
			xr_map<u32,xr_vector<CALifeObject*> >::iterator J = tpSGMap.find((*I)->m_dwSpawnGroup);
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
	};
	virtual 					~CSpawn()
	{
		for (int i=0; i<(int)m_tpSpawnPoints.size(); i++)
			xr_delete(m_tpSpawnPoints[i]);
		xr_delete(m_tpAI_Map);
		xr_delete(m_tpCrossTable);
	};

	void						Execute()
	{
		thProgress				= 0.0f;
		u32						dwStart = tpGraph->m_tGraphHeader.dwVertexCount, dwFinish = tpGraph->m_tGraphHeader.dwVertexCount, dwCount = 0;
		for (int i=0; i<(int)tpGraph->m_tGraphHeader.dwVertexCount; i++)
			if (tpGraph->m_tpaGraph[i].tLevelID == m_dwLevelID)
				dwCount++;
		float fRelation = float(dwCount)/(float(dwCount) + 2*m_tpSpawnPoints.size());
		for (int i=0; i<(int)tpGraph->m_tGraphHeader.dwVertexCount; i++)
			if (tpGraph->m_tpaGraph[i].tLevelID == m_dwLevelID) {
				if (dwStart > (u32)i)
					dwStart = (u32)i;
				thProgress = float(i - dwStart + 1)/float(dwCount)*float(fRelation);
			}
			else {
				if ((dwStart <= (u32)i) && (dwFinish > (u32)i)) {
					dwFinish = i;
					break;
				}
			}
		if (dwStart >= dwFinish) {
			string4096			S;
			sprintf				(S,"There are no graph vertices in the game graph for the level '%s' !\n",m_tLevel.caLevelName);
			R_ASSERT2			(dwStart < dwFinish,S);
		}
		for (int i=0; i<(int)m_tpSpawnPoints.size(); i++, thProgress = fRelation + float(i)/float(m_tpSpawnPoints.size())*(1.f - fRelation)) {
			if ((m_tpSpawnPoints[i]->m_tNodeID = m_tpAI_Map->dwfFindCorrespondingNode(m_tpSpawnPoints[i]->o_Position)) == -1) {
				string4096 S1;
				char *S = S1;
				S += sprintf(S,"SPAWN POINT %s IS REMOVED! (Reason : can't find a corresponding NODE)",m_tpSpawnPoints[i]->s_name_replace);
//				S += sprintf(S,"Level ID    : %d\n",m_dwLevelID);
//				S += sprintf(S,"Spawn index : %d\n",i);
//				S += sprintf(S,"Spawn point : [%7.2f][%7.2f][%7.2f]\n",VPUSH(m_tpSpawnPoints[i]->o_Position));
//				S += sprintf(S,"\n");
				Msg(S1);
				m_tpSpawnPoints.erase(m_tpSpawnPoints.begin() + i--);
				continue;
				//R_ASSERT2(false,S1);
			}
			u32 dwBest = m_tpCrossTable->m_tpaCrossTable[m_tpSpawnPoints[i]->m_tNodeID].tGraphIndex;
			float fCurrentBestDistance = m_tpCrossTable->m_tpaCrossTable[m_tpSpawnPoints[i]->m_tNodeID].fDistance;
			if (dwBest == u32(-1)) {
				string4096 S1;
				char *S = S1;
				S += sprintf(S,"Can't find a corresponding GRAPH VERTEX for the spawn-point %s\n",m_tpSpawnPoints[i]->s_name_replace);
				S += sprintf(S,"Level ID    : %d\n",m_dwLevelID);
				S += sprintf(S,"Spawn index : %d\n",i);
				S += sprintf(S,"Spawn node  : %d\n",m_tpSpawnPoints[i]->m_tNodeID);
				S += sprintf(S,"Spawn point : [%7.2f][%7.2f][%7.2f]\n",m_tpSpawnPoints[i]->o_Position.x,m_tpSpawnPoints[i]->o_Position.y,m_tpSpawnPoints[i]->o_Position.z);
				R_ASSERT2(dwBest != -1,S1);
			}
			m_tpSpawnPoints[i]->m_tGraphID	= dwBest;
			m_tpSpawnPoints[i]->m_fDistance	= fCurrentBestDistance;
			thProgress						= 1.0f;
		}
	};

	void						Save(CMemoryWriter &fs, u32 &dwID)
	{
		NET_Packet		P;
		for (u32 i=0 ; i<m_tpSpawnPoints.size(); i++, dwID++) {
			CAbstractServerObject*	E	= m_tpSpawnPoints[i];

			fs.open_chunk		(dwID);

			// Spawn
			E->Spawn_Write		(P,TRUE);
			fs.w_u16			(u16(P.B.count));
			fs.w				(P.B.data,P.B.count);

			// Update
			P.w_begin			(M_UPDATE);
			E->UPDATE_Write		(P);
			fs.w_u16			(u16(P.B.count));
			fs.w				(P.B.data,P.B.count);

			fs.close_chunk		();
		}
	};
};

void xrMergeSpawns(LPCSTR name)
{
	// load all the graphs
	Phase						("Loading game graph");
	char						S[256];
	FS.update_path				(S,"$game_data$","game.graph");
	tpGraph						= xr_new<CALifeGraph>(S);
	
	Phase						("Reading level graphs");
	CInifile 					*Ini = xr_new<CInifile>(INI_FILE);
	SSpawnHeader				tSpawnHeader;
	tSpawnHeader.dwVersion		= XRAI_CURRENT_VERSION;
	tSpawnHeader.dwLevelCount	= 0;
	tSpawnHeader.dwSpawnCount	= 0;
	u32							dwGroupOffset = 0;
	xr_vector<CSpawn *>			tpLevels;
	CALifeGraph::SLevel			tLevel;
    LPCSTR						N,V;
	R_ASSERT2					(Ini->section_exist("levels"),"Can't find section 'levels' in the 'game.ltx'!");
    for (u32 k = 0; Ini->r_line("levels",k,&N,&V); k++) {
		if (!Ini->section_exist(N)) {
			string4096			S;
			sprintf				(S,"There is no level section '%s' being included in the section levels in the 'game.ltx'\n",N);
			R_ASSERT2			(Ini->section_exist(N),S);
		}
		V						= Ini->r_string(N,"name");
		Memory.mem_copy			(tLevel.caLevelName,V,strlen(V) + 1);
		Msg						("Reading level %s...",tLevel.caLevelName);
		u32						id = Ini->r_s32(N,"id");
		tpLevels.push_back		(xr_new<CSpawn>(name,tLevel,id,&dwGroupOffset));
    }
	R_ASSERT2					(tpLevels.size(),"There are no levels in the section 'levels' in the 'game.ltx' to build 'game.spawn' from!");
	
	CThreadManager				tThreadManager;		// multithreading

	Phase						("Searching for corresponding graph vertices");
	for (u32 i=0, N = tpLevels.size(); i<N; i++)
		tThreadManager.start	(tpLevels[i]);
	tThreadManager.wait();
	
	Phase						("Merging spawn files");
	for (u32 i=0, N = tpLevels.size(); i<N; i++)
		tSpawnHeader.dwSpawnCount += tpLevels[i]->m_tpSpawnPoints.size();
	
	CMemoryWriter					tMemoryStream;
	tMemoryStream.open_chunk	(SPAWN_POINT_CHUNK_VERSION);
	tMemoryStream.w				(&tSpawnHeader,sizeof(tSpawnHeader));
	tMemoryStream.close_chunk	();
	for (u32 i=0, dwID = 0, N = tpLevels.size(); i<N; i++)
		tpLevels[i]->Save		(tMemoryStream,dwID);
	tMemoryStream.save_to		("game.spawn");

	Phase						("Freeing resources being allocated");
	xr_delete					(tpGraph);
	for (u32 i=0, N = tpLevels.size(); i<N; i++)
		xr_delete				(tpLevels[i]);
}