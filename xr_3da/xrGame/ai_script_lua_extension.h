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
	void		vfPrintOutput				(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR	caScriptName);
	void		vfExportGlobals				(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportFvector				(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportFmatrix				(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportGame				(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportLevel				(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportParticles			(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportSound				(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportHit					(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportActions				(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfExportObject				(CLuaVirtualMachine *tpLuaVirtualMachine);
	void		vfLoadStandardScripts		(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR	caScriptName);
	void		vfExportToLua				(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR	caScriptName);
	void		vfLoadFileIntoNamespace		(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR	caScriptName,	bool bCall = true);
#ifdef DEBUG
	LPCSTR		cafEventToString			(int				iEventCode);
	void		vfPrintError				(CLuaVirtualMachine *tpLuaVirtualMachine, int		iErrorCode);
	bool		bfListLevelVars				(CLuaVirtualMachine *tpLuaVirtualMachine, int		iStackLevel);
#endif
};
