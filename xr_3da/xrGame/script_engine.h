////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine.h
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_storage.h"
#include "script_export_space.h"
#include "script_space_forward.h"
#include <lua.h>

//#define DBG_DISABLE_SCRIPTS

class CScriptProcess;
class CScriptThread;
struct lua_State;
struct lua_Debug;

#ifdef USE_DEBUGGER
	class CScriptDebugger;
#endif

class CScriptEngine : public CScriptStorage {
public:
	typedef CScriptStorage inherited;
	typedef xr_map<LPCSTR,CScriptProcess*,pred_str> CScriptProcessStorage;

private:
	bool						m_reload_modules;

protected:
	CScriptProcessStorage		m_script_processes;
	int							m_stack_level;
	shared_str					m_class_registrators;

protected:
#ifdef USE_DEBUGGER
	CScriptDebugger				*m_scriptDebugger;
#endif

public:
								CScriptEngine				();
	virtual						~CScriptEngine				();
	virtual	void				unload						();
	static	int					lua_panic					(lua_State *L);
	static	void				lua_error					(lua_State *L);
	static	int					lua_pcall_failed			(lua_State *L);
	static	void				lua_hook_call				(lua_State *L, lua_Debug *dbg);
			void				setup_callbacks				();
			void				load_common_scripts			();
			bool				load_file					(LPCSTR	caScriptName,	bool	bCall = true);
	IC		CScriptProcess		*script_process				(LPCSTR process_name) const;
	IC		void				add_script_process			(LPCSTR process_name, CScriptProcess *script_process);
			void				remove_script_process		(LPCSTR process_name);
			void				setup_auto_load				();
			void				process_file_if_exists		(LPCSTR file_name, bool warn_if_not_exist);
			void				process_file				(LPCSTR file_name);
			void				process_file				(LPCSTR file_name, bool reload_modules);
			void				script_export				();
			bool				function_object				(LPCSTR function_to_call, luabind::object &object, int type = LUA_TFUNCTION);
			void				register_script_classes		();
	IC		void				parse_script_namespace		(LPCSTR function_to_call, LPSTR name_space, LPSTR functor);
			void				load_class_registrators		();

	template <typename _result_type>
	IC		bool				functor						(LPCSTR function_to_call, luabind::functor<_result_type> &lua_function);

#ifdef USE_DEBUGGER
			void				stopDebugger				();
			void				restartDebugger				();
			CScriptDebugger		*debugger					();
#endif

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CScriptEngine)
#undef script_type_list
#define script_type_list save_type_list(CScriptEngine)

#include "script_engine_inline.h"