//////////////////////////////////////////////////////////////////////
// inventory_owner_scripts.h:	функции для работы со скриптами	
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InventoryOwner.h"

void	CInventoryOwner::zero_callbacks		()
{
	m_pPdaCallback.m_lua_object = NULL;
	m_pPdaCallback.m_lua_function = NULL;
	m_pInfoCallback.m_lua_object = NULL;
	m_pInfoCallback.m_lua_function = NULL;
}
void	CInventoryOwner::reset_callbacks		()
{
	xr_delete(m_pPdaCallback.m_lua_object);
	xr_delete(m_pPdaCallback.m_lua_function);
	xr_delete(m_pInfoCallback.m_lua_object);
	xr_delete(m_pInfoCallback.m_lua_function);
}

void CInventoryOwner::set_pda_callback	(const luabind::object &lua_object, LPCSTR method)
{
	xr_delete					(m_pPdaCallback.m_lua_object);
	m_pPdaCallback.m_lua_object		= xr_new<luabind::object>(lua_object);
	m_pPdaCallback.m_method_name		= method;
}
void CInventoryOwner::set_pda_callback	(const luabind::functor<void> &lua_function)
{
	xr_delete					(m_pInfoCallback.m_lua_function);
	m_pPdaCallback.m_lua_function		= xr_new<luabind::functor<void> >(lua_function);
}

void CInventoryOwner::set_info_callback	(const luabind::object &lua_object, LPCSTR method)
{
	xr_delete							(m_pInfoCallback.m_lua_object);
	m_pInfoCallback.m_lua_object		= xr_new<luabind::object>(lua_object);
	m_pInfoCallback.m_method_name		= method;
}
void CInventoryOwner::set_info_callback	(const luabind::functor<void> &lua_function)
{
	xr_delete					(m_pInfoCallback.m_lua_function);
	m_pInfoCallback.m_lua_function		= xr_new<luabind::functor<void> >(lua_function);
}
void CInventoryOwner::clear_pda_callback ()
{
	xr_delete					(m_pPdaCallback.m_lua_function);
	xr_delete					(m_pPdaCallback.m_lua_object);
}

void CInventoryOwner::clear_info_callback ()
{
	xr_delete					(m_pInfoCallback.m_lua_function);
	xr_delete					(m_pInfoCallback.m_lua_object);
}