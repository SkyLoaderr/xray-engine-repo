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

CScriptEngine::CScriptEngine	()
{
	lua_setgcthreshold		(m_virtual_machine,64);
}

CScriptEngine::~CScriptEngine			()
{
	while (!m_script_processors.empty())
		remove_script_processor(m_script_processors.begin()->first);
	flush_log				();
}

void CScriptEngine::unload				()
{
	lua_settop				(m_virtual_machine,0);
}

void CScriptEngine::lua_error(CLuaVirtualMachine *L)
{
	Debug.fatal				("LUA error: %s",lua_tostring(L,-1));
}

void CScriptEngine::lua_cast_failed(CLuaVirtualMachine *L, LUABIND_TYPE_INFO info)
{
	Debug.fatal				("LUA error: cannot cast lua value to %s",info->name());
}

void CScriptEngine::export()
{
	luabind::open				(lua());
	
	luabind::set_error_callback			(CScriptEngine::lua_error);
	luabind::set_cast_failed_callback	(CScriptEngine::lua_cast_failed);
	lua_atpanic							(lua(),CScriptEngine::lua_panic);

	export_globals				();
	export_fvector				();
	export_fmatrix				();
	export_game					();
	export_level				();
	export_device				();
	export_particles			();
	export_sound				();
	export_hit					();
	export_actions				();
	export_object				();
	export_effector				();
	export_artifact_merger		();
	export_memory_objects		();
	export_action_management	();
	export_motivation_management();

#ifdef DEBUG
	lua_sethook					(lua(),CScriptEngine::lua_hook_call,	LUA_HOOKCALL | LUA_HOOKRET | LUA_HOOKLINE | LUA_HOOKTAILRET,	0);
#endif

	load_common_scripts			();
}

bool CScriptEngine::load_file(LPCSTR caScriptName, bool bCall)
{
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
	while (!m_load_queue.empty()) {
		LPSTR					S2 = m_load_queue.front();
		m_load_queue.pop_front	();
		if (!xr_strlen(S2) || !xr_strcmp(S2,"_G") || !namespace_loaded(S2)) {
			FS.update_path		(S,"$game_scripts$",strconcat(S1,S2,".script"));
			Msg					("* loading script %s",S1);
			load_file			(S,true);
		}

		xr_free					(S2);
	}
}
