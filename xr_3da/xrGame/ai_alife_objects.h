////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_objects.h
//	Created 	: 05.01.2003
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life objects
////////////////////////////////////////////////////////////////////////////

#pragma once

#define ALIFE_SUPPORT_CONSOLE_COMMANDS

#include "ai_alife_interfaces.h"

class CSE_ALifeGameTime : public IPureALifeLSObject {
public:
	_TIME_ID						m_tGameTime;
	_TIME_ID						m_tLastSurgeTime;
	_TIME_ID						m_tNextSurgeTime;
	float							m_fTimeFactor;
	u32								m_dwStartTime;
	u64								m_qwStartProcessorCycle;

	virtual void					Save(IWriter	&tMemoryStream)
	{
		m_tGameTime					= tfGetGameTime();
		m_dwStartTime				= Device.TimerAsync();
		tMemoryStream.open_chunk	(GAME_TIME_CHUNK_DATA);
		tMemoryStream.w				(&m_tGameTime,		sizeof(m_tGameTime));
		tMemoryStream.w				(&m_tLastSurgeTime,	sizeof(m_tLastSurgeTime));
		tMemoryStream.w				(&m_tNextSurgeTime,	sizeof(m_tNextSurgeTime));
		tMemoryStream.w_float		(m_fTimeFactor);
		tMemoryStream.close_chunk	();
	};
	
	virtual void					Load(IReader	&tFileStream)
	{
		R_ASSERT2					(tFileStream.find_chunk(GAME_TIME_CHUNK_DATA),"Can't find chunk GAME_TIME_CHUNK_DATA!");
		tFileStream.r				(&m_tGameTime,		sizeof(m_tGameTime));
		tFileStream.r				(&m_tLastSurgeTime,	sizeof(m_tLastSurgeTime));
		tFileStream.r				(&m_tNextSurgeTime,	sizeof(m_tNextSurgeTime));
		m_fTimeFactor				= tFileStream.r_float();
		m_qwStartProcessorCycle		= CPU::GetCycleCount();
		m_dwStartTime				= Device.TimerAsync();
	};
	
#ifdef ALIFE_SUPPORT_CONSOLE_COMMANDS
	IC void							vfSetTimeFactor(float fTimeFactor)
	{
		m_tGameTime					= tfGetGameTime();
		m_dwStartTime				= Device.TimerAsync();
		m_fTimeFactor				= fTimeFactor;
	};
#endif

	IC _TIME_ID						tfGetGameTime()
	{
		return						(m_tGameTime + iFloor(m_fTimeFactor*float(Device.TimerAsync() - m_dwStartTime)));
	};
};

class CSE_ALifeHeader : public IPureALifeLSObject {
public:
	u32								m_tALifeVersion;
	EZoneState						m_tZoneState;
	
									CSE_ALifeHeader()
	{
		m_tALifeVersion				= ALIFE_VERSION;
		m_tZoneState				= eZoneStateAfterSurge;
	}

	virtual void					Save(IWriter	&tMemoryStream)
	{
		tMemoryStream.open_chunk	(ALIFE_CHUNK_DATA);
		tMemoryStream.w				(&m_tALifeVersion,	sizeof(m_tALifeVersion));
		tMemoryStream.w				(&m_tZoneState,		sizeof(m_tZoneState));
		tMemoryStream.close_chunk	();
	}
	
	virtual void					Load(IReader	&tFileStream)
	{
		R_ASSERT2					(tFileStream.find_chunk(ALIFE_CHUNK_DATA),"Can't find chunk ALIFE_CHUNK_DATA");
		tFileStream.r				(&m_tALifeVersion,	sizeof(m_tALifeVersion));
		R_ASSERT2					(m_tALifeVersion == ALIFE_VERSION,"ALife version mismatch!");
		tFileStream.r				(&m_tZoneState,		sizeof(m_tZoneState));
	};
};

class CSE_ALifeSpawnHeader : public IPureALifeLObject {
public:
	u32								m_tSpawnVersion;
	u32								m_dwSpawnCount;
	u32								m_dwLevelCount;
	
	virtual void					Load(IReader	&tFileStream);
};

class CSE_ALifeKnownAnomaly : public IPureALifeLSObject {
public:
	EAnomalousZoneType				m_tAnomalousZoneType;
	float							m_fAnomalyPower;
	float							m_fDistance;

									CSE_ALifeKnownAnomaly()
	{
	}

	virtual void					Save(IWriter	&tMemoryStream)
	{
		tMemoryStream.w				(&m_tAnomalousZoneType,	sizeof(m_tAnomalousZoneType));
		tMemoryStream.w_float		(m_fAnomalyPower);
		tMemoryStream.w_float		(m_fDistance);
	}
	
	virtual void					Load(IReader	&tFileStream)
	{
		tFileStream.r				(&m_tAnomalousZoneType,	sizeof(m_tAnomalousZoneType));
		m_fAnomalyPower				= tFileStream.r_float();
		m_fDistance					= tFileStream.r_float();
	};
};

class CSE_ALifeArtefactDemand : public IPureALifeLSObject {
public:
	LPCSTR							m_caSection;
	u32								m_dwMinArtefactCount;
	u32								m_dwMaxArtefactCount;
	u32								m_dwMinArtefactPrice;
	u32								m_dwMaxArtefactPrice;
	u32								m_dwRealArtefactCount;
	u32								m_dwRealArtefactPrice;

									CSE_ALifeArtefactDemand(LPCSTR caSection, u32 dwMinArtefactCount, u32 dwMaxArtefactCount, u32 dwMinArtefactPrice, u32 dwMaxArtefactPrice) :	m_caSection(caSection), m_dwMinArtefactCount(dwMinArtefactCount), m_dwMaxArtefactCount(dwMaxArtefactCount), m_dwMinArtefactPrice(dwMinArtefactPrice), m_dwMaxArtefactPrice(dwMaxArtefactPrice),	m_dwRealArtefactCount(0), m_dwRealArtefactPrice(0)
	{
	}
	
	virtual							~CSE_ALifeArtefactDemand()
	{
	}

	virtual void					Save(IWriter	&tMemoryStream)
	{
		tMemoryStream.w_u32			(m_dwRealArtefactCount);
		tMemoryStream.w_u32			(m_dwRealArtefactPrice);
	}

	virtual void					Load(IReader	&tFileStream)
	{
		m_dwRealArtefactCount		= tFileStream.r_u32();
		m_dwRealArtefactPrice		= tFileStream.r_u32();
	};
};

class CSE_ALifeArtefactNeed {
public:
	LPCSTR							m_caSection;
	u32								m_dwArtefactCount;

									CSE_ALifeArtefactNeed(LPCSTR caSection, u32 dwArtefactCount) :	m_caSection(caSection), m_dwArtefactCount(dwArtefactCount)
	{
	}

	virtual							~CSE_ALifeArtefactNeed()
	{
	}
};

class CSE_ALifeDiscovery : public IPureALifeLSObject {
public:
	LPCSTR							m_caDiscoveryIdentifier;
	float							m_fSuccessProbability;
	float							m_fDestroyProbability;
	_TIME_ID						m_tFreezeTime;
	_TIME_ID						m_tResearchTime;
	DEMAND_P_VECTOR					m_tpArtefactDemand;
	NEED_P_VECTOR					m_tpArtefactNeed;
	LPCSTR_VECTOR					m_tpDependency;
	CLSID_VECTOR					m_tpClassIDs;


									CSE_ALifeDiscovery	(LPCSTR caSection);
	virtual							~CSE_ALifeDiscovery	();
	virtual void					Save				(IWriter &tMemoryStream);
	virtual void					Load				(IReader &tFileStream);
};

class CSE_ALifeOrganization : public IPureALifeLSObject {
public:
	LPCSTR							m_caOrganizationIdentifier;
	LPCSTR_VECTOR					m_tpPossibleDiscoveries;
	float							m_fJoinProbability;
	float							m_fLeftProbability;


									CSE_ALifeOrganization(LPCSTR caSection);
									~CSE_ALifeOrganization();
	virtual void					Save				(IWriter &tMemoryStream);
	virtual void					Load				(IReader &tFileStream);
};
