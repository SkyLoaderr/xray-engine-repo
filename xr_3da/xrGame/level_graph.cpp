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

#include "graph_engine.h"

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
	m_header					= (CHeader*)m_reader->pointer();
	R_ASSERT					(header().version() == XRAI_CURRENT_VERSION);
	m_reader->advance			(sizeof(CHeader));
	m_nodes						= (CVertex*)m_reader->pointer();
	m_row_length				= iFloor((header().box().max.z - header().box().min.z)/header().cell_size() + EPS_L + 1.5f);
	m_column_length				= iFloor((header().box().max.x - header().box().min.x)/header().cell_size() + EPS_L + 1.5f);
	m_ref_counts.assign			(header().vertex_count(),0);
#pragma todo("Dima to Dima : Optimize the nearest node by position search")
	m_valid_nodes.assign		(m_row_length*m_column_length,false);
	for (u32 i=0, n = header().vertex_count(); i<n; ++i)
		m_valid_nodes[vertex(i)->position().xz()] = true;
}

CLevelGraph::~CLevelGraph		()
{
	xr_delete					(m_reader);
}

u32	CLevelGraph::vertex		(const Fvector &position) const
{
	CLevelGraph::CPosition	_node_position;
	vertex_position			(_node_position,position);
	bool					inside_vertex = false;
	float					min_dist = flt_max;
	u32						selected;
	set_invalid_vertex		(selected);
	for (u32 i=0; i<header().vertex_count(); ++i) {
		CVertex				*_vertex = vertex(i);
		if (inside			(_vertex,_node_position)) {
			float			dist = _abs(vertex_plane_y(_vertex) - position.y);
			if (dist < min_dist) {
				inside_vertex = true;
				min_dist	= dist;
				selected	= i;
			}
		}
	}
	
	if (!valid_vertex_id(selected)) {
		for (u32 i=0; i<header().vertex_count(); ++i) {
			float		dist =  distance(i,position);
			if (dist < min_dist) {
				min_dist	= dist;
				selected	= i;
			}
		}
	}

	VERIFY					(valid_vertex_id(selected));
	return					(selected);
}

u32 CLevelGraph::vertex		(u32 current_node_id, const Fvector& position, bool full_search) const
{
#ifndef AI_COMPILER
	Device.Statistic.AI_Node.Begin	();
	
	if (valid_vertex_id(current_node_id) && !m_valid_nodes[vertex_position(position).xz()])
		return				(current_node_id);

	u32						id;
	if (valid_vertex_id(current_node_id)) {
		if (inside(vertex(current_node_id),position)) {
			Device.Statistic.AI_Node.End();
			return			(current_node_id);
		}
		id					= check_position_in_direction(current_node_id,vertex_position(current_node_id),position);
		if (valid_vertex_id(id) && inside(vertex(id),position) && (position.distance_to(vertex_position(id)) < 1.5f)) {
			Device.Statistic.AI_Node.End();
			return			(id);
		}
	}

	u32						id_prev = valid_vertex_id(current_node_id) ? current_node_id : 0;
	if (position.distance_to(vertex_position(id_prev)) < 30.f) {
		CGraphEngine::CPositionParameters	position_params(position,1.f,30.f);

		if (ai().graph_engine().search(*this,id_prev,id_prev,0,position_params)) {
			VERIFY				(valid_vertex_id(position_params.m_vertex_id));
			Device.Statistic.AI_Node.End		();
			return				(position_params.m_vertex_id);
		}

		if (position_params.m_distance < 1.5f) {
			VERIFY				(valid_vertex_id(position_params.m_vertex_id));
			Device.Statistic.AI_Node.End		();
			return				(position_params.m_vertex_id);
		}
	}

	id						= vertex(position);
	if (valid_vertex_id(id)) {
		Device.Statistic.AI_Node.End		();
		return				(id);
	}

	Device.Statistic.AI_Node.End		();
#endif
	return					(current_node_id);
}