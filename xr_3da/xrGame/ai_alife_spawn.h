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