////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder.cpp
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script objects binder
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_binder.h"
#include "ai_script_lua_extension.h"

CScriptBinder::CScriptBinder		()
{
	init					();
}

CScriptBinder::~CScriptBinder		()
{
	VERIFY					(!m_lua_instance);
}

void CScriptBinder::init			()
{
	m_lua_instance			= 0;
}

void CScriptBinder::reinit			()
{
	inherited::reinit		();
	VERIFY					(!m_lua_instance);
}

void CScriptBinder::Load			(LPCSTR section)
{
	inherited::Load			(section);
}

void CScriptBinder::reload			(LPCSTR section)
{
	inherited::reload		(section);
	if (!pSettings->line_exist(section,"script_binding"))
		return;
	
	LPCSTR					string_to_run = pSettings->r_string(section,"script_binding");
	if (!xr_strlen(string_to_run))
		return;

	string256				name_space, function;
	LPCSTR					I = string_to_run, J = 0;
	for ( ; ; J=I,++I) {
		I					= strchr(I,'.');
		if (!I)
			break;
	}
	strcpy					(name_space,"");
	if (!J)
		strcpy				(function,string_to_run);
	else {
		Memory.mem_copy		(name_space,string_to_run, u32(J - string_to_run)*sizeof(char));
		name_space[u32(J - string_to_run)] = 0;
		strcpy				(function,J + 1);
	}

	if (Script::bfIsObjectPresent(ai().lua(),name_space,function,LUA_TFUNCTION))
		lua_dostring		(ai().lua(),strconcat(name_space,string_to_run,"()"));
	else
		Lua::LuaOut			(Lua::eLuaMessageTypeError,"function %s is not loaded!",string_to_run);
}

BOOL CScriptBinder::net_Spawn		(LPVOID DC)
{
	if (!inherited::net_Spawn(DC))
		return				(FALSE);

	return					(TRUE);
}

void CScriptBinder::net_Destroy		()
{
	inherited::net_Destroy	();
	xr_delete				(m_lua_instance);
}

void CScriptBinder::net_Import		(NET_Packet &net_packet)
{
}

void CScriptBinder::net_Export		(NET_Packet &net_packet)
{
}

void CScriptBinder::set_lua_object	(const luabind::object &object)
{
	m_lua_instance			= xr_new<luabind::object>(object);
}
