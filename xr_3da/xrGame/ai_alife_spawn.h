////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_spawn.h
//	Created 	: 22.01.2003
//  Modified 	: 22.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life spawn-points
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_interfaces.h"
#include "ai_alife_templates.h"

class CALifeSpawnHeader : public IPureALifeLObject {
public:
	u32								m_tSpawnVersion;
	u32								m_dwSpawnCount;
	u32								m_dwLevelCount;
	
	virtual void					Load(CStream	&tFileStream)
	{
		R_ASSERT(tFileStream.FindChunk(SPAWN_POINT_CHUNK_VERSION));
		m_tSpawnVersion				= tFileStream.Rdword();
		if (m_tSpawnVersion != XRAI_CURRENT_VERSION)
			THROW;
		m_dwSpawnCount				= tFileStream.Rdword();
		m_dwLevelCount				= tFileStream.Rdword();
	};
};

class CALifeSpawnPoint : public IPureALifeLObject {
public:
	_GRAPH_ID						m_tNearestGraphPointID;
	string64						m_caModel;
	Fvector							m_tPosition;
	
	virtual void					Load(CStream	&tFileStream)
	{
		m_tNearestGraphPointID		= tFileStream.Rword();
		tFileStream.RstringZ		(m_caModel);
		tFileStream.Rvector			(m_tPosition);
	};
};

class CALifeZoneSpawnPoint : public CALifeSpawnPoint {
public:
	typedef CALifeSpawnPoint inherited;

	EAnomalousZoneType				m_tAnomalousZoneType;

	virtual void					Load(CStream	&tFileStream)
	{
		inherited::Load				(tFileStream);
		tFileStream.Read			(&m_tAnomalousZoneType,sizeof(m_tAnomalousZoneType));
	};
};

class CALifeObjectSpawnPoint : public CALifeSpawnPoint {
public:
	typedef CALifeSpawnPoint inherited;

	EArtefactType					m_tArtefactType;

	virtual void					Load(CStream	&tFileStream)
	{
		inherited::Load				(tFileStream);
		tFileStream.Read			(&m_tArtefactType,sizeof(m_tArtefactType));
	};
};

class CALifeCreatureSpawnPoint : public CALifeSpawnPoint {
public:
	typedef CALifeSpawnPoint inherited;

	u8								m_ucTeam;
	u8								m_ucSquad;
	u8								m_ucGroup;
	u16								m_wGroupID;
	u16								m_wCount;
	float							m_fBirthRadius;
	float							m_fBirthProbability;
	float							m_fIncreaseCoefficient;
	GRAPH_VECTOR					m_tpRouteGraphPoints;
	
	virtual void					Load(CStream	&tFileStream)
	{
		inherited::Load				(tFileStream);
		m_ucTeam					= tFileStream.Rbyte();
		m_ucSquad					= tFileStream.Rbyte();
		m_ucGroup					= tFileStream.Rbyte();
		m_wGroupID					= tFileStream.Rword();
		m_wCount					= tFileStream.Rword();
		m_fBirthRadius				= tFileStream.Rfloat();
		m_fBirthProbability			= tFileStream.Rfloat();
		m_fIncreaseCoefficient		= tFileStream.Rfloat();
		load_base_vector			(m_tpRouteGraphPoints, tFileStream);
	};
};