////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.cpp
//	Created 	: 25.12.2002
//  Modified 	: 25.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"

CAI_ALife::CAI_ALife()
{
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

	if (!Engine.FS.Exist(caFileName, Path.GameData, "game.alife")) {
		Generate();
		Save();
	}
	Engine.FS.Close(tpStream);
	
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
		m_tpNPC[i].wCount				= tpStream->Rword();
		m_tpNPC[i].wSpawnPoint			= tpStream->Rword();
		m_tpNPC[i].dwLastUpdateTime		= tpStream->Rdword();
		m_tpNPC[i].tpUsefulObject.resize(tpStream->Rword());
		for (int j=0; j<(int)m_tpNPC[i].tpUsefulObject.size(); j++)
			tpStream->Read(&(m_tpNPC[i].tpUsefulObject[j]),sizeof(SUsefulObject));
	}
	Engine.FS.Close(tpStream);
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
	//Msg("* %7.2fs",Level().timeServer()/1000.f);
	u64	qwStartTime = CPU::GetCycleCount();
	if (m_tNPCHeader.dwCount)
		for (int i=1; ; i++, m_dwNPCBeingProcessed = (m_dwNPCBeingProcessed + 1) % m_tNPCHeader.dwCount) {
			vfProcessNPC(m_dwNPCBeingProcessed);
			if ((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i > m_qwMaxProcessTime)
				break;
		}
	//u64 t2x = CPU::GetCycleCount() - qwStartTime;
	//Msg("* %.3f microseconds",CPU::cycles2microsec*t2x);
}

void CAI_ALife::vfProcessNPC(u32 dwNPCIndex)
{
	
}