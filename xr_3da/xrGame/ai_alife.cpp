////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.cpp
//	Created 	: 25.12.2002
//  Modified 	: 29.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"

#define WRITE_TO_LOG

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
	m_tpLocationOwner.resize(Level().AI.GraphHeader().dwVertexCount);
	for (u16 i=0; i<m_tNPCHeader.dwCount; i++)
		for (int j=0; j<(int)m_tpSpawnPoint[m_tpNPC[i].wSpawnPoint].ucRoutePointCount; j++)
			m_tpGraphObject[m_tpSpawnPoint[m_tpNPC[i].wSpawnPoint].wpRouteGraphPoints[j]].push_back(i);
}

void CAI_ALife::vfInitGraph()
{
	m_tpGraphObject.resize(Level().AI.GraphHeader().dwVertexCount);
	for (u16 i=0; i<m_tNPCHeader.dwCount; i++)
		m_tpGraphObject[m_tpNPC[i].wGraphPoint].push_back(i);
}

// temporary
IC bool	bfSpawnPointPredicate(CAI_ALife::SSpawnPoint v1, CAI_ALife::SSpawnPoint v2)
{
	return(v1.wGroupID <= v2.wGroupID);
}

void CAI_ALife::vfGenerateSpawnPoints(u32 dwSpawnCount)
{
#define MODEL_COUNT				1
	vector<bool>				tpMarks;
	tpMarks.resize				(Level().AI.GraphHeader().dwVertexCount);
	tpMarks.assign				(tpMarks.size(),false);
	AI::SGraphVertex			*tpaGraph = Level().AI.m_tpaGraph;
	const char					*caModels[] = {"m_rat_e"};
	m_tpSpawnPoint.resize		(dwSpawnCount);
	u16 wGroupID				= 0;
	m_tSpawnHeader.dwCount		= dwSpawnCount;
	m_tSpawnHeader.dwVersion	= NPC_SPAWN_POINT_VERSION;
	::Random.seed(0);
	for (int i=0; i<(int)dwSpawnCount; i++) {
		m_tpSpawnPoint[i].wNearestGraphPoint		= (u16)::Random.randI(Level().AI.GraphHeader().dwVertexCount);
		bool bOk = false;
		for (int j=0; j<i; j++)
			if (m_tpSpawnPoint[j].wNearestGraphPoint == m_tpSpawnPoint[i].wNearestGraphPoint) {
				bOk = true;
				m_tpSpawnPoint[i].wGroupID = m_tpSpawnPoint[j].wGroupID;
				break;
			}
		if (!bOk)
			m_tpSpawnPoint[i].wGroupID = wGroupID++;

		j = ::Random.randI(0,MODEL_COUNT);
		memcpy(m_tpSpawnPoint[i].caModel,caModels[j],(1 + strlen(caModels[j]))*sizeof(char));
		m_tpSpawnPoint[i].ucTeam					= (u8)::Random.randI(255);
		m_tpSpawnPoint[i].ucSquad					= (u8)::Random.randI(255);
		m_tpSpawnPoint[i].ucGroup					= (u8)::Random.randI(255);
		m_tpSpawnPoint[i].wCount					= (u16)::Random.randI(5,45);
		m_tpSpawnPoint[i].fBirthRadius				= 10.f;
		m_tpSpawnPoint[i].fBirthProbability			= 1.0f;
		m_tpSpawnPoint[i].fIncreaseCoefficient		= 1.01f;
		m_tpSpawnPoint[i].fAnomalyDeathProbability	= 0.0f;
		m_tpSpawnPoint[i].wpRouteGraphPoints.push_back(m_tpSpawnPoint[i].wNearestGraphPoint);
		u16				wPoint = m_tpSpawnPoint[i].wNearestGraphPoint;
		int				wCount = tpaGraph[wPoint].dwNeighbourCount;
		AI::SGraphEdge	*tpaEdges = (AI::SGraphEdge *)((BYTE *)tpaGraph + tpaGraph[wPoint].dwEdgeOffset);
		for (int j=0; j<(int)wCount; j++) {
			if (!tpMarks[tpaEdges[j].dwVertexNumber]) {
				m_tpSpawnPoint[i].wpRouteGraphPoints.push_back((u16)tpaEdges[j].dwVertexNumber);
				tpMarks[tpaEdges[j].dwVertexNumber] = true;
			}
			u32				wPoint1 = tpaEdges[j].dwVertexNumber;
			int				wCount1 = tpaGraph[wPoint1].dwNeighbourCount;
			AI::SGraphEdge	*tpaEdges1 = (AI::SGraphEdge *)((BYTE *)tpaGraph + tpaGraph[wPoint1].dwEdgeOffset);
			for (int k=0; k<wCount1; k++)
				if (!tpMarks[tpaEdges1[k].dwVertexNumber]) {
					m_tpSpawnPoint[i].wpRouteGraphPoints.push_back((u16)tpaEdges1[k].dwVertexNumber);
					tpMarks[tpaEdges1[k].dwVertexNumber] = true;
				}
		}
		m_tpSpawnPoint[i].ucRoutePointCount			= (u8)m_tpSpawnPoint[i].wpRouteGraphPoints.size();
		for ( j=0; j<(int)m_tpSpawnPoint[i].ucRoutePointCount; j++)
			tpMarks[m_tpSpawnPoint[i].wpRouteGraphPoints[j]] = false;
	}
	sort(m_tpSpawnPoint.begin(),m_tpSpawnPoint.end(),bfSpawnPointPredicate);
}

void CAI_ALife::vfSaveSpawnPoints()
{
	CFS_Memory	tStream;
	tStream.open_chunk	(NPC_SPAWN_POINT_CHUNK_VERSION);
	tStream.write		(&m_tSpawnHeader,sizeof(m_tSpawnHeader));
	tStream.close_chunk	();
	tStream.open_chunk	(NPC_SPAWN_POINT_CHUNK_DATA);
	for (int i=0; i<(int)m_tSpawnHeader.dwCount; i++) {
		tStream.Wword	(m_tpSpawnPoint[i].wNearestGraphPoint);
		tStream.Wstring	(m_tpSpawnPoint[i].caModel);
		tStream.Wbyte	(m_tpSpawnPoint[i].ucTeam);
		tStream.Wbyte	(m_tpSpawnPoint[i].ucSquad);
		tStream.Wbyte	(m_tpSpawnPoint[i].ucGroup);
		tStream.Wword	(m_tpSpawnPoint[i].wGroupID);
		tStream.Wword	(m_tpSpawnPoint[i].wCount);
		tStream.Wfloat	(m_tpSpawnPoint[i].fBirthRadius);
		tStream.Wfloat	(m_tpSpawnPoint[i].fBirthProbability);
		tStream.Wfloat	(m_tpSpawnPoint[i].fIncreaseCoefficient);
		tStream.Wfloat	(m_tpSpawnPoint[i].fAnomalyDeathProbability);
		tStream.Wbyte	(m_tpSpawnPoint[i].ucRoutePointCount);
		for (int j=0; j<(int)m_tpSpawnPoint[i].ucRoutePointCount; j++)
			tStream.Wword(m_tpSpawnPoint[i].wpRouteGraphPoints[j]);
	}
	tStream.close_chunk	();
	tStream.SaveTo		("game.spawn",0);
}

void CAI_ALife::Load()
{
	shedule_Min					=   100;
	shedule_Max					= 10000;
	m_dwNPCBeingProcessed		=     0;
	m_qwMaxProcessTime			= 100000*CPU::cycles_per_microsec;
	
	m_tNPCHeader.dwVersion		= ALIFE_VERSION;
	m_tNPCHeader.dwCount		= 0;

	// checking if graph is loaded
	if (!Level().AI.m_tpaGraph)
		return;

	// loading spawn-points
	CStream		*tpStream;
	FILE_NAME	caFileName;
	if (!Engine.FS.Exist(caFileName, Path.GameData, "game.spawn")) {
//		THROW;
#ifdef DEBUG
		vfGenerateSpawnPoints(10);
		vfSaveSpawnPoints();
#else
		return;
#endif
	}
	else {
		tpStream = Engine.FS.Open(caFileName);
		R_ASSERT(tpStream->FindChunk(NPC_SPAWN_POINT_CHUNK_VERSION));
		tpStream->Read(&m_tSpawnHeader,sizeof(m_tSpawnHeader));
		if (m_tSpawnHeader.dwVersion != NPC_SPAWN_POINT_VERSION)
			THROW;
		R_ASSERT(tpStream->FindChunk(NPC_SPAWN_POINT_CHUNK_DATA));
		m_tpSpawnPoint.resize(m_tSpawnHeader.dwCount);
		for (int i=0; i<(int)m_tSpawnHeader.dwCount; i++) {
			m_tpSpawnPoint[i].wNearestGraphPoint		= tpStream->Rword();
			tpStream->Rstring							(m_tpSpawnPoint[i].caModel);
			m_tpSpawnPoint[i].ucTeam					= tpStream->Rbyte();
			m_tpSpawnPoint[i].ucSquad					= tpStream->Rbyte();
			m_tpSpawnPoint[i].ucGroup					= tpStream->Rbyte();
			m_tpSpawnPoint[i].wGroupID					= tpStream->Rword();
			m_tpSpawnPoint[i].wCount					= tpStream->Rword();
			m_tpSpawnPoint[i].fBirthRadius				= tpStream->Rfloat();
			m_tpSpawnPoint[i].fBirthProbability			= tpStream->Rfloat();
			m_tpSpawnPoint[i].fIncreaseCoefficient		= tpStream->Rfloat();
			m_tpSpawnPoint[i].fAnomalyDeathProbability	= tpStream->Rfloat();
			m_tpSpawnPoint[i].ucRoutePointCount			= tpStream->Rbyte();
			m_tpSpawnPoint[i].wpRouteGraphPoints.resize	(m_tpSpawnPoint[i].ucRoutePointCount);
			for (int j=0; j<(int)m_tpSpawnPoint[i].ucRoutePointCount; j++)
				m_tpSpawnPoint[i].wpRouteGraphPoints[j] = tpStream->Rword();
		}
		Engine.FS.Close(tpStream);
	}

	// loading NPCs
	if (!Engine.FS.Exist(caFileName,Path.GameData,"game.alife")) {
		Generate();
		Save();
	}
	
	if (!Engine.FS.Exist(caFileName,Path.GameData,"game.alife"))
		THROW;
	tpStream = Engine.FS.Open(caFileName);
	R_ASSERT(tpStream->FindChunk(ALIFE_CHUNK_VERSION));
	tpStream->Read(&m_tNPCHeader,sizeof(m_tNPCHeader));
	if (m_tNPCHeader.dwVersion != ALIFE_VERSION)
		THROW;
	R_ASSERT(tpStream->FindChunk(ALIFE_CHUNK_DATA));
	m_tpNPC.resize(m_tNPCHeader.dwCount);
	for (int i=0; i<(int)m_tNPCHeader.dwCount; i++) {
		
		m_tpNPC[i].dwLastUpdateTime		= tpStream->Rdword();
		m_tpNPC[i].wSpawnPoint			= tpStream->Rword();
		m_tpNPC[i].wCount				= tpStream->Rword();
		m_tpNPC[i].wGraphPoint			= tpStream->Rword();
		m_tpNPC[i].wNextGraphPoint		= tpStream->Rword();
		m_tpNPC[i].wPrevGraphPoint		= tpStream->Rword();
		m_tpNPC[i].fSpeed				= tpStream->Rfloat();
		m_tpNPC[i].fDistanceFromPoint	= tpStream->Rfloat();
		m_tpNPC[i].iHealth				= tpStream->Rint();
		m_tpNPC[i].tpUsefulObject.resize(tpStream->Rword());
		for (int j=0; j<(int)m_tpNPC[i].tpUsefulObject.size(); j++)
			tpStream->Read(&(m_tpNPC[i].tpUsefulObject[j]),sizeof(SUsefulObject));

	}
	Engine.FS.Close(tpStream);

	vfInitGraph();
	vfInitTerrain();
	vfInitLocationOwners();
	m_bLoaded					= true;
}

void CAI_ALife::Save()
{
	CFS_Memory	tStream;
	// version chunk
	tStream.open_chunk	(ALIFE_CHUNK_VERSION);
	tStream.Wdword		(m_tNPCHeader.dwVersion);
	tStream.Wdword		(m_tNPCHeader.dwCount);
	tStream.close_chunk	();
	
	// data chunk
	tStream.open_chunk	(ALIFE_CHUNK_DATA);
	for (int i=0; i<(int)m_tNPCHeader.dwCount; i++) {
		
		tStream.Wdword	(m_tpNPC[i].dwLastUpdateTime);
		tStream.Wword	(m_tpNPC[i].wSpawnPoint);
		tStream.Wword	(m_tpNPC[i].wCount);
		tStream.Wword	(m_tpNPC[i].wGraphPoint);
		tStream.Wword	(m_tpNPC[i].wNextGraphPoint);
		tStream.Wword	(m_tpNPC[i].wPrevGraphPoint);
		tStream.Wfloat	(m_tpNPC[i].fSpeed);
		tStream.Wfloat	(m_tpNPC[i].fDistanceFromPoint);
		tStream.Wdword	(m_tpNPC[i].iHealth);
		tStream.Wword	((u16)m_tpNPC[i].tpUsefulObject.size());
		for (int j=0; j<(int)m_tpNPC[i].tpUsefulObject.size(); j++)
            tStream.write(&(m_tpNPC[i].tpUsefulObject[j]),sizeof(SUsefulObject));

	}
	tStream.close_chunk	();
	tStream.SaveTo		("game.alife",0);
}

void CAI_ALife::Generate()
{
	for (int i=0; i<(int)m_tSpawnHeader.dwCount; ) {
		u16	wGroupID = m_tpSpawnPoint[i].wGroupID;
		float fSum = m_tpSpawnPoint[i].fBirthProbability;
		for (int j= i + 1; (j<(int)m_tSpawnHeader.dwCount) && (m_tpSpawnPoint[j].wGroupID == wGroupID); j++)
			fSum += m_tpSpawnPoint[j].fBirthProbability;
		float fProbability = ::Random.randF(0,fSum);
		fSum = m_tpSpawnPoint[i].fBirthProbability;
		int m = j, k = i;
		for ( j= i + 1; (j<(int)m_tSpawnHeader.dwCount) && (m_tpSpawnPoint[j].wGroupID == wGroupID); j++) {
			fSum += m_tpSpawnPoint[j].fBirthProbability;
			if (fSum > fProbability) {
				k = j;
				break;
			}
		}
		SALifeNPC tALifeNPC;
		
		tALifeNPC.dwLastUpdateTime		= Level().timeServer();
		tALifeNPC.wSpawnPoint			= (u16)k;
		tALifeNPC.wCount				= m_tpSpawnPoint[k].wCount;
		tALifeNPC.wGraphPoint			= m_tpSpawnPoint[k].wNearestGraphPoint;
		tALifeNPC.wNextGraphPoint		= tALifeNPC.wGraphPoint;
		tALifeNPC.wPrevGraphPoint		= tALifeNPC.wGraphPoint;
		tALifeNPC.fSpeed				= 0.f;
		tALifeNPC.fDistanceFromPoint	= 0.f;
		tALifeNPC.iHealth				= pSettings->ReadINT(m_tpSpawnPoint[i].caModel, "Health");
		tALifeNPC.tpUsefulObject.clear	();

		m_tpNPC.push_back				(tALifeNPC);
		i = m;	
	}
	m_tNPCHeader.dwVersion	= ALIFE_VERSION;
	m_tNPCHeader.dwCount	= m_tpNPC.size();
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
	if (m_tNPCHeader.dwCount) {
		int i=0;
		do {
			i++;
			m_dwNPCBeingProcessed = ((m_dwNPCBeingProcessed + 1) % m_tNPCHeader.dwCount);
			vfProcessNPC(m_dwNPCBeingProcessed);
		}
		while (((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i < m_qwMaxProcessTime) && (i < (int)m_tNPCHeader.dwCount));
	}
#ifdef WRITE_TO_LOG
	u64 t2x = CPU::GetCycleCount() - qwStartTime;
	Msg("* %.3f microseconds",CPU::cycles2microsec*t2x);
#endif
}

void CAI_ALife::vfProcessNPC(u32 dwNPCIndex)
{
	Msg						("* Monster %d",dwNPCIndex);
//	Msg						("* * Time       : %d",m_tpNPC[dwNPCIndex].dwLastUpdateTime);
//	Msg						("* * Spawn      : %d",m_tpNPC[dwNPCIndex].wSpawnPoint);
//	Msg						("* * Count      : %d",m_tpNPC[dwNPCIndex].wCount);
	vfCheckForTheBattle		(dwNPCIndex);
	vfChooseNextRoutePoint	(dwNPCIndex);
	m_tpNPC[dwNPCIndex].dwLastUpdateTime = Level().timeServer();
	Msg						("* * PrevPoint  : %d",m_tpNPC[dwNPCIndex].wPrevGraphPoint);
	Msg						("* * GraphPoint : %d",m_tpNPC[dwNPCIndex].wGraphPoint);
	Msg						("* * NextPoint  : %d",m_tpNPC[dwNPCIndex].wNextGraphPoint);
	Msg						("* * Speed      : %5.2f",m_tpNPC[dwNPCIndex].fSpeed);
	Msg						("* * Distance   : %5.2f",m_tpNPC[dwNPCIndex].fDistanceFromPoint);
//	Msg						("* * Health     : %d",m_tpNPC[dwNPCIndex].iHealth);
}

void CAI_ALife::vfCheckForTheBattle(u32 dwNPCIndex)
{
}

void CAI_ALife::vfChooseNextRoutePoint(u32 dwNPCIndex)
{
	if (m_tpNPC[dwNPCIndex].wNextGraphPoint == m_tpNPC[dwNPCIndex].wGraphPoint) {
		u16					wGraphPoint		= m_tpNPC[dwNPCIndex].wGraphPoint;
		AI::SGraphVertex	*tpaGraph		= Level().AI.m_tpaGraph;
		u16					wNeighbourCount = (u16)tpaGraph[wGraphPoint].dwNeighbourCount;
		AI::SGraphEdge		*tpaEdges		= (AI::SGraphEdge *)((BYTE *)tpaGraph + tpaGraph[wGraphPoint].dwEdgeOffset);
		int					iPointCount		= (int)m_tpSpawnPoint[m_tpNPC[dwNPCIndex].wSpawnPoint].ucRoutePointCount;
		vector<u16>			&wpaVertexes	= m_tpSpawnPoint[m_tpNPC[dwNPCIndex].wSpawnPoint].wpRouteGraphPoints;
		bool				bOk				= false;
		float				fDistance		= 0.0f;
		for (int i=0; i<wNeighbourCount; i++) {
			for (int j=0; j<iPointCount; j++)
				if ((tpaEdges[i].dwVertexNumber == wpaVertexes[j]) && (wpaVertexes[j] != m_tpNPC[dwNPCIndex].wPrevGraphPoint)) {
					m_tpNPC[dwNPCIndex].wNextGraphPoint = wpaVertexes[j];
					fDistance = tpaEdges[i].fPathDistance;
					bOk = true;
					break;
				}
			if (bOk)
				break;
		}
		if (!bOk) {
			for (int i=0; i<wNeighbourCount; i++) {
				for (int j=0; j<iPointCount; j++)
					if (tpaEdges[i].dwVertexNumber == wpaVertexes[j]) {
						m_tpNPC[dwNPCIndex].wNextGraphPoint = wpaVertexes[j];
						fDistance = tpaEdges[i].fPathDistance;
						bOk = true;
						break;
					}
				if (bOk)
					break;
			}
		}
		if (!bOk) {
			m_tpNPC[dwNPCIndex].fSpeed = 0.0f;
			m_tpNPC[dwNPCIndex].fDistanceFromPoint = 0.0f;
		}
		else {
			m_tpNPC[dwNPCIndex].fSpeed = 1.5f;
			m_tpNPC[dwNPCIndex].fDistanceFromPoint = fDistance;
		}
	}
	else {
		u32 dwCurTime = Level().timeServer();
		m_tpNPC[dwNPCIndex].fDistanceFromPoint -= float(dwCurTime)/1000.f * m_tpNPC[dwNPCIndex].fSpeed;
		if (m_tpNPC[dwNPCIndex].fDistanceFromPoint < EPS_L) {
			m_tpNPC[dwNPCIndex].fDistanceFromPoint	= 0.0f;
			m_tpNPC[dwNPCIndex].wPrevGraphPoint		= m_tpNPC[dwNPCIndex].wGraphPoint;
			m_tpNPC[dwNPCIndex].wGraphPoint			= m_tpNPC[dwNPCIndex].wNextGraphPoint;
		}
	}
}