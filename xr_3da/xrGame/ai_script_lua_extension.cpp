////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_extension.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script extensions
////////////////////////////////////////////////////////////////////////////

#include <stdarg.h>
#include "stdafx.h"
#include "ai_script_space.h"
#include "ai_script_lua_extension.h"
#include "ai_space.h"

using namespace Script;

int __cdecl Lua::LuaOut(Lua::ELuaMessageType tLuaMessageType, LPCSTR caFormat, ...)
{
	LPCSTR		S = "", SS = "";
	LPSTR		S1;
	string4096	S2;
	switch (tLuaMessageType) {
		case Lua::eLuaMessageTypeInfo : {
			S	= "* [LUA] ";
			SS	= "[INFO]    ";
			break;
		}
		case Lua::eLuaMessageTypeError : {
			S	= "! [LUA] ";
			SS	= "[ERROR]   ";
			break;
		}
		case Lua::eLuaMessageTypeMessage : {
			S	= "[LUA] ";
			SS	= "[MESSAGE] ";
			break;
		}
		default : NODEFAULT;
	}
	
	va_list	l_tMarker;
	
	va_start(l_tMarker,caFormat);

	strcpy	(S2,S);
	S1		= S2 + strlen(S);
	int		l_iResult = vsprintf(S1,caFormat,l_tMarker);
	Msg		("%s",S2);
	
	strcpy	(S2,SS);
	S1		= S2 + strlen(SS);
	vsprintf(S1,caFormat,l_tMarker);
	getAI().m_tpLuaOutput->w_string(S2);

	va_end	(l_tMarker);

	return	(l_iResult);
}

void Script::vfLoadStandardScripts(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	string256		S,S1;
	FS.update_path	(S,"$game_data$","script.ltx");
	CInifile		*l_tpIniFile = xr_new<CInifile>(S);
	R_ASSERT		(l_tpIniFile);
	LPCSTR			caScriptString = l_tpIniFile->r_string("common","script");

	u32				caNamespaceName = _GetItemCount(caScriptString);
	string16		I;
	for (u32 i=0; i<caNamespaceName; i++) {
		FS.update_path(S,"$game_scripts$",strconcat(S1,_GetItem(caScriptString,i,I),".script"));
		bfLoadFile	(tpLuaVirtualMachine,S,true);
	}
	xr_delete		(l_tpIniFile);
}

void Script::vfExportToLua(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	luabind::open	(tpLuaVirtualMachine);

	vfExportGlobals	(tpLuaVirtualMachine);
	vfExportFvector	(tpLuaVirtualMachine);
	vfExportFmatrix	(tpLuaVirtualMachine);
	vfExportGame	(tpLuaVirtualMachine);
	vfExportLevel	(tpLuaVirtualMachine);
	vfExportDevice	(tpLuaVirtualMachine);
	vfExportParticles(tpLuaVirtualMachine);
	vfExportSound	(tpLuaVirtualMachine);
	vfExportHit		(tpLuaVirtualMachine);
	vfExportActions	(tpLuaVirtualMachine);
	vfExportObject	(tpLuaVirtualMachine);
	vfExportEffector(tpLuaVirtualMachine);

	vfLoadStandardScripts(tpLuaVirtualMachine);
}

bool Script::bfLoadFile(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR caScriptName, bool bCall)
{
	string256		l_caNamespaceName;
	_splitpath		(caScriptName,0,0,l_caNamespaceName,0);
	if (!strlen(l_caNamespaceName))
		return		(bfLoadFileIntoNamespace(tpLuaVirtualMachine,caScriptName,"_G",bCall));
	else
		return		(bfLoadFileIntoNamespace(tpLuaVirtualMachine,caScriptName,l_caNamespaceName,bCall));
}

bool bfCreateNamespaceTable(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR caNamespaceName)
{
	lua_pushstring	(tpLuaVirtualMachine,"_G");
	lua_gettable	(tpLuaVirtualMachine,LUA_GLOBALSINDEX);
	LPSTR			S2 = (char*)xr_malloc((strlen(caNamespaceName) + 1)*sizeof(char)), S = S2;
	strcpy			(S,caNamespaceName);
	for (;;) {
		if (!strlen(S)) {
			lua_pop		(tpLuaVirtualMachine,1);
			LuaOut		(Lua::eLuaMessageTypeError,"the namespace name %s is incorrect!",caNamespaceName);
			xr_free		(S2);
			return		(false);
		}
		LPSTR			S1 = strchr(S,'.');
		if (S1)
			*S1				= 0;
		lua_pushstring	(tpLuaVirtualMachine,S);
		lua_gettable	(tpLuaVirtualMachine,-2);
		if (lua_isnil(tpLuaVirtualMachine,-1)) {
			lua_pop			(tpLuaVirtualMachine,1);
			lua_newtable	(tpLuaVirtualMachine);
			lua_pushstring	(tpLuaVirtualMachine,S);
			lua_pushvalue	(tpLuaVirtualMachine,-2);
			lua_settable	(tpLuaVirtualMachine,-4);
		}
		else
			if (!lua_istable(tpLuaVirtualMachine,-1)) {
				xr_free			(S2);
				lua_pop			(tpLuaVirtualMachine,2);
				LuaOut			(Lua::eLuaMessageTypeError,"the namespace name %s is already being used by the non-table object!",caNamespaceName);
				return			(false);
			}
			lua_remove		(tpLuaVirtualMachine,-2);
			if (S1)
				S			= ++S1;
			else
				break;
	}
	xr_free			(S2);
	return			(true);
}

void vfCopyGlobals(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	lua_newtable	(tpLuaVirtualMachine);
	lua_pushstring	(tpLuaVirtualMachine,"_G");
	lua_gettable	(tpLuaVirtualMachine,LUA_GLOBALSINDEX);
	lua_pushnil		(tpLuaVirtualMachine);
	while (lua_next(tpLuaVirtualMachine, -2) != 0) {
		lua_pushvalue	(tpLuaVirtualMachine,-2);
		lua_pushvalue	(tpLuaVirtualMachine,-2);
		lua_settable	(tpLuaVirtualMachine,-6);
		lua_pop			(tpLuaVirtualMachine, 1);
	}
}

bool Script::bfLoadBuffer(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR caBuffer, size_t tSize, LPCSTR caScriptName)
{
	int				l_iErrorCode = luaL_loadbuffer(tpLuaVirtualMachine,caBuffer,tSize,caScriptName);

	if (l_iErrorCode) {
#ifdef DEBUG
		if (!bfPrintOutput	(tpLuaVirtualMachine,caScriptName,l_iErrorCode));
			vfPrintError(tpLuaVirtualMachine,l_iErrorCode);
#endif
		return			(false);
	}
	return			(true);
}

bool bfDoFile(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR caScriptName, bool bCall)
{
	string256		l_caLuaFileName;
	IReader			*l_tpFileReader = FS.r_open(caScriptName);
	R_ASSERT		(l_tpFileReader);
	strconcat		(l_caLuaFileName,"@",caScriptName);
	
	if (!bfLoadBuffer(tpLuaVirtualMachine,static_cast<LPCSTR>(l_tpFileReader->pointer()),(size_t)l_tpFileReader->length(),l_caLuaFileName)) {
		lua_pop			(tpLuaVirtualMachine,2);
		FS.r_close		(l_tpFileReader);
		return		(false);
	}
	FS.r_close		(l_tpFileReader);

	if (bCall) {
		lua_call	(tpLuaVirtualMachine,0,0);
//		int			l_iErrorCode = lua_pcall(tpLuaVirtualMachine,0,0,0);
//		if (l_iErrorCode) {
//#ifdef DEBUG
//			bfPrintOutput	(tpLuaVirtualMachine,caScriptName,l_iErrorCode);
//			vfPrintError	(tpLuaVirtualMachine,l_iErrorCode);
//#endif
//			return	(false);
//		}
	}
	else
		lua_insert		(tpLuaVirtualMachine,-4);

	return			(true);
}

void vfSetNamespace(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	lua_pushnil		(tpLuaVirtualMachine);
	while (lua_next(tpLuaVirtualMachine, -2) != 0) {
		lua_pushvalue	(tpLuaVirtualMachine,-2);
		lua_gettable	(tpLuaVirtualMachine,-5);
		if (lua_isnil(tpLuaVirtualMachine,-1)) {
			lua_pop			(tpLuaVirtualMachine,1);
			lua_pushvalue	(tpLuaVirtualMachine,-2);
			lua_pushvalue	(tpLuaVirtualMachine,-2);
			lua_pushvalue	(tpLuaVirtualMachine,-2);
			lua_pushnil		(tpLuaVirtualMachine);
			lua_settable	(tpLuaVirtualMachine,-7);
			lua_settable	(tpLuaVirtualMachine,-7);
		}
		else {
			lua_pop			(tpLuaVirtualMachine,1);
			lua_pushvalue	(tpLuaVirtualMachine,-2);
			lua_gettable	(tpLuaVirtualMachine,-4);
			if (!lua_equal(tpLuaVirtualMachine,-1,-2)) {
				lua_pushvalue	(tpLuaVirtualMachine,-3);
				lua_pushvalue	(tpLuaVirtualMachine,-2);
				lua_pushvalue	(tpLuaVirtualMachine,-2);
				lua_pushvalue	(tpLuaVirtualMachine,-5);
				lua_settable	(tpLuaVirtualMachine,-8);
				lua_settable	(tpLuaVirtualMachine,-8);
			}
			lua_pop			(tpLuaVirtualMachine,1);
		}
		lua_pushvalue	(tpLuaVirtualMachine,-2);
		lua_pushnil		(tpLuaVirtualMachine);
		lua_settable	(tpLuaVirtualMachine,-6);
		lua_pop			(tpLuaVirtualMachine, 1);
	}
	lua_pop			(tpLuaVirtualMachine,3);
}

bool Script::bfLoadFileIntoNamespace(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR caScriptName, LPCSTR caNamespaceName, bool bCall)
{
	if (!bfCreateNamespaceTable(tpLuaVirtualMachine,caNamespaceName))
		return		(false);
	vfCopyGlobals	(tpLuaVirtualMachine);
	if (!bfDoFile(tpLuaVirtualMachine,caScriptName,bCall))
		return		(false);
	vfSetNamespace	(tpLuaVirtualMachine);
	return			(true);
}