////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine.cpp
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_engine.h"
#include "ai_script_processor.h"
#include "ai_space.h"
#include "object_factory.h"

CScriptEngine::CScriptEngine	()
{
//	lua_setgcthreshold		(lua(),64*1024);
	m_current_thread		= 0;
	m_stack_level			= 0;
	m_reload_modules		= false;
}

CScriptEngine::~CScriptEngine			()
{
	while (!m_script_processors.empty())
		remove_script_processor(m_script_processors.begin()->first);
	flush_log				();
}

void CScriptEngine::unload				()
{
	lua_settop				(lua(),m_stack_level);
}

int CScriptEngine::lua_panic(CLuaVirtualMachine *L)
{
	script_log		(eLuaMessageTypeError,"PANIC");
	if (!print_output(L,"unknown script"))
		print_error(L,LUA_ERRRUN);
	return			(0);
}

void CScriptEngine::lua_hook_call(CLuaVirtualMachine *L, lua_Debug *tpLuaDebug)
{
	ai().script_engine().script_stack_tracker().script_hook(L,tpLuaDebug);
}

void CScriptEngine::lua_error(CLuaVirtualMachine *L)
{
	print_error				(L,LUA_ERRRUN);
	Debug.fatal				("LUA error: %s",lua_tostring(L,-1));
}

void CScriptEngine::lua_cast_failed(CLuaVirtualMachine *L, LUABIND_TYPE_INFO info)
{
//	print_output			(L,ai().script_engine().current_thread(),0);
	print_error				(L,LUA_ERRRUN);
	Debug.fatal				("LUA error: cannot cast lua value to %s",info->name());
}

void CScriptEngine::export()
{
	luabind::open						(lua());
	
	luabind::set_error_callback			(CScriptEngine::lua_error);
	luabind::set_cast_failed_callback	(CScriptEngine::lua_cast_failed);
	lua_atpanic							(lua(),CScriptEngine::lua_panic);

	export_globals						();
	export_fvector						();
	export_fmatrix						();
	export_game							();
	export_level						();
	export_device						();
	export_particles					();
	export_sound						();
	export_hit							();
	export_actions						();
	export_ini							();
	export_object						();
	export_effector						();
	export_artifact_merger				();
	export_memory_objects				();
	export_alife						();
	export_action_management			();
	export_motivation_management		();
	export_monster_info					();

	object_factory().register_script	();

#ifdef DEBUG
	lua_sethook							(lua(),CScriptEngine::lua_hook_call,	LUA_HOOKCALL | LUA_HOOKRET | LUA_HOOKLINE | LUA_HOOKTAILRET,	0);
#endif

	load_common_scripts					();
	m_stack_level						= lua_gettop(lua());
}

bool CScriptEngine::load_file(LPCSTR caScriptName, bool bCall)
{
	VERIFY			(bCall);
	string256		l_caNamespaceName;
	_splitpath		(caScriptName,0,0,l_caNamespaceName,0);
	if (!xr_strlen(l_caNamespaceName))
		return		(load_file_into_namespace(caScriptName,"_G",bCall));
	else
		return		(load_file_into_namespace(caScriptName,l_caNamespaceName,bCall));
}

void CScriptEngine::remove_script_processor	(LPCSTR processor_name)
{
	CScriptProcessorStorage::iterator	I = m_script_processors.find(processor_name);
	if (I != m_script_processors.end()) {
		xr_delete						((*I).second);
		m_script_processors.erase		(I);
	}
}

void CScriptEngine::add_file			(LPCSTR file_name)
{
	m_load_queue.push_back	(xr_strdup(file_name));
}

void CScriptEngine::load_common_scripts()
{
	string256		S;
	FS.update_path	(S,"$game_data$","script.ltx");
	CInifile		*l_tpIniFile = xr_new<CInifile>(S);
	R_ASSERT		(l_tpIniFile);
	LPCSTR			caScriptString = l_tpIniFile->r_string("common","script");

	u32				caNamespaceName = _GetItemCount(caScriptString);
	string256		I;
	for (u32 i=0; i<caNamespaceName; ++i) {
		add_file	(_GetItem(caScriptString,i,I));
		process		();
		if (object("_G",strcat(I,"_initialize"),LUA_TFUNCTION))
			lua_dostring(lua(),strcat(I,"()"));
	}
	xr_delete		(l_tpIniFile);
}

void CScriptEngine::process	()
{
	string256					S,S1;
	for (u32 i=0, n=m_load_queue.size(); !m_load_queue.empty(); ++i) {
		LPSTR					S2 = m_load_queue.front();
		m_load_queue.pop_front	();
		if (!xr_strlen(S2) || !xr_strcmp(S2,"_G") || (m_reload_modules && i<n) || !namespace_loaded(S2)) {
			FS.update_path		(S,"$game_scripts$",strconcat(S1,S2,".script"));
			Msg					("* loading script %s",S1);
			load_file			(S,true);
		}
		xr_free					(S2);
	}
	m_reload_modules			= false;
}
