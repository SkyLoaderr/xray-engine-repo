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
private:
	_Graph										*m_graph;
	bool										m_actuality;
	_VertexEvaluator							*m_evaluator;
	u32											m_intermediate_index;
	xr_vector<_vertex_id_type>					m_path;
public:


	IC					CAbstractPathManager		();
	IC	virtual			~CAbstractPathManager		();
	IC	virtual void	Init						(_Graph *graph = 0);
	IC			void	build_path					(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id);
	IC			void	select_intermediate_vertex	();
	IC	_vertex_id_type	get_intermediate_vertex_id	() const;
	IC			u32		get_intermediate_index		() const;
	IC			bool	path_actual					(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id) const;
	IC			bool	path_completed				() const;
	IC			bool	path_failed					() const;
	IC			void	set_evaluator				(_VertexEvaluator *evaluator);
	IC	const xr_vector<_vertex_id_type> &path		() const;
};

#include "abstract_path_manager_inline.h"