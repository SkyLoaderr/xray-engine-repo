////////////////////////////////////////////////////////////////////////////
//	Module 		: script_storage.h
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Storage
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_space.h"
#include "script_storage_space.h"

using namespace ScriptStorage;

class CScriptStorage {
protected:
	CMemoryWriter				m_output;
	lua_State					*m_virtual_machine;

protected:
			bool				create_namespace			(LPCSTR	caNamespaceName);
			void				copy_globals				();
			bool				do_file						(LPCSTR	caScriptName,		LPCSTR caNameSpaceName,		bool	bCall);
			void				set_namespace				();

public:
								CScriptStorage				();
	virtual						~CScriptStorage				();
	static	int __cdecl			script_log					(ELuaMessageType message,	LPCSTR	caFormat, ...);
	static	bool				print_output				(CLuaVirtualMachine *L,		LPCSTR	caScriptName,		int		iErorCode = 0);
	static	LPCSTR				event2string				(int	iEventCode);
	static	void				print_error					(CLuaVirtualMachine *L,		int		iErrorCode);
	static	bool				print_stack_level			(CLuaVirtualMachine *L,		int		iStackLevel);
			bool				load_buffer					(CLuaVirtualMachine *L,		LPCSTR	caBuffer,			size_t	tSize,				LPCSTR	caScriptName, LPCSTR caNameSpaceName = 0);
			bool				load_file_into_namespace	(LPCSTR	caScriptName,		LPCSTR	caNamespaceName,	bool	bCall);
			bool				namespace_loaded			(LPCSTR	caName);
			bool				object						(LPCSTR	caIdentifier,		int		type);
			bool				object						(LPCSTR	caNamespaceName,	LPCSTR	caIdentifier,		int		type);
			luabind::object		name_space					(LPCSTR	namespace_name);
	IC		CLuaVirtualMachine	*lua						();
			void				flush_log					();
};

#include "script_storage_inline.h"