////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_extension.h
//	Created 	: 19.09.2003
//  Modified 	: 22.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script extensions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_script_space.h"

namespace Script {
	void		vfExportToLua				(CLuaVirtualMachine *tpLuaVirtualMachine);
	int			ifSuspendThread				(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		_Msg						(LPCSTR				caFormat,...);
#ifdef DEBUG
	LPCSTR		cafEventToString			(int				iEventCode);
	void		vfPrintError				(CLuaVirtualMachine *tpLuaVirtualMachine, int iErrorCode);
	bool		bfListLevelVars				(CLuaVirtualMachine *tpLuaVirtualMachine, int level);
#endif
};
