////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path.cpp
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol path
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "patrol_path.h"
#include "levelgamedef.h"

CPatrolPath	&CPatrolPath::load_path(IReader &stream)
{
	R_ASSERT		(stream.find_chunk(WAYOBJECT_CHUNK_POINTS));
	u32				vertex_count = stream.r_u16();
	for (u32 i=0; i<vertex_count; ++i)
		add_vertex	(CPatrolPoint().load(stream),i);

	R_ASSERT		(stream.find_chunk(WAYOBJECT_CHUNK_LINKS));
	u32				edge_count = stream.r_u16();
	for (u32 i=0; i < edge_count; ++i) {
		u16			vertex0 = stream.r_u16();
		u16			vertex1 = stream.r_u16();
		float		probability = stream.r_float();
		add_edge	(vertex0,vertex1,probability);
	}

	return			(*this);
}

CPatrolPath::~CPatrolPath	()
{
}
