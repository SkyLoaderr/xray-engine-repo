////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_object_location.h
//	Created 	: 27.11.2003
//  Modified 	: 27.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI object location
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_space.h"

IC	CAI_ObjectLocation::CAI_ObjectLocation()
{
	m_dwCurrentFrame		= u32(-1);
	init					();
}

IC	CAI_ObjectLocation::~CAI_ObjectLocation()
{
}

IC	void CAI_ObjectLocation::init()
{
	if (ai().get_level_graph())
		ai().level_graph().set_invalid_vertex	(m_level_vertex_id);
	else
		m_level_vertex_id	= u32(-1);
	ai().game_graph().set_invalid_vertex	(m_game_vertex_id);
}

IC	void CAI_ObjectLocation::reinit()
{
	if (m_dwCurrentFrame == Device.dwFrame)
		return;

	m_dwCurrentFrame		= Device.dwFrame;

	init					();
}

IC	void CAI_ObjectLocation::set_game_vertex(const CGameGraph::CVertex *game_vertex)
{
	VERIFY				(ai().game_graph().valid_vertex_id(ai().game_graph().vertex_id(game_vertex)));
	m_game_vertex_id	= ai().game_graph().vertex_id(game_vertex);
}

IC	void CAI_ObjectLocation::set_game_vertex(const ALife::_GRAPH_ID	game_vertex_id)
{
	VERIFY				(ai().game_graph().valid_vertex_id(game_vertex_id));
	m_game_vertex_id	= game_vertex_id;
}

IC	const CGameGraph::CVertex *CAI_ObjectLocation::game_vertex() const
{
	VERIFY				(ai().game_graph().valid_vertex_id(m_game_vertex_id));
	return				(ai().game_graph().vertex(m_game_vertex_id));
}

IC	const ALife::_GRAPH_ID CAI_ObjectLocation::game_vertex_id() const
{
	return				(m_game_vertex_id);
}

IC	void CAI_ObjectLocation::set_level_vertex(const CLevelGraph::CVertex *level_vertex)
{
	VERIFY				(ai().level_graph().valid_vertex_id(ai().level_graph().vertex_id(level_vertex)));
	m_level_vertex_id	= ai().level_graph().vertex_id(level_vertex);
}

IC	void CAI_ObjectLocation::set_level_vertex(const u32 level_vertex_id)
{
	VERIFY				(ai().level_graph().valid_vertex_id(level_vertex_id));
	m_level_vertex_id	= level_vertex_id;
}

IC	const CLevelGraph::CVertex *CAI_ObjectLocation::level_vertex() const
{
	VERIFY				(ai().level_graph().valid_vertex_id(m_level_vertex_id));
	return				(ai().level_graph().vertex(m_level_vertex_id));
}

IC	const u32 CAI_ObjectLocation::level_vertex_id() const
{
	return				(m_level_vertex_id);
}