////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_smart_terrain_task.h
//	Created 	: 20.09.2005
//  Modified 	: 20.09.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife smart terrain task
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "game_graph_space.h"

class CALifeSmartTerrainTask {
public:
	GameGraph::_GRAPH_ID	m_game_vertex_id;
	u32						m_level_vertex_id;
};

#include "alife_smart_terrain_task_inline.h"