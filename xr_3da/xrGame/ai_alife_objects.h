////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_objects.h
//	Created 	: 05.01.2003
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life objects
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_interfaces.h"
#include "ai_alife_templates.h"
//#include "ai_alife_spawn.h"

class CALifeGameTime : public IPureALifeLSObject {
public:
	_TIME_ID						m_tGameTime;
	_TIME_ID						m_tTimeAfterSurge;
	u32								m_dwStartTime;
	float							m_fTimeFactor;

	virtual void					Save(CFS_Memory	&tMemoryStream)
	{
		m_tGameTime					= tfGetGameTime();
		m_dwStartTime				= Level().timeServer();
		tMemoryStream.open_chunk	(GAME_TIME_CHUNK_DATA);
		tMemoryStream.write			(&m_tGameTime,		sizeof(m_tGameTime));
		tMemoryStream.write			(&m_tTimeAfterSurge,sizeof(m_tTimeAfterSurge));
		tMemoryStream.Wfloat		(m_fTimeFactor);
		tMemoryStream.close_chunk	();
	};
	
	virtual void					Load(CStream	&tFileStream)
	{
		R_ASSERT					(tFileStream.FindChunk(GAME_TIME_CHUNK_DATA));
		tFileStream.Read			(&m_tGameTime,		sizeof(m_tGameTime));
		tFileStream.Read			(&m_tTimeAfterSurge,sizeof(m_tTimeAfterSurge));
		m_fTimeFactor				= tFileStream.Rfloat();
		m_dwStartTime				= Level().timeServer();
	};
	
	IC void							vfSetTimeFactor(float fTimeFactor)
	{
		m_tGameTime					= tfGetGameTime();
		m_dwStartTime				= Level().timeServer();
		m_fTimeFactor				= fTimeFactor;
	};

	IC _TIME_ID						tfGetGameTime()
	{
		return(m_tGameTime + iFloor(m_fTimeFactor*float(Level().timeServer() - m_dwStartTime)));
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

	virtual void					Save(CFS_Memory	&tMemoryStream)
	{
		tMemoryStream.open_chunk	(ALIFE_CHUNK_DATA);
		tMemoryStream.write			(&m_tALifeVersion,	sizeof(m_tALifeVersion));
		tMemoryStream.write			(&m_tZoneState,		sizeof(m_tZoneState));
		tMemoryStream.close_chunk	();
	}
	
	virtual void					Load(CStream	&tFileStream)
	{
		R_ASSERT					(tFileStream.FindChunk(ALIFE_CHUNK_DATA));
		tFileStream.Read			(&m_tALifeVersion,	sizeof(m_tALifeVersion));
		if (m_tALifeVersion != ALIFE_VERSION)
			THROW;
		tFileStream.Read			(&m_tZoneState,		sizeof(m_tZoneState));
	};
};