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
#include "location_manager.h"

enum ESelectionType {
	eSelectionTypeMask = u32(0),
	eSelectionTypeRandomBranching,
	eSelectionTypeDummy = u32(-1),
};

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
private:
	ESelectionType			m_selection_type;
	ALife::_GRAPH_ID		m_previous_vertex_id;
	u32						m_time_to_change;
	CLocationManager		*m_location_manager;

protected:
	IC			void		select_random_location	(const _vertex_id_type start_vertex_id, _vertex_id_type &dest_vertex_id);

public:
	IC						CBaseLocationSelector	();
	IC	virtual				~CBaseLocationSelector	();
	IC			void		init					();
	IC	virtual void		reinit					(const _Graph *graph = 0);
	IC			void		set_selection_type		(const ESelectionType selection_type);
	IC			void		selection_type			() const;
	IC			bool		actual					(const _vertex_id_type start_vertex_id, bool path_completed);
	IC			void		select_location			(const _vertex_id_type start_vertex_id, _vertex_id_type &dest_vertex_id);
};

#include "game_location_selector_inline.h"
