////////////////////////////////////////////////////////////////////////////
//	Module 		: game_graph_script.cpp
//	Created 	: 02.11.2005
//  Modified 	: 02.11.2005
//	Author		: Dmitriy Iassenev
//	Description : Game graph class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game_graph.h"
#include "script_space.h"
#include "ai_space.h"

using namespace luabind;

const CGameGraph *get_game_graph		()
{
	return				(&ai().game_graph());
}

const CGameGraph::CHeader *get_header	(const CGameGraph *self)
{
	return				(&self->header());
}

bool get_accessible1					(const CGameGraph *self, const u32 &vertex_id)
{
	return				(self->accessible(vertex_id));
}

void get_accessible2					(const CGameGraph *self, const u32 &vertex_id, bool value)
{
	self->accessible	(vertex_id,value);
}

Fvector CVertex__level_point			(const CVertex *vertex)
{
	THROW				(vertex);
	return				(vertex->level_point());
}

Fvector CVertex__game_point				(const CVertex *vertex)
{
	THROW				(vertex);
	return				(vertex->game_point());
}

void CGameGraph::script_register		(lua_State *L)
{
	module(L)
	[
		def("game_graph",	&get_game_graph),

		class_<CGameGraph>("CGameGraph")
			.def("accessible",		&get_accessible1)
			.def("accessible",		&get_accessible2)
			.def("valid_vertex_id",	&CGameGraph::valid_vertex_id)
			.def("vertex",			&CGameGraph::vertex)
			.def("vertex_id",		&CGameGraph::vertex_id),

		class_<CVertex>("GameGraph__CVertex")
			.def("level_point",		&CVertex__level_point)
			.def("game_point",		&CVertex__game_point)
			.def("level_id",		&CVertex::level_id)
			.def("level_vertex_id",	&CVertex::level_vertex_id)
	];
}
