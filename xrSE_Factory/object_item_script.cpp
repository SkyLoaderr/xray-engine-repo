////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_script.cpp
//	Created 	: 27.05.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item script class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "object_item_script.h"
#include "object_factory.h"
#include <luabind/adopt_policy.hpp>

#ifndef NO_XR_GAME
#	include "attachable_item.h"

ObjectFactory::CLIENT_BASE_CLASS *CObjectItemScript::client_object	() const
{
	ObjectFactory::CLIENT_SCRIPT_BASE_CLASS	*object = luabind::object_cast<ObjectFactory::CLIENT_SCRIPT_BASE_CLASS*>(m_client_creator(),luabind::adopt(luabind::result));
	R_ASSERT								(object);
	return									(object);
}

#endif

ObjectFactory::SERVER_BASE_CLASS *CObjectItemScript::server_object	(LPCSTR section) const
{
	ObjectFactory::SERVER_SCRIPT_BASE_CLASS	*object = luabind::object_cast<ObjectFactory::SERVER_SCRIPT_BASE_CLASS*>(m_server_creator(section),luabind::adopt(luabind::result));
	R_ASSERT								(object);
	return									(object);
}

CObjectItemScript::CObjectItemScript	(
#ifndef NO_XR_GAME
	luabind::object		client_creator, 
#endif
	luabind::object		server_creator, 
	const CLASS_ID		&clsid, 
	LPCSTR				script_clsid
) : 
	inherited								(clsid,script_clsid)
{
#ifndef NO_XR_GAME
	m_client_creator						= client_creator;
#endif
	m_server_creator						= server_creator;
}

#ifndef NO_XR_GAME

CObjectItemScript::CObjectItemScript	(
	luabind::object		unknown_creator, 
	const CLASS_ID		&clsid, 
	LPCSTR				script_clsid
) : 
	inherited								(clsid,script_clsid)
{
	m_client_creator = m_server_creator		= unknown_creator;
}

#endif
