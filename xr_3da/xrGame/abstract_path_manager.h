////////////////////////////////////////////////////////////////////////////
//	Module 		: abstract_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Abstract path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
class CAbstractPathManager {
protected:
	_Graph										*m_graph;
	bool										m_path_actuality;
public:
	xr_vector<_vertex_id_type>					m_path;
	u32											m_cur_node_index;
	_VertexEvaluator							*m_path_evaluator;


	IC					CAbstractPathManager		();
	IC	virtual			~CAbstractPathManager		();
	IC	virtual void	init						();
	IC			void	build_path					(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id);
	IC			void	select_intermediate_vertex	();
	IC			bool	path_actual					(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id) const;
	IC			bool	path_completed				() const;
};

#include "abstract_path_manager_inline.h"