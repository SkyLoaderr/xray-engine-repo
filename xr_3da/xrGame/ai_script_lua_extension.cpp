////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_extension.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script extensions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script_lua_extension.h"

using namespace Script;

void Script::vfLoadStandardScripts(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR caScriptName)
{
	string256		S,S1;
	FS.update_path	(S,"$game_data$","script.ltx");
	CInifile		*l_tpIniFile = xr_new<CInifile>(S);
//	R_ASSERT		(l_tpIniFile);
	LPCSTR			caScriptString = l_tpIniFile->r_string("common","script");

	u32				N = _GetItemCount(caScriptString);
	string16		I;
	for (u32 i=0; i<N; i++) {
		FS.update_path(S,"$game_scripts$",strconcat(S1,_GetItem(caScriptString,i,I),".script"));
//		R_ASSERT3	(FS.exist(S),"Script file not found!",S);
		IReader		*F = FS.r_open(S);
//		R_ASSERT	(F);
		strconcat	(S1,"@",S);		
		int			l_iErrorCode = lua_dobuffer(tpLuaVirtualMachine,static_cast<LPCSTR>(F->pointer()),F->length(),S1);
		Msg			("* Loading common script %s",S);
		vfPrintOutput(tpLuaVirtualMachine,S);
		if (l_iErrorCode)
			vfPrintError(tpLuaVirtualMachine,l_iErrorCode);
		FS.r_close	(F);
	}
	xr_delete		(l_tpIniFile);

	sprintf			(S,"\nscript_name = %s\n",caScriptName);
	lua_dostring	(tpLuaVirtualMachine,S);
}

void Script::vfExportToLua(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR caScriptName)
{
	open			(tpLuaVirtualMachine);

	vfExportGlobals	(tpLuaVirtualMachine);
	vfExportFvector	(tpLuaVirtualMachine);
	vfExportFmatrix	(tpLuaVirtualMachine);
	vfExportGame	(tpLuaVirtualMachine);
	vfExportLevel	(tpLuaVirtualMachine);
	vfExportParticles(tpLuaVirtualMachine);
	vfExportSound	(tpLuaVirtualMachine);
	vfExportHit		(tpLuaVirtualMachine);
	vfExportActions	(tpLuaVirtualMachine);
	vfExportObject	(tpLuaVirtualMachine);

	vfLoadStandardScripts(tpLuaVirtualMachine,caScriptName);
}