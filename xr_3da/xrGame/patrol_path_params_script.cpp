////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_params_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol path parameters class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "patrol_path_params.h"
#include "script_space.h"

using namespace luabind;

void CPatrolPathParams::script_register(lua_State *L)
{
	module(L)
	[
		class_<CPatrolPathParams>("patrol")
			.enum_("start")
			[
				value("start",					int(CPatrolPathManager::ePatrolStartTypeFirst)),
				value("stop",					int(CPatrolPathManager::ePatrolStartTypeLast)),
				value("nearest",				int(CPatrolPathManager::ePatrolStartTypeNearest)),
				value("custom",					int(CPatrolPathManager::ePatrolStartTypePoint)),
				value("next",					int(CPatrolPathManager::ePatrolStartTypeNext)),
				value("dummy",					int(CPatrolPathManager::ePatrolStartTypeDummy))
			]
			.enum_("stop")
			[
				value("stop",					int(CPatrolPathManager::ePatrolRouteTypeStop)),
				value("continue",				int(CPatrolPathManager::ePatrolRouteTypeContinue)),
				value("dummy",					int(CPatrolPathManager::ePatrolRouteTypeDummy))
			]
			.def(								constructor<LPCSTR>())
			.def(								constructor<LPCSTR,const CPatrolPathManager::EPatrolStartType>())
			.def(								constructor<LPCSTR,const CPatrolPathManager::EPatrolStartType,const CPatrolPathManager::EPatrolRouteType>())
			.def(								constructor<LPCSTR,const CPatrolPathManager::EPatrolStartType,const CPatrolPathManager::EPatrolRouteType, bool>())
			.def(								constructor<LPCSTR,const CPatrolPathManager::EPatrolStartType,const CPatrolPathManager::EPatrolRouteType, bool, u32>())
			.def("count",						&CPatrolPathParams::count)
			.def("level_vertex_id",				&CPatrolPathParams::level_vertex_id)
			.def("point",						(const Fvector &(CPatrolPathParams::*)(u32)				const)	(CPatrolPathParams::point))
			.def("index",						(u32			(CPatrolPathParams::*)(LPCSTR)			const)	(CPatrolPathParams::point))
			.def("get_nearest",					(u32			(CPatrolPathParams::*)(const Fvector &) const)	(CPatrolPathParams::point))
			.def("flag",						&CPatrolPathParams::flag)
	];
}
