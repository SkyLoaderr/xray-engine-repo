////////////////////////////////////////////////////////////////////////////
//	Module 		: cover_point_script.cpp
//	Created 	: 24.03.2004
//  Modified 	: 24.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Cover point class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cover_point.h"
#include "script_space.h"

using namespace luabind;

void CCoverPoint::script_register(lua_State *L)
{
	module(L)
	[
		class_<CCoverPoint>("cover_point")
			.def("position",			&CCoverPoint::position)
			.def("level_vertex_id",		&CCoverPoint::level_vertex_id)
	];
}
