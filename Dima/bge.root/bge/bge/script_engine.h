////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine.h
//	Created 	: 03.12.2004
//  Modified 	: 03.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Scripting engine
////////////////////////////////////////////////////////////////////////////

#pragma once

struct lua_State;

class CScriptEngine {
private:
	lua_State			*m_virtual_machine;

public:
						CScriptEngine	();
	virtual				~CScriptEngine	();
			void		init			();
	IC		lua_State	*lua			() const;
};

#include "script_engine_inline.h"