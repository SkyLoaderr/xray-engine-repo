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

// #define DBG_DISABLE_SCRIPTS

class CScriptProcess;
class CScriptThread;
struct lua_State;

#ifdef USE_DEBUGGER
	class CScriptDebugger;
#endif

class CScriptEngine : public CScriptStorage {
public:
	typedef CScriptStorage inherited;
	typedef xr_map<LPCSTR,CScriptProcess*,pred_str> CScriptProcessStorage;

protected:
	CScriptProcessStorage		m_script_processes;
	xr_deque<LPSTR>				m_load_queue;
	CScriptStackTracker 		*m_current_thread;
	int							m_stack_level;
	bool						m_reload_modules;
	ref_str						m_class_registrators;
	luabind::object				*m_return_passed_object_functor;
	bool						m_global_script_loaded;
#ifdef USE_DEBUGGER
	CScriptDebugger				*m_scriptDebugger;
#endif

public:
								CScriptEngine				();
	virtual						~CScriptEngine				();
	virtual	void				unload						();
	static	void				lua_hook_call				(lua_State *L, lua_Debug *tpLuaDebug);
	static	int					lua_panic					(lua_State *L);
	static	void				lua_error					(lua_State *L);
			void				load_common_scripts			();
			bool				load_file					(LPCSTR	caScriptName,	bool	bCall = true);
	IC		CScriptProcess		*script_process				(LPCSTR process_name) const;
	IC		void				add_script_process			(LPCSTR process_name, CScriptProcess *script_process);
			void				remove_script_process		(LPCSTR process_name);
			void				add_file					(LPCSTR file_name);
			void				process						();
			void				script_export				();
	IC		void				set_current_thread			(CScriptStackTracker *new_thread);
	IC		CScriptStackTracker	*current_thread				();
	IC		CScriptStackTracker	&script_stack_tracker		();
	IC		void				reload_modules				(bool flag);
			bool				function_object				(LPCSTR function_to_call, luabind::object &object);
			void				register_script_classes		();
	IC		void				parse_script_namespace		(LPCSTR function_to_call, LPSTR name_space, LPSTR functor);
			void				initialize_return_passed_object();
			void				load_class_registrators		();

	template <typename _result_type>
	IC		bool				functor						(LPCSTR function_to_call, luabind::functor<_result_type> &lua_function);
	
	template <typename T>
	IC		T					get_value_from_object		(luabind::object object);

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CScriptEngine)
#undef script_type_list
#define script_type_list save_type_list(CScriptEngine)

#include "script_engine_inline.h"