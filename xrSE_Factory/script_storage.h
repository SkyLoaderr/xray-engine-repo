////////////////////////////////////////////////////////////////////////////
//	Module 		: script_storage.h
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Storage
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_storage_space.h"
#include "script_stack_tracker.h"
#include "script_space_forward.h"

struct lua_State;

#ifdef	DEBUG
	#ifndef ENGINE_BUILD
	#	define	USE_DEBUGGER
	#endif
#endif

using namespace ScriptStorage;

class CScriptStorage : public CScriptStackTracker {
protected:
	CMemoryWriter				m_output;

protected:
			bool				parse_namespace				(LPCSTR caNamespaceName, LPSTR b, LPSTR c);
			bool				do_file						(LPCSTR	caScriptName,		LPCSTR caNameSpaceName,		bool	bCall);

public:
								CScriptStorage				();
	virtual						~CScriptStorage				();
	static	int __cdecl			script_log					(ELuaMessageType message,	LPCSTR	caFormat, ...);
	static	bool				print_output				(lua_State *L,		LPCSTR	caScriptName,		int		iErorCode = 0);
	static	LPCSTR				event2string				(int	iEventCode);
	static	void				print_error					(lua_State *L,		int		iErrorCode);
	static	bool				print_stack_level			(lua_State *L,		int		iStackLevel);
			bool				load_buffer					(lua_State *L,		LPCSTR	caBuffer,			size_t	tSize,				LPCSTR	caScriptName, LPCSTR caNameSpaceName = 0);
			bool				load_file_into_namespace	(LPCSTR	caScriptName,		LPCSTR	caNamespaceName,	bool	bCall);
			bool				namespace_loaded			(LPCSTR	caName, bool remove_from_stack = true);
			bool				object						(LPCSTR	caIdentifier,		int		type);
			bool				object						(LPCSTR	caNamespaceName,	LPCSTR	caIdentifier,		int		type);
			luabind::object		name_space					(LPCSTR	namespace_name);
			void				flush_log					();
			void				print_stack					(lua_State *L);
			void				print_table					(lua_State *L, LPCSTR S, bool bRecursive = false);
};

#include "script_storage_inline.h"