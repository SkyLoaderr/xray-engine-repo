////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine.h
//	Created 	: 03.12.2004
//  Modified 	: 03.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Scripting engine
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"
#include "ui.h"

struct lua_State;

class CScriptEngine {
private:
	lua_State			*m_virtual_machine;

public:
						CScriptEngine	();
	virtual				~CScriptEngine	();
			void		init			();
			void		run_string		(LPCSTR string_to_run);

public:
	IC		lua_State	*lua			() const;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CScriptEngine)
#undef script_type_list
#define script_type_list save_type_list(CScriptEngine)

#include "script_engine_inline.h"