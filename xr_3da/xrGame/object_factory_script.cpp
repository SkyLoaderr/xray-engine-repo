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
			ai().script_engine().create_object_creator<void>(client_class,""),
#endif
			ai().script_engine().create_object_creator<void>(server_class,"section"),
			TEXT2CLSID(clsid),
			script_clsid
		)
	);
}

void CObjectFactory::register_script_class			(LPCSTR unknwon_class, LPCSTR clsid, LPCSTR script_clsid)
{
	luabind::functor<void>		creator = ai().script_engine().create_object_creator<void>(unknwon_class,"");
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

#ifndef NO_XR_GAME
IC	void CObjectFactory::set_instance	(CLIENT_SCRIPT_BASE_CLASS *instance) const
{
	m_client_instance			= instance;
}
#endif

IC	void CObjectFactory::set_instance	(SERVER_SCRIPT_BASE_CLASS *instance) const
{
	m_server_instance			= instance;
}

void CObjectFactory::register_script_classes()
{
	ai().script_engine().register_script_classes();
#ifndef NO_XR_GAME
	set_instance	((CLIENT_SCRIPT_BASE_CLASS*)0);
#endif
	set_instance	((SERVER_SCRIPT_BASE_CLASS*)0);
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
			.def("register",		(void (CObjectFactory::*)(LPCSTR,LPCSTR,LPCSTR,LPCSTR))(CObjectFactory::register_script_class))
			.def("register",		(void (CObjectFactory::*)(LPCSTR,LPCSTR,LPCSTR))(CObjectFactory::register_script_class))
#ifndef NO_XR_GAME
			.def("set_instance",	(void (CObjectFactory::*)(CObjectFactory::CLIENT_SCRIPT_BASE_CLASS*) const)(CObjectFactory::set_instance), adopt(_2))
#endif
			.def("set_instance",	(void (CObjectFactory::*)(CObjectFactory::SERVER_SCRIPT_BASE_CLASS*) const)(CObjectFactory::set_instance), adopt(_2))
	];
}
