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
#include "net_utils.h"
#include "ai_alife_templates.h"

DEFINE_VECTOR(CSE_ALifeObject *,	ALIFE_OBJECT_P_VECTOR,	ALIFE_OBJECT_P_IT);

CSE_ALifeGraph						*tpGraph = 0;

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
	CSE_ALifeGraph::SLevel		m_tLevel;
	ALIFE_OBJECT_P_VECTOR		m_tpSpawnPoints;
	xr_vector<CSE_LevelPoint*>	m_tpLevelSpawnPoints;
	u32							m_dwLevelID;
	CAI_Map						*m_tpAI_Map;
	CSE_ALifeCrossTable			*m_tpCrossTable;
	xr_vector<SLevelPoint>		m_tpLevelPoints;

								CSpawn(LPCSTR name, const CSE_ALifeGraph::SLevel &tLevel, u32 dwLevelID, u32 *dwGroupOffset) : CThread(dwLevelID)
	{
		thDestroyOnComplete		= FALSE;
		// loading AI map
		m_tLevel				= tLevel;
		m_dwLevelID				= dwLevelID;
		string256				fName;
		FS.update_path			(fName,name,m_tLevel.caLevelName);
		strcat					(fName,"\\");
		m_tpAI_Map				= xr_new<CAI_Map>(fName);
		// loading cross table
		strcat					(fName,CROSS_TABLE_NAME);
		m_tpCrossTable			= xr_new<CSE_ALifeCrossTable>(fName);
		// loading spawn points
		FS.update_path			(fName,name,m_tLevel.caLevelName);
		strcat					(fName,"\\level.spawn");
		IReader					*SP = FS.r_open(fName);
		IReader					*S = 0;
		NET_Packet				P;
		int						S_id	= 0;
		xr_map<LPCSTR,xr_vector<CSE_ALifeObject*>*,pred_str>	l_tpSpawnGroupObjectsMap;
		xr_map<LPCSTR,CSE_SpawnGroup*,pred_str>					l_tpSpawnGroupControlsMap;
		while ( 0 != (S = SP->open_chunk(S_id))) {
			P.B.count			= S->length();
			S->r				(P.B.data,P.B.count);
			S->close			();
			u16					ID;
			P.r_begin			(ID);
			R_ASSERT2			(M_SPAWN==ID,"ID doesn't match to the spawn-point ID!");
			string64			s_name;
			P.r_string			(s_name);
			// create server entity
			CSE_Abstract		*E = F_entity_Create			(s_name);
			if (!E) {
				string4096		S;
				sprintf			(S,"Can't create entity '%s' !\n",E->s_name_replace);
				R_ASSERT2		(E,S);
			}
			E->Spawn_Read		(P);
			
			//
			if ((E->s_gameid == GAME_SINGLE) || (E->s_gameid == GAME_ANY)) {
				CSE_ALifeObject	*tpALifeObject = dynamic_cast<CSE_ALifeObject*>(E);
				if (tpALifeObject) {
					m_tpSpawnPoints.push_back(tpALifeObject);
					if (!strlen(tpALifeObject->m_caGroupControl))
						tpALifeObject->m_dwSpawnGroup = ++*dwGroupOffset;
					else {
						xr_map<LPCSTR,xr_vector<CSE_ALifeObject*>*,pred_str>::iterator I = l_tpSpawnGroupObjectsMap.find(tpALifeObject->m_caGroupControl);
						if (I == l_tpSpawnGroupObjectsMap.end()) {
							xr_vector<CSE_ALifeObject*> *tpTemp = xr_new<xr_vector<CSE_ALifeObject*> >();
							tpTemp->clear();
							tpTemp->push_back(tpALifeObject);
							l_tpSpawnGroupObjectsMap.insert(mk_pair(tpALifeObject->m_caGroupControl,tpTemp));
						}
						else
							(*I).second->push_back(tpALifeObject);
					}
				}
				else {
					CSE_SpawnGroup *l_tpSpawnGroup = dynamic_cast<CSE_SpawnGroup*>(E);
					if (l_tpSpawnGroup) {
						l_tpSpawnGroup->m_dwSpawnGroup = ++*dwGroupOffset;
						l_tpSpawnGroupControlsMap.insert(mk_pair(l_tpSpawnGroup->s_name_replace,l_tpSpawnGroup));
					}
					else {
						CSE_LevelPoint *l_tpLevelPoint = dynamic_cast<CSE_LevelPoint*>(E);
						if (l_tpLevelPoint)
							m_tpLevelSpawnPoints.push_back(l_tpLevelPoint);
						else
							xr_delete(E);
					}
				}
			}
			else
				xr_delete(E);
			S_id++;
		}
		
		R_ASSERT2(m_tpSpawnPoints.size(),"There are no spawn-points!");
		
		{
			xr_map<LPCSTR,xr_vector<CSE_ALifeObject*>*,pred_str>::iterator	I = l_tpSpawnGroupObjectsMap.begin();
			xr_map<LPCSTR,xr_vector<CSE_ALifeObject*>*,pred_str>::iterator	E = l_tpSpawnGroupObjectsMap.end();
			
			for ( ; I != E; I++) {
				R_ASSERT(strlen((*I).first));
				
				xr_map<LPCSTR,CSE_SpawnGroup*,pred_str>::iterator			J = l_tpSpawnGroupControlsMap.find((*I).first);

				R_ASSERT3(J != l_tpSpawnGroupControlsMap.end(),"Specified group control not found! (%s)",(*(*I).second)[0]->s_name_replace);
				R_ASSERT((*I).second);
				
				ALIFE_OBJECT_P_IT	i = (*I).second->begin();
				ALIFE_OBJECT_P_IT	e = (*I).second->end();
				float				fSum = 0.f;
				for ( ; i != e; i++)
					fSum += (*i)->m_fProbability;

				fSum /= (*J).second->m_fGroupProbability;

				i = (*I).second->begin();
				for ( ; i != e; i++) {
					(*i)->m_fProbability /= fSum;
					(*i)->m_dwSpawnGroup = (*J).second->m_dwSpawnGroup;
					CSE_ALifeAnomalousZone *l_tpAnomalousZone =dynamic_cast<CSE_ALifeAnomalousZone*>(*i);
					if (l_tpAnomalousZone) {
						float l_fSum = 0.f;
						for (int ii=0; ii<l_tpAnomalousZone->m_wItemCount; ii++)
							l_fSum += l_tpAnomalousZone->m_faWeights[ii];
						l_fSum /= l_tpAnomalousZone->m_fBirthProbability;
						for (int ii=0; ii<l_tpAnomalousZone->m_wItemCount; ii++)
							l_tpAnomalousZone->m_faWeights[ii] /= l_fSum;
					}
				}
			}
		}
		// freeing resources being allocated
		{
			xr_map<LPCSTR,xr_vector<CSE_ALifeObject*>*,pred_str>::iterator	I = l_tpSpawnGroupObjectsMap.begin();
			xr_map<LPCSTR,xr_vector<CSE_ALifeObject*>*,pred_str>::iterator	E = l_tpSpawnGroupObjectsMap.end();
			
			for ( ; I != E; I++) {
				(*I).second->clear();
				xr_free((*I).second);
			}
		}
		delete_data					(l_tpSpawnGroupControlsMap);
	};
	virtual 					~CSpawn()
	{
		delete_data				(m_tpLevelSpawnPoints);
		//delete_data				(m_tpSpawnPoints);
		delete_data				(m_tpAI_Map);
		delete_data				(m_tpCrossTable);
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
		for (int i=0; i<(int)m_tpSpawnPoints.size(); i++, thProgress = .5f*(fRelation + float(i)/float(m_tpSpawnPoints.size())*(1.f - fRelation))) {
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
		}
		thProgress				= .5f;
		vfGenerateArtefactSpawnPositions();
		thProgress				= 1.0f;
	};

	void						vfGenerateArtefactSpawnPositions()
	{
		m_tpLevelPoints.clear	();
		xr_vector<u32>			l_tpaStack;
		l_tpaStack.reserve		(1024);
		m_tpAI_Map->q_mark_bit.assign(m_tpAI_Map->q_mark_bit.size(),false);
		ALIFE_OBJECT_P_IT		B = m_tpSpawnPoints.begin(), I = B;
		ALIFE_OBJECT_P_IT		E = m_tpSpawnPoints.end();
		for ( ; I != E; I++) {
			CSE_ALifeAnomalousZone *l_tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(*I);
			if (!l_tpALifeAnomalousZone)
				continue;

			m_tpAI_Map->vfShallowGraphSearch(l_tpALifeAnomalousZone->o_Position,l_tpALifeAnomalousZone->m_tNodeID,l_tpALifeAnomalousZone->m_fRadius,l_tpaStack,m_tpAI_Map->q_mark_bit);

			if (l_tpALifeAnomalousZone->m_wArtefactSpawnCount >= l_tpaStack.size()) {
				l_tpALifeAnomalousZone->m_wArtefactSpawnCount	= l_tpaStack.size();
				l_tpALifeAnomalousZone->m_dwStartIndex			= m_tpLevelPoints.size();
				m_tpLevelPoints.resize							(l_tpALifeAnomalousZone->m_dwStartIndex + l_tpALifeAnomalousZone->m_wArtefactSpawnCount);
				xr_vector<SLevelPoint>::iterator				I = m_tpLevelPoints.begin() + l_tpALifeAnomalousZone->m_dwStartIndex;
				xr_vector<SLevelPoint>::iterator				E = m_tpLevelPoints.end();
				xr_vector<u32>::iterator						i = l_tpaStack.begin();
				for ( ; I != E; I++, i++) {
					(*I).tNodeID	= *i;
					(*I).tPoint		= m_tpAI_Map->tfGetNodeCenter(*i);
					(*I).fDistance	= m_tpCrossTable->m_tpaCrossTable[*i].fDistance;
				}
			}
			else {
				//random_shuffle									(l_tpaStack.begin(),l_tpaStack.end());
				l_tpALifeAnomalousZone->m_dwStartIndex			= m_tpLevelPoints.size();
				m_tpLevelPoints.resize							(l_tpALifeAnomalousZone->m_dwStartIndex + l_tpALifeAnomalousZone->m_wArtefactSpawnCount);
				xr_vector<SLevelPoint>::iterator				I = m_tpLevelPoints.begin() + l_tpALifeAnomalousZone->m_dwStartIndex;
				xr_vector<SLevelPoint>::iterator				E = m_tpLevelPoints.end();
				xr_vector<u32>::iterator						i = l_tpaStack.begin();
				for ( ; I != E; I++, i++) {
					(*I).tNodeID	= *i;
					(*I).tPoint		= m_tpAI_Map->tfGetNodeCenter(*i);
					(*I).fDistance	= m_tpCrossTable->m_tpaCrossTable[*i].fDistance;
				}
			}
		}
	}

	void						Save(CMemoryWriter &fs, u32 &dwID, xr_vector<SLevelPoint> &tpLevelPoints)
	{
		NET_Packet		P;
		for (u32 i=0 ; i<m_tpSpawnPoints.size(); i++, dwID++) {
			CSE_Abstract		*E = m_tpSpawnPoints[i];
			CSE_ALifeObject		*l_tpALifeObject = dynamic_cast<CSE_ALifeObject*>(E);
			R_ASSERT3			(l_tpALifeObject->m_tNodeID && (l_tpALifeObject->m_tNodeID < m_tpAI_Map->m_header.count),"Invalid node for object ",l_tpALifeObject->s_name_replace);
			R_ASSERT2			(l_tpALifeObject,"Non-ALife object!");
			strcpy				(l_tpALifeObject->m_caGroupControl,"");
			CSE_ALifeAnomalousZone *l_tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(E);
			if (l_tpALifeAnomalousZone) {
				u32									l_dwStartIndex = tpLevelPoints.size();
				tpLevelPoints.resize				(l_dwStartIndex + l_tpALifeAnomalousZone->m_wArtefactSpawnCount);
				xr_vector<SLevelPoint>::iterator	I = m_tpLevelPoints.begin() + l_tpALifeAnomalousZone->m_dwStartIndex;
				xr_vector<SLevelPoint>::iterator	E = I + l_tpALifeAnomalousZone->m_wArtefactSpawnCount;
				xr_vector<SLevelPoint>::iterator	i = tpLevelPoints.begin() + l_dwStartIndex;
				for ( ; I != E; I++,i++)
					*i = *I;
				l_tpALifeAnomalousZone->m_dwStartIndex = l_dwStartIndex;
			}

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

LPCSTR cafGetActorLevelName(xr_vector<CSpawn *> &tpLevels, string256 &S)
{
	CSE_ALifeCreatureActor	*l_tpActor = 0;
	for (u32 i=0; i<tpLevels.size(); i++) {
		ALIFE_OBJECT_P_IT	I = tpLevels[i]->m_tpSpawnPoints.begin();
		ALIFE_OBJECT_P_IT	E = tpLevels[i]->m_tpSpawnPoints.end();
		for ( ; I != E; I++)
			if (!!(l_tpActor = dynamic_cast<CSE_ALifeCreatureActor*>(*I)))
				break;
		if (l_tpActor)
			return			(strconcat(S,tpLevels[i]->m_tLevel.caLevelName,".spawn"));
	}
	R_ASSERT2				(false,"There is no actor!");
	return					("game.spawn");
}

void xrMergeSpawns(LPCSTR name)
{
	xr_vector<SLevelPoint>		l_tpLevelPoints;
	l_tpLevelPoints.clear		();

	// load all the graphs
	Phase						("Loading game graph");
	char						S[256];
	FS.update_path				(S,"$game_data$","game.graph");
	tpGraph						= xr_new<CSE_ALifeGraph>(S);
	
	Phase						("Reading level graphs");
	CInifile 					*Ini = xr_new<CInifile>(INI_FILE);
	SSpawnHeader				tSpawnHeader;
	tSpawnHeader.dwVersion		= XRAI_CURRENT_VERSION;
	tSpawnHeader.dwLevelCount	= 0;
	tSpawnHeader.dwSpawnCount	= 0;
	u32							dwGroupOffset = 0;
	xr_vector<CSpawn *>			tpLevels;
	CSE_ALifeGraph::SLevel		tLevel;
    LPCSTR						N,V;
	R_ASSERT2					(Ini->section_exist("levels"),"Can't find section 'levels' in the 'game.ltx'!");
    for (u32 k = 0; Ini->r_line("levels",k,&N,&V); k++) {
		if (!Ini->section_exist(N)) {
			string4096			S;
			sprintf				(S,"There is no level section '%s' being included in the section levels in the 'game.ltx'\n",N);
			R_ASSERT2			(Ini->section_exist(N),S);
		}
		V						= Ini->r_string(N,"name");
		if (strlen(name) && stricmp(name,V))
			continue;
		Memory.mem_copy			(tLevel.caLevelName,V,(u32)strlen(V) + 1);
		Msg						("Reading level %s...",tLevel.caLevelName);
		u32						id = Ini->r_s32(N,"id");
		tpLevels.push_back		(xr_new<CSpawn>("$game_levels$",tLevel,id,&dwGroupOffset));
    }
	R_ASSERT2					(tpLevels.size(),"There are no levels in the section 'levels' in the 'game.ltx' to build 'game.spawn' from!");
	
	CThreadManager				tThreadManager;		// multithreading

	Phase						("Searching for corresponding graph vertices");
	for (u32 i=0, N = tpLevels.size(); i<N; i++)
		tThreadManager.start	(tpLevels[i]);
	tThreadManager.wait			();
	
	Phase						("Merging spawn files");
	for (u32 i=0, N = tpLevels.size(); i<N; i++)
		tSpawnHeader.dwSpawnCount += tpLevels[i]->m_tpSpawnPoints.size();
	
	CMemoryWriter					tMemoryStream;
	tMemoryStream.open_chunk	(SPAWN_POINT_CHUNK_VERSION);
	tMemoryStream.w				(&tSpawnHeader,sizeof(tSpawnHeader));
	tMemoryStream.close_chunk	();
	for (u32 i=0, dwID = 0, N = tpLevels.size(); i<N; i++)
		tpLevels[i]->Save		(tMemoryStream,dwID,l_tpLevelPoints);
	
	tMemoryStream.open_chunk	(dwID++);
	save_data					(l_tpLevelPoints,tMemoryStream);
	tMemoryStream.close_chunk	();

	string256					l_caFileName;
	FS.update_path				(l_caFileName,"$game_spawn$",cafGetActorLevelName(tpLevels,S));
	tMemoryStream.save_to		(l_caFileName);

	Phase						("Freeing resources being allocated");
	xr_delete					(tpGraph);
	for (u32 i=0, N = tpLevels.size(); i<N; i++)
		xr_delete				(tpLevels[i]);
}