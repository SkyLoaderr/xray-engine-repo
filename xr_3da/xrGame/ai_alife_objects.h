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

class CALifeGameTime : public IPureALifeLSObject {
public:
	_TIME_ID						m_tGameTime;
	_TIME_ID						m_tTimeAfterSurge;
	u32								m_dwStartTime;
	float							m_fTimeFactor;

	virtual void					Save(CMemoryWriter	&tMemoryStream)
	{
		m_tGameTime					= tfGetGameTime();
		m_dwStartTime				= Device.dwTimeGlobal;
		tMemoryStream.open_chunk	(GAME_TIME_CHUNK_DATA);
		tMemoryStream.w				(&m_tGameTime,		sizeof(m_tGameTime));
		tMemoryStream.w				(&m_tTimeAfterSurge,sizeof(m_tTimeAfterSurge));
		tMemoryStream.w_float		(m_fTimeFactor);
		tMemoryStream.close_chunk	();
	};
	
	virtual void					Load(IReader	&tFileStream)
	{
		R_ASSERT					(tFileStream.find_chunk(GAME_TIME_CHUNK_DATA));
		tFileStream.r				(&m_tGameTime,		sizeof(m_tGameTime));
		tFileStream.r				(&m_tTimeAfterSurge,sizeof(m_tTimeAfterSurge));
		m_fTimeFactor				= tFileStream.r_float();
		m_dwStartTime				= Device.dwTimeGlobal;
	};
	
#ifdef ALIFE_SUPPORT_CONSOLE_COMMANDS
	IC void							vfSetTimeFactor(float fTimeFactor)
	{
		m_tGameTime					= tfGetGameTime();
		m_dwStartTime				= Device.dwTimeGlobal;
		m_fTimeFactor				= fTimeFactor;
	};
#endif

	IC _TIME_ID						tfGetGameTime()
	{
		return(m_tGameTime + iFloor(m_fTimeFactor*float(Device.dwTimeGlobal - m_dwStartTime)));
	};
};

class CALifeHeader : public IPureALifeLSObject {
public:
	u32								m_tALifeVersion;
	EZoneState						m_tZoneState;
	
									CALifeHeader()
	{
		m_tALifeVersion				= ALIFE_VERSION;
		m_tZoneState				= eZoneStateAfterSurge;
	}

	virtual void					Save(CMemoryWriter	&tMemoryStream)
	{
		tMemoryStream.open_chunk	(ALIFE_CHUNK_DATA);
		tMemoryStream.w				(&m_tALifeVersion,	sizeof(m_tALifeVersion));
		tMemoryStream.w				(&m_tZoneState,		sizeof(m_tZoneState));
		tMemoryStream.close_chunk	();
	}
	
	virtual void					Load(IReader	&tFileStream)
	{
		R_ASSERT					(tFileStream.find_chunk(ALIFE_CHUNK_DATA));
		tFileStream.r			(&m_tALifeVersion,	sizeof(m_tALifeVersion));
		if (m_tALifeVersion != ALIFE_VERSION)
			THROW;
		tFileStream.r			(&m_tZoneState,		sizeof(m_tZoneState));
	};
};

class CALifeSpawnHeader : public IPureALifeLObject {
public:
	u32								m_tSpawnVersion;
	u32								m_dwSpawnCount;
	u32								m_dwLevelCount;
	 
	virtual void					Load(IReader	&tFileStream)
	{
		R_ASSERT(tFileStream.find_chunk(SPAWN_POINT_CHUNK_VERSION));
		m_tSpawnVersion				= tFileStream.r_u32();
		if (m_tSpawnVersion != XRAI_CURRENT_VERSION)
			THROW;
		m_dwSpawnCount				= tFileStream.r_u32();
		m_dwLevelCount				= tFileStream.r_u32();
	};
};