////////////////////////////////////////////////////////////////////////////
//	Module 		: game_location_selector.h
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game location selector
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "abstract_location_selector.h"
#include "game_graph.h"
#include "ai_object_location.h"

template <
	typename _VertexEvaluator,
	typename _vertex_id_type
>
class 
	CBaseLocationSelector<
		CGameGraph,
		_VertexEvaluator,
		_vertex_id_type
	> :
	public CAbstractLocationSelector <
		CGameGraph,
		_VertexEvaluator,
		_vertex_id_type
	>,
	virtual public CAI_ObjectLocation
{
	typedef CGameGraph _Graph;
	typedef CAbstractLocationSelector <
		CGameGraph,
		_VertexEvaluator,
		_vertex_id_type
	> inherited;
public:
	enum ESelectionType {
		eSelectionTypeMask = u32(0),
		eSelectionTypeRandomBranching,
		eSelectionTypeDummy = u32(-1),
	};

private:
	ESelectionType			m_selection_type;
	ALife::_GRAPH_ID		m_previous_vertex_id;
	ALife::TERRAIN_VECTOR	m_vertex_types;
	u32						m_time_to_change;

				void		select_random_location	(const _vertex_id_type start_vertex_id, _vertex_id_type &dest_vertex_id);
public:
	IC						CBaseLocationSelector	();
	IC	virtual				~CBaseLocationSelector	();
	IC	virtual void		Init					(const _Graph *graph = 0);
	IC			void		set_selection_type		(const ESelectionType selection_type);
	IC			void		get_selection_type		() const;
				void		select_location			(const _vertex_id_type start_vertex_id, _vertex_id_type &dest_vertex_id);
};

#include "game_location_selector_inline.h"
