////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_object_location.h
//	Created 	: 27.11.2003
//  Modified 	: 27.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI object location
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"
#include "level_graph.h"
#include "game_graph.h"

class CAI_ObjectLocation {
private:
	u32						m_level_vertex_id;
	ALife::_GRAPH_ID		m_game_vertex_id;
	u32						m_dwCurrentFrame;
public:
	IC										CAI_ObjectLocation	();
	IC	virtual								~CAI_ObjectLocation	();
	IC			void						init				();
	IC	virtual	void						reinit				();
	IC			void						set_game_vertex		(const CGameGraph::CVertex *game_vertex);
	IC			void						set_game_vertex		(const ALife::_GRAPH_ID		game_vertex_id);
	IC			const CGameGraph::CVertex	*game_vertex		() const;
	IC			const ALife::_GRAPH_ID		game_vertex_id		() const;
	IC			void						set_level_vertex	(const CLevelGraph::CVertex *level_vertex);
	IC			void						set_level_vertex	(const u32					level_vertex_id);
	IC			const CLevelGraph::CVertex	*level_vertex		() const;
	IC			const u32					level_vertex_id		() const;
};

#include "ai_object_location_inline.h"