////////////////////////////////////////////////////////////////////////////
//	Module 		: script_thread.cpp
//	Created 	: 19.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script thread class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "lua.h"
#include "lstate.h"
#include "script_engine.h"
#include "script_thread.h"
#include "ai_space.h"

#ifdef USE_DEBUGGER
#	include "script_debugger.h"
#endif

CScriptThread::CScriptThread(LPCSTR caNamespaceName)
{
	string256			S;
	m_bActive			= false;
	m_script_name		= xr_strdup(caNamespaceName);
	Msg					("* Loading design script %s",caNamespaceName);

	ai().script_engine().add_file(caNamespaceName);
	ai().script_engine().process();

	m_virtual_machine	= lua_newthread(ai().script_engine().lua());
	VERIFY2				(lua(),"Cannot create new Lua thread");
	m_thread_reference	= luaL_ref(ai().script_engine().lua(),LUA_REGISTRYINDEX);
	
#ifdef USE_DEBUGGER
	if( !CScriptDebugger::GetDebugger()->Active() )
#endif
		lua_sethook		(lua(),CScriptEngine::lua_hook_call,	LUA_HOOKCALL | LUA_HOOKRET | LUA_HOOKLINE | LUA_HOOKTAILRET,	0);
#ifdef USE_DEBUGGER
	else
		lua_sethook		(lua(), CDbgLuaHelper::hookLua,			LUA_MASKLINE|LUA_MASKCALL|LUA_MASKRET, 0);
#endif

	sprintf				(S,"%s.main()",caNamespaceName);
	if (!ai().script_engine().load_buffer(lua(),S,xr_strlen(S),"@_thread_main"))
		return;

	m_bActive			= true;
}

CScriptThread::~CScriptThread()
{
	luaL_unref				(ai().script_engine().lua(),LUA_REGISTRYINDEX,m_thread_reference);
	xr_delete				(m_script_name);
}

bool CScriptThread::Update()
{
	if (!m_bActive)
		R_ASSERT2		(false,"Cannot resume dead Lua thread!");
	
	ai().script_engine().set_current_thread	(this);
	
	int					l_iErrorCode = lua_resume(lua(),0);
	
	if (l_iErrorCode) {
		if (!ai().script_engine().print_output(lua(),m_script_name,l_iErrorCode))
			ai().script_engine().print_error(lua(),l_iErrorCode);
		m_bActive		= false;
	}
	else
		if (!(lua()->ci->state & CI_YIELD)) {
			m_bActive	= false;
			ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeInfo,"Script %s is finished!",m_script_name);
		}
		else {
#ifdef USE_DEBUGGER
			if( !CScriptDebugger::GetDebugger()->Active() ) 
#endif
			{
				VERIFY		(m_current_stack_level);
				--m_current_stack_level;
			}
			VERIFY2		(!lua_gettop(lua()),"Do not pass any value to coroutine.yield()!");
		}
	
	ai().script_engine().set_current_thread	(0);
	
	return				(m_bActive);
}
