////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine_script.cpp
//	Created 	: 25.12.2002
//  Modified 	: 13.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator script engine export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_engine.h"
#include "script_space.h"
#include "ai_space.h"

using namespace luabind;

void LuaLog(LPCSTR caMessage)
{
	ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeMessage,"%s",caMessage);
}

void LoadScriptModule(LPCSTR script_name)
{
	ai().script_engine().add_file(script_name);
}

void FlushLogs()
{
#ifdef DEBUG
	FlushLog();
	ai().script_engine().flush_log();
#endif
}

void verify_if_thread_is_running()
{
	if (!ai().script_engine().current_thread()) {
		ai().script_engine().script_stack_tracker().print_stack(ai().script_engine().lua());
		VERIFY2		(ai().script_engine().current_thread(),"coroutine.yield() is called outside the LUA thread!");
	}
}

bool editor()
{
#ifdef XRGAME_EXPORTS
	return		(false);
#else
	return		(true);
#endif
}

void CScriptEngine::script_register(lua_State *L)
{
	function	(L,	"log",							LuaLog);
	function	(L,	"flush",						FlushLogs);
	function	(L,	"module",						LoadScriptModule);
	function	(L,	"verify_if_thread_is_running",	verify_if_thread_is_running);
	function	(L,	"editor",						editor);
}
