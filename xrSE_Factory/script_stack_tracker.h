////////////////////////////////////////////////////////////////////////////
//	Module 		: script_stack_tracker.h
//	Created 	: 21.04.2004
//  Modified 	: 21.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Script stack tracker
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_space.h"

class CScriptStackTracker {
protected:
	enum consts {
		max_stack_size = u32(64),
	};

protected:
	lua_Debug			m_stack[max_stack_size];
	int					m_current_stack_level;
	CLuaVirtualMachine	*m_virtual_machine;

public:
	IC							CScriptStackTracker		();
	virtual						~CScriptStackTracker	();
			void				script_hook				(CLuaVirtualMachine *L, lua_Debug *dbg);
			void				print_stack				(CLuaVirtualMachine *L);
	IC		CLuaVirtualMachine	*lua					();
};

#include "script_stack_tracker_inline.h"