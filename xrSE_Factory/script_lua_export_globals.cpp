////////////////////////////////////////////////////////////////////////////
//	Module 		: script_lua_export_globals.cpp
//	Created 	: 19.09.2003
//  Modified 	: 22.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script export globals
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "ai_script_classes.h"
#include "ai_script_actions.h"
#include "ai_script_sound.h"
#include "ai_script_hit.h"
#include "ai_script_snd_info.h"
#include "ai_script_monster_hit_info.h"
#include "luabind/return_reference_to_policy.hpp"
#include "luabind/out_value_policy.hpp"
#include "luabind/adopt_policy.hpp"
#include "script_engine.h"

using namespace luabind;

extern CLuaGameObject	*tpfGetActor();

void LuaLog(LPCSTR caMessage)
{
	ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeMessage,"%s",caMessage);
}

double get_time()
{
	return((double)Level().GetGameTime());
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

void CScriptEngine::export_globals()
{
	function	(lua(),	"log",							LuaLog);
	function	(lua(),	"flush",						FlushLogs);
	function	(lua(),	"module",						LoadScriptModule);
	function	(lua(),	"verify_if_thread_is_running",	verify_if_thread_is_running);
	function	(lua(),	"editor",						editor);
}
