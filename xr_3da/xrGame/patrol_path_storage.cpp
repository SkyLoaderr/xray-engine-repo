////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_storage.cpp
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol path storage
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "patrol_path_storage.h"
#include "patrol_path.h"
#include "patrol_point.h"
#include "levelgamedef.h"

CPatrolPathStorage::~CPatrolPathStorage		()
{
	delete_data		(m_registry);
}

void CPatrolPathStorage::load				(IReader &stream)
{
	IReader					*chunk = stream.open_chunk(WAY_PATROLPATH_CHUNK);

	if (!chunk)
		return;
		
	int						chunk_iterator = 0;
	for (IReader *sub_chunk = chunk->open_chunk(chunk_iterator++); sub_chunk; sub_chunk = chunk->open_chunk(chunk_iterator++)) {
		R_ASSERT			(sub_chunk->find_chunk(WAYOBJECT_CHUNK_VERSION));
		R_ASSERT			(sub_chunk->r_u16() == WAYOBJECT_VERSION);
		R_ASSERT			(sub_chunk->find_chunk(WAYOBJECT_CHUNK_NAME));

		shared_str				patrol_name;
		sub_chunk->r_stringZ(patrol_name);
		m_registry.insert	(std::make_pair(patrol_name,&xr_new<CPatrolPath>()->load(*sub_chunk)));

		sub_chunk->close	();
	}
	
	chunk->close			();
}
