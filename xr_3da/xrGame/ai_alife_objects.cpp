////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_objects.cpp
//	Created 	: 23.06.2003
//  Modified 	: 23.06.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life objects
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife_registries.h"

#ifdef AI_COMPILER
	#include "xrLevel.h"
#else
	#include "..\\xrLevel.h"
#endif


void CSE_ALifeSpawnHeader::Load	(IReader	&tFileStream)
{
	R_ASSERT2					(tFileStream.find_chunk(SPAWN_POINT_CHUNK_VERSION),"Can't find chunk SPAWN_POINT_CHUNK_VERSION!");
	m_tSpawnVersion				= tFileStream.r_u32();
	R_ASSERT2					(m_tSpawnVersion == XRAI_CURRENT_VERSION,"'game.spawn' version mismatch!");
	m_dwSpawnCount				= tFileStream.r_u32();
	m_dwLevelCount				= tFileStream.r_u32();
};
