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

#define LUABIND_HAS_BUGS_WITH_LUA_THREADS

#ifdef USE_DEBUGGER
#	include "script_debugger.h"
#endif

CScriptThread::CScriptThread(LPCSTR caNamespaceName)
{
	try {
		string256			S;
		m_active			= false;
		m_script_name		= caNamespaceName;

		ai().script_engine().add_file(caNamespaceName);
		ai().script_engine().process();

		m_virtual_machine	= lua_newthread(ai().script_engine().lua());
		VERIFY2				(lua(),"Cannot create new Lua thread");
		m_thread_reference	= luaL_ref(ai().script_engine().lua(),LUA_REGISTRYINDEX);
		
#ifdef DEBUG
#	ifdef USE_DEBUGGER
		if(ai().script_engine().debugger() && ai().script_engine().debugger()->Active())
			lua_sethook		(lua(), CDbgLuaHelper::hookLua,			LUA_MASKLINE|LUA_MASKCALL|LUA_MASKRET, 0);
#	endif
#endif

		sprintf				(S,"%s.main()",caNamespaceName);
		if (!ai().script_engine().load_buffer(lua(),S,xr_strlen(S),"@_thread_main"))
			return;

		m_active			= true;
	}
	catch(...) {
		m_active			= false;
	}
}

CScriptThread::~CScriptThread()
{
#ifdef DEBUG
	Msg						("* Destroying script thread %s",m_script_name);
#endif
	try {
#ifndef LUABIND_HAS_BUGS_WITH_LUA_THREADS
		luaL_unref			(ai().script_engine().lua(),LUA_REGISTRYINDEX,m_thread_reference);
#endif
	}
	catch(...) {
	}
}

bool CScriptThread::Update()
{
	if (!m_active)
		R_ASSERT2		(false,"Cannot resume dead Lua thread!");
	
	try {
		ai().script_engine().current_thread	(this);
		
		int					l_iErrorCode = lua_resume(lua(),0);
		
		if (l_iErrorCode) {
			ai().script_engine().print_output(lua(),*script_name(),l_iErrorCode);
			m_active		= false;
		}
		else
			if (!(lua()->ci->state & CI_YIELD)) {
				m_active	= false;
				ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeInfo,"Script %s is finished!",m_script_name);
			}
			else {
				VERIFY2		(!lua_gettop(lua()),"Do not pass any value to coroutine.yield()!");
			}
		
		ai().script_engine().current_thread	(0);
	}
	catch(...) {
		m_active		= false;
	}
	return				(m_active);
}
