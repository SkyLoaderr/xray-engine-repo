////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_script.cpp
//	Created 	: 27.05.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item script class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_item_script.h"
#include "object_factory.h"

#ifndef NO_XR_GAME
#	include "attachable_item.h"

ObjectFactory::CLIENT_BASE_CLASS *CObjectItemScript::client_object	() const
{
	object_factory().set_instance((ObjectFactory::CLIENT_SCRIPT_BASE_CLASS*)0);
	m_client_creator			(const_cast<CObjectFactory*>(&object_factory()));
	R_ASSERT					(object_factory().client_instance());
	return						(object_factory().client_instance());
}

#endif

ObjectFactory::SERVER_BASE_CLASS *CObjectItemScript::server_object	(LPCSTR section) const
{
	object_factory().set_instance((ObjectFactory::SERVER_SCRIPT_BASE_CLASS*)0);
	m_server_creator			(const_cast<CObjectFactory*>(&object_factory()),section);
	R_ASSERT					(object_factory().server_instance());
	return						(object_factory().server_instance());
}
