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
#include <luabind/out_value_policy.hpp>

using namespace luabind;

CScriptIniFile *get_system_ini()
{
	return	((CScriptIniFile*)pSettings);
}

bool r_line(CScriptIniFile *self, LPCSTR S, int L,	xr_string &N, xr_string &V)
{
	THROW3			(self->section_exist(S),"Cannot find section",S);
	THROW2			((int)self->line_count(S) > L,"Invalid line number");
	
	N				= "";
	V				= "";
	
	LPCSTR			n,v;
	bool			result = !!self->r_line(S,L,&n,&v);
	if (!result)
		return		(false);

	N				= n;
	V				= v;
	return			(true);
}

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
			.def("r_string_wq",		&CScriptIniFile::r_string_wb	)
			.def("line_count",		&CScriptIniFile::line_count)
			.def("r_string",		&CScriptIniFile::r_string)
			.def("r_u32",			&CScriptIniFile::r_u32)
			.def("r_s32",			&CScriptIniFile::r_s32)
			.def("r_float",			&CScriptIniFile::r_float)
			.def("r_vector",		&CScriptIniFile::r_fvector3)
			.def("r_line",			&::r_line, out_value(_4) + out_value(_5)),

		def("system_ini",			&get_system_ini)
	];
}