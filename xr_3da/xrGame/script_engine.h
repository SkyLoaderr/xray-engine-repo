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

#ifdef XRGAME_EXPORTS
	class CScriptProcess;
	class CScriptThread;
	class CScriptDebugger;
#endif

class CScriptEngine : public CScriptStorage {
public:
	typedef CScriptStorage inherited;
#ifdef XRGAME_EXPORTS
	typedef xr_map<LPCSTR,CScriptProcess*,pred_str> CScriptProcessStorage;
#endif

protected:
#ifdef XRGAME_EXPORTS
	CScriptProcessStorage		m_script_processes;
#endif
	xr_deque<LPSTR>				m_load_queue;
	CScriptStackTracker 		*m_current_thread;
	int							m_stack_level;
	bool						m_reload_modules;
	ref_str						m_class_registrators;

public:
								CScriptEngine				();
	virtual						~CScriptEngine				();
	virtual	void				unload						();
	static	void				lua_hook_call				(CLuaVirtualMachine *L, lua_Debug *tpLuaDebug);
	static	int					lua_panic					(CLuaVirtualMachine *L);
	static	void				lua_error					(CLuaVirtualMachine *L);
	static	void				lua_cast_failed				(CLuaVirtualMachine *L, LUABIND_TYPE_INFO info);
			void				load_common_scripts			();
			bool				load_file					(LPCSTR	caScriptName,	bool	bCall = true);
#ifdef XRGAME_EXPORTS
	IC		CScriptProcess		*script_process				(LPCSTR process_name) const;
	IC		void				add_script_process			(LPCSTR process_name, CScriptProcess *script_process);
			void				remove_script_process		(LPCSTR process_name);
#endif
			void				add_file					(LPCSTR file_name);
			void				process						();
			void				export						();
	IC		void				set_current_thread			(CScriptStackTracker *new_thread);
	IC		CScriptStackTracker	*current_thread				();
	IC		CScriptStackTracker	&script_stack_tracker		();
	IC		void				reload_modules				(bool flag);

	template <typename _result_type>
	IC		bool				functor						(LPCSTR function_to_call, luabind::functor<_result_type> &lua_function);
			void				register_script_classes		();
	IC		void				parse_script_namespace		(LPCSTR function_to_call, LPSTR name_space, LPSTR functor);

	template <typename _result_type>
	IC		luabind::functor<_result_type>	create_object_creator	(LPCSTR class_name, LPCSTR arguments);
			void				load_class_registrators		();
	
#ifdef XRGAME_EXPORTS
	CScriptDebugger	*			m_scriptDebugger;
#endif
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CScriptEngine)
#undef script_type_list
#define script_type_list save_type_list(CScriptEngine)

#include "script_engine_inline.h"