////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory_script.cpp
//	Created 	: 27.05.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_factory.h"
#include "script_space.h"
#include <luabind/adopt_policy.hpp>

using namespace luabind;

void CObjectFactory::script_register(lua_State *L)
{
	module(L)
	[
		class_<CObjectFactory>("object_factory")
			.def("register",		&CObjectFactory::register_script_class)
			.def("set_instance",	(void (CObjectFactory::*)(CObjectFactory::CLIENT_SCRIPT_BASE_CLASS*) const)(CObjectFactory::set_instance), adopt(_2))
			.def("set_instance",	(void (CObjectFactory::*)(CObjectFactory::SERVER_SCRIPT_BASE_CLASS*) const)(CObjectFactory::set_instance), adopt(_2))
	];
}