////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_spawn_registry_header.cpp
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife spawn registry header
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_spawn_registry_header.h"

CALifeSpawnHeader::~CALifeSpawnHeader	()
{
}

void CALifeSpawnHeader::load			(IReader	&file_stream)
{
	R_ASSERT2				(file_stream.find_chunk(SPAWN_POINT_CHUNK_VERSION),"Can't find chunk SPAWN_POINT_CHUNK_VERSION!");
	m_version				= file_stream.r_u32();
	R_ASSERT2				(XRAI_CURRENT_VERSION == m_version,"'game.spawn' version mismatch!");
	m_count					= file_stream.r_u32();
	m_level_count			= file_stream.r_u32();
}

