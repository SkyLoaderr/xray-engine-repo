////////////////////////////////////////////////////////////////////////////
//	Module 		: game_location_selector.h
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game location selector
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "path_manager_game_selector.h"

class CGameLocationSelector {
protected:
	u32										m_game_locate_query_time;
	u32										m_game_location_query_interval;
	bool									m_game_selector_failed;
	PathManagers::SVertexType				*m_game_selector_evaluator;
	ALife::_GRAPH_ID						m_selected_game_vertex_id;

public:
					CGameLocationSelector	();
	virtual			~CGameLocationSelector	();
			void	select_game_vertex		();
			bool	game_vertex_selection_actual();
//	virtual void	init_evaluator			(PathManagers::CAbstractNodeEvaluator *node_evaluator);
//	template <u64 flags>
//			void	select_location			(PathManagers::CVertexEvaluator<flags> *node_evaluator);
private:
			void perform_search				(const ALife::_GRAPH_ID game_vertex_id);
};