////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine.h
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_storage.h"
#include "script_class_id.h"

class CScriptProcessor;
class CScript;

class CScriptEngine : 
	public CScriptStorage,
	public CLSID_Holder
{
public:
	typedef CScriptStorage inherited;
	typedef xr_map<LPCSTR,CScriptProcessor*,pred_str> CScriptProcessorStorage;

protected:
	CScriptProcessorStorage		m_script_processors;
	xr_deque<LPSTR>				m_load_queue;
	CScriptStackTracker 		*m_current_thread;
	int							m_stack_level;
	bool						m_reload_modules;

protected:
			void				export_globals				();
			void				export_fvector				();
			void				export_fmatrix				();
			void				export_game					();
			void				export_level				();
			void				export_device				();
			void				export_particles			();
			void				export_sound				();
			void				export_hit					();
			void				export_actions				();
			void				export_ini					();
			void				export_object				();
			void				export_effector				();
			void				export_artifact_merger		();
			void				export_memory_objects		();
			void				export_action_management	();
			void				export_motivation_management();
			void				export_sound_info			();

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
	IC		CScriptProcessor	*script_processor		(LPCSTR processor_name) const;
	IC		void				add_script_processor		(LPCSTR processor_name, CScriptProcessor *script_processor);
			void				remove_script_processor		(LPCSTR processor_name);
			void				add_file					(LPCSTR file_name);
			void				process						();
			void				export						();
	IC		void				set_current_thread			(CScriptStackTracker *new_thread);
	IC		CScriptStackTracker	*current_thread				();
	IC		CScriptStackTracker	&script_stack_tracker		();
	IC		void				reload_modules				(bool flag);
};

#include "script_engine_inline.h"