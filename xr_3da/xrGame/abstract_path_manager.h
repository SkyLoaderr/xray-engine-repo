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
	_VertexEvaluator							*m_path_evaluator;
	u32											m_intermediate_index;


	IC					CAbstractPathManager		();
	IC	virtual			~CAbstractPathManager		();
	IC	virtual void	Init						();
	IC			void	build_path					(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id);
	IC			void	select_intermediate_vertex	();
	IC	_vertex_id_type	get_intermediate_vertex_id	() const;
	IC			u32		get_intermediate_index		() const;
	IC			bool	path_actual					(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id) const;
	IC			bool	path_completed				() const;
	IC			bool	path_failed					() const;
};

#include "abstract_path_manager_inline.h"