////////////////////////////////////////////////////////////////////////////
//	Module 		: game_location_selector.cpp
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game location selector
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game_location_selector.h"
#include "ai_space.h"
#include "graph_search_engine.h"

CGameLocationSelector::CGameLocationSelector	()
{
	m_game_selector_failed		= true;
	m_selected_game_vertex_id	= ALife::_GRAPH_ID(-1);
}

CGameLocationSelector::~CGameLocationSelector	()
{
}

void CGameLocationSelector::perform_search		(const ALife::_GRAPH_ID game_vertex_id)
{
	VERIFY				(m_game_selector_evaluator);
	ai().graph_search_engine().build_path(ai().game_graph(),game_vertex_id,game_vertex_id,0,m_game_selector_evaluator);
	m_game_selector_failed	= !ai().game_graph().valid_vertex_id(m_game_selector_evaluator->m_vertex_id) || (m_game_selector_evaluator->m_vertex_id == m_selected_game_vertex_id);
}

void CGameLocationSelector::select_game_vertex	(const ALife::_GRAPH_ID game_vertex_id)
{
	if (!m_game_selector_failed) {
		perform_search	(game_vertex_id);
		return;
	}
}

bool CGameLocationSelector::game_vertex_selection_actual()
{
	perform_search		(game_vertex_id);
	return				(m_game_selector_failed);
}
