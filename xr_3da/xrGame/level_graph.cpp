////////////////////////////////////////////////////////////////////////////
//	Module 		: level_graph.cpp
//	Created 	: 02.10.2001
//  Modified 	: 11.11.2003
//	Author		: Oles Shihkovtsov, Dmitriy Iassenev
//	Description : Level graph
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "level_graph.h"

CLevelGraph::CLevelGraph		()
{
#ifdef DEBUG
	sh_debug.create				("debug\\ai_nodes","$null");
#endif
	m_reader					= 0;
	string256					file_name;
	if (!FS.exist(file_name,"$level$","level.ai"))
		return;
//	R_ASSERT					(!FS.exist(file_name,"$level$","level.ai"));
	m_reader					= FS.r_open	(file_name);

	// m_header & data
	m_reader->r					(&m_header,sizeof(m_header));
	R_ASSERT					(header().version() == XRAI_CURRENT_VERSION);
	m_row_length				= iFloor((header().box().max.z - header().box().min.z)/header().cell_size() + EPS_L + .5f) + 1;
	m_palette_size				= m_reader->r_u32();
	m_cover_palette				= (Cover*)m_reader->pointer();
	m_reader->advance			(m_palette_size*sizeof(Cover));
	m_nodes						= (u8*)m_reader->pointer();

	// dispatch table
	m_nodes_ptr					= (CVertex**)xr_malloc(header().vertex_count()*sizeof(void*));
	{
		for (u32 i=0; i<header().vertex_count(); ++i) {
			m_nodes_ptr[i]		= (CVertex*)m_reader->pointer();
			CVertex		vertex;
			m_reader->r			(&vertex,sizeof(vertex));
		}
	}
}

CLevelGraph::~CLevelGraph		()
{
	xr_free						(m_nodes_ptr);
	xr_delete					(m_reader);
}

u32	CLevelGraph::vertex		(const Fvector &position) const
{
	VERIFY					(loaded());

	CLevelGraph::CPosition	_node_position;
	vertex_position			(_node_position,position);
	for (u32 i=0, selected = u32(-1), min_dist = u32(-1); i<header().vertex_count(); ++i) {
		CVertex				*vertex = m_nodes_ptr[i];
		if (inside			(vertex,_node_position)) {
			u32				dist = iFloor(_abs(vertex_plane_y(vertex) - position.y) + .5f);
			if (dist < min_dist) {
				min_dist	= dist;
				selected	= i;
			}
		}
	}
	
	return					(selected);
}

u32 CLevelGraph::vertex		(u32 current_node_id, const Fvector& position, bool full_search) const
{
	VERIFY					(loaded());

	Device.Statistic.AI_Node.Begin	();

	if (valid_vertex_id(current_node_id) && 
		inside			(vertex(current_node_id),position)) {
		Device.Statistic.AI_Node.End();
		return				(current_node_id);
	}

#pragma todo("Dima to Dima : implement pathfinding search here!")
	
	u32						id = vertex(position);
	
	if (valid_vertex_id(id))
		return				(id);

	Device.Statistic.AI_Node.End		();
	return					(current_node_id);
}