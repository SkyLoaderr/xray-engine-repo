////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.cpp
//	Created 	: 25.12.2002
//  Modified 	: 25.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"

//#define WRITE_TO_LOG

CAI_ALife::CAI_ALife()
{
	m_bLoaded					= false;
}

CAI_ALife::~CAI_ALife()
{
	shedule_Unregister	();
	Save				();
}

void CAI_ALife::Load()
{
	shedule_Min					=   100;
	shedule_Max					= 10000;
	m_dwNPCBeingProcessed		=     0;
	m_qwMaxProcessTime			=   100*CPU::cycles_per_microsec;
	
	m_tNPCHeader.dwVersion		= ALIFE_VERSION;
	m_tNPCHeader.dwCount		= 0;
	m_bLoaded					= true;

	// checking if graph is loaded
	if (!Level().AI.m_tpaGraph)
		return;

	// loading spawn-points
	FILE_NAME	caFileName;
	if (!Engine.FS.Exist(caFileName, Path.GameData, "game.spawn"))
//		THROW;
		return;
	
	CStream *tpStream = Engine.FS.Open(caFileName);
	R_ASSERT(tpStream->FindChunk(NPC_SPAWN_POINT_CHUNK_VERSION));
	tpStream->Read(&m_tSpawnHeader,sizeof(m_tSpawnHeader));
	if (m_tSpawnHeader.dwVersion != NPC_SPAWN_POINT_VERSION)
		THROW;
	R_ASSERT(tpStream->FindChunk(NPC_SPAWN_POINT_CHUNK_DATA));
	m_tpSpawnPoint.resize(m_tSpawnHeader.dwCount);
	for (int i=0; i<(int)m_tSpawnHeader.dwCount; i++) {
		m_tpSpawnPoint[i].wGraphPoint				= tpStream->Rword();
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

	// loading NPCs
	if (!Engine.FS.Exist(caFileName, Path.GameData, "game.alife")) {
		Generate();
		Save();
	}
	
	if (!Engine.FS.Exist(caFileName, Path.GameData, "game.alife"))
		THROW;
	tpStream = Engine.FS.Open(caFileName);
	R_ASSERT(tpStream->FindChunk(ALIFE_CHUNK_VERSION));
	tpStream->Read(&m_tNPCHeader,sizeof(m_tNPCHeader));
	if (m_tNPCHeader.dwVersion != ALIFE_VERSION)
		THROW;
	R_ASSERT(tpStream->FindChunk(ALIFE_CHUNK_DATA));
	m_tpNPC.resize(m_tNPCHeader.dwCount);
	for (int i=0; i<(int)m_tNPCHeader.dwCount; i++) {
		m_tpNPC[i].wGraphPoint			= tpStream->Rword();
		m_tpNPC[i].wCount				= tpStream->Rword();
		m_tpNPC[i].wSpawnPoint			= tpStream->Rword();
		m_tpNPC[i].dwLastUpdateTime		= tpStream->Rdword();
		m_tpNPC[i].tpUsefulObject.resize(tpStream->Rword());
		for (int j=0; j<(int)m_tpNPC[i].tpUsefulObject.size(); j++)
			tpStream->Read(&(m_tpNPC[i].tpUsefulObject[j]),sizeof(SUsefulObject));
	}
	Engine.FS.Close(tpStream);

	vfInitGraph();
	vfInitTerrain();
	vfInitLocationOwners();
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
		tStream.Wword		(m_tpNPC[i].wGraphPoint);
		tStream.Wword		(m_tpNPC[i].wCount);
		tStream.Wword		(m_tpNPC[i].wSpawnPoint);
		tStream.Wdword		(m_tpNPC[i].dwLastUpdateTime);
		tStream.Wword		((u16)m_tpNPC[i].tpUsefulObject.size());
		for (int j=0; j<(int)m_tpNPC[i].tpUsefulObject.size(); j++)
            tStream.write(&(m_tpNPC[i].tpUsefulObject[j]),sizeof(SUsefulObject));
	}
	tStream.close_chunk	();
	
	tStream.SaveTo	("game.alife",0);
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
		tALifeNPC.wGraphPoint			= m_tpSpawnPoint[k].wGraphPoint;
		tALifeNPC.wSpawnPoint			= (u16)k;
		tALifeNPC.wCount				= m_tpSpawnPoint[k].wCount;
		tALifeNPC.dwLastUpdateTime		= Level().timeServer();
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
	if (m_tNPCHeader.dwCount)
		for (int i=1; ; i++, m_dwNPCBeingProcessed = (m_dwNPCBeingProcessed + 1) % m_tNPCHeader.dwCount) {
			vfProcessNPC(m_dwNPCBeingProcessed);
			if ((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i > m_qwMaxProcessTime)
				break;
		}
#ifdef WRITE_TO_LOG
	u64 t2x = CPU::GetCycleCount() - qwStartTime;
	Msg("* %.3f microseconds",CPU::cycles2microsec*t2x);
#endif
}

void CAI_ALife::vfProcessNPC(u32 dwNPCIndex)
{
		
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