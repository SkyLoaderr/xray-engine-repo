////////////////////////////////////////////////////////////////////////////
//	Module 		: script_stack_tracker_inline.h
//	Created 	: 21.04.2004
//  Modified 	: 21.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Script stack tracker inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	lua_State *CScriptStackTracker::lua	()
{
	return				(m_virtual_machine);
}

