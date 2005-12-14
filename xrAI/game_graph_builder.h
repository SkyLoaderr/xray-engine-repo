////////////////////////////////////////////////////////////////////////////
//	Module 		: game_graph_builder.h
//	Created 	: 14.12.2005
//  Modified 	: 14.12.2005
//	Author		: Dmitriy Iassenev
//	Description : Game graph builder
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "xrLevel.h"

class CLevelGraph;
class CGameLevelCrossTable;

template <
	typename _data_type,
	typename _edge_weight_type,
	typename _vertex_id_type
>
class CGraphAbstract;

namespace GameGraph {
	struct CVertex;
};

class NET_Packet;

class CGameGraphBuilder {
private:
	typedef GameGraph::CVertex						vertex_type;
	typedef CGraphAbstract<vertex_type,float,u32>	graph_type;
	typedef xr_vector<xr_vector<u32> >				DISTANCES;

private:
	shared_str				m_level_name;
	CLevelGraph				*m_level_graph;
	graph_type				*m_graph;
	xrGUID					m_graph_guid;
	// cross table generation stuff
	xr_vector<bool>			m_marks;
	xr_vector<u32>			m_mark_stack;
	DISTANCES				m_distances;
	xr_vector<u32>			m_current_fringe;
	xr_vector<u32>			m_next_fringe;
	// cross table itself
	CGameLevelCrossTable	*m_cross_table;


private:
			void		create_graph				(const float &start, const float &amount);
			void		load_level_graph			(const float &start, const float &amount);
			void		load_graph_point			(NET_Packet &net_packet);
			void		load_graph_points			(const float &start, const float &amount);
			void		remove_incoherent_points	(const float &start, const float &amount);

private:
			void		mark_vertices				(u32 level_vertex_id);
			void		fill_marks					(const float &start, const float &amount);
			void		fill_distances				(const float &start, const float &amount);
			void		recursive_update			(const u32 &index, const float &start, const float &amount);
			void		iterate_distances			(const float &start, const float &amount);
			void		save_cross_table			(const float &start, const float &amount);
			void		build_cross_table			(const float &start, const float &amount);
			void		load_cross_table			(const float &start, const float &amount);
			
private:
			void		build_neighbours			(const float &start, const float &amount);
			void		generate_edges				(const float &start, const float &amount);
			void		optimize_graph				(const float &start, const float &amount);
			void		connectivity_check			(const float &start, const float &amount);
			void		save_graph					(const float &start, const float &amount);
			void		build_graph					(const float &start, const float &amount);

private:
	IC		CLevelGraph	&level_graph				() const;
	IC		graph_type	&graph						() const;

public:
						CGameGraphBuilder			();
						~CGameGraphBuilder			();
			void		build_graph					(LPCSTR level_name);
};

#include "game_graph_builder_inline.h"