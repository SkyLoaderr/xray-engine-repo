////////////////////////////////////////////////////////////////////////////
//	Module 		: game_graph_builder.h
//	Created 	: 14.12.2005
//  Modified 	: 14.12.2005
//	Author		: Dmitriy Iassenev
//	Description : Game graph builder
////////////////////////////////////////////////////////////////////////////

#pragma once

class CLevelGraph;

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

private:
	shared_str			m_level_name;
	CLevelGraph			*m_level_graph;
	graph_type			*m_graph;

private:
			void		load_level_graph	();
			void		load_graph_point	(NET_Packet &net_packet);
			void		load_graph_points	();

private:
	IC		CLevelGraph	&level_graph		() const;
	IC		graph_type	&graph				() const;

public:
						CGameGraphBuilder	();
						~CGameGraphBuilder	();
			void		build_graph			(LPCSTR level_name);
};

#include "game_graph_builder_inline.h"