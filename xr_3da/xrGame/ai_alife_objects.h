////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_objects.h
//	Created 	: 05.01.2003
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life objects
////////////////////////////////////////////////////////////////////////////

#pragma once

#define ALIFE_SUPPORT_CONSOLE_COMMANDS

#include "..\\xrLevel.h"
#include "ai_alife_interfaces.h"
#include "ai_alife_templates.h"

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
	
	virtual void					Load(IReader	&tFileStream)
	{
		R_ASSERT2					(tFileStream.find_chunk(SPAWN_POINT_CHUNK_VERSION),"Can't find chunk SPAWN_POINT_CHUNK_VERSION!");
		m_tSpawnVersion				= tFileStream.r_u32();
		R_ASSERT2					(m_tSpawnVersion == XRAI_CURRENT_VERSION,"'game.spawn' version mismatch!");
		m_dwSpawnCount				= tFileStream.r_u32();
		m_dwLevelCount				= tFileStream.r_u32();
	};
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

//class CSE_ALifeDiscovery : public IPureALifeLSObject {
//public:
//	EOrganizationType				m_tOrganizationType;
//
//
//									CSE_ALifeDiscovery()
//	{
//	}
//
//	virtual void					Save(IWriter	&tMemoryStream)
//	{
//		tMemoryStream.w				(&m_tOrganizationType,	sizeof(m_tOrganizationType));
//	}
//
//	virtual void					Load(IReader	&tFileStream)
//	{
//		tFileStream.r				(&m_tOrganizationType,	sizeof(m_tOrganizationType));
//	};
//};
//
//class CSE_ALifeOrganization : public IPureALifeLSObject {
//public:
//	EOrganizationType				m_tOrganizationType;
//	LPCSTR							m_caOrganizationIdentifier;
//	ARTEFACT_VECTOR					m_tpArtefacts;
//
//
//									CSE_ALifeKnownAnomaly()
//	{
//	}
//
//	virtual void					Save(IWriter	&tMemoryStream)
//	{
//		tMemoryStream.w				(&m_tOrganizationType,	sizeof(m_tOrganizationType));
//	}
//
//	virtual void					Load(IReader	&tFileStream)
//	{
//		tFileStream.r				(&m_tOrganizationType,	sizeof(m_tOrganizationType));
//	};
//};
