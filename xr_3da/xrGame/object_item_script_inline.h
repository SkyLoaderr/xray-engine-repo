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
	inherited					(clsid,script_clsid)
{
#ifndef NO_XR_GAME
	m_client_creator			= client_creator;
#endif
	m_server_creator			= server_creator;
}

#ifndef NO_XR_GAME

IC	CObjectItemScript::CObjectItemScript	(
	luabind::functor<void>		unknown_creator, 
	const CLASS_ID				&clsid, 
	LPCSTR						script_clsid
) : 
	inherited					(clsid,script_clsid)
{
	m_client_creator = m_server_creator = unknown_creator;
}

#endif
