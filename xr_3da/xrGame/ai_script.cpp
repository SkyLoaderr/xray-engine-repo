////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Scripting system with Lua as a scripting language usage
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "script_engine.h"
#include "ai_script.h"

//void print_stack(lua_State *L)
//{
//	Msg					("Lua stack");
//	for (int i=0; lua_type(L, -i-1); i++)
//		Msg				("%2d : %s",-i-1,lua_typename(L, lua_type(L, -i-1)));
//}
//
CScript::CScript(LPCSTR caNamespaceName)
{
	string256			S;
	m_bActive			= false;
	m_script_name		= xr_strdup(caNamespaceName);
	Msg					("* Loading design script %s",caNamespaceName);

	ai().script_engine().add_file(caNamespaceName);
	ai().script_engine().process();

	m_tpLuaThread		= lua_newthread(ai().script_engine().lua());
	
	sprintf				(S,"\n\n%s.main()\n",caNamespaceName);
	if (!luaL_loadbuffer(m_tpLuaThread,S,xr_strlen(S) + 1,"@internal.script")) {
		if (!ai().script_engine().print_output(m_tpLuaThread,m_script_name,1))
			ai().script_engine().print_error(m_tpLuaThread,1);
		return;
	}

#ifdef DEBUG
	lua_sethook			(m_tpLuaThread, CScriptEngine::lua_hook_call,	LUA_HOOKCALL | LUA_HOOKRET | LUA_HOOKLINE | LUA_HOOKTAILRET,	0);
#endif

	m_bActive			= true;
}

CScript::~CScript()
{
	xr_delete			(m_script_name);
}

bool CScript::Update()
{
	Msg					("Updating script %s",m_script_name);
	if (!m_bActive)
		R_ASSERT2		(false,"Cannot resume dead Lua thread!");
	ai().script_engine().set_current_thread	(m_script_name);
	int					l_iErrorCode = lua_resume(m_tpLuaThread,0);
	ai().script_engine().set_current_thread	("");
	if (l_iErrorCode) {
#ifdef DEBUG
		if (!ai().script_engine().print_output(m_tpLuaThread,m_script_name,l_iErrorCode))
			ai().script_engine().print_error(m_tpLuaThread,l_iErrorCode);
		m_bActive		= false;
		return			(false);
#endif
	}
	return				(true);
}