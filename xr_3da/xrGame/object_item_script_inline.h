////////////////////////////////////////////////////////////////////////////
//	Module 		: object_item_script_inline.h
//	Created 	: 27.05.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object item script class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CObjectItemScript::CObjectItemScript	(
#ifndef NO_XR_GAME
	luabind::functor<void>		client_creator, 
#endif
	luabind::functor<void>		server_creator, 
	const CLASS_ID				&clsid, 
	LPCSTR						script_clsid
) : 
#ifndef NO_XR_GAME
	CObjectItemScriptClient		(client_creator,clsid,script_clsid),
#endif
	CObjectItemScriptServer		(server_creator,clsid,script_clsid),
	CObjectItemAbstract			(clsid,script_clsid)
{
}

#ifndef NO_XR_GAME
ObjectFactory::CLIENT_BASE_CLASS *CObjectItemScript::client_object	() const
{
	return			(CObjectItemScriptClient::client_object());
}
#endif

ObjectFactory::SERVER_BASE_CLASS *CObjectItemScript::server_object	(LPCSTR section) const
{
	return			(CObjectItemScriptServer::server_object(section));
}
