////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.cpp
//	Created 	: 25.12.2002
//  Modified 	: 29.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"

//#define WRITE_TO_LOG
#define MONSTER_TO_GENERATE	40

CAI_ALife::CAI_ALife()
{
	m_bLoaded			= false;
}

CAI_ALife::~CAI_ALife()
{
	shedule_Unregister	();
	//Save				();
}

void CAI_ALife::vfInitTerrain()
{
	m_tpTerrain.resize(256);
	for (u16 i=0; i<Level().AI.GraphHeader().dwVertexCount; i++)
		m_tpTerrain[Level().AI.m_tpaGraph[i].ucVertexType].push_back(i);
}

void CAI_ALife::vfInitLocationOwners()
{
	m_tpLocationOwners.resize(Level().AI.GraphHeader().dwVertexCount);
	OBJECT_PAIR_IT   it = m_tObjectRegistry.m_tppMap.begin();
	OBJECT_PAIR_IT   E  = m_tObjectRegistry.m_tppMap.end();
	for ( ; it != E; it++) {
		_SPAWN_ID	tSpawnID	= (*it).second->m_tSpawnID;
		_OBJECT_ID	tObjectID	= (*it).second->m_tObjectID;
		for (int j=0, iCount = (int)m_tpSpawnPoints[tSpawnID].ucRoutePointCount; j<iCount; j++)
			m_tpLocationOwners[m_tpSpawnPoints[tSpawnID].wpRouteGraphPoints[j]].push_back(tObjectID);
	}
}

void CAI_ALife::vfInitGraph()
{
	m_tpGraphObjects.resize(Level().AI.GraphHeader().dwVertexCount);
	OBJECT_PAIR_IT	it = m_tObjectRegistry.m_tppMap.begin();
	OBJECT_PAIR_IT	E  = m_tObjectRegistry.m_tppMap.end();
	for ( ; it != E; it++)
		m_tpGraphObjects[(*it).second->m_tGraphID].push_back((*it).second->m_tObjectID);
}

void CAI_ALife::vfInitScheduledObjects()
{
	m_tpScheduledObjects.clear();
	OBJECT_PAIR_IT	it = m_tObjectRegistry.m_tppMap.begin();
	OBJECT_PAIR_IT	E  = m_tObjectRegistry.m_tppMap.end();
	for ( ; it != E; it++) {
		CALifeMonster	*tpALifeMonster = dynamic_cast<CALifeMonster *>((*it).second);
		if (tpALifeMonster)
			m_tpScheduledObjects.push_back(tpALifeMonster);
	}
}

// temporary
IC bool	bfSpawnPointPredicate(SSpawnPoint v1, SSpawnPoint v2)
{
	return(v1.wGroupID <= v2.wGroupID);
}

void CAI_ALife::vfGenerateSpawnPoints(u32 dwSpawnCount)
{
#define MODEL_COUNT				2
	vector<bool>				tpMarks;
	tpMarks.resize				(Level().AI.GraphHeader().dwVertexCount);
	tpMarks.assign				(tpMarks.size(),false);
	AI::SGraphVertex			*tpaGraph = Level().AI.m_tpaGraph;
	const char					*caModels[MODEL_COUNT] = {"m_rat_e","m_zombie_e"};
	m_tpSpawnPoints.resize		(dwSpawnCount);
	u16 wGroupID				= 0;
	m_tSpawnHeader.dwCount		= dwSpawnCount;
	m_tSpawnHeader.dwVersion	= SPAWN_POINT_VERSION;
	::Random.seed(0);
	for (int i=0; i<(int)dwSpawnCount; i++) {
		m_tpSpawnPoints[i].tNearestGraphPointID		= (u16)::Random.randI(Level().AI.GraphHeader().dwVertexCount);
		bool bOk = false;
		for (int j=0; j<i; j++)
			if (m_tpSpawnPoints[j].tNearestGraphPointID == m_tpSpawnPoints[i].tNearestGraphPointID) {
				bOk = true;
				m_tpSpawnPoints[i].wGroupID = m_tpSpawnPoints[j].wGroupID;
				break;
			}
		if (!bOk)
			m_tpSpawnPoints[i].wGroupID = wGroupID++;
		j = ::Random.randI(0,MODEL_COUNT);
		memcpy(m_tpSpawnPoints[i].caModel,caModels[j],(1 + strlen(caModels[j]))*sizeof(char));
		m_tpSpawnPoints[i].ucTeam					= (u8)::Random.randI(255);
		m_tpSpawnPoints[i].ucSquad					= (u8)::Random.randI(255);
		m_tpSpawnPoints[i].ucGroup					= (u8)::Random.randI(255);
		m_tpSpawnPoints[i].wCount					= (u16)::Random.randI(5,45);
		m_tpSpawnPoints[i].fBirthRadius				= 10.f;
		m_tpSpawnPoints[i].fBirthProbability			= 1.0f;
		m_tpSpawnPoints[i].fIncreaseCoefficient		= 1.01f;
		m_tpSpawnPoints[i].fAnomalyDeathProbability	= 0.0f;
		m_tpSpawnPoints[i].wpRouteGraphPoints.push_back(m_tpSpawnPoints[i].tNearestGraphPointID);
		u16				wPoint = m_tpSpawnPoints[i].tNearestGraphPointID;
		int				wCount = tpaGraph[wPoint].dwNeighbourCount;
		AI::SGraphEdge	*tpaEdges = (AI::SGraphEdge *)((BYTE *)tpaGraph + tpaGraph[wPoint].dwEdgeOffset);
		for ( j=0; j<(int)wCount; j++) {
			if (!tpMarks[tpaEdges[j].dwVertexNumber]) {
				m_tpSpawnPoints[i].wpRouteGraphPoints.push_back((u16)tpaEdges[j].dwVertexNumber);
				tpMarks[tpaEdges[j].dwVertexNumber] = true;
			}
			u32				wPoint1 = tpaEdges[j].dwVertexNumber;
			int				wCount1 = tpaGraph[wPoint1].dwNeighbourCount;
			AI::SGraphEdge	*tpaEdges1 = (AI::SGraphEdge *)((BYTE *)tpaGraph + tpaGraph[wPoint1].dwEdgeOffset);
			for (int k=0; k<wCount1; k++)
				if (!tpMarks[tpaEdges1[k].dwVertexNumber]) {
					m_tpSpawnPoints[i].wpRouteGraphPoints.push_back((u16)tpaEdges1[k].dwVertexNumber);
					tpMarks[tpaEdges1[k].dwVertexNumber] = true;
				}
		}
		m_tpSpawnPoints[i].ucRoutePointCount			= (u8)m_tpSpawnPoints[i].wpRouteGraphPoints.size();
		for ( j=0; j<(int)m_tpSpawnPoints[i].ucRoutePointCount; j++)
			tpMarks[m_tpSpawnPoints[i].wpRouteGraphPoints[j]] = false;
	}
	sort(m_tpSpawnPoints.begin(),m_tpSpawnPoints.end(),bfSpawnPointPredicate);
}

void CAI_ALife::vfSaveSpawnPoints()
{
	CFS_Memory	tStream;
	tStream.open_chunk	(SPAWN_POINT_CHUNK_VERSION);
	tStream.write		(&m_tSpawnHeader,sizeof(m_tSpawnHeader));
	tStream.close_chunk	();
	tStream.open_chunk	(SPAWN_POINT_CHUNK_DATA);
	for (int i=0; i<(int)m_tSpawnHeader.dwCount; i++) {
		tStream.Wword	(m_tpSpawnPoints[i].tNearestGraphPointID);
		tStream.Wstring	(m_tpSpawnPoints[i].caModel);
		tStream.Wbyte	(m_tpSpawnPoints[i].ucTeam);
		tStream.Wbyte	(m_tpSpawnPoints[i].ucSquad);
		tStream.Wbyte	(m_tpSpawnPoints[i].ucGroup);
		tStream.Wword	(m_tpSpawnPoints[i].wGroupID);
		tStream.Wword	(m_tpSpawnPoints[i].wCount);
		tStream.Wfloat	(m_tpSpawnPoints[i].fBirthRadius);
		tStream.Wfloat	(m_tpSpawnPoints[i].fBirthProbability);
		tStream.Wfloat	(m_tpSpawnPoints[i].fIncreaseCoefficient);
		tStream.Wfloat	(m_tpSpawnPoints[i].fAnomalyDeathProbability);
		tStream.Wbyte	(m_tpSpawnPoints[i].ucRoutePointCount);
		for (int j=0; j<(int)m_tpSpawnPoints[i].ucRoutePointCount; j++)
			tStream.Wword(m_tpSpawnPoints[i].wpRouteGraphPoints[j]);
	}
	tStream.close_chunk	();
	tStream.SaveTo		("game.spawn",0);
}

void CAI_ALife::Load()
{
	shedule_Min					=     1;
	shedule_Max					=    20;
	m_dwObjectsBeingProcessed		=     0;
	m_qwMaxProcessTime			=  100*CPU::cycles_per_microsec;
	
	m_tALifeHeader.dwVersion	=	ALIFE_VERSION;
	m_tALifeHeader.tTimeID		=	0;

	// checking if graph is loaded
	if (!Level().AI.m_tpaGraph)
		return;

	// loading spawn-points
	CStream		*tpStream;
	FILE_NAME	caFileName;
	if (!Engine.FS.Exist(caFileName, Path.GameData, "game.spawn")) {
//		THROW;
#ifdef DEBUG
		vfGenerateSpawnPoints(MONSTER_TO_GENERATE);
		vfSaveSpawnPoints();
#else
		return;
#endif
	}
	else {
		tpStream = Engine.FS.Open(caFileName);
		R_ASSERT(tpStream->FindChunk(SPAWN_POINT_CHUNK_VERSION));
		tpStream->Read(&m_tSpawnHeader,sizeof(m_tSpawnHeader));
		if (m_tSpawnHeader.dwVersion != SPAWN_POINT_VERSION)
			THROW;
		R_ASSERT(tpStream->FindChunk(SPAWN_POINT_CHUNK_DATA));
		m_tpSpawnPoints.resize(m_tSpawnHeader.dwCount);
		for (int i=0; i<(int)m_tSpawnHeader.dwCount; i++) {
			m_tpSpawnPoints[i].tNearestGraphPointID		= tpStream->Rword();
			tpStream->Rstring							(m_tpSpawnPoints[i].caModel);
			m_tpSpawnPoints[i].ucTeam					= tpStream->Rbyte();
			m_tpSpawnPoints[i].ucSquad					= tpStream->Rbyte();
			m_tpSpawnPoints[i].ucGroup					= tpStream->Rbyte();
			m_tpSpawnPoints[i].wGroupID					= tpStream->Rword();
			m_tpSpawnPoints[i].wCount					= tpStream->Rword();
			m_tpSpawnPoints[i].fBirthRadius				= tpStream->Rfloat();
			m_tpSpawnPoints[i].fBirthProbability		= tpStream->Rfloat();
			m_tpSpawnPoints[i].fIncreaseCoefficient		= tpStream->Rfloat();
			m_tpSpawnPoints[i].fAnomalyDeathProbability	= tpStream->Rfloat();
			m_tpSpawnPoints[i].ucRoutePointCount		= tpStream->Rbyte();
			m_tpSpawnPoints[i].wpRouteGraphPoints.resize(m_tpSpawnPoints[i].ucRoutePointCount);
			for (int j=0; j<(int)m_tpSpawnPoints[i].ucRoutePointCount; j++)
				m_tpSpawnPoints[i].wpRouteGraphPoints[j] = tpStream->Rword();
		}
		Engine.FS.Close(tpStream);
	}

	if (!Engine.FS.Exist(caFileName,Path.GameData,"game.alife")) {
		Generate();
		Save();
	}
	
	if (!Engine.FS.Exist(caFileName,Path.GameData,"game.alife"))
		THROW;
	
	tpStream = Engine.FS.Open(caFileName);
	
	R_ASSERT(tpStream->FindChunk(ALIFE_CHUNK_VERSION));
	tpStream->Read(&m_tALifeHeader,sizeof(m_tALifeHeader));
	if (m_tALifeHeader.dwVersion != ALIFE_VERSION)
		THROW;
	
	R_ASSERT(tpStream->FindChunk(OBJECT_CHUNK_DATA));
	m_tObjectRegistry.Load(*tpStream);
	
	R_ASSERT(tpStream->FindChunk(EVENT_CHUNK_DATA));
	m_tEventRegistry.Load(*tpStream);
	
	R_ASSERT(tpStream->FindChunk(TASK_CHUNK_DATA));
	m_tTaskRegistry.Load(*tpStream);
	
	Engine.FS.Close(tpStream);

	vfInitTerrain();
	vfInitLocationOwners();
	vfInitGraph();
	vfInitScheduledObjects();

	m_bLoaded = true;
}

void CAI_ALife::Save()
{
	CFS_Memory	tStream;
	// version chunk
	tStream.open_chunk	(ALIFE_CHUNK_VERSION);
	tStream.write		(&m_tALifeHeader,sizeof(m_tALifeHeader));
	tStream.close_chunk	();
	
	tStream.open_chunk	(OBJECT_CHUNK_DATA);
	m_tObjectRegistry.Save(tStream);
	tStream.close_chunk	();
	
	tStream.open_chunk	(EVENT_CHUNK_DATA);
	m_tEventRegistry.Save(tStream);
	tStream.close_chunk	();
	
	tStream.open_chunk	(TASK_CHUNK_DATA);
	m_tTaskRegistry.Save(tStream);
	tStream.close_chunk	();
	
	tStream.SaveTo		("game.alife",0);
}

void CAI_ALife::Generate()
{
	SPAWN_IT	B = m_tpSpawnPoints.begin();
	SPAWN_IT	E  = m_tpSpawnPoints.end();
	for (SPAWN_IT it = B ; it != E; ) {
		u16	wGroupID = it->wGroupID;
		float fSum = it->fBirthProbability;
		for (SPAWN_IT j= it + 1; (j != E) && (j->wGroupID == wGroupID); j++)
			fSum += j->fBirthProbability;
		float fProbability = ::Random.randF(0,fSum);
		fSum = it->fBirthProbability;
		SPAWN_IT m = j, k = it;
		for ( j= it + 1; (j != E) && (j->wGroupID == wGroupID); j++) {
			fSum += j->fBirthProbability;
			if (fSum > fProbability) {
				k = j;
				break;
			}
		}
		CALifeItem	*tpALifeObject;
		if (pSettings->ReadBOOL(k->caModel, "Scheduled"))
			if (pSettings->ReadBOOL(k->caModel, "Human"))
				tpALifeObject	= new CALifeHuman;
			else
				tpALifeObject	= new CALifeMonster;
		else
			tpALifeObject		= new CALifeItem;

		tpALifeObject->Init(_SPAWN_ID(k - B),m_tpSpawnPoints);
		m_tObjectRegistry.Add(tpALifeObject);
		it = m;	
	}
	m_tALifeHeader.dwVersion	= ALIFE_VERSION;
	m_tALifeHeader.tTimeID		= 0;
}

void CAI_ALife::Update(u32 dt)
{
	inherited::Update(dt);
	if (!m_bLoaded)
		return;
#ifdef WRITE_TO_LOG
	Msg("* %7.2fs",Level().timeServer()/1000.f);
#endif
	u64	qwStartTime = CPU::GetCycleCount();
	if (m_tpScheduledObjects.size()) {
		int i=0;
		do {
			i++;
			m_dwObjectsBeingProcessed = ((m_dwObjectsBeingProcessed + 1) % m_tpScheduledObjects.size());
			vfProcessNPC(m_tpScheduledObjects[m_dwObjectsBeingProcessed]);
		}
		while (((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i < m_qwMaxProcessTime) && (i < (int)m_tpScheduledObjects.size()));
	}
#ifdef WRITE_TO_LOG
	u64 t2x = CPU::GetCycleCount() - qwStartTime;
	Msg("* %.3f microseconds",CPU::cycles2microsec*t2x);
#endif
}

void CAI_ALife::vfProcessNPC(CALifeMonster	*tpALifeMonster)
{
//	Msg						("* Monster %d",dwNPCIndex);
//	Msg						("* * Time       : %d",tpALifeMonster->m_dwLastUpdateTime);
//	Msg						("* * Spawn      : %d",tpALifeMonster->m_tSpawnID);
//	Msg						("* * Count      : %d",tpALifeMonster->m_wCount);
	vfChooseNextRoutePoint	(tpALifeMonster);
	vfCheckForTheBattle		(tpALifeMonster);
	CALifeHuman *tpALifeHuman = dynamic_cast<CALifeHuman *>(tpALifeMonster);
	if (tpALifeHuman) {
		vfCheckForDeletedEvents	(tpALifeHuman);
		vfCheckForItems			(tpALifeHuman);
	}
	tpALifeMonster->m_tTimeID = Level().timeServer();
//	Msg						("* * PrevPoint  : %d",tpALifeMonster->m_tPrevGraphID);
//	Msg						("* * GraphPoint : %d",tpALifeMonster->m_tGraphID);
//	Msg						("* * NextPoint  : %d",tpALifeMonster->m_tNextGraphID);
//	Msg						("* * Speed      : %5.2f",tpALifeMonster->m_fSpeed);
//	Msg						("* * Distance   : %5.2f",tpALifeMonster->m_fDistanceFromPoint);
//	Msg						("* * Path       : %5.2f",tpALifeMonster->m_fDistanceToPoint);
//	Msg						("* * Health     : %d",tpALifeMonster->m_iHealth);
}

void CAI_ALife::vfChooseNextRoutePoint(CALifeMonster	*tpALifeMonster)
{
	if (tpALifeMonster->m_tNextGraphID != tpALifeMonster->m_tGraphID) {
		u32 dwCurTime = Level().timeServer();
		tpALifeMonster->m_fDistanceFromPoint += float(dwCurTime - tpALifeMonster->m_tTimeID)/1000.f * tpALifeMonster->m_fCurSpeed;
		if (tpALifeMonster->m_fDistanceToPoint - tpALifeMonster->m_fDistanceFromPoint < EPS_L) {
			vfChangeGraphPoint(tpALifeMonster->m_tObjectID,tpALifeMonster->m_tGraphID,tpALifeMonster->m_tNextGraphID);
			tpALifeMonster->m_fDistanceToPoint	= tpALifeMonster->m_fDistanceFromPoint	= 0.0f;
			tpALifeMonster->m_tPrevGraphID		= tpALifeMonster->m_tGraphID;
			tpALifeMonster->m_tGraphID			= tpALifeMonster->m_tNextGraphID;
		}
	}
	if (tpALifeMonster->m_tNextGraphID == tpALifeMonster->m_tGraphID) {
		_GRAPH_ID			tGraphID		= tpALifeMonster->m_tGraphID;
		AI::SGraphVertex	*tpaGraph		= Level().AI.m_tpaGraph;
		u16					wNeighbourCount = (u16)tpaGraph[tGraphID].dwNeighbourCount;
		AI::SGraphEdge		*tpaEdges		= (AI::SGraphEdge *)((BYTE *)tpaGraph + tpaGraph[tGraphID].dwEdgeOffset);
		int					iPointCount		= (int)m_tpSpawnPoints[tpALifeMonster->m_tSpawnID].ucRoutePointCount;
		GRAPH_VECTOR		&wpaVertexes	= m_tpSpawnPoints[tpALifeMonster->m_tSpawnID].wpRouteGraphPoints;
		int					iBranches		= 0;
		bool bOk = false;
		for (int i=0; i<wNeighbourCount; i++)
			for (int j=0; j<iPointCount; j++)
				if ((tpaEdges[i].dwVertexNumber == wpaVertexes[j]) && (wpaVertexes[j] != tpALifeMonster->m_tPrevGraphID))
					iBranches++;
		if (!iBranches) {
			for (int i=0; i<wNeighbourCount; i++) {
				for (int j=0; j<iPointCount; j++)
					if (tpaEdges[i].dwVertexNumber == wpaVertexes[j]) {
						tpALifeMonster->m_tNextGraphID = wpaVertexes[j];
						tpALifeMonster->m_fDistanceToPoint = tpaEdges[i].fPathDistance;
						bOk = true;
						break;
					}
				if (bOk)
					break;
			}
		}
		else {
			int iChosenBranch = ::Random.randI(0,iBranches);
			iBranches = 0;
			for (int i=0; i<wNeighbourCount; i++) {
				for (int j=0; j<iPointCount; j++)
					if ((tpaEdges[i].dwVertexNumber == wpaVertexes[j]) && (wpaVertexes[j] != tpALifeMonster->m_tPrevGraphID)) {
						if (iBranches == iChosenBranch) {
							tpALifeMonster->m_tNextGraphID = wpaVertexes[j];
							tpALifeMonster->m_fDistanceToPoint = tpaEdges[i].fPathDistance;
							bOk = true;
							break;
						}
						iBranches++;
					}
				if (bOk)
					break;
			}
		}
		tpALifeMonster->m_fDistanceFromPoint	= 0.0f;
		if (!bOk) {
			tpALifeMonster->m_fCurSpeed			= 0.0f;
			tpALifeMonster->m_fDistanceToPoint	= 0.0f;
		}
		else {
			tpALifeMonster->m_fCurSpeed			= tpALifeMonster->m_fMinSpeed;
		}
	}
}

void CAI_ALife::vfCheckForTheBattle(CALifeMonster	*tpALifeMonster)
{
}

void CAI_ALife::vfCheckForDeletedEvents(CALifeHuman	*tpALifeHuman)
{
	PERSONAL_EVENT_IT it = remove_if(tpALifeHuman->m_tpEvents.begin(),tpALifeHuman->m_tpEvents.end(),CRemovePredicate(m_tEventRegistry.m_tpMap));
	tpALifeHuman->m_tpEvents.erase(it,tpALifeHuman->m_tpEvents.end());
}

void CAI_ALife::vfCheckForItems(CALifeHuman	*tpALifeHuman)
{
	OBJECT_IT it = m_tpGraphObjects[tpALifeHuman->m_tGraphID].begin();
	OBJECT_IT E  = m_tpGraphObjects[tpALifeHuman->m_tGraphID].end();
	for( ; it != E; it++) {
		OBJECT_PAIR_IT	i = m_tObjectRegistry.m_tppMap.find(*it);
		VERIFY(i != m_tObjectRegistry.m_tppMap.end());
		CALifeItem *tpALifeItem = (*i).second;
		VERIFY(tpALifeItem);
		CALifeCorp *tpALifeCorp = dynamic_cast<CALifeCorp *>(tpALifeItem);
		if (!tpALifeCorp) {
			CALifeMonster *tpALifeMonster = dynamic_cast<CALifeMonster *>(tpALifeItem);
			if (!tpALifeMonster) {
				if (tpALifeHuman->m_fItemMass + tpALifeItem->m_fMass < tpALifeHuman->m_fMaxItemMass) {
					tpALifeHuman->m_tpItemIDs.push_back(*it);
					m_tpGraphObjects[tpALifeHuman->m_tGraphID].erase(it);
				}
				else {

				}
			}
		}
	}
}