////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory.cpp
//	Created 	: 27.05.2004
//  Modified 	: 27.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "object_factory.h"
#include "object_broker.h"

#ifndef NO_XR_GAME
#	include "script_space.h"
#	include "ai_space.h"
#	include "script_engine.h"

#	include "xrServer_Objects_ALife_All.h"
#	include "attachable_item.h"
#endif

CObjectFactory::CObjectFactory			()
{
	m_initialized				= false;
	register_classes			();
#ifndef NO_XR_GAME
	ai().script_engine().register_script_classes();
	set_instance				((CLIENT_SCRIPT_BASE_CLASS*)0);
	set_instance				((SERVER_SCRIPT_BASE_CLASS*)0);
#endif
	std::sort					(m_clsids.begin(),m_clsids.end(),CObjectItemPredicate());
	m_initialized				= true;
}

CObjectFactory::~CObjectFactory			()
{
	delete_data					(m_clsids);
}

#ifndef NO_XR_GAME
void CObjectFactory::register_script	() const
{
	luabind::class_<CInternal>	instance("clsid");
	
	const_iterator				I = clsids().begin(), B = I;
	const_iterator				E = clsids().end();
	for ( ; I != E; ++I)
		instance.enum_			("_clsid")[luabind::value(*(*I)->script_clsid(),int(I - B))];
	
	luabind::module				(ai().script_engine().lua())[instance];
}

IC	CObjectFactory::CObjectItemScript::CObjectItemScript	(
	luabind::functor<void>		client_creator, 
	luabind::functor<void>		server_creator, 
	const CLASS_ID				&clsid, 
	LPCSTR						script_clsid
) : 
	inherited					(clsid,script_clsid)
{
	m_client_creator			= client_creator;
	m_server_creator			= server_creator;
}

CObjectFactory::CLIENT_BASE_CLASS	*CObjectFactory::CObjectItemScript::client_object	() const
{
	object_factory().set_instance((CLIENT_SCRIPT_BASE_CLASS*)0);
	m_client_creator			(const_cast<CObjectFactory*>(&object_factory()));
	return						(object_factory().client_instance());
}

CObjectFactory::SERVER_BASE_CLASS	*CObjectFactory::CObjectItemScript::server_object	(LPCSTR section) const
{
	object_factory().set_instance((SERVER_SCRIPT_BASE_CLASS*)0);
	m_server_creator			(const_cast<CObjectFactory*>(&object_factory()),section);
	return						(object_factory().server_instance());
}

void CObjectFactory::register_script_class	(LPCSTR client_class, LPCSTR server_class, LPCSTR clsid, LPCSTR script_clsid)
{
	
	add							(
		xr_new<CObjectItemScript>(
			ai().script_engine().create_object_creator<void>(client_class,""),
			ai().script_engine().create_object_creator<void>(server_class,"section"),
			TEXT2CLSID(clsid),
			script_clsid
		)
	);
}

void CObjectFactory::set_instance	(CLIENT_SCRIPT_BASE_CLASS *instance) const
{
	m_client_instance			= instance;
}

void CObjectFactory::set_instance	(SERVER_SCRIPT_BASE_CLASS *instance) const
{
	m_server_instance			= instance;
}

CObjectFactory::CLIENT_SCRIPT_BASE_CLASS *CObjectFactory::client_instance() const
{
	return						(m_client_instance);
}

CObjectFactory::SERVER_SCRIPT_BASE_CLASS *CObjectFactory::server_instance() const
{
	return						(m_server_instance);
}
#endif
