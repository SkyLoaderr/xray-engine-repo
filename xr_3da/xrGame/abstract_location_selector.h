////////////////////////////////////////////////////////////////////////////
//	Module 		: abstract_location_selector.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Abstract location selector
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
class CAbstractLocationSelector {
private:
	bool						m_selector_failed;
	_VertexEvaluator			*m_selector_evaluator;
	_vertex_id_type				m_selected_vertex_id;
	_Graph						*m_graph;

	IC		void				perform_search				(const _vertex_id_type game_vertex_id);
protected:
	u32							m_last_query_time;
	u32							m_query_interval;
public:
	IC							CAbstractLocationSelector	();
	IC	virtual					~CAbstractLocationSelector	();
	IC	virtual void			Init						();
	IC			_vertex_id_type selected_vertex_id			() const;
	IC			bool			selector_failed				() const;
	IC			void			set_query_interval			(const u32 query_interval);
	IC			bool			location_selection_actual	(const _vertex_id_type start_vertex_id);
	IC			void			set_selector_evaluator		(_VertexEvaluator *vertex_evaluator);
	IC			void			select_location				(const _vertex_id_type start_vertex_id, _vertex_id_type &dest_vertex_id);
	IC			bool			selector_used				() const;
};

#include "abstract_location_selector_inline.h"
