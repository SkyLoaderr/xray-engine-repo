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
CLevelGraph::CLevelGraph					(LPCSTR filename, u32 current_version)
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
#ifndef AI_COMPILER
	R_ASSERT					(header().version() == XRAI_CURRENT_VERSION);
#else
	if (XRAI_CURRENT_VERSION != current_version)
		if (header().version() != current_version)
			return;
	R_ASSERT					(header().version() == current_version);
#endif
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
		const CVertex		*_vertex = vertex(i);
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

u32 CLevelGraph::vertex		(u32 current_node_id, const Fvector& position) const
{
#ifndef AI_COMPILER
	Device.Statistic.AI_Node.Begin	();

	u32						id;

	if (!valid_vertex_id(current_node_id)) {
		// so, we do not have a correct current node
		// performing very slow full search
#ifdef _DEBUG
		//Msg					("%6d Full search (%d,[%f][%f][%f])",Level().timeServer(),current_node_id,VPUSH(position));
#endif
		id					= vertex(position);
		VERIFY				(valid_vertex_id(id));
		Device.Statistic.AI_Node.End();
		return				(id);
	}

	if (valid_vertex_position(position)) {
		if (inside(vertex(current_node_id),position)) {
			// so, our node corresponds to the position
#ifdef _DEBUG
			//Msg					("%6d No search (%d,[%f][%f][%f])",Level().timeServer(),current_node_id,VPUSH(position));
#endif
			Device.Statistic.AI_Node.End();
			return				(current_node_id);
		}

		// so, our position is inside the level graph bounding box
		// so, there is a node which corresponds with x and z to the position
		// try to search it with O(logN) time algorithm
#ifdef _DEBUG
		//Msg						("%6d Logarithmic search search (%d,[%f][%f][%f])",Level().timeServer(),current_node_id,VPUSH(position));
#endif
		u32						_vertex_id = vertex_id(position);
		if (valid_vertex_id(_vertex_id)) {
			Device.Statistic.AI_Node.End();
			return				(_vertex_id);
		}
	}

	// so, our position is outside the level graph bounding box
	// or
	// there is no node for the current position
	// try to search the nearest one iteratively

#ifdef _DEBUG
	//Msg					("%6d Neighbour search (%d,[%f][%f][%f])",Level().timeServer(),current_node_id,VPUSH(position));
#endif
	SContour			contour;
	Fvector				point;
	u32					best_vertex_id = current_node_id;
	ai().level_graph().contour(contour,current_node_id);
	ai().level_graph().nearest(point,position,contour);
	float				best_distance_sqr = position.distance_to_sqr(point);
	const_iterator		i,e;
	begin				(current_node_id,i,e);
	for ( ; i != e; ++i) {
		u32				level_vertex_id = value(current_node_id,i);
		if (!valid_vertex_id(level_vertex_id))
			continue;

		ai().level_graph().contour(contour,level_vertex_id);
		ai().level_graph().nearest(point,position,contour);
		float			distance_sqr = position.distance_to_sqr(point);
		if (best_distance_sqr > distance_sqr) {
			best_distance_sqr	= distance_sqr;
			best_vertex_id		= level_vertex_id;
		}
	}

	Device.Statistic.AI_Node.End();
	return					(best_vertex_id);
#else
	return					(current_node_id);
#endif
}

struct SSortNodesPredicate1 {
	IC	bool	operator()			(const CLevelGraph::CVertex &vertex, u32 xz) const
	{
		return		(vertex.position().xz() < xz);
	}
};

u32	CLevelGraph::vertex_id				(const Fvector &position) const
{
	CPosition			_vertex_position = vertex_position(position);
	CVertex				*B = m_nodes;
	CVertex				*E = m_nodes + header().vertex_count();
	CVertex				*I = std::lower_bound	(B,E,_vertex_position.xz(),SSortNodesPredicate1());
	if ((I == E) || ((*I).position().xz() != _vertex_position.xz()))
		return			(u32(-1));

	u32					best_vertex_id = u32(I - B);
	float				y = vertex_plane_y(best_vertex_id,position.x,position.z);
	for (++I ; I != E; ++I) {
		if ((*I).position().xz() != _vertex_position.xz())
			break;
		u32				new_vertex_id = u32(I - B);
		float			_y = vertex_plane_y(new_vertex_id,position.x,position.z);
		if (y <= position.y) {
			// so, current node is under the specified position
			if (_y <= position.y) {
				// so, new node is under the specified position
				if (position.y - _y < position.y - y) {
					// so, new node is closer to the specified position
					y				= _y;
					best_vertex_id	= new_vertex_id;
				}
			}
		}
		else
			// so, current node is over the specified position
			if (_y <= position.y) {
				// so, new node is under the specified position
				y				= _y;
				best_vertex_id	= new_vertex_id;
			}
			else
				// so, new node is over the specified position
				if (_y - position.y  < y - position.y) {
					// so, new node is closer to the specified position
					y				= _y;
					best_vertex_id	= new_vertex_id;
				}
	}

	return			(best_vertex_id);
}