////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory_script.cpp
//	Created 	: 27.05.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_factory.h"
#include "ai_space.h"
#include "script_engine.h"
#include "script_space.h"
#include "object_item_script.h"
#include <luabind/adopt_policy.hpp>

void CObjectFactory::register_script_class	(LPCSTR client_class, LPCSTR server_class, LPCSTR clsid, LPCSTR script_clsid)
{
	
	add							(
		xr_new<CObjectItemScript>(
#ifndef NO_XR_GAME
			ai().script_engine().name_space(client_class),
#endif
			ai().script_engine().name_space(server_class),
			TEXT2CLSID(clsid),
			script_clsid
		)
	);
}

void CObjectFactory::register_script_class			(LPCSTR unknwon_class, LPCSTR clsid, LPCSTR script_clsid)
{
	luabind::object				creator = ai().script_engine().name_space(unknwon_class);
	add							(
		xr_new<CObjectItemScript>(
#ifndef NO_XR_GAME
			creator,
#endif
			creator,
			TEXT2CLSID(clsid),
			script_clsid
		)
	);
}

void CObjectFactory::register_script_classes()
{
	ai().script_engine().register_script_classes();
}

using namespace luabind;

struct CInternal{};

void CObjectFactory::register_script	() const
{
	luabind::class_<CInternal>	instance("clsid");

	const_iterator				I = clsids().begin(), B = I;
	const_iterator				E = clsids().end();
	for ( ; I != E; ++I)
		instance.enum_			("_clsid")[luabind::value(*(*I)->script_clsid(),int(I - B))];

	luabind::module				(ai().script_engine().lua())[instance];
}

void CObjectFactory::script_register(lua_State *L)
{
	module(L)
	[
		class_<CObjectFactory>("object_factory")
			.def("register",	(void (CObjectFactory::*)(LPCSTR,LPCSTR,LPCSTR,LPCSTR))(CObjectFactory::register_script_class))
			.def("register",	(void (CObjectFactory::*)(LPCSTR,LPCSTR,LPCSTR))(CObjectFactory::register_script_class))
	];
}
