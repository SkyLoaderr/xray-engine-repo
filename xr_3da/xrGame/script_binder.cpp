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
#include "xrServer_Objects_ALife.h"
#include "script_binder_object.h"

CScriptBinder::CScriptBinder		()
{
	init					();
}

CScriptBinder::~CScriptBinder		()
{
	VERIFY					(!m_object);
}

void CScriptBinder::init			()
{
	m_object				= 0;
}

void CScriptBinder::reinit			()
{
	inherited::reinit		();
	if (m_object)
		m_object->reinit	();
}

void CScriptBinder::Load			(LPCSTR section)
{
	inherited::Load			(section);
}

void CScriptBinder::reload			(LPCSTR section)
{
	inherited::reload		(section);
	VERIFY					(!m_object);
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
	strcpy					(name_space,"_G");
	if (!J)
		strcpy				(function,string_to_run);
	else {
		Memory.mem_copy		(name_space,string_to_run, u32(J - string_to_run)*sizeof(char));
		name_space[u32(J - string_to_run)] = 0;
		strcpy				(function,J + 1);
	}

	if	(!Script::bfIsObjectPresent(ai().lua(),name_space,function,LUA_TFUNCTION)) {
		Lua::LuaOut			(Lua::eLuaMessageTypeError,"function %s is not loaded!",string_to_run);
		return;
	}

	luabind::object			lua_namespace	= luabind::get_globals(ai().lua())[name_space];
	luabind::functor<void>	lua_function	= luabind::object_cast<luabind::functor<void> >(lua_namespace[function]);
	lua_function			(lua_game_object());
	
	if (m_object)
		m_object->reload	(section);
}

BOOL CScriptBinder::net_Spawn		(LPVOID DC)
{
	if (!inherited::net_Spawn(DC))
		return				(FALSE);

//	CSE_Abstract			*abstract = (CSE_Abstract*)DC;
	if (m_object && !m_object->net_Spawn(0))//DC))
		return				(FALSE);

	return					(TRUE);
}

void CScriptBinder::net_Destroy		()
{
	inherited::net_Destroy	();
	if (m_object)
		m_object->net_Destroy	();
	xr_delete				(m_object);
}

void CScriptBinder::net_Import		(NET_Packet &net_packet)
{
	if (m_object)
		m_object->net_Import(*(int*)((void*)&net_packet));
}

void CScriptBinder::net_Export		(NET_Packet &net_packet)
{
	if (m_object)
		m_object->net_Export(*(int*)((void*)&net_packet));
}

void CScriptBinder::set_object		(CScriptBinderObject *object)
{
	m_object				= object;
}
