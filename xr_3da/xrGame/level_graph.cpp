////////////////////////////////////////////////////////////////////////////
//	Module 		: level_graph.cpp
//	Created 	: 02.10.2001
//  Modified 	: 11.11.2003
//	Author		: Oles Shihkovtsov, Dmitriy Iassenev
//	Description : Level graph
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "level_graph.h"
#ifndef AI_COMPILER
	#include "ai_space.h"
#endif

#include "graph_search_engine.h"

#ifndef AI_COMPILER
CLevelGraph::CLevelGraph					()
#else
CLevelGraph::CLevelGraph					(LPCSTR filename)
#endif
{
#ifndef AI_COMPILER
#ifdef DEBUG
	sh_debug.create				("debug\\ai_nodes","$null");
#endif
	string256					file_name;
	FS.update_path				(file_name,"$level$","level.ai");
#else
	string256					file_name;
	strconcat					(file_name,filename,"level.ai");
#endif
	m_reader					= FS.r_open	(file_name);

	// m_header & data
	m_reader->r					(&m_header,sizeof(m_header));
	R_ASSERT					(header().version() == XRAI_CURRENT_VERSION);
	m_row_length				= iFloor((header().box().max.z - header().box().min.z)/header().cell_size() + EPS_L + .5f);
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

	bool	b;
	for (int i=1; i<(int)header().vertex_count(); ++i) {
		b		= inside(i,vertex_position(i));
		if (!b) {
			b=b;
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
	CLevelGraph::CPosition	_node_position;
	vertex_position			(_node_position,position);
	float					min_dist = flt_max;
	for (u32 i=1, selected = u32(-1); i<header().vertex_count(); ++i) {
		CVertex				*_vertex = vertex(i);
		if (inside			(_vertex,_node_position)) {
			float			dist = _abs(vertex_plane_y(_vertex) - position.y);
			if (dist < min_dist) {
				min_dist	= dist;
				selected	= i;
			}
		}
	}
	
	if (selected == u32(-1)) {
		for (u32 i=1, selected = u32(-1); i<header().vertex_count(); ++i) {
			CVertex				*_vertex = vertex(i);
			if (inside			(_vertex,_node_position)) {
				float			dist = _abs(vertex_plane_y(_vertex) - position.y);
				if (dist < min_dist) {
					min_dist	= dist;
					selected	= i;
				}
			}
		}
	}
	return					(selected);
}

u32 CLevelGraph::vertex		(u32 current_node_id, const Fvector& position, bool full_search) const
{
#ifndef AI_COMPILER
	Device.Statistic.AI_Node.Begin	();

	if (valid_vertex_id(current_node_id) && 
		inside			(vertex(current_node_id),position)) {
		Device.Statistic.AI_Node.End();
		return				(current_node_id);
	}

	u32						id, id_prev = valid_vertex_id(current_node_id) ? current_node_id : 1;
	float					distance = dInfinity;
	CGraphSearchEngine::CPositionParameters	position_params(position,1.f,30.f);

	if (ai().graph_search_engine().build_path(*this,id_prev,id_prev,0,position_params)) {
		VERIFY				(valid_vertex_id(position_params.m_vertex_id));
		return				(position_params.m_vertex_id);
	}

	if (position_params.m_distance < 1.f) {
		VERIFY				(valid_vertex_id(position_params.m_vertex_id));
		return				(position_params.m_vertex_id);
	}
	
	id						= vertex(position);
	if (valid_vertex_id(id))
		return				(id);

	Device.Statistic.AI_Node.End		();
#endif
	return					(current_node_id);
}