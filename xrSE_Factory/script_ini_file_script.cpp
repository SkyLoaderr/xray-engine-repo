////////////////////////////////////////////////////////////////////////////
//	Module 		: script_ini_file_script.cpp
//	Created 	: 25.06.2004
//  Modified 	: 25.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script ini file class export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_ini_file.h"
#include "script_space.h"

using namespace luabind;

void CScriptIniFile::script_register(lua_State *L)
{
	module(L)
	[
		class_<CScriptIniFile>("ini_file")
			.def(					constructor<LPCSTR>())
			.def("section_exist",	&CScriptIniFile::section_exist	)
			.def("line_exist",		&CScriptIniFile::line_exist		)
			.def("r_clsid",			&CScriptIniFile::r_clsid		)
			.def("r_bool",			&CScriptIniFile::r_bool			)
			.def("r_token",			&CScriptIniFile::r_token		)
			.def("r_string",		(LPCSTR (CScriptIniFile::*)	(LPCSTR, LPCSTR))(CScriptIniFile::r_string))
			.def("r_u32",			(u32	(CScriptIniFile::*)	(LPCSTR, LPCSTR))(CScriptIniFile::r_u32))
			.def("r_s32",			(int	(CScriptIniFile::*)	(LPCSTR, LPCSTR))(CScriptIniFile::r_s32))
			.def("r_float",			(float	(CScriptIniFile::*)	(LPCSTR, LPCSTR))(CScriptIniFile::r_float))
			.def("r_vector",		(Fvector(CScriptIniFile::*)	(LPCSTR, LPCSTR))(CScriptIniFile::r_fvector3))
			.def("line_count",		(u32	(CScriptIniFile::*)	(LPCSTR))		 (CScriptIniFile::line_count))
	];
}